/*
 * ATclock.c
 * 5/16/90
 * ATclock - read or set the IBM AT real-time clock
 * Usage:  /etc/ATclock [YY[MM[DD[HH[MM[.SS]]]]]]
 * cc -s -i -O -o ATclock ATclock.c ATclockas.s
 *
 */

#include <stdio.h>
#include <fcntl.h>

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

#define	DEV_CMOS	"/dev/cmos"
#define	DEV_CLOCK	"/dev/clock"

#define CLKLEN	10

/* Forward. */
int	bcd();
void	clock();
void	clockcheck();
void	fatal();
void	sanity();
void	set();
void	usage();

int	zget();
void	zput();

/* Globals. */
unsigned char clkbuf[CLKLEN];	/* Clock image buffer (ten BCD digits). */

int	fd_cmos;
int	fd_clock;

/*
 * zget()
 *
 * read a byte from a specified offset in CMOS
 *
 */
int
zget(offset)
int offset;
{
	int ret;

	lseek(fd_cmos, (long)offset, 0);
	if(read(fd_cmos, &ret, 1) != 1) {
		fprintf(stderr, "Can't read CMOS byte %d\n", offset);
		exit (1);
	}
	return ret & 0xff;
}

main(argc, argv) int argc; char *argv[];
{
	if((fd_cmos = open(DEV_CMOS, O_RDONLY)) == -1) {
		fprintf(stderr, "ATclock: can't read %s.\n",
		  DEV_CMOS);
		exit(1);
	}

	if((fd_clock = open(DEV_CLOCK, O_RDONLY)) == -1) {
		fprintf(stderr, "ATclock: can't read %s.\n",
		  DEV_CLOCK);
		exit(1);
	}

	if (argc > 2)
		usage();

	if (zget(DIAG) & BADCLK)
		fatal("bad clock");		/* check for valid clock */

	if (read(fd_clock, clkbuf, CLKLEN) != CLKLEN) {
		fprintf(stderr, "Can't read %d bytes from %s.\n",
		  CLKLEN, DEV_CLOCK);
		exit(1);
	}

	if (argc == 2) {
		close(fd_clock);
		if((fd_clock = open(DEV_CLOCK, O_RDWR)) == -1) {
			fprintf(stderr, "ATclock: can't write %s.\n",
			  DEV_CLOCK);
			exit(1);
		}
		set(argv[1]);			/* initialize clkbuf[] */
		lseek(fd_clock, 0L, 0);
		if (write(fd_clock, clkbuf, CLKLEN) != CLKLEN) {
			fprintf(stderr, "Can't write %d bytes to %s.\n",
			  CLKLEN, DEV_CLOCK);
			exit(1);
		}
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

	close(fd_cmos);
	close(fd_clock);
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
