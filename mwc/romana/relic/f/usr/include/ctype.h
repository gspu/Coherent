/* (-lgl
 * 	COHERENT Version 4.1.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * ctype.h
 * C character handling library header.
 * Draft Proposed ANSI C Standard, Section 4.3, 5/13/88 draft.
 * Implemented by table lookup.
 */

#ifndef	__CTYPE_H__
#define	__CTYPE_H__

#include <common/feature.h>
#include <common/ccompat.h>

__EXTERN_C_BEGIN__

int		isalnum		__PROTO ((int _c));
int		isalpha		__PROTO ((int _c));
int		iscntrl		__PROTO ((int _c));
int		isdigit		__PROTO	((int _c));
int		isgraph		__PROTO ((int _c));
int		islower		__PROTO ((int _c));
int		isprint		__PROTO ((int _c));
int		ispunct		__PROTO ((int _c));
int		isspace		__PROTO ((int _c));
int		isupper		__PROTO ((int _c));
int		isxdigit	__PROTO ((int _c));
int		tolower		__PROTO ((int _c));
int		toupper		__PROTO ((int _c));

#if	! _STDC_SOURCE && ! _POSIX_SOURCE

int		isascii		__PROTO ((int _c));
int		toascii		__PROTO ((int _c));
int		_tolower	__PROTO ((int _c));
int		_toupper	__PROTO ((int _c));

#endif	/* ! _STDC_SOURCE && ! _POSIX_SOURCE */

__EXTERN_C_END__

/*
 * Type table and bit classifications.
 * Table indices: 0==EOF, 1==NUL, ..., 128==DEL, 129==0x80, ..., 256==0xFF.
 * Indices 0 and 129 to 256 are nonASCII characters.
 */
#define	_CTYPEN	257			/* Table size			*/
extern	unsigned char _ctype[_CTYPEN];	/* Type table			*/
#define	_U	0x01			/* Upper case alphabetic	*/
#define	_L	0x02			/* Lower case alphabetic	*/
#define	_A	(_U|_L)			/* Alphabetic			*/
#define	_N	0x04			/* Numeric			*/
#define	_S	0x08			/* White space character	*/
#define	_P	0x10			/* Punctuation character	*/
#define	_C	0x20			/* Control character		*/
#define	_B	0x40			/* Printable but nothing else	*/
#define	_X	0x80			/* Hexadecimal digit		*/

/* Macros covering ANSI Standard functions. */
#define	isalnum(c)	(_ctype[(c)+1]&(_A|_N))
#define	isalpha(c)	(_ctype[(c)+1]&_A)
#define	iscntrl(c)	(_ctype[(c)+1]&_C)
#define	isdigit(c)	(_ctype[(c)+1]&_N)
#define	isgraph(c)	(_ctype[(c)+1]&(_P|_A|_N))
#define	islower(c)	(_ctype[(c)+1]&_L)
#define	isprint(c)	(_ctype[(c)+1]&(_P|_B|_A|_N))
#define	ispunct(c)	(_ctype[(c)+1]&_P)
#define	isspace(c)	(_ctype[(c)+1]&_S)
#define	isupper(c)	(_ctype[(c)+1]&_U)
#define	isxdigit(c)	(_ctype[(c)+1]&_X)

#if	! _STDC_SOURCE && ! _POSIX_SOURCE

#define	isascii(c)	(((c)&~0x7F)==0)
#define	toascii(c)	((c)&0x7F)
#define	_tolower(c)	((c)|('a'-'A'))
#define	_toupper(c)	((c)&~('a'-'A'))

#endif	/* ! _STDC_SOURCE && ! _POSIX_SOURCE */

#endif	/* ! defined (__CTYPE_H__) */
