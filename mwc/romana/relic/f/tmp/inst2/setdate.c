/*
 * setdate: used in post processing of install to setup /etc/timezone
 */

/*
 * includes
 */

#include <stdio.h>
#include <time.h>
#include "build0.h"


/*
 * declarations
 */

char	tzone[NBUF];			/* timezone		*/
char	tzone5[NBUF];			/* timezone for Sys V	*/

/*
 * Set up a nonstandard timezone.
 */
void
get_timezone(dstflag)
int dstflag;
{
	register char *s;
	int diff;
	char std_abbr[20], dst_abbr[20];
	int east_of_gr;

	/* tzone5 is like tzone except no colons and number is in hours */

	printf(
"You need to specify an abbreviation for your timezone,\n"
"whether you are east or west of Greenwich, England,\n"
"and the difference in minutes between your timezone\n"
"and Greenwich Time (called UT or GMT).  For example,\n"
"Germany is 60 minutes of time east of Greenwich.\n"
		);
	s = get_line("Abbreviation for your timezone:");
	strcpy(std_abbr, s);
	east_of_gr = yes_no("Is your timezone east of Greenwich");
	s = get_line("Difference in minutes from GMT:");
	diff = atoi(s);
	if (east_of_gr)
		diff = -diff;
	if (dstflag) {
		s = get_line("Abbreviation for your daylight savings timezone:");
		strcpy(dst_abbr, s);
		sprintf(tzone, "%s:%d:%s:1.1.4", std_abbr, diff, dst_abbr);
		sprintf(tzone5, "%s%d%s", std_abbr, diff/60, dst_abbr);
	} else {
		sprintf(tzone, "%s:%d:", std_abbr, diff);
		sprintf(tzone5, "%s%d", std_abbr, diff/60);
	}
}


/*
 * Date and time.
 */

main()
{
	register char *s;
	int dst_conv;		/* 1 if DST conversion will be used */
	int dst_now;		/* 1 if DST in effect today */
	int n;
	char *tz;
	time_t now;
	struct tm *tmp;
	char *timestr;

again:
	/* new set_date */

	/*
	 * yyy:
	 *
	 * dst_conv = FALSE
	 * dst_now = FALSE
	 *
	 * if using DST conversion
	 *	dst_conv = TRUE
	 *	if DST in effect today
	 *		dst_now = TRUE
	 * get date from system clock
	 * if dst_conv and dst_now
	 *	add 1 hour to date fetched
	 * display date
	 * while date not correct
	 *	if proceed without setting clock
	 *		goto xxx
	 *	read date from kb
	 *	write date to CMOS clock and RAM clock
	 *	if dst_conv and dst_now
	 *		subtract 1 hour from date entered
	 *		write adjusted date to CMOS clock
	 * xxx:
	 * set TIMEZONE and TZ variables
	 * if date, TIMEZONE, and TZ not all correct
	 *	goto yyy
	 */
	cls(0);
	dst_conv = 0;
	dst_now = 0;
	printf(
"You can run COHERENT with or without conversion for daylight savings time\n"
"(summer time).  You should normally run with daylight savings time\n"
"conversion.  However, if you are going to use both COHERENT and MS-DOS\n"
"and you choose to run with daylight savings time conversion,\n"
"your time will be wrong (by one hour) during daylight savings time\n"
"while you are running under MS-DOS.\n"
"\n"
		);
	if (yes_no(
	  "Do you want COHERENT to use daylight savings time conversion")) {
		dst_conv = 1;
		printf(
"\n"
"By default, COHERENT assumes daylight savings time begins on the\n"
"first Sunday in April and ends on the last Sunday in October.\n"
"If you want to change the defaults, edit the file \"/etc/timezone\"\n"
"after you finish installing COHERENT.\n"
"\n"
		);
		if (yes_no("Is daylight savings time currently in effect"))
			dst_now = 1;
	}
	system("/bin/date `/etc/ATclock` > /dev/null");
	now = time(0);
	if (dst_conv && dst_now)
		now += 3600;
	timestr = ctime(&now);
	printf(
"\nAccording to your system clock, your local date and time are:\n"
	);
	printf("%s\n", timestr);
	if (!yes_no("Is this correct")) {
		n = 0;
		do {
			if (++n > 3) {
				printf(
"The command which sets the internal real-time clock of your system is\n"
"failing repeatedly.  Either you are entering the date and time incorrectly\n"
"or your clock hardware is not completely AT-compatible.  If your clock\n"
"hardware is incompatible, you can continue with the installation without\n"
"setting the clock correctly.  However, if you do so, subsequent clock\n"
"references (including file access and modification time information) will be\n"
"incorrect and some commands (such as \"date\") will not function correctly.\n"
					);
				if (yes_no("Do you want to proceed without setting the clock correctly"))
					break;
				n = 0;
			}
			s = get_line(
"\nEnter the correct date and time in the form YYMMDDHHMM.SS:"
				);
			sprintf(cmd, "/etc/ATclock %s >/dev/null", s);
		} while (sys(cmd, S_NONFATAL) != 0);
		sys("/bin/date `/etc/ATclock` >/dev/null", S_NONFATAL);

		if (dst_conv && dst_now) {
			/* Adjust for DST: set hardware clock back one hour. */
			now = time(0) - 3600;
			tmp = localtime(&now);
			sprintf(cmd,
			  "/etc/ATclock %02d%02d%02d%02d%02d.%02d >/dev/null",
			  tmp->tm_year, tmp->tm_mon + 1, tmp->tm_mday,
			  tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
			sys(cmd, S_NONFATAL);
		}

	}

	/* Timezone. */
	cls(0);
	printf(
"Please choose one of the following timezones:\n"
"\t0\tCentral European\n"
"\t1\tGreenwich\n"
"\t2\tNewfoundland\n"
"\t3\tAtlantic\n"
"\t4\tEastern\n"
"\t5\tCentral\n"
"\t6\tMountain\n"
"\t7\tPacific\n"
"\t8\tYukon\n"
"\t9\tAlaska\n"
"\t10\tBering\n"
"\t11\tHawaii\n"
"\t12\tOther\n"
		);
	do {
		s = get_line("Timezone code:");
	} while ((n = atoi(s)) < 0 || n > 12);
	switch (n) {
	/* N.B. entries truncated at tz[8] below if !dst_conv. */
	case 0:		tz = "EST:-60:EDT:1.1.4";	break;
	case 1:		tz = "GMT:000:GDT:1.1.4";	break;
	case 2:		tz = "NST:210:NDT:1.1.4";	break;
	case 3:		tz = "AST:240:ADT:1.1.4";	break;
	case 4:		tz = "EST:300:EDT:1.1.4";	break;
	case 5:		tz = "CST:360:CDT:1.1.4";	break;
	case 6:		tz = "MST:420:MDT:1.1.4";	break;
	case 7:		tz = "PST:480:PDT:1.1.4";	break;
	case 8:		tz = "YST:540:YDT:1.1.4";	break;
	case 9:		tz = "AST:600:ADT:1.1.4";	break;
	case 10:	tz = "BST:660:BDT:1.1.4";	break;
	case 11:	tz = "HST:600:HDT:1.1.4";	break;
	case 12:	tz = NULL;			break;
	}

	if (tz == NULL)
		get_timezone(dst_conv);
	else {
		strcpy(tzone, tz);
		if (dst_conv) {
			/* for TZ, AST:240:ADT becomes AST4ADT */
			sprintf(tzone5, "%.3s%d%cDT",
			  tz, atoi(tzone + 4)/60, tz[0]);
		} else {
			/* for TZ, AST:240 becomes AST4 */
			sprintf(tzone5, "%.3s%d", tz, atoi(tzone + 4)/60);
			tzone[8] = '\0';
		}
	}
	/* Done, print current time and retry if user botched it. */
	printf("\nYour current local date and time are now:\n");
	sprintf(cmd, "TIMEZONE='%s' /bin/date -s `/etc/ATclock`", tzone);
	sys(cmd, S_NONFATAL);

	/* Write the timezone to /tmp/timezone for debug */
	sprintf(cmd, "/bin/echo export TIMEZONE=\\\"%s\\\" >/tmp/timezone", tzone);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/bin/echo export TZ=\\\"%s\\\" >>/tmp/timezone", tzone5);
	sys(cmd, S_NONFATAL);
	if (!yes_no("Is this correct"))
		goto again;
	system("/bin/cp /tmp/timezone /etc/timezone");
	system("/bin/chmog 644 bin bin /etc/timezone");
	exit(0);
}

