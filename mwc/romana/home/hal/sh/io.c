#include <sys/compat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "shellio.h"
#include "sh.h"

/*
 * This module deals with character output to the Unix standard error and
 * standard output streams. In a shell, we really do not have the freedom
 * to use the ISO library facilities, since the shell redirection operator
 * causes too many problems with it.
 */

typedef	int		stream_t;


#define	COMMAND_NAME	"sh"
#define	NUM_BUF_SIZE	64


/*
 * Convert a long integer to a string.
 *
 * Note that we don't use ldiv () to construct the string in order to avoid
 * problems with the most negative number.
 */

#if	USE_PROTO
unsigned char * ltoa (unsigned char * dest, size_t len, long num, int radix,
		      int precision)
#else
unsigned char *
ltoa (dest, len, num, radix, precision)
unsigned char *	dest;
size_t		len;
long		num;
int		radix;
int		precision;
#endif
{
static	unsigned char	radixstr [] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned char *	ptr = dest + len;
	unsigned long	temp = num < 0 ? - num : num;

	* -- ptr = 0;

	do {
		if (ptr == dest)
			break;

		* -- ptr = radixstr [temp % radix];
		temp = temp / radix;
	} while (temp != 0 || -- precision > 0);

	if (num < 0 && ptr != dest)
		* -- ptr = '-';

	return ptr;
}


/*
 * Convert an integer to a string.
 */

#if	USE_PROTO
unsigned char * itoa (unsigned char * dest, size_t len, int num, int radix,
		      int precision)
#else
unsigned char *
itoa (dest, len, num, radix, precision)
unsigned char *	dest;
size_t		len;
int		num;
int		radix;
int		precision;
#endif
{
	return ltoa (dest, len, (long) num, radix, precision);
}


/*
 * Forward declarations.
 */

LOCAL	void		io_panic	PROTO ((stream_t _fd));

/*
 * Write a single character to the indicated stream. Ignore EBADF errors, make
 * other errors fatal.
 */

#if	USE_PROTO
LOCAL void write_char (stream_t fd, unsigned char ch)
#else
LOCAL void
write_char (fd, ch)
stream_t	fd;
unsigned char	ch;
#endif
{
	if (write (fd, & ch, 1) == -1) {
#if	defined (EBADF)
		if (errno == EBADF)
			return;
#endif
		io_panic (fd);
	}
}


/*
 * Write a counted buffer to the indicated stream. Ignore EBADF errors and
 * make other errors fatal.
 */

#if	USE_PROTO
LOCAL void write_buffer (stream_t fd, CONST VOID * buf, size_t len)
#else
LOCAL void
write_buffer (fd, buf, len)
stream_t	fd;
CONST VOID    *	buf;
size_t		len;
#endif
{
	if (write (fd, buf, len) == -1) {
#if	defined (EBADF)
		if (errno == EBADF)
			return;
#endif
		io_panic (fd);
	}
}


/*
 * Write a string to the indicated stream. Ignore EBADF errors, make other
 * errors fatal.
 */

#if	USE_PROTO
LOCAL void write_string (stream_t fd, CONST unsigned char * str)
#else
LOCAL void
write_string (fd, str)
stream_t	fd;
CONST unsigned char * str;
#endif
{
	write_buffer (fd, str, strlen (str));
}


/*
 * Write an long to the indicated stream. Ignore EBADF errors, make other
 * errors fatal.
 */

#if	USE_PROTO
LOCAL void write_long (stream_t fd, long num)
#else
LOCAL void
write_long (fd, num)
stream_t	fd;
long		num;
#endif
{
	unsigned char	buf [NUM_BUF_SIZE];

	write_string (fd, ltoa (buf, sizeof (buf), num, 10, 0));
}


/*
 * Deal with some kind of fatal I/O error.
 */

#if	USE_PROTO
LOCAL void io_panic (stream_t fd)
#else
LOCAL void
io_panic (fd)
stream_t	fd;
#endif
{
static	int		in_panic;
	int		saved_error = errno;

	if (in_panic ++ != 0)
		exit (3);

	if (fd == STDOUT_FILENO)
		fd = STDERR_FILENO;
	else if (isatty (fd)) {
	} else if ((fd = open ("/dev/tty", O_WRONLY)) != -1) {
	} else
		exit (3);

	write_string (fd, COMMAND_NAME);
	write_string (fd, ": fatal output error \"");
	write_string (fd, strerror (saved_error));
	write_string (fd, "\"\n");
}


/*
 * error message prologue - print line number and file if
 *	not interactive.
 */

#if	USE_PROTO
LOCAL void error_vicinity (void)
#else
LOCAL void
error_vicinity ()
#endif
{
	if (sesp->s_type == SFILE) {
		shellerr_outstr (sesp->s_strp);
		shellerr_outstr (": shell script ");
		if ((sesp->s_flag & SESSION_EOF) != 0)
			shellerr_outstr ("EOF");
		else {
			shellerr_outstr ("line ");
			shellerr_outint (sesp->s_line);
		}
		shellerr_outstr (": ");
	} else
		shellerr_outstr ("sh: ");
}


#define	SHELL_FILENO	STDERR_FILENO
#define	SHELLERR_FILENO	STDERR_FILENO
#define	CMD_FILENO	STDOUT_FILENO
#define	CMDERR_FILENO	STDERR_FILENO


/*
 * Write data from the shell.
 */

#if	USE_PROTO
void shell_outstr (CONST unsigned char * str)
#else
void
shell_outstr (str)
CONST unsigned char   *	str;
#endif
{
	write_string (SHELL_FILENO, str);
}


#if	USE_PROTO
void shell_outchar (unsigned char ch)
#else
void
shell_outchar (ch)
unsigned char	ch;
#endif
{
	write_char (SHELL_FILENO, ch);
}


#if	USE_PROTO
void shell_outint (int num)
#else
void
shell_outint (num)
int		num;
#endif
{
	write_long (SHELL_FILENO, (long) num);
}


#if	USE_PROTO
void shell_outlong (long num)
#else
void
shell_outlong (num)
long		num;
#endif
{
	write_long (SHELL_FILENO, num);
}


#if	USE_PROTO
void shell_endl (void)
#else
void
shell_endl ()
#endif
{
	write_char (SHELL_FILENO, '\n');
}


/*
 * Write an error report from the shell.
 */

#if	USE_PROTO
int shellerr_begin (CONST unsigned char * locus)
#else
int
shellerr_begin (locus)
CONST unsigned char   *	locus;
#endif
{
	errflag ++;

	if (noeflag)
		return 0;

	error_vicinity ();
	write_string (SHELLERR_FILENO, locus);
	write_string (SHELLERR_FILENO, ": ");

	return 1;
}


#if	USE_PROTO
void shellerr_outstr (CONST unsigned char * str)
#else
void
shellerr_outstr (str)
CONST unsigned char   *	str;
#endif
{
	write_string (SHELLERR_FILENO, str);
}


#if	USE_PROTO
void shellerr_outchar (unsigned char ch)
#else
void
shellerr_outchar (ch)
unsigned char	ch;
#endif
{
	write_char (SHELLERR_FILENO, ch);
}


#if	USE_PROTO
void shellerr_outint (int num)
#else
void
shellerr_outint (num)
int		num;
#endif
{
	write_long (SHELLERR_FILENO, (long) num);
}


#if	USE_PROTO
void shellerr_outlong (long num)
#else
void
shellerr_outlong (num)
long		num;
#endif
{
	write_long (SHELLERR_FILENO, num);
}


#if	USE_PROTO
void shellerr_endl (void)
#else
void
shellerr_endl ()
#endif
{
	write_char (SHELLERR_FILENO, '\n');
}


#if	USE_PROTO
void shellerr_simple (CONST unsigned char * locus, CONST unsigned char * str)
#else
void
shellerr_simple (locus, str)
CONST unsigned char   *	locus;
CONST unsigned char   *	str;
#endif
{
	if (shellerr_begin (locus)) {

		shellerr_outstr (str);
		shellerr_endl ();
	}
}


/*
 * Write data from a command.
 */

#if	USE_PROTO
void cmd_outstr (CONST unsigned char * str)
#else
void
cmd_outstr (str)
CONST unsigned char   *	str;
#endif
{
	write_string (CMD_FILENO, str);
}

#if	USE_PROTO
void cmd_outchar (unsigned char ch)
#else
void
cmd_outchar (ch)
unsigned char	ch;
#endif
{
	write_char (CMD_FILENO, ch);
}

#if	USE_PROTO
void cmd_outint (int num)
#else
void
cmd_outint (num)
int		num;
#endif
{
	write_long (CMD_FILENO, (long) num);
}

#if	USE_PROTO
void cmd_outlong (long num)
#else
void
cmd_outlong (num)
long		num;
#endif
{
	write_long (CMD_FILENO, num);
}

#if	USE_PROTO
void cmd_endl (void)
#else
void
cmd_endl ()
#endif
{
	write_char (CMD_FILENO, '\n');
}



/*
 * Write an error report from a builtin.
 */

#if	USE_PROTO
int cmderr_begin (CONST unsigned char * name)
#else
int
cmderr_begin (name)
CONST unsigned char   *	name;
#endif
{
	if (noeflag)
		return 0;

	error_vicinity ();
	write_string (CMDERR_FILENO, "builtin \"");
	write_string (CMDERR_FILENO, name);
	write_string (CMDERR_FILENO, "\": ");

	return 1;
}

#if	USE_PROTO
void cmderr_outstr (CONST unsigned char * str)
#else
void
cmderr_outstr (str)
CONST unsigned char   *	str;
#endif
{
	write_string (CMDERR_FILENO, str);
}

#if	USE_PROTO
void cmderr_outchar (unsigned char ch)
#else
void
cmderr_outchar (ch)
unsigned char	ch;
#endif
{
	write_char (CMDERR_FILENO, ch);
}

#if	USE_PROTO
void cmderr_outint (int num)
#else
void
cmderr_outint (num)
int		num;
#endif
{
	write_long (CMDERR_FILENO, (long) num);
}

#if	USE_PROTO
void cmderr_outlong (long num)
#else
void
cmderr_outlong (num)
long		num;
#endif
{
	write_long (CMDERR_FILENO, num);
}

#if	USE_PROTO
void cmderr_endl (void)
#else
void
cmderr_endl ()
#endif
{
	write_char (CMDERR_FILENO, '\n');
}
