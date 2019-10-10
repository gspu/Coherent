#ifndef	SHELLIO_H
#define	SHELLIO_H	1

#include <sys/compat.h>
#include <stddef.h>

/*
 * Routines for managing shell output.
 *
 * In the shell, there are four logical output streams, all oriented around
 * sending lines of text to an intreractive user. They are:
 *	normal shell output (which goes to the standard error stream)
 *	shell error reports (which go to the standard error stream, but
 *		which may also be logged and should be very unambiguous about
 *		the source of the report)
 *	builtin output (which goes to the standard output stream)
 *	builtin error (which goes to the standard error stream)
 *
 * If the shell was to use the ISO Standard C Library, then there would be
 * numerous problematic interactions between the shell redirection facilities
 * and the standard I/O library, especially in terms of buffering. The
 * Standard library does not export sufficient features to deal with this
 * problem. Since shell output is line-oriented, we can get by instead with a
 * simpler output manager that does no (or at most, line) buffering.
 *
 * There are no functions in this package that are varadic in the sense of the
 * Standard Library printf () function; instead, consider this package to be
 * in the style of C++ iostreams. The use of the _endl suffix is intended to
 * suggest this.
 */

EXTERN_C_BEGIN

unsigned char *	ltoa			PROTO ((unsigned char * _dest,
						size_t _len, long _num,
						int _radix, int _precision));

unsigned char *	itoa			PROTO ((unsigned char * _dest,
						size_t _len, int _num,
						int _radix, int _precision));

void		shell_outstr		PROTO ((CONST unsigned char * _str));
void		shell_outchar		PROTO ((unsigned char _ch));
void		shell_outint		PROTO ((int _num));
void		shell_outlong		PROTO ((long _num));
void		shell_endl		PROTO ((void));

int		shellerr_begin		PROTO ((CONST unsigned char * _locus));
void		shellerr_outstr		PROTO ((CONST unsigned char * _str));
void		shellerr_outchar	PROTO ((unsigned char _ch));
void		shellerr_outint		PROTO ((int _num));
void		shellerr_outlong	PROTO ((long _num));
void		shellerr_endl		PROTO ((void));
void		shellerr_simple		PROTO ((CONST unsigned char * _locus,
						CONST unsigned char * _str));

void		cmd_outstr		PROTO ((CONST unsigned char * _str));
void		cmd_outchar		PROTO ((unsigned char _ch));
void		cmd_outint		PROTO ((int _num));
void		cmd_outlong		PROTO ((long _num));
void		cmd_endl		PROTO ((void));

int		cmderr_begin		PROTO ((CONST unsigned char * _cmd));
void		cmderr_outstr		PROTO ((CONST unsigned char * _str));
void		cmderr_outchar		PROTO ((unsigned char _ch));
void		cmderr_outint		PROTO ((int _num));
void		cmderr_outlong		PROTO ((long _num));
void		cmderr_endl		PROTO ((void));

EXTERN_C_END

#endif	/* ! defined (SHELLIO_H) */

