/*
 * Time command
 */

#include <stdio.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <signal.h>
#include <sys/machine.h>

#define	MILLI	1000		/* Parts of a second */

extern	char	*signame[];

main(argc, argv)
char *argv[];
{
	if (argc <= 1) {
		exit(0);
	}
	dotime(argc-1, argv+1);
	exit(0);
}

dotime(argc, argv)
char *argv[];
{
	int pid, status;
	register n;
	struct timeb before, after;
	struct tbuffer tb;
	long tdiff;
	int tpart;
	char *command = argv[0];

	ftime(&before);
	if ((pid = fork()) < 0) {
		eprint("Try again.\n");
		exit(1);
	}
	if (pid) {
		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTERM, SIG_IGN);
		while (wait(&status) >= 0)
			if (n = status&0177) {
				if (n <= NSIG)
					eprint("%s", signame[n]);
				else
					eprint("sig. %d", n);
				if (status & 0200)
					eprint(" -- core dumped");
				fprintf(stderr ,"\n");
			}
		ftime(&after);
		times(&tb);
		tdiff = after.time-before.time;
		tpart = after.millitm - before.millitm;
		if (tpart < 0) {
			tpart += MILLI;
			tdiff--;
		}
		eprint("\n");
		output("Real:", tdiff, (tpart+MILLI/20)/(MILLI/10));
		tpart = tb.tb_cutime%HZ;
		output("User:", tb.tb_cutime/HZ, (tpart+HZ/20)/(HZ/10));
		tpart = tb.tb_cstime%HZ;
		output("Sys: ", tb.tb_cstime/HZ, (tpart+HZ/20)/(HZ/10));
	} else {
		execvp(command, argv);
		eprint("%s: not found\n", command);
		exit(1);
	}
	exit(n);
}

/*
 * Format the output for a time entry.
 */
output(name, tsec, tenths)
char *name;
long tsec;
{
	long hours;
	int mins;
	int secs;

	/*
	 * Fudge for sloppy rounding above
	 */
	if (tenths > 9) {
		tenths = 0;
		tsec++;
	}
	hours = tsec/(60*60);
	tsec %= (60*60);
	mins = tsec/60;
	secs = tsec%60;
	eprint("%s", name);
	if (hours != 0)
		eprint("%5d:%02d:%02d", (int)hours, mins, secs);
	else {
		eprint("      ");
		if (mins)
			eprint("%2d:%02d", mins, secs);
		else
			eprint("   %2d", secs);
	}
	eprint(".%d\n", tenths);
}

/* VARARGS */
eprint(x)
{
	fprintf(stderr, "%r", &x);
}
