#if KLAATU
#define SERIAL_CONSOLE	1
#define INS8250 0x3f8	/* klaatu */
#endif

#if GORT
#define SERIAL_CONSOLE	1
#define	INS8250	0x290	/* gort */
#endif

#ifndef INS8250
#define INS8250 0x3f8
#endif

/*
 * die.c -- Get information out from a very young kernel which probably
 * can't do printf()'s.
 */

#include <sys/coherent.h>
#include <sys/mmu.h>
#include <ctype.h>

#define COLOR	((char *) (0x0000B0000 + ctob(SBASE-PBASE)))
#define MONO	((char *) (0x0000B8000 + ctob(SBASE-PBASE)))
#define PAGING	(0x80000000)	/* Paging bit in cr0, set if paging is on.  */
static int chirp_off;

/*
 * void _chirp(char c, off);
 * Put character 'c' directly in video memory at offset 'off';
 *
 * This routine must not use any static variables, since the .data
 * segment is not necessarily available yet.
 */
void
_chirp(c, off)
char c;
int off;
{
#if SERIAL_CONSOLE
	__putchar(c);
#else
	if (!paging()) {
		*(COLOR + off) = c;
		*(MONO + off) = c;
	} else {
		*((char *) (ctob(VIDEOa) + off)) = c;
		*((char *) (ctob(VIDEOb) + off)) = c;
	}
#endif
} /* _chirp() */

/*
 * void chirp(char c);
 * Put character 'c' directly in the first character of video memory;
 *
 * This routine must not use any static variables, since the .data
 * segment is not necessarily available yet.
 */
void
chirp(c)
char c;
{
	_chirp(c, 158);
} /* chirp() */

/*
 * void strchirp(char *str);
 * Put string 'str' directly in the next character of video memory;
 * Note that calls to chirp and dchirp do not effect what mchirp considers
 *      to be the next character.
 *
 * This routine uses a ds variable, so it must not be used until the .data
 * segment is available (this currently happens in the middle of mchinit).
 */
void
strchirp(str)
char *str;
{
	char c;
	
	while (c = *str++) {
		_chirp(c, chirp_off);
		chirp_off += 2;
	}
} /* strchirp() */

/*
 * void mchirp(char c);
 * Put character 'c' directly in the next character of video memory;
 * If c == 0 reset the "next" character to be the first character.
 * Note that calls to chirp and dchirp do not effect what mchirp considers
 *      to be the next character.
 *
 * This routine uses a static variable, so it must not be used until the .data
 * segment is available (this currently happens in the middle of mchinit).
 */
void
mchirp(c)
char c;
{
	if ('\0' != c) {
		_chirp(c, chirp_off);
		chirp_off += 2;
	}
	else
		chirp_off = 0;
} /* mchirp() */

/*
 * void dchirp(char c, charpos);
 * Put character 'c' directly in the 'charpos' character of video memory;
 *
 * This routine must not use any static variables, since the .data
 * segment is not necessarily available yet.
 */
void
dchirp(c, charpos)
	char c;
{
	_chirp(c, charpos<<1);
} /* dchirp() */

/*
 * void die(char c);
 * Put character 'c' directly in video memory, and then halt.
 */
void
die(c)
	char c;
{
	_chirp(c, 0);
	for (;;) {
		halt();
	}
}

#define LINESIZE 80

/*
 * puts() -- put a NUL terminated string.
 * Takes one argument--a pointer to a NUL terminated character string.
 */
void
puts(s)
	register char *s;
{
	while (*s != '\0') {
		mchirp(*s++);
	}

} /* puts() */


#define BS '\010'
#define DEL '\0'	/* This is really what getchar() returns!  */
#define NAK '\025'

#define BITS_PER_INT16		16	/* Number of bits in an int16.  */
#define DIGITS_PER_INT16	4	/* Maximum hex digits in a 16 bit number.  */
#define DIGITS_PER_INT8		2	/* Maximum hex digits in an 8 bit number.  */

/*
 * Print a 32/16/8 bit integers in hexadecimal.
 */
void print32(num)	{hexprint(num,8);}
void print16(num)	{hexprint(num,4);}
void print8(num)	{hexprint(num,2);}

#if SERIAL_CONSOLE
#define OUTCH(c)	__putchar(c)
#else
#define OUTCH(c)	mchirp(c)
#endif

hexprint(n, hexdigits)
int n, hexdigits;
{
	int shift;
	char digit, outch;

	for (shift = 4*(hexdigits-1); shift >= 0; shift -= 4) {
		digit = (n >> shift) & 0xF;
		if (digit > 9)
			outch = 'A'+digit-10;
		else
			outch = '0'+digit;
		OUTCH(outch);
	}
	OUTCH(' ');
}

outch(c)
int c;
{
	OUTCH(c);
}

#define	ASCII	1
#define	XONXOFF	1
#define	BAUD	9600

/*
 *	file:	i8251.c
 *
 *	This version of putchar works with the serial lines.
 *	Various configurations are possible through conditional
 *	defines:
 *
 *	BAUD	default 9600
 *		specifies the baudrate, can be as high as 38400
 *
 *	CONSOLE	default COM1:
 *		is the base address of the uart
 *
 *	ASCII	default BINARY
 *		if set, maps '\n' into CR, LF and reduces input
 *		to 7-bit
 *
 *	XONXOFF	default not enabled
 *		allows user to control output
 *
 */

#define	THR	(INS8250+0)
#define	IER	(INS8250+1)
#define	IIR	(INS8250+2)
#define	LCR	(INS8250+3)
#define	MCR	(INS8250+4)
#define	LSR	(INS8250+5)
#define	MSR	(INS8250+6)

#define	RBR	THR
#define	DLL	THR
#define	DLM	IER

#define	DR	0x01
#define	THRE	0x20

__cinit()
{
#if SERIAL_CONSOLE
	register rate;

#if	BAUD
	rate = 115200L / BAUD;
#else
	rate = 1;
#endif
	outb(LCR, 0x00);
	outb(IER, 0x00);
	outb(LCR, 0x80);
	outb(DLL, rate);
	outb(DLM, rate>>8);
	outb(LCR, 0x03);
	outb(MCR, 0x03);
	__putchar('\007');
	__putchar('g');
#endif
}

#define CTLQ	0021
#define CTLS	0023

__getchar()
{
	register c;

	while( (inb(LSR) & DR) == 0 )
		;
	c = inb( RBR );
#if	ASCII
	c &= 0x7F;
#endif
	return( c );
}

__putchar( c )
register c;
{
	register f;

#if	ASCII
	if (c == '\n')
		putchar( '\r' );
#endif
	while( ((f=inb(LSR)) & THRE) == 0 )
		;
#if	XONXOFF
	if( (f & DR) != 0 ) {
		f = inb( RBR ) & 0x7F;
		if (f == CTLS) {
			do {
				while( (inb(LSR) & DR) == 0)
					;
				f = inb( RBR ) & 0x7F;
			} while (f != CTLQ);
		}
	}
#endif
	outb( THR, c );
	return( c );
}
