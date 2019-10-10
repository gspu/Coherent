#include <time.h>
#include <sys/types.h>

char *month[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August", "September",
	"October", "November", "December"
};

char *weekday[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday"
};

main()
{
	char buf[20];
	time_t tnum;
	struct tm *ts;
	int hour = 0;

	time(&tnum);	/* get time from system */

	/* convert time to tm struct */
	ts=localtime(&tnum);

	if (ts->tm_hour==0)
		sprintf(buf,"12:%02d:%02d A.M.",
			ts->tm_min, ts->tm_sec);
	else
		if (ts->tm_hour>=12) {
			hour=ts->tm_hour-12;
			if (hour==0)
				hour=12;
			sprintf(buf,"%02d:%02d:%02d P.M.",
				hour, ts->tm_min,ts->tm_sec);

		} else
			sprintf(buf,"%02d:%02d:%02d A.M.", ts->tm_hour,
				ts->tm_min,ts->tm_sec);

	printf("\n%s %d %s 19%d %s\n",
		weekday[ts->tm_wday], ts->tm_mday,
		month[ts->tm_mon], ts->tm_year, buf);

	printf("Today is the %d day of 19%d\n",
		ts->tm_yday, ts->tm_year);

	if (ts->tm_isdst)
		printf("Daylight Saving Time is in effect\n");
	else
		printf("Daylight Saving Time is not in effect\n");
}	
