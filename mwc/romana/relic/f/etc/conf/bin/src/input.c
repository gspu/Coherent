/*
 * Input stream abstraction.
 */

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		USE_PROTO
 *		ARGS ()
 *		LOCAL
 *	<stddef.h>
 *		NULL
 *		offsetof ()
 *	<stdio.h>
 *		EOF
 *		FILE
 *		stderr
 *		fprintf ()
 *		fputs ()
 *		getc ()
 *		putc ()
 *	<stdlib.h>
 *		free ()
 *		malloc ()
 *	<string.h>
 *		memcpy ()
 *		strlen ()
 */

#include <common/tricks.h>
#include <sys/compat.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "devadm.h"

#include "lex.h"

#include "input.h"
#include "mkinput.h"


/*
 * Structures for dealing with input from a file.
 *
 * A new feature added to this for the 'devadm' utility is support for an
 * output file attached to the input file that can be used to support input
 * filtering effectively. Comments in the input source are copied to the
 * output by the lower layer, while higher-level software can perform semantic
 * processing and then copy the resulting changes out, retaining the relative
 * ordering of the components. This may cause some reordering (moving comments
 * at the end of a line to the beginning of a line), but will in general be
 * benign.
 */

typedef struct fileinput filein_t;

struct fileinput {
	input_t		fi_input;

	char	      *	fi_name;
	FILE	      *	fi_infile;	/* input stream */
	int		fi_inclose;	/* close when stream is closed? */
	FILE	      *	fi_outfile;	/* output stream */
	int		fi_outclose;	/* close when stream is closed? */
	int		fi_lineno;	/* line number in file */
	int		fi_colno;	/* current column number */
	int		fi_prevline;
	int		fi_prevcol;
	int		fi_comment;	/* comment character (maybe none) */
	int		fi_prevchar;	/* previous character */
	int		fi_outcol;	/* output column number */

	char		fi_line [64];	/* line buffer for errors */
};

#define	ARRAY_LEN(a)	(sizeof (a) / sizeof (* a))


/*
 * Structure for processing input from a string.
 */

typedef struct strinput	strin_t;

struct strinput {
	input_t		si_input;

	CONST unsigned char
		      *	si_data;	/* data being read */
	size_t		si_pos;		/* position in input stream */
	size_t		si_len;		/* length of input stream */
};


/*
 * Send a single character to the filter output.
 */

#if	USE_PROTO
LOCAL int _fi_putc (int ch, filein_t * fi)
#else
LOCAL int
_fi_putc (ch, fi)
int		ch;
filein_t      *	fi;
#endif
{
	FILE	      *	file;

	if ((file = fi->fi_outfile) == NULL) {
#if	TRACE
		if (do_debug == 0)
			return 0;
		file = stderr;
#else
		return 0;
#endif
	}

	if (fputc (ch, file) < 0)
		return -1;

	switch (ch) {
	case '\n':
		fi->fi_outcol = 0;
		break;

	case '\t':
		fi->fi_outcol = ROUND_UP_TO_MULTIPLE (fi->fi_outcol + 1, 8);
		break;

	default:
		fi->fi_outcol ++;
		break;
	}

	return 0;
}


/*
 * Read a character from a file.
 */

#if	USE_PROTO
LOCAL int fi_read (input_t * input)
#else
LOCAL int
fi_read (input)
input_t	      *	input;
#endif
{
	filein_t      *	fi = (filein_t *) input;
	int		ch;

	/*
	 * Get a character from the input file, recording the data for the
	 * purpose of error reporting.
	 */

	fi->fi_prevcol = fi->fi_colno;
	fi->fi_prevline = fi->fi_lineno;

another_line:
	if ((ch = getc (fi->fi_infile)) == EOF)
		return IN_EOF;

	fi->fi_line [fi->fi_colno ++ % ARRAY_LEN (fi->fi_line)] = ch;

	if (ch == fi->fi_comment) {
		/*
		 * Consume data to EOL, then return EOL.
		 */

		do {
			_fi_putc (ch, fi);

			if ((ch = getc (fi->fi_infile)) == EOF)
				return IN_EOF;
		} while (ch != '\n');

		if (fi->fi_colno > 1)
			_fi_putc (ch, fi);
	}


	if (ch == '\n') {

		fi->fi_lineno ++;

		if (fi->fi_colno == 1) {

			_fi_putc (ch, fi);
			goto another_line;
		};

		fi->fi_colno = 0;
	}

	fi->fi_prevchar = ch;
	return ch;
}


/*
 * Return the last read character to the input stream.
 */

#if	USE_PROTO
LOCAL void fi_unread (input_t * input)
#else
LOCAL void
fi_unread (input)
input_t	      *	input;
#endif
{
	filein_t      *	fi = (filein_t *) input;

	if (fi->fi_prevchar != IN_EOF) {

		ungetc (fi->fi_prevchar, fi->fi_infile);

		fi->fi_prevchar = IN_EOF;
		fi->fi_colno = fi->fi_prevcol;
		fi->fi_lineno = fi->fi_prevline;
	}
}


/*
 * Close the input file.
 */

#if	USE_PROTO
LOCAL void fi_close (input_t * input)
#else
LOCAL void
fi_close (input)
input_t	      *	input;
#endif
{
	filein_t      *	fi = (filein_t *) input;

	if (fi->fi_infile != NULL && fi->fi_inclose)
		(void) fclose (fi->fi_infile);
	if (fi->fi_outfile != NULL && fi->fi_outclose)
		(void) fclose (fi->fi_outfile);
	free (fi);
}


/*
 * Produce an error report for this input source.
 */

#if	USE_PROTO
LOCAL void fi_error (input_t * input)
#else
LOCAL void
fi_error (input)
input_t	      *	input;
#endif
{
	filein_t      *	fi = (filein_t *) input;
	int		col;
	int		ch;
	int		end;

	(void) fprintf (stderr, "Error vicinity : file \"%s\", line %d, column %d\n",
			fi->fi_name, fi->fi_prevline, fi->fi_prevcol);

	col = fi->fi_prevcol < 40 ? 0 : fi->fi_prevcol - 40;

	if (fi->fi_colno > (end = fi->fi_prevcol))
		end ++;

	while (col < end) {
		/*
		 * Should think about escaping controls.
		 */

		ch = fi->fi_line [col ++ % ARRAY_LEN (fi->fi_line)];
		(void) putc (ch, stderr);
	}


	fputs (" <---\n", stderr);
}


/*
 * Perform columnar formatted output.
 */

#if	USE_PROTO
LOCAL int fi_filter (input_t * input, CONST char * format, ...)
#else
LOCAL int
fi_filter (input, format)
input_t	      *	input;
CONST char    *	format;
#endif
{
	filein_t      *	fi = (filein_t *) input;
	va_list		args;
	FILE	      *	file;

	if ((file = fi->fi_outfile) == NULL) {
#if	TRACE
		if (do_debug == 0)
			return 0;
		file = stderr;
#else
		return 0;
#endif
	}

	va_start (args, format);

	while (* format) {
		char		ch;
		int		cols;
		char	      *	tmp;
		static char	digits [] = "0123456789";
		int		long_datum;

		/*
		 * We accept either a '%' or '<' sign to introduce a format
		 * so that we don't clutter things up with too many '%'-signs.
		 */

		if (((ch = * format ++) != '%' && ch != '<') ||
		    * format == ch) {
			_fi_putc (ch, fi);
			continue;
		}

		if ((ch = * format ++) == 0)
			return -1;

		/*
	 	 * After a percent-sign, look for a number of tab-columns to
		 * align the output to.
		 */

		cols = 0;
		while ((tmp = strchr (digits, ch)) != NULL) {

			cols = (cols * 10) + (tmp - digits);
			if ((ch = * format ++) == 0)
				break;
		}

		/*
		 * Dispatch on the format character and record the number of
		 * characters written (which will be negative on error).
		 */

		if ((long_datum = ch == 'l') != 0)
			ch = * format ++;

		switch (ch) {
		case 'c':
			ch = fprintf (file, "%*c", cols,
				      va_arg (args, int));
			break;

		case 's':
			ch = fprintf (file, "%*s", cols,
				      va_arg (args, char *));
			break;

		case 'd':
			ch = fprintf (file, "%*ld", cols,
				      long_datum ? va_arg (args, long) :
						   (long) va_arg (args, int));
			break;

		case 'x':
			ch = fprintf (file, "%*lx", cols,
				      long_datum ? va_arg (args, long) :
						   (long) va_arg (args, int));
			break;

		case '>':
			/*
			 * Here is what we add: <{digits}> indicates a column
			 * number to seek forward to.
			 */

			cols = cols * 8 - fi->fi_outcol;

			if (cols <= 0)
				_fi_putc (' ', fi);

			while (cols > 0) {
				_fi_putc ('\t', fi);
				cols -= 8;
			}

			ch = 0;
			break;

		default:
			return -1;
		}

		if (ch < 0)
			return -1;

		fi->fi_outcol += ch;
	}

	va_end (args);
	return 0;
}


/*
 * Create a source of input from a file. The 'comment' parameter gives the
 * code of a character which can be used to indicate input sections that are
 * to be ignored. The 'filter' parameter points to an optional FILE where the
 * input is going to be copied.
 */

#if	USE_PROTO
input_t * (make_filter) (FILE * infile, CONST char * inname, int inclose,
			 int comment, FILE * outfile, int outclose)
#else
input_t *
make_filter ARGS ((infile, inname, inclose, comment, outfile, outclose))
FILE	      *	infile;
CONST char    *	inname;
int		inclose;
int		comment;
FILE	      *	outfile;
int		outclose;
#endif
{
	filein_t      *	fi;
	size_t		namelen;

	/* ASSERT that fi_input is the first member of the structure */

	if (offsetof (filein_t, fi_input) != 0)
		return NULL;

	if (inname == NULL)
		inname = "no name";

	namelen = strlen (inname) + 1;

	if ((fi = (filein_t *) malloc (sizeof (* fi) + namelen)) == NULL)
		return NULL;

	fi->fi_lineno = 1;
	fi->fi_colno = 0;
	fi->fi_infile = infile;
	fi->fi_inclose = inclose;
	fi->fi_outfile = outfile;
	fi->fi_outclose = outclose;
	fi->fi_name = (char *) (fi + 1);

	fi->fi_comment = comment;
	fi->fi_prevchar = IN_EOF;

	if (namelen > 0)
		memcpy (fi->fi_name, inname, namelen);

	fi->fi_input.in_read = fi_read;
	fi->fi_input.in_unread = fi_unread;
	fi->fi_input.in_readtok = NULL;
	fi->fi_input.in_close = fi_close;
	fi->fi_input.in_error = fi_error;
	fi->fi_input.in_filter = fi_filter;

	return & fi->fi_input;
}


/*
 * Return a character from an input string, or EOF when the end-of-string is
 * reached.
 */

#if	USE_PROTO
LOCAL int si_read (input_t * input)
#else
LOCAL int
si_read (input)
input_t	      *	input;
#endif
{
	strin_t	      *	si = (strin_t *) input;

	if (si->si_pos >= si->si_len)
		return IN_EOF;

	return si->si_data [si->si_pos ++];
}


/*
 * Return a character to the input stream to be read again.
 */

#if	USE_PROTO
LOCAL void si_unread (input_t * input)
#else
LOCAL void
si_unread (input)
input_t	      *	input;
#endif
{
	strin_t	      *	si = (strin_t *) input;

	if (si->si_pos > 0)
		si->si_pos --;
}


/*
 * Reading a token, specialized to string input streams.
 */

#if	USE_PROTO
LOCAL CONST unsigned char * si_readtok (input_t * input, lex_t * lexp,
					size_t * lengthp)
#else
LOCAL CONST unsigned char *
si_readtok (input, lexp, lengthp)
input_t	      *	input;
lex_t	      *	lexp;
size_t	      *	lengthp;
#endif
{
	strin_t	      *	si = (strin_t *) input;
	int		start = -1;
	int		ch;

	for (;;) {
		if ((ch = si_read (input)) == IN_EOF)
			break;

		switch (classify (lexp, ch, start == -1)) {

		case CLASS_FLUSH:
			continue;

		case CLASS_SEP:
			si_unread (input);
			goto separator;

		default:
			break;
		}

		if (start == -1)
			start = si->si_pos - 1;
	}

separator:
	* lengthp = start == -1 ? 0 : si->si_pos - start;
	return start == -1 ? NULL : & si->si_data [start];
}


/*
 * Finish with a string input stream.
 */

#if	USE_PROTO
LOCAL void si_close (input_t * input)
#else
LOCAL void
si_close (input)
input_t	      *	input;
#endif
{
	free (input);
}


/*
 * Display an error locus in a string input stream.
 */

#if	USE_PROTO
LOCAL void si_error (input_t * input)
#else
LOCAL void
si_error (input)
input_t	      *	input;
#endif
{
	strin_t       *	si = (strin_t *) input;
	int		col;

	col = si->si_pos < 40 ? 0 : si->si_pos - 40;

	(void) fprintf (stderr, "Error vicinity in input string : %.*s <---\n",
			(int) si->si_pos - col, & si->si_data [col]);
}


/*
 * The filtering concept does not really apply to strings yet.
 */

#if	USE_PROTO
LOCAL int si_filter (input_t * NOTUSED (input), CONST char * NOTUSED (format),
		     ...)
#else
LOCAL int
si_filter (input, format)
input_t	      *	input;
CONST char    *	format;
#endif
{
	return -1;
}


/*
 * Create a source of input from a string. Depending on the "action" code,
 * the string may or may not be copied or left alone. If the user wants some
 * clean processing to be done at close time, then another version of this
 * code should be set in place which allows registration of a close-time
 * callback (which is messy, so it hasn't been done).
 */

#if	USE_PROTO
input_t * (make_string_input) (CONST unsigned char * str, int copy)
#else
input_t *
make_string_input ARGS ((str, copy))
CONST unsigned char
	      *	str;
int		copy;
#endif
{
	strin_t	      *	si;
	size_t		len;

	/* ASSERT that si_input is the first member of the structure */

	if (offsetof (strin_t, si_input) != 0)
		return NULL;

	len = strlen ((char *) str);

	if ((si = (strin_t *) malloc (sizeof (* si) +
				      (copy ? len : 0))) == NULL)
		return NULL;

	si->si_pos = 0;
	si->si_len = len;

	if (copy) {

		si->si_data = (CONST unsigned char *) (si + 1);
		memcpy (si + 1, str, len);
	} else
		si->si_data = str;

	si->si_input.in_read = si_read;
	si->si_input.in_unread = si_unread;
	si->si_input.in_readtok = si_readtok;
	si->si_input.in_close = si_close;
	si->si_input.in_error = si_error;
	si->si_input.in_filter = si_filter;

	return & si->si_input;
}
