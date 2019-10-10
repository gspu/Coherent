/*
 *  calendar
 *  Reminder utililty: read calendar files and lines with dates matching
 *  the current date or the date specified in the option string.
 */

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <ctype.h>

char *message =
"Usage: calendar [ -a ] [ -ffile ]... [ -d[date] ] [ -w[date] ] [ -m[month] ]\n\
Options:\n\
	-a		Search calendars of all users and send mail.\n\
	-ffile		Search each \"file\" in order given.\n\
	-d[date]	Print all entries matching \"date\".\n\
	-w[date]	Print entries in the week beginning with \"date\".\n\
	-m[month]	Print entries in the given \"month\".\n\
The default calendar is $HOME/.calendar.  The default date is today.\n\
";

char *argv0;
int all = 0;
int wday;
int advance;
char CurLine[512];
char *CurLinep;
char *getenv();

enum {NONE, DAY, WEEK, MONTH} mflag;

#define	CALFILE	"/.calendar"

main(argc, argv)
int argc;
char *argv[];
{
	register int arg = 1;
	register char *cp;
	char *thisline;
	char *matchstr;
	char *filename[10];
	FILE *fp[10];
	int matchdate;
	int foundfiles;
	int nfiles;
	int thismonth, thisday, thisyear;
	int thisdate;
	char *atsign;

	argv0 = argv[0];
	mflag = NONE; 				/* Default to no match */
	for (arg = 1; arg < argc; arg++)  { 	/* Read option string */
		cp = argv[arg];
	sw:	switch (*cp) {
			case '-':
				cp++;
				goto sw;
			case 'a':
				all = 1;
				break;
			case 'f':
				filename[arg-1] = ++cp;
				nfiles++;
				continue;
			case 'd':
				matchstr = ++cp;
				mflag = DAY;
				continue;
			case 'w':
				matchstr = ++cp;
				mflag = WEEK;
				continue;
			case 'm':
				matchstr = ++cp;
				mflag = MONTH;
				continue;
			default:
				fprintf(stderr, "%s: unrecognized option '%c'\n", argv0, *cp);
				usage();
		}
	}
	if (all)
		doall();
	/*
	 * Open files.
	 */
	if (nfiles)  {
		for (arg = 0; arg < nfiles; arg++ )  {
			if ((fp[arg] = fopen(filename[arg], "r")) == NULL)
				fprintf(stderr, "cannot open file %s\n", filename[arg]);
			else
				foundfiles++;
		}
		if (!foundfiles)
			fatal("cannot open any files specified");
	} else {
		char *hp;

		nfiles = 1;
		if ((hp = getenv("HOME")) == NULL)
			fatal("can't find my way back HOME");
		filename[0] = malloc(strlen(hp) + strlen(CALFILE) + 1);
		if (filename[0] == NULL)
			fatal("out of memory");
		strcpy(filename[0], hp);
		strcat(filename[0], CALFILE);
		if ((fp[0] = fopen(filename[0], "r")) == NULL)
			fatal("cannot open file $HOME/.calendar");
	}
	/*
	 * Find match condition from options or current date
	 */
	switch (mflag)  {
		case NONE:
			matchdate = current(0);
			break;
		case DAY:
		case WEEK:
			if (*matchstr == '\0') 
				matchdate = current(0);
			else {
				strncpy(CurLine, matchstr, sizeof(CurLine));
				CurLinep = &CurLine[0];
				if ((thismonth = findmon()) == -1) 
					fatal("invalid month in match date");
				if ((thisday = findday()) == -1)  
					fatal("invalid day in match date");
				if ((thisyear = findyear()) == -1)  
					thisyear = current(1);
				matchdate = date(thisday, thismonth, thisyear);
			}
			break;
		case MONTH:
			if (*matchstr == '\0')
				matchdate = current(2);
			else  {
				strncpy(CurLine, matchstr, sizeof(CurLine));
				CurLinep = &CurLine[0];
				if ((matchdate = findmon()) == -1)
					fatal("invalid month in match date");
			}
			break;
	}
	/*
	 * Read the calendar files, print matched lines.
	 */
	for (arg = 0; arg < nfiles; arg++) {
		if (fp[arg] == NULL)
			continue;
		while ((thisline = fgets(CurLine,sizeof(CurLine),fp[arg]))!=NULL) {
			CurLinep = &CurLine[0];
			advance = 0;
			if ((atsign = strchr(CurLinep, '@')) != NULL)
				advance = atoi(atsign + 1);
			if ((thismonth = findmon()) == -1)
				thismonth = 0;
			if ((thisday = findday()) == -1)
				thisday = 0;
			if ((thisyear = findyear()) == -1)
				thisyear = current(1);
			thisdate = date(thisday, thismonth, thisyear);
			if (thisdate >= matchdate &&
			    thisdate <= matchdate + advance)
				printf("%s", thisline);
			else switch (mflag)  {
				case NONE:
					if (wday == 6)
						if (thisdate == matchdate ||
						  thisdate == matchdate + 1 ||
						  thisdate == matchdate + 2 ||
						  thisdate == matchdate + 3)
							printf("%s", thisline);
					if (wday == 7)
						if (thisdate == matchdate ||
						  thisdate == matchdate + 1 ||
						  thisdate == matchdate + 2)
							printf("%s", thisline);
					if (0 <= wday && wday < 6)
						if (thisdate == matchdate ||
						   thisdate == matchdate + 1)
							printf("%s", thisline);
					break;
				case DAY:
					if (thisdate == matchdate)
						printf("%s", thisline);
					break;
				case WEEK:
					if (matchdate <= thisdate &&
						    thisdate <= matchdate+7)
						printf("%s", thisline);
					break;
				case MONTH:
					if (thismonth == matchdate)
						printf("%s", thisline);
					break;
			}
			thisline = NULL;
		}
	}
}

usage()
{
	fprintf(stderr, "%s", message);
	exit(1);
}

fatal(str)
char *str;
{
	fprintf(stderr, "%s: %r\n", argv0, &str);
	exit(1);
}

int
findmon()
{
	register int i;
	register int month;
	register char *t;
	static char tbuf[100];
	static char *mon[12] = { 
		"[Jj][Aa][Nn][Uu. :1-9]",
		"[Ff][Ee][Bb][Rr. :1-9]",
		"[Mm][Aa][Rr][Cc. :1-9]",
		"[Aa][Pp][Rr][Ii. :1-9]",
		"[Mm][Aa][Yy][ :1-9]",
		"[Jj][Uu][Nn][Ee. :1-9]",
		"[Jj][Uu][Ll][Yy. :1-9]",
		"[Aa][Uu][Gg][Uu. :1-9]",
		"[Ss][Ee][Pp][Tt. :1-9]",
		"[Oo][Cc][Tt][Oo. :1-9]",
		"[Nn][Oo][Vv][Ee. :1-9]",
		"[Dd][Ee][Cc][Ee. :1-9]"
	};

	t = &tbuf[0];
	for (i = 0;
	     ((*t = CurLinep[i]) != ':') && (*t != '\n') && (*t != '\0');
	     i++)
		++t;
	t[1] = '\0';
	t = &tbuf[0];

	month = 0;
	for (i = 0; i <= 11; i++)    /* Look for month in word form. */
		if (pnmatch(t, mon[i], 1)) {
			month = i+1;
			break;
		}
	if (month == 0)  { 	/* Month must be in numerical form  */
		for (; *t && !isdigit(*t); t++)
			CurLinep++;
		while (*t && isdigit(*t))  {
			month = 10*month + *t++ - '0';
			CurLinep++;
		}
		CurLinep++;	   /* Truncate global line pointer */
	}
	if (month == 0 || month > 12)
		month = -1;
	return (month);
}

findday()
{
	register int day = 0;
	
	for (; *CurLinep && !isdigit(*CurLinep); CurLinep++);
		;
	while (*CurLinep && isdigit(*CurLinep))
		day = 10*day + *CurLinep++ - '0';
	if (day == 0 || day > 31)		/* Invalid day of the month */
		day = -1;
	return (day);
}

findyear()
{
	register int c;
	register int year;

	year = 0;
	for (; (c = *CurLinep) != '\0' && !isdigit(c); CurLinep++)
		if (c == '\n' || c == ':' || c == '\0')
			break;
	while (isdigit(*CurLinep))
		year = 10*year + *CurLinep++ -'0';
	if (year >= 83 && year <= 99)  
		year += 1900;
	else if (year < 1990 || year > 3000)
		year = -1;
	return(year);
}

int
current(opt)
int opt;
{
	struct tm *stimep;
	time_t timep;
	int retval;
	
	time(&timep);
	stimep = localtime(&timep);
	switch (opt)  {
		case 0:
			retval = date(stimep->tm_mday, stimep->tm_mon+1, stimep
					->tm_year+1900);
			wday = stimep->tm_wday;
			break;
		case 1:
			retval = stimep->tm_year+1900;
			break;
		case 2:
			retval = stimep->tm_mon+1;
			break;
		default:
			fatal("bad opt to current");
	}
	return (retval);
}

int
date(day, month, year)
int day, month, year;
{
	int date=0;
	register int i;
	static int m[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (year == 0)
		year = current(1);
	for (i = 0; i < month-1; i++)
		date += m[i];
	date += day;
	if ((year%4) == 0 && month > 2 && (year%100) != 0)
		date++;
	for (i = 1984; i < year; i++) {
		if ((i%4) == 0)
			date += 366;
		else
			date += 365;
	}
	return (date);
}

doall()
{
	register FILE *fp;
	static char pline[256];	/* Current line in passwd file */
	static char uname[32];
	static char ucalfile[128];
	static char cmd[128];
	register char *cp1;
	char *cp2, *cp3;
	int i;

	if ((fp = fopen("/etc/passwd", "r")) == NULL)
		fatal("cannot open /etc/passwd");
	while ((fgets(&pline[0],sizeof(pline),fp) != NULL) && *pline) {
		for (cp1 = &pline[0]; *cp1 != '\0'; cp1++)
			if (*cp1 == '\n')  {
				*cp1 = '\0';
				break;
			}
		cp1 = &pline[0];
		for (cp2 = &uname[0]; *cp1 != '\0' && *cp1 != ':'; )
			*cp2++ = *cp1++;
		*cp2 = '\0';
		for (i = 0; i < 4 && *cp1 != '\0'; i++)
			for (cp1++; *cp1 != '\0' && *cp1 != ':'; cp1++)
				;
		cp1++;
		cp3 = &ucalfile[0];
		for (; *cp1 != '\0' && *cp1 != ':'; )
			*cp3++ = *cp1++;
		*cp3 = '\0';
		if (ucalfile[0] == '\0')
			continue;
		strcat(ucalfile, "/.calendar");
		if (open(ucalfile, 0) >= 0) {		/* file is readable */
			strcpy(cmd, argv0);
			strcat(cmd, " -f");
			strcat(cmd, ucalfile);
			strcat(cmd, " | /bin/mail ");
			strcat(cmd, uname);
			system(cmd);
		}
	}
	exit(0);
}
