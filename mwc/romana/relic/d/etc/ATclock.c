/*
 * ATclock.c
 * 5/16/90
 * ATclock - read or set the IBM AT real-time clock
 * Usage:  /etc/ATclock [YY[MM[DD[HH[MM[.SS]]]]]]
 * cc -s -i -O -o ATclock ATclock.c ATclockas.s
 *
 *	Prints the current date and time in format 'YYMMDDHHMM.SS'.
 *	Author: Allan Cornish, INETCO Systems, Nov 1984.
 *	Modifications (c) Copyright INETCO Systems Ltd. (1985)
 *
 *	$Log:	/usr/src.inetco/etc/ATclock.c,v $
 * Revision 1.2	90/05/31  14:07:22 	root
 * steve 5/31/90
 * Added sanity checking for values read from or written to clock;
 * the old source allowed out of range values without complaint.
 * Changed system call output to use stdio, bigger but saner.
 * Changed spacing, rearranged code, added #defines for clarity.
 * 
 *	86/07/16	Allan Cornish
 *	Validation of NVRAM contents is no longer performed.  It checked
 *	fields which were unused, and could incorrectly report errors.
 *
 *	85/09/??	Krish Nair
 *	Modified so that the ATclock may be set.
 *	The "set" format must be 'YYMMDDHHMM.SS'.
 *	The "read" format is always of type 'YYMMDDHHMM.SS'.
 */

#include <stdio.h>

#define	USAGE	"Usage: /etc/ATclock [YY[MM[DD[HH[MM[.SS]]]]]]\n"

/* Offsets within clock memory. */
#define	DIAG	14		/* Diagnostic port: bit(7) -> powerloss */
				/*		    bit(2) -> bad time  */
#define	BADCLK	0x84
#define	STAT	10		/* Status port:     bit(7) -> updating  */
#define	UPDATE	0x80
#define	YEAR	9
#define	MON	8
#define	DAY	7
#define	HOUR	4
#define	MIN	2
#define	SEC	0

/* Forward. */
int	bcd();
void	clock();
void	clockcheck();
void	fatal();
void	sanity();
void	set();
void	usage();
/* ATclockas.s */
void	sphi();
void	splo();
int	zget();
void	zput();

/* Globals. */
unsigned char clkbuf[10];	/* Clock image buffer (ten BCD digits). */

main(argc, argv) int argc; char *argv[];
{
	register char *a, *s;

	if (argc > 2)
		usage();

	if (zget(DIAG) & BADCLK)
		fatal("bad clock");		/* check for valid clock */
	clock(0);				/* read it to clkbuf[] */

	if (argc == 2) {
		set(argv[1]);			/* initialize clkbuf[] */
 		clock(1);			/* and set the clock */
	} else
		clockcheck();			/* check clock */

	/* Print formatted date. */
	printf("%d%d%d%d%d%d%d%d%d%d.%d%d\n",
		clkbuf[YEAR] >> 4, clkbuf[YEAR] & 15,
		clkbuf[MON]  >> 4, clkbuf[MON]  & 15,
		clkbuf[DAY]  >> 4, clkbuf[DAY]  & 15,
		clkbuf[HOUR] >> 4, clkbuf[HOUR] & 15,
		clkbuf[MIN]  >> 4, clkbuf[MIN]  & 15,
		clkbuf[SEC]  >> 4, clkbuf[SEC]  & 15
		);
	exit (0);
}

/*
 * Change a numeric character string into two bcd digits.
 */
int
bcd(s) register char *s;
{
	if (('0' <= s[0]) && (s[0] <= '9') && ('0' <= s[1]) && (s[1] <= '9'))
		return (s[0] << 4) | (s[1] & 15);
	fatal("argument contains nondigit");
}

/*
 * Check the clkbuf[] for sanity.
 */
void
clockcheck()
{
	sanity(YEAR, 0, 99, "year");
	sanity(MON, 1, 12, "month");
	sanity(DAY, 1, 31, "day");
	sanity(HOUR, 0, 23, "hour");
	sanity(MIN, 0, 59, "minute");
	sanity(SEC, 0, 59, "second");
}

/*
 * Wait for clock to stabilize or timeout to occur, then read or set it.
 */
void
clock(flag) register int flag;
{
	register int i;

	i = 0;
	do {
		splo();
		if (--i == 0)
			fatal("bad clock");
		sphi();
	} while (zget(STAT) & UPDATE);

	if (flag == 0) {
		/* Read the clock. */
		for (i = 0; i < sizeof clkbuf; i++)
			clkbuf[i] = zget(i);
	} else {
		/* Write the clock. */
		for (i = 0; i < sizeof clkbuf; i++)
			zput(i, clkbuf[i]);
	}
	splo();
}

/*
 * Cry and die.
 */
void
fatal(args) char *args;
{
	fprintf(stderr, "/etc/ATclock: %r\n", &args);
	exit(1);
}

/*
 * Sanity check: make sure BCD clkbuf[i] is in the given range.
 */
void
sanity(i, min, max, msg) register int i; int min, max; char *msg;
{
	register int n;

	n = 10 * (clkbuf[i] >> 4) + (clkbuf[i] & 15);
	if (n < min || n > max)
		fatal("%s %d not in range %d to %d", msg, n, min, max);
}

/*
 * Set clkbuf[] according to the argument string.
 */
void
set(s) register char *s;
{
	register char *a;

	a = s + strlen(s);
	if ((&a[-3] >= s) && (a[-3] == '.')) {
		clkbuf[SEC] = bcd(a -= 2);
		a--;
	} else
		clkbuf[SEC] = 0;
	if (&a[-2] >= s)
		clkbuf[MIN] = bcd(a -= 2);
	if (&a[-2] >= s)
		clkbuf[HOUR] = bcd(a -= 2);
	if (&a[-2] >= s)
		clkbuf[DAY] = bcd(a -= 2);
	if (&a[-2] >= s)
		clkbuf[MON] = bcd(a -= 2);
	if (&a[-2] >= s)
		clkbuf[YEAR] = bcd(a -= 2);
	if (a != s)
		usage();
	clockcheck();
}

/*
 * Print a usage message and exit.
 */
void
usage()
{
	fprintf(stderr, USAGE);
	exit(1);
}

/* end of ATclock.c */
