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

#include <sys/compat.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

#include "input.h"
#include "mkinput.h"


/*
 * Structures for dealing with input from a file.
 */

typedef struct fileinput filein_t;

struct fileinput {
	input_t		fi_input;

	char	      *	fi_name;
	FILE	      *	fi_file;	/* input stream */
	int		fi_lineno;	/* line number in file */
	int		fi_colno;	/* current column number */
	int		fi_prevline;
	int		fi_prevcol;
	int		fi_comment;	/* comment character (maybe none) */
	int		fi_prevchar;	/* previous character */

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
 * Read a character from a file.
 */

#ifdef	USE_PROTO
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

	if ((ch = getc (fi->fi_file)) == EOF)
		return IN_EOF;

	fi->fi_line [fi->fi_colno ++ % ARRAY_LEN (fi->fi_line)] = ch;

	if (ch == fi->fi_comment) {
		/*
		 * Consume data to EOL, then return EOL.
		 */

		do {
			if ((ch = getc (fi->fi_file)) == EOF)
				return IN_EOF;
		} while (ch != '\n');
	}


	if (ch == '\n') {

		fi->fi_colno = 0;
		fi->fi_lineno ++;
	}

	fi->fi_prevchar = ch;
	return ch;
}


/*
 * Return the last read character to the input stream.
 */

#ifdef	USE_PROTO
LOCAL void fi_unread (input_t * input)
#else
LOCAL void
fi_unread (input)
input_t	      *	input;
#endif
{
	filein_t      *	fi = (filein_t *) input;

	if (fi->fi_prevchar != IN_EOF) {

		ungetc (fi->fi_prevchar, fi->fi_file);

		fi->fi_prevchar = IN_EOF;
		fi->fi_colno = fi->fi_prevcol;
		fi->fi_lineno = fi->fi_prevline;
	}
}


/*
 * Close the input file.
 */

#ifdef	USE_PROTO
LOCAL void fi_close (input_t * input)
#else
LOCAL void
fi_close (input)
input_t	      *	input;
#endif
{
	filein_t      *	fi = (filein_t *) input;

	(void) fclose (fi->fi_file);
	free (fi);
}


/*
 * Produce an error report for this input source.
 */

#ifdef	USE_PROTO
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
 * Create a source of input from a file.
 */

#ifdef	USE_PROTO
input_t * (make_file_input) (FILE * file, CONST char * name, int comment)
#else
input_t *
make_file_input ARGS ((file, name, comment))
FILE	      *	file;
CONST char    *	name;
int		comment;
#endif
{
	filein_t      *	fi;
	size_t		namelen;

	/* ASSERT that fi_input is the first member of the structure */

	if (offsetof (filein_t, fi_input) != 0)
		return NULL;

	if (name == NULL)
		name = "no name";

	namelen = strlen (name) + 1;

	if ((fi = (filein_t *) malloc (sizeof (* fi) + namelen)) == NULL)
		return NULL;

	fi->fi_lineno = 1;
	fi->fi_colno = 0;
	fi->fi_file = file;
	fi->fi_name = (char *) (fi + 1);

	fi->fi_comment = comment;
	fi->fi_prevchar = IN_EOF;

	if (namelen > 0)
		memcpy (fi->fi_name, name, namelen);

	fi->fi_input.in_read = fi_read;
	fi->fi_input.in_unread = fi_unread;
	fi->fi_input.in_readtok = NULL;
	fi->fi_input.in_close = fi_close;
	fi->fi_input.in_error = fi_error;

	return & fi->fi_input;
}


/*
 * Return a character from an input string, or EOF when the end-of-string is
 * reached.
 */

#ifdef	USE_PROTO
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

#ifdef	USE_PROTO
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

#ifdef	USE_PROTO
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

#ifdef	USE_PROTO
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

#ifdef	USE_PROTO
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

	(void) fprintf (stderr, "Error vicinity : %.*s <---\n",
			si->si_pos - col, & si->si_data [col]);
}


/*
 * Create a source of input from a string. Depending on the "action" code,
 * the string may or may not be copied or left alone. If the user wants some
 * clean processing to be done at close time, then another version of this
 * code should be set in place which allows registration of a close-time
 * callback (which is messy, so it hasn't been done).
 */

#ifdef	USE_PROTO
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

	return & si->si_input;
}

