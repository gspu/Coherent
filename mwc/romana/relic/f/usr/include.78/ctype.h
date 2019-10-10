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

/* ANSI Standard functions. */
extern	int	isalnum(/* int c */);	/* 4.3.1.1  */
extern	int	isalpha(/* int c */);	/* 4.3.1.2  */
extern	int	iscntrl(/* int c */);	/* 4.3.1.3  */
extern	int	isdigit(/* int c */);	/* 4.3.1.4  */
extern	int	isgraph(/* int c */);	/* 4.3.1.5  */
extern	int	islower(/* int c */);	/* 4.3.1.6  */
extern	int	isprint(/* int c */);	/* 4.3.1.7  */
extern	int	ispunct(/* int c */);	/* 4.3.1.8  */
extern	int	isspace(/* int c */);	/* 4.3.1.9  */
extern	int	isupper(/* int c */);	/* 4.3.1.10 */
extern	int	isxdigit(/* int c */);	/* 4.3.1.11 */
extern	int	tolower(/* int c */);	/* 4.3.2.1  */
extern	int	toupper(/* int c */);	/* 4.3.2.2  */

/* Non-ANSI Standard functions. */
extern	int	isascii(/* int c */);
extern	int	toascii(/* int c */);
extern	int	_tolower(/* int c */);
extern	int	_toupper(/* int c */);

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

/* Macros covering non-ANSI Standard functions. */
#define	isascii(c)	(((c)&~0x7F)==0)
#define	toascii(c)	((c)&0x7F)
#define	_tolower(c)	((c)|('a'-'A'))
#define	_toupper(c)	((c)&~('a'-'A'))

#endif

/* end of ctype.h */
