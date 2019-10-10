#include <sys/compat.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>


/*
 * Try and convert an argument into a number for the numberic conversions.
 * If the conversion is not completely successful, create a disgnostic message
 * string.
 */

#if	USE_PROTO
LOCAL void scan_number (CONST char * str, long * number, CONST char ** diag)
#else
LOCAL void
scan_number (str, number, diag)
CONST char    *	str;
long	      *	number;
CONST char   **	diag;
#endif
{
	* number = 0;
	* diag = NULL;

	if (str == NULL)
		return;

	if (* str == '\'' || * str == '"') {
		* number = str [1];
		return;
	}

	errno = 0;
	* number = strtol (str, diag, 0);
	if (errno == ERANGE)
		* number = strtoul (str, diag, 0);
	if (errno == ERANGE) {
		* diag = "arithmetic overflow";
		return;
	}

	if (* diag != NULL && ** diag != 0) {
		* diag = * number == 0 ? "expected numeric value" :
					 "not completely converted";
	} else
		* diag = NULL;
}


/*
 * Return a newly-allocated string corresponding to the input string with
 * backslash-escapes converted into their corresponding characters.
 */

#if	USE_PROTO
LOCAL char * process_escapes (CONST char * src, int * cflag)
#else
LOCAL char *
process_escapes (src, cflag)
CONST char    *	src;
int	      *	cflag;
#endif
{
	char	      *	temp;
	char	      *	retval;

	if ((retval = malloc (strlen (src) + 1)) == NULL) {
		fprintf (stderr,
			 "printf: cannot allocate memory for argument \"%s\"\n",
			 temp);
		return NULL;
	}

	temp = retval;

	while ((* temp ++ = * src ++) != 0) {
		int		num;

		if (temp [- 1] != '\\')
			continue;

		switch (* src ++) {
		case '\\': temp [- 1] = '\\'; continue;
		case 'a':  temp [- 1] = '\a'; continue;
		case 'b':  temp [- 1] = '\b'; continue;
		case 'f':  temp [- 1] = '\f'; continue;
		case 'n':  temp [- 1] = '\n'; continue;
		case 'r':  temp [- 1] = '\r'; continue;
		case 't':  temp [- 1] = '\t'; continue;
		case 'v':  temp [- 1] = '\v'; continue;

		case 'c':
			temp [- 1] = 0;
			* cflag = 1;
			return retval;

		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7':
			src --;
			/* FALL THROUGH */

		case '0':
			num = 0;

			if (* src >= '0' && * src < '8') {
				num = * src ++ - '0';

				if (* src >= '0' && * src < '8') {
					num = num * 8 + * src ++ - '0';

					if (* src >= '0' && * src < '8')
						num = num * 8 + * src ++ - '0';
				}
			}

			temp [- 1] = num;
			continue;
		}
	}
}


/*
 * Flags for printf conversion specifications.
 */

#define	SIGN		1
#define	LEFT_JUSTIFY	2
#define	ALTERNATE	4
#define	ZERO_FILL	8
#define	SPACE		16
#define	PRECISION	32

#if	USE_PROTO
int main (int argc, char ** argv)
#else
int
main (argc, argv)
int		argc;
char	     **	argv;
#endif
{
	CONST char    *	work;
	CONST char    *	format;
	char	     **	oldargv;
	char		ch;

	if (argc < 1) {
		fprintf (stderr, "printf: missing format argument\n");
		return -1;
	}

	work = format = argv [1];
	argv += 2;

	oldargv = argv;

	while (* argv != NULL || * work != 0) {
		long		number;
		int		width;
		int		precision;
		int		flags;
		char		buf [sizeof (unsigned long) * 3 + 1];
		char	      *	bufptr;
		char	      *	number_message;
		char	      *	radix_string;
		char		sign_char;
		unsigned	radix;

		if ((ch = * work ++) == 0) {
			/*
			 * We need to process the format argument again.
			 */

			if (oldargv == argv) {
				/*
				 * No conversions were specified, but there
				 * are arguments; bail out.
				 */

				break;
			}
			work = format;
			continue;
		}

		if (ch == '\\') {
			switch (ch = * work ++) {
			case '0':	case '1':
			case '2':	case '3':
			case '4':	case '5':
			case '6':	case '7':
				break;

			case '\\': ch = '\\'; goto printit;
			case 'a':  ch = '\a'; goto printit;
			case 'b':  ch = '\b'; goto printit;
			case 'f':  ch = '\f'; goto printit;
			case 'n':  ch = '\n'; goto printit;
			case 'r':  ch = '\r'; goto printit;
			case 't':  ch = '\t'; goto printit;
			case 'v':  ch = '\v'; goto printit;
printit:
			default:
				putchar (ch);
				continue;
			}

			/*
			 * Octal escape.
			 */

			number = ch - '0';
			if ((ch = * work) >= '0' && ch < '8') {
				number = number * 8 + ch - '0';
				work ++;
				if ((ch = * work) >= '0' && ch < '8') {
					work ++;
					number = number * 8 + ch - '0';
				}
			}

			putchar (number);
			continue;
		}

		if (ch != '%') {
			putchar (ch);
			continue;
		}

		flags = 0;

		for (;;) {
			switch (ch = * work ++) {
			case '+':
				flags |= SIGN;
				continue;

			case '-':
				flags |= LEFT_JUSTIFY;
				continue;

			case '#':
				flags |= ALTERNATE;
				continue;

			case '0':
				flags |= ZERO_FILL;
				continue;

			case ' ':
				flags |= SPACE;
				continue;

			default:
				break;
			}
			break;
		}

		/*
		 * Try and convert a parameter into a number, just in case.
		 */

		scan_number (* argv, & number, & number_message);

		width = 0;
		if (ch == '*') {
			width = number;
			ch = * work ++;

			if (* argv != NULL)
				argv ++;
			scan_number (* argv, & number, & number_message);
		} else {
			while (isdigit (ch)) {
				width = width * 10 + ch - '0';
				ch = * work ++;
			}
		}

		precision = 0;
		if (ch == '.') {
			flags |= PRECISION;

			if ((ch = * work ++) == '*') {
				precision = number;
				ch = * work ++;

				if (* argv != NULL)
					argv ++;
				scan_number (* argv, & number,
					     & number_message);
			} else {
				while (isdigit (ch)) {
					precision = precision * 10 + ch - '0';
					ch = * work ++;
				}
			}
		}

		sign_char = 0;
		bufptr = NULL;
		radix_string = "0123456789ABCDEF";

		switch (ch) {

		case 'd':
		case 'i':
			if (number < 0) {
				sign_char = '-';
				number = - number;
			} else if ((flags & SIGN) != 0)
				sign_char = '+';
			else if ((flags & SPACE) != 0)
				sign_char = ' ';
			/* FALL THROUGH */

		case 'u':
			radix = 10;
			break;

		case 'o':
			radix = 8;
			break;

		case 'x':
			radix_string = "0123456789abcdef";
			/* FALL THROUGH */

		case 'X':
			radix = 16;
			break;

		case 's':
			if (* argv == NULL)
				bufptr = "";
			else
				bufptr = * argv ++;
			break;

		case 'c':
			if (* argv == NULL)
				bufptr = "";
			else {
				bufptr = * argv ++;
				precision = 1;
				flags |= PRECISION;
			}
			break;

		case 'b':
			radix = 0;
			if (* argv == NULL ||
			    (bufptr = process_escapes (* argv,
						       & radix)) == NULL) {
				bufptr = "";
				radix_string = NULL;
			} else
				radix_string = bufptr;
			break;

		case '%':
			putchar (ch);
			continue;
		}

		if (bufptr == NULL) {
			unsigned long	temp;

			/*
			 * Numeric conversion to string.
			 */

			if (number_message != NULL)
				fprintf (stderr, "printf: \"%s\" %s\n",
					 * argv, number_message);
			if (* argv != NULL)
				argv ++;

			bufptr = buf + sizeof (buf);
			* -- bufptr = 0;

			if ((flags & PRECISION) == 0)
				precision = 1;
			else
				flags &= ~ ZERO_FILL;

			if ((flags & ALTERNATE) == 0 && ch == 'u') {
				unsigned	newprec = 1;

				/*
				 * Increase the precision so at least one
				 * leading zero is printed.
				 */

				temp = number;
				while (temp != 0) {
					newprec ++;
					temp /= radix;
				}

				if (precision < newprec)
					precision = newprec;
			}

			temp = number;
			while (precision > 0 || temp != 0) {
				* -- bufptr = radix_string [temp % radix];
				temp /= radix;
				precision --;
				width --;
			}
			if (sign_char != 0)
				width --;

			if ((flags & ALTERNATE) != 0 && number != 0 &&
			    (ch == 'x' || ch == 'X'))
				width -= 2;

			precision = strlen (bufptr);
		} else {
			if ((flags & PRECISION) == 0)
				precision = INT_MAX;

			/*
			 * From here on, the precision is the maximum number of
			 * output characters.
			 */

			if (precision > strlen (bufptr))
				precision = strlen (bufptr);

			width -= precision;
		}

		/*
		 * Now we know what we are going to be printing, deal with
		 * leading padding.
		 */

		if ((flags & (LEFT_JUSTIFY | ZERO_FILL)) == 0)
			while (width > 0) {
				putchar (' ');
				width --;
			}

		if (sign_char)
			putchar (sign_char);

		if ((flags & ALTERNATE) != 0 && number != 0 &&
		    (ch == 'x' || ch == 'X')) {
			putchar ('0');
			putchar ('x');
		}

		if ((flags & LEFT_JUSTIFY) == 0)
			while (width > 0) {
				putchar ('0');
				width --;
			}

		while (precision > 0) {
			putchar (* bufptr ++);
			precision --;
		}

		while (width > 0) {
			putchar (' ');
			width --;
		}

		if (ch == 'b') {
			free (radix_string);
			if (radix)
				break;
		}
	}

	return 0;
}


