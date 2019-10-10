/*
 * Coherent.
 * Print formatted.
 *
 * $Log:	printf.c,v $
 * Revision 1.1  92/07/17  15:18:10  bin
 * Initial revision
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
