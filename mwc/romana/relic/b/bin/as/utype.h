/*
 * utype.h
 * C character handling library header.
 * Modified for 80386 assembler. _ and . are upper and lower case.
 * Implemented by table lookup.
 */

/*
 * Type table and bit classifications.
 * Table indices: 0==EOF, 1==NUL, ..., 128==DEL, 129==0x80, ..., 256==0xFF.
 * Indices 0 and 129 to 256 are nonASCII characters.
 */
#define	_CTYPEN	257			/* Table size			*/
extern	unsigned char _utype[_CTYPEN];	/* Type table			*/
#define	_U	0x01			/* Upper case alphabetic	*/
#define	_L	0x02			/* Lower case alphabetic	*/
#define	_A	(_U|_L)			/* Alphabetic			*/
#define	_H	0x04			/* Hexadecimal digit		*/
#define	_D	0x08			/* Digit			*/
#define	_S	0x10			/* White space character	*/
#define	_P	0x20			/* Punctuation character	*/
#define	_C	0x40			/* Control character		*/
#define	_X	0x80			/* Printable but nothing else	*/

/*
 * Macros covering standard functions.
 */
#define	isalnum(c)	(_utype[(c)+1]&(_A|_D))
#define	isalpha(c)	(_utype[(c)+1]&_A)
#define	iscntrl(c)	(_utype[(c)+1]&_C)
#define	isdigit(c)	(_utype[(c)+1]&_D)
#define	isgraph(c)	(_utype[(c)+1]&(_P|_A|_D))
#define	islower(c)	(_utype[(c)+1]&_L)
#define	isprint(c)	(_utype[(c)+1]&(_P|_X|_A|_D))
#define	ispunct(c)	(_utype[(c)+1]&_P)
#define	isspace(c)	(_utype[(c)+1]&_S)
#define	isupper(c)	(_utype[(c)+1]&_U)
#define	isxdigit(c)	(_utype[(c)+1]&_H)
#define	tolower(c)	(*_tolowerp)(c)
#define	toupper(c)	(*_toupperp)(c)

/*
 * Macros for non-ANSI functions.
 */
#define	isascii(c)	(((c)&0x80)==0)
#define	toascii(c)	((c)&0x7F)
#define	_tolower(c)	((c)|('a'-'A'))
#define	_toupper(c)	((c)&~('a'-'A'))

