/*
 * Standard output printf using putchar.
 * Non-portable things:
 * 1) alignment of arguments is assumed to be completely contiguous.
 * 2) the smallest number is assumed to negate to itself.
 * 3) All of long, int, char*, and double are assumed to
 *    be held in an exact number of ints.
 */
#include "fsck.h"
#include <sys/mdata.h>

union	alltypes {
	char	c;
	int	i;
	unsigned u;
	long	l;
	double	d;
	char	*s;
};

#define	bump(p,s)	(p+=sizeof(s)/sizeof(int))

char	*printi();
char	*printl();

static	char	null[] = "{NULL}";

printf(args)
union alltypes args;
{
	xprintf(&args);
}
xprintf(argp)
union alltypes *argp;
{
	register char *cbp;
	int *iap;
	register c;
	char *s;
	char *cbs;
	char adj, pad;
	int prec;
	int fwidth;
	int pwidth;
	register char *fmt;
	union alltypes elem;
	char cbuf[64];

	iap = (int *)argp;
	fmt = *(char **)iap;
	bump(iap, char*);
	for (;;) {
		while((c = *fmt++) != '%') {
			if(c == '\0') {
				return;
			}
			putchar(c);
		}
		pad = ' ';
		fwidth = -1;
		prec = -1;
		c = *fmt++;
		if (c == '-') {
			adj = 1;
			c = *fmt++;
		} else
			adj = 0;
		if (c == '0') {
			pad = '0';
			c = *fmt++;
		}
		if (c == '*') {
			fwidth = *iap++;
			c = *fmt++;
		} else
			for (fwidth = 0; c>='0' && c<='9'; c = *fmt++)
				fwidth = fwidth*10 + c-'0';
		if (c == '.') {
			c = *fmt++;
			if (c == '*') {
				prec = *iap++;
				c = *fmt++;
			} else
				for (prec=0; c>='0' && c<='9'; c=*fmt++)
					prec = prec*10 + c-'0';
		}
		if (c == 'l') {
			c = *fmt++;
			if (c=='d' || c=='o' || c=='u' || c=='x')
				c += 'A' - 'a';
		}
		cbp = cbs = cbuf;
		switch (c) {

		case 'd':
			elem.i = *iap++;
			if (elem.i < 0) {
				elem.i = -elem.i;
				*cbp++ = '-';
			}
			cbp = printi(cbp, elem.i, 10);
			break;

		case 'u':
			cbp = printi(cbp, *iap++, 10);
			break;
	
		case 'o':
			cbp = printi(cbp, *iap++, 8);
			break;

		case 'x':
			cbp = printi(cbp, *iap++, 16);
			break;

		case 'D':
			elem.l = *(long *)iap;
			bump(iap, long);
			if (elem.l < 0) {
				elem.l = -elem.l;
				*cbp++ = '-';
			}
			cbp = printl(cbp, elem.l, 10);
			break;

		case 'U':
			cbp = printl(cbp, *(long *)iap, 10);
			bump(iap, long);
			break;

		case 'O':
			cbp = printl(cbp, *(long *)iap, 8);
			bump(iap, long);
			break;

		case 'X':
			cbp = printl(cbp, *(long *)iap, 16);
			bump(iap, long);
			break;


		case 's':
			if ((s = *(char **)iap) == NULL)
				s = null;
			bump(iap, char*);
			/*
			 * Do %s specially so it can be longer.
			 */
			cbp = cbs = s;
			while (*cbp++ != '\0')
				if (prec>=0 && cbp-s>prec)
					break;
			cbp--;
			break;
	
		case 'c':
			elem.c = *iap++;
			*cbp++ = elem.c;
			break;
	
		case 'r':
			xprintf(*(char ***)iap);
			bump(iap, char**);
			break;
	
		default:
			putchar(c);
			continue;
		}
		if ((pwidth = fwidth + cbs-cbp) < 0)
			pwidth = 0;
		if (!adj)
			while (pwidth-- != 0)
				putchar(pad);
		while (cbs < cbp)
			putchar(*cbs++);
		if (adj)
			while (pwidth-- != 0)
				putchar(pad);
	}
}

static	char	digits[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F'
};

/*
 * Print an unsigned integer in base b.
 */
static
char *
printi(cp, n, b)
char *cp;
register unsigned n;
{
	register a;
	register char *ep;
	char pbuf[10];

	ep = &pbuf[10];
	*--ep = 0;
	for ( ; a = n/b; n=a)
		*--ep = digits[n%b];
	*--ep = digits[n];
	while (*ep)
		*cp++ = *ep++;
	return (cp);
}

/*
 * Print an unsigned long in base b.
 */
static
char *
printl(cp, n, b)
register char *cp;
unsigned long n;
register b;
{
	char pbuf[13];
	unsigned long a;
	register char *ep;

	ep = &pbuf[13];
	*--ep = '\0';
	for ( ; (a = n/b) != 0; n = a)
		*--ep = digits[n%b];
	*--ep = digits[n];
	while (*ep)
		*cp++ = *ep++;
	return (cp);
}
