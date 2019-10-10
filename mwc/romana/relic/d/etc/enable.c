/*
 * Rec'd from Lauren Weinstein, 7-16-84.
 * enable/disable/ttystat
 *
 * All are links to the same executable.
 *
 * enable tty ...   -- enable tty(s) for dialup use and return status
 * disable tty ...  -- disable tty(s) for dialup use and return status
 * ttystat [-d] tty -- return status for tty 
 *
 * When only one tty is specified the commands return exit status 1 if the  
 * tty was enabled when the command was given. They return exit status 0 if
 * the tty was disabled when the command was given.  
 * 
 * If multiple ttys are specified to "enable" or "disable", the exit
 * value will be based on the last specified tty which was successfully
 * found by the program, unless no tty's were found, in which case -1
 * is returned.
 *
 * If the -d option is given to "ttystat", the a message giving the status
 * of the terminal will be written to standard output.  Otherwise, the return
 * value of 1 (if enabled) or 0 (if not enabled) is given.
 *
 * All three commands return exit status -2 in case of serious errors
 * such as bad usage, file access failures, etc.
 *
 * Typical usage:  enable tty50
 *
 * -d option to ttystat added by John Hobson, 1-7-85.
 */ 
 
#include <stdio.h>
#include <signal.h>

#define	TTYS	"/etc/ttys"
#define DIRSIZ	14

#define ONCHAR '1'
#define OFFCHAR '0'

struct terms {
	char enable;
	char linetype;
	char gettype;
	char name[DIRSIZ];
} t[200];

extern	char	*rindex();

main(argc, argv)
int argc;
char **argv;
{
	register FILE *ttys;
	register int c;
	register struct terms *tp = t;
	char *cp, *cmd, *ttysave, flagchar, display;
	char *myname;
	int found, current;
	char fulldev[20] = {"/dev/"};

	if ((cmd = rindex(argv[0], '/')) == NULL)  /* get command name */
		cmd = argv[0]; 
	else
		cmd++;

	if(strcmp(cmd, "enable") == 0)	
		flagchar = ONCHAR;	/* flag enable */
	else if(strcmp(cmd, "disable") == 0)
		flagchar = OFFCHAR;	/* flag disable */
	else if(strcmp(cmd, "ttystat") == 0) 	
		flagchar = -1;		/* flag ttystat */
	else {
		fprintf(stderr, "Unknown program name: %s\n", cmd);
		exit(-2);
	}

	if (flagchar == -1 && (strcmp(argv[1], "-d") == 0))
	{
		display = 1;
		ttysave = argv[2];
		argc--;
		argv++;
	}
	else
		display = 0;

	if (flagchar != -1 && argc < 2) /* disable and enable */
	{
		fprintf(stderr, "Usage: /etc/%s tty ...\n", cmd);
		exit(-2);
	}
	else if (argc != 2)		/* ttystat */
	{
		fprintf(stderr, "Usage: /etc/ttystat [-d] tty\n");
		exit(-2);
	}

	if ((ttys = fopen(TTYS, "r")) == NULL) {
		fprintf(stderr, "%s: can't open %s\n", cmd, TTYS);
		exit(-2);
	}

	while ((c = getc(ttys)) != EOF) {
		tp->enable = c;
#if	NEWTTYS
		tp->linetype = getc(ttys);  /* get line type */
#endif
		tp->gettype = getc(ttys);
		for (cp = tp->name; (((c = getc(ttys)) != '\n') && c != EOF);
		    *cp++ = c);
		if (cp != &tp->name[DIRSIZ])
			*cp = '\0';
		tp++;
	}
	myname = rindex(ttyname(0), '/') + 1;

	for (cp = *(++argv); --argc; cp = *(++argv)) {
		if (strncmp(cp, "/dev/", 5) == 0)
			cp += 5;
		strcat(fulldev,cp);
		found = 0;
		for (tp = t; *tp->name; tp++) {
		   if (strncmp(cp, tp->name, DIRSIZ) == 0) 
		      found++;
		   else
		      continue;  /* go for next entry */
		   if (strncmp(cp, myname, DIRSIZ) == 0) {
		      fprintf(stderr, 
			 "%s: invalid on your own terminal\n", cmd);
		      break;
		   }
		   current = tp->enable;   /* get current status */
		   tp->enable = flagchar;  /* set new status */
		}
		if (found == 0)
			fprintf(stderr, "%s: tty line %s not found\n",
			    cmd, cp);
	}
	fclose(ttys);

	if (!found)   /* if no tty's found */
	   exit(-1);

	if (flagchar == -1)  /* if just checking status */
	{
		if (display == 1)
		{
			if (current == '1')
				printf("%s is enabled\n", ttysave);
			else
				printf("%s is disabled\n", ttysave);
		}
		exit(current == '1' ? 1 : 0);  /* return appropriate status */
	}

	if ((ttys = fopen(TTYS, "w")) == NULL) {
		fprintf(stderr, "%s: can't write %s\n", cmd, TTYS);
		exit(-2);
	}

	for (tp = t; *tp->name; tp++) {
		putc(tp->enable, ttys);
#if	NEWTTYS
		putc(tp->linetype, ttys);
#endif
		putc(tp->gettype, ttys);
		for (cp = tp->name; *cp && cp < &tp->name[DIRSIZ];)
			putc(*cp++, ttys);
		putc('\n', ttys);
	}
	fclose(ttys);
	if (kill(1, SIGQUIT) < 0) {
		fprintf(stderr, "%s: can't signal init\n", cmd);
		exit(-2);
	}
	sleep(4);   /* wait for init, so "disable tty5;enable tty5" works */

	/* change the modes of the device to allow global r/w, this is to
	 * allow uucp to disable a local device and then call out on a local
	 * device.
	 */

	if (flagchar == '0'){
		chmod(fulldev, 0666);
	}

	exit(current == '1' ? 1 : 0);  /* return appropriate status */
}




