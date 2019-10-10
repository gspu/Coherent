/*
 * libc/gen/strftime.c
 * C standard date and time library.
 * strftime()
 * ANSI 4.12.3.5.
 * "C" locale-specific time and date formatting,
 * with the locale-dependent stuff wired in here.
 * Special thanks to the ANSI committee for this marvelous idea.
 */

#include <stddef.h>
#include <string.h>
#include <time.h>

/* Forward. */
static char *toasc();
static char *convert();

/* Static data. */
static const char *weekdays[] = {
 "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
static const char *months[] = {
 "January", "February", "March", "April", "May", "June",
 "July", "August", "September", "October", "November", "December"
};

size_t
strftime(s, maxsize, format, timeptr)
char *s;
size_t maxsize;
const char *format;
const struct tm *timeptr;
{
	register size_t nchars, i;
	register int j;
	register char *x;
	char c;

	for (nchars = 0; ; ) {
		if ((c = *format++) != '%') {
			if (++nchars > maxsize)
				return 0;
			*s++ = c;
			if (c == '\0')
				return --nchars;
		} else {

			/* Perform a conversion. */
			x = NULL;
			switch (c = *format++) {

			case 'a':
			case 'A':
				x = weekdays[timeptr->tm_wday];
				i = (c == 'a') ? 3 : strlen(x);
				break;

			case 'b':
			case 'B':
				x = months[timeptr->tm_mon];
				i = (c == 'b') ? 3 : strlen(x);
				break;

			case 'c':
				x = asctime(timeptr);
				i = 24;			/* suppress the '\n' */
				break;

			case 'd':
				j = timeptr->tm_mday;
				i = 2;
				break;

			case 'H':
				j = timeptr->tm_hour;
				i = 2;
				break;

			case 'I':
				if ((j = timeptr->tm_hour % 12) == 0)
					j = 12;
				i = 2;
				break;

			case 'j':
				j = timeptr->tm_yday + 1;
				i = 3;
				break;

			case 'm':
				j = timeptr->tm_mon + 1;
				i = 2;
				break;

			case 'M':
				j = timeptr->tm_min;
				i = 2;
				break;

			case 'p':
				x = (timeptr->tm_hour) < 12 ? "AM" : "PM";
				i = 2;
				break;

			case 'S':
				j = timeptr->tm_sec;
				i = 2;
				break;

			case 'U':
				j = (timeptr->tm_yday + 7 - timeptr->tm_wday) / 7;
				i = 2;
				break;

			case 'w':
				j = (timeptr->tm_yday + 8 - timeptr->tm_wday) / 7;
				if (timeptr->tm_wday == 0)
					--j;
				i = 2;
				break;

			case 'W':
				j = timeptr->tm_wday;
				i = 1;
				break;

			case 'x':
				x = convert(timeptr->tm_mon+1,
					    timeptr->tm_mday,
					    timeptr->tm_year,
					    '/');
				i = 8;
				break;

			case 'X':
				x = convert(timeptr->tm_hour,
					    timeptr->tm_min,
					    timeptr->tm_sec,
					    ':');
				i = 8;
				break;

			case 'y':
				j = timeptr->tm_year;
				i = 2;
				break;

			case 'Y':
				j = timeptr->tm_year + 1900;
				i = 4;
				break;

			case 'z':
				x = &tzname[(timeptr->tm_isdst==1) ? 1 :0][0];
				i = strlen(x);
				break;

			/* Default case has undefined behavior. */
			/* This copies the given character, e.g. "%e" copies "e". */
			case '%':
			default:
				x = &c;
				i = 1;
				break;
			}

			/* Convert j to i ASCII digits if necessary. */
			if (x == NULL)
				x = toasc(j, i);

			/* Copy i characters from x to the result string. */
			/* If nchars+i == maxsize, there will be no room for NUL. */
			if ((nchars += i) >= maxsize)
				return 0;
			strncpy(s, x, i);
			s += i;
		}
	}
}

/*
 * Convert i to n ASCII decimal digits.
 */
static
char *
toasc(i, n) register unsigned int i, n;
{
	static char *buf[6];
	char *cp;

	cp = &buf[5];
	*cp = '\0';
	while (n--) {
		*--cp = '0' + (i % 10);
		i /= 10;
	}
	return cp;
}

/*
 * Convert a time or date to "hh:mm:ss" or "mm/dd/yy".
 * The result is not NUL-terminated.
 */
static
char *
convert(i1, i2, i3, sep) int i1, i2, i3, sep;
{
	static char buf[8];
	register char *s;

	s = &buf[0];
	*s++ = '0' + i1 / 10;
	*s++ = '0' + i1 % 10;
	*s++ = sep;
	*s++ = '0' + i2 / 10;
	*s++ = '0' + i2 % 10;
	*s++ = sep;
	*s++ = '0' + i3 / 10;
	*s++ = '0' + i3 % 10;
	return buf;
}

/* end of libc/gen/strftime.c */
