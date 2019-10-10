/* $Header: /src386/kernel/coh.386/RCS/printf.c,v 1.6 93/04/16 06:49:16 bin Exp Locker: bin $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Coherent.
 * Print formatted.
 *
 * $Log:	printf.c,v $
 * Revision 1.6  93/04/16  06:49:16  bin
 * Hal: kernel 76 update
 * 
 * Revision 1.2  92/01/06  12:00:01  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:14:13	src
 * Initial revision
 * 
 * 87/09/20	Allan Cornish		/usr/src/sys/coh/printf.c
 * %U now correctly displays in base 10 rather than base 16.
 *
 * 86/12/16	Allan Cornish		/usr/src/sys/coh/printf.c
 * Added '%D' and '%X options to printf().
 */
#include <sys/coherent.h>

/*
 * For indirecting and incrementing argument pointer.
 */
#define ind(p, t)	(*((t *) p))
#define inc(t1, t2)	((sizeof(t2 *)+sizeof(t1)-1) / sizeof(t1))

/*
 * Table for printing out digits.
 */
char digtab[] ={
	'0',	'1',	'2',	'3',	'4',	'5',	'6',	'7',
	'8',	'9',	'A',	'B',	'C',	'D',	'E',	'F'
};

/*
 * A simple printf.
 */
printf(fp, a1)
register char *fp;
{
	char * cp;
	register int c;
	register unsigned *ap;
	int lflag;

	ap = (char *)&a1;
	for (;;) {
		while ((c=*fp++) != '%') {
			if (c == '\0')
				return;
			putchar(c);
		}

		lflag = 0;
		if ( *fp == 'l' ) {
			lflag = 1;
			fp++;
		}

		switch ( c = *fp++ ) {

		case 'c':
			putchar(*ap++);
			continue;

		case 'd':
			if ( lflag == 0 ) {
				if ( ((int)(*ap)) < 0 ) {
					putchar('-');
					printn( -((long) ((int)(*ap))), 10 );
				}
				else
					printn( ((long)(*ap)), 10 );
				ap++;
				continue;
			}
			/* fall through */
		case 'D':
			if ( *((long *)(ap)) < 0 ) {
				putchar('-');
				printn( - *((long *)(ap)), 10 );
			}
			else
				printn(   *((long *)(ap)), 10 );

			((long *)(ap))++;
			continue;

		case 'o':
			if ( lflag == 0 ) {
				printn( ((long)(*ap)), 8);
				ap++;
				continue;
			}
			/* fall through */
		case 'O':
			printf( *((long *)(ap)), 8 );
			((long *)(ap))++;
			continue;

		case 'r':
			ap = *((int **) ap);
			fp = ind(ap, char *);
			ap += inc(int, char *);
			continue;

		case 's':
			cp = ind(ap, char *);
			ap += inc(int, char *);
			while ((c=*cp++) != '\0')
				putchar(c);
			continue;

		case 'x':
			if ( lflag == 0 ) {
				printn( ((long)(*ap)), 16 );
				ap++;
				continue;
			}
			/* fall through */
		case 'X':
			printn( *((long *)(ap)), 16 );
			((long *)(ap))++;
			continue;

		case 'u':
			if ( lflag == 0 ) {
				printn( ((long)(*ap)), 10);
				ap++;
				continue;
			}
			/* fall through */
		case 'U':
			printn( *((long *)(ap)), 10 );
			((long *)(ap))++;
			continue;

		case 'p':
			if (sizeof(char *) > sizeof(int)) {
				printn( ((long)(*ap)), 16);
				putchar(':');
				ap++;
			}
			printn( ((long)(*ap)), 16);
			ap++;
			continue;

		default:
			putchar(c);
			continue;
		}
	}
}

/*
 * Print out the unsigned long `v' in the base `b'.
 */
printn( v, b )
unsigned long v;
{
	unsigned long n;

	if ((n=v/b) != 0)
		printn(n, b);

	putchar(digtab[v%b]);
}
