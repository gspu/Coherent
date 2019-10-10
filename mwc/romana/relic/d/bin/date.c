/*
 * date.c
 * Print and set the date.
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>

#if COHERENT
#include <utmp.h>
#include <sys/stat.h>
#endif

void	error();
void	usage();

#define TFCFLAG	0		/* Truly French GMT is UTC */
int	gmtflag;		/* Do things in Greenwich Mean Time */
int	sflag;			/* Suppress DST conversion when setting */
struct	tm	*gtime();
struct	timeb	tb;
time_t	timep[2];
#define	MIN	(60L)
#define	HOUR	(60L*60L)
#define	DAY	(60L*60L*24L)
#define	APR	3
#define	OCT	9

#if COHERENT
struct	utmp	utmps[2];
char	utmpf[] = "/usr/adm/wtmp";
struct	stat	sbuf;
char	btimf[] = "/etc/boottime";
#endif

/*
 * Table of days per month.
 * This will be adjusted for leap years.
 */
char	dpm[] = {
	31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

main(argc, argv)
char *argv[];
{
	register char *s;
	register struct tm *tmp;
	long ltime;
	char *tzone;

	if (argc>1 && *argv[1]=='-') {
		if (argv[1][1]=='u' && argv[1][2]=='\0')
			gmtflag = 1;
		else if (argv[1][1]=='s' && argv[1][2]=='\0')
			sflag = 1;
		else
			usage();
		argc--;
		argv++;
	}
	if (argc > 2)
		usage();
	if (argc > 1)
		sdate(argv[1]);
	ftime(&tb);
	ltime = tb.time;
	s = asctime(tmp = gtime(&ltime));
	s[24] = '\0';
	if (!gmtflag)
		tzone = tzname[tmp->tm_isdst];
	else
		tzone = TFCFLAG ? "UTC" : "GMT";
	printf("%s %s\n", s, tzone);
	exit(0);
}

/*
 * Set the date/time based on
 * string 's'.
 */
sdate(s)
char *s;
{
	register char *sp;
	register int year;
	register int month;
	int status;
#if COHERENT
	int ufd;
#endif
	long ltime;
	struct tm *tmp;

	ftime(&tb);
	ltime = tb.time;
#if COHERENT
	utmps[0].ut_time = ltime;
#endif
	tmp = gtime(&ltime);
	for (sp = s; *sp != '\0'; sp++)
		;
	if ((sp -= 2) < s)
		usage();
	if (sp>s && sp[-1]=='.') {
		tmp->tm_sec = convert(sp, 60, s);
		sp--;
	} else {
		sp += 2;
		tmp->tm_sec = 0;
	}
	tmp->tm_min = convert(sp-=2, 60, s);
	tmp->tm_hour = convert(sp-=2, 24, s);
	if ((sp-=2) >= s) {
		tmp->tm_mday = convert(sp, 31+1, s);
		if ((sp-=2) >= s) {
			tmp->tm_mon = convert(sp, 12+1, s) - 1;
			if ((sp-=2) >= s)
				tmp->tm_year = convert(sp, 100, s);
		}
	}
	if (tmp->tm_mday > dpm[tmp->tm_mon])
		usage();
	/*
	 * Convert using the year, month, day, hour, minute,
	 * and second values in the 'tm' structure.
	 * When all is completed, take care of timezone.
	 */
	ltime = tmp->tm_sec + MIN*tmp->tm_min + HOUR*tmp->tm_hour;
	ltime += (tmp->tm_mday-1) * DAY;
	year = 1900 + tmp->tm_year;
	/*
	 * Adjust length of February in leap years.
	 */
	if (!isleap(year))
		dpm[1] = 28;
	month = tmp->tm_mon;
	while (--month >= 0)
		ltime += dpm[month] * DAY;
	while (--year >= 1970)
		ltime += (long)DAY * (isleap(year) ? 366 : 365);
	if (!gmtflag) {
		ltime += timezone;
		tmp = localtime(&ltime);
		if (!sflag && tmp->tm_isdst)
			ltime -= HOUR;
	}
	status = stime(&ltime);
	if (status < 0) {
#if COHERENT
		error("no permission");
#else
		error("bad date");
#endif
		return;
	}

#if COHERENT
	/* Correct the modtime of /etc/boottime */
	stat(btimf, &sbuf);
	timep[0] = ltime - utmps[0].ut_time + sbuf.st_mtime;
	timep[1] = timep[0];
	utime(btimf, timep);

	/* Note the change of time in /usr/adm/wtmp */
	utmps[1].ut_time = ltime;
	utmps[0].ut_line[0] = '|';
	utmps[1].ut_line[0] = '}';
	if ((ufd = open(utmpf, 1)) >= 0) {
		lseek(ufd, 0L, 2);
		write(ufd, utmps, sizeof (utmps));
		close(ufd);
	}
#endif
}

/*
 * Return 1 on leap years.
 */
isleap(yr)
register yr;
{
	if (yr%4000 == 0)
		return (0);
	if (yr%400==0 || (yr%100!=0 && yr%4==0))
		return (1);
	return (0);
}

/*
 * Convert a piece of the time.
 * The pointer cp should never fall before
 * beg and the two digits parsed should be
 * less than max.
 */
convert(cp, max, beg)
register char *cp;
char *beg;
{
	register val;

	if (cp<beg || !isdigit(cp[0]) || !isdigit(cp[1]))
		usage();
	val = (cp[0]-'0')*10 + cp[1]-'0';
	if (val >= max)
		usage();
	return (val);
}

/*
 * Return either local time or Greenwich
 * Mean 'tm' in a 'tm' structure.
 */
struct tm *
gtime(tp)
register time_t *tp;
{
	return (gmtflag ? gmtime(tp) : localtime(tp));
}

void
usage()
{
	fprintf(stderr, "Usage: date [-u] [yymmddhhmm[.ss]]\n");
	exit(1);
}

void
error(x)
{
	fprintf(stderr, "date: %r\n", &x);
	exit(1);
}

/* end of date.c */
