/*
 * Rec'd from Lauren Weinstein, 7-16-84.
 * Write to another user
 * on the system.
 * This is setuid in order to
 * use execute permission on the
 * terminal to allow/disallow writes.
 *
 * Compile -s -n -i
 */

#include <stdio.h>
#include <utmp.h>
#include <signal.h>
#include <sys/stat.h>
#include <sgtty.h>

#define FALSE	0
#define TRUE 	1

#define	NSYSC	150		/* Maximum system call line */
FILE	*tf;
struct	stat	sb;
struct	sgttyb sgttyb, sgttys;	/* for user tty modes */
struct  sgttyb sgttyd;		/* for dest tty modes */
struct  tchars tchars;		/* for determining EOF char */
char sighup, sigquit, sigint;   /* signal change flags */
int     eofmark;		/* EOF marker */		
char	dtty[40] = "/dev/";
int	conopen;		/* TRUE if connection open */
int	status;			/* exit status */	
int     dcrtmode; 		/* non-zero for dest tty crt mode */
int     ucrtmode;	        /* non-zero for user tty crt mode */
int	istty;			/* TRUE if input is a tty */
int	quit();
char	*getutmp();
char	*getlogin();
char	*ttyname();
char 	crtwarn[] = 
"Warning: One or both ttys is not a screen-oriented device.\n\
         Backspacing/character erase will not function properly...\n";

main(argc, argv)
char **argv;
{
	char *totty, *touname;
	char *fromtty, *fromuname;
	char crtmode = 0; 	/* TRUE if both ttys are crt's */
	register int c;
	register int nlf = 1;

	if (argc == 3)
		totty = argv[2];
	else {
		if (argc != 2)
			usage();
		totty = NULL;
	}
	touname = argv[1];

	gtty(0, &sgttyb);             /* get tty modes into buffer */
	if (sgttyb.sg_flags & CRT)    /* crt mode on user tty? */
	   ucrtmode++;		      /* yes */ 
	gtty(0, &sgttys);             /* save copy of original modes */

	fromuname = getlogin(); 	 
	totty = getutmp(totty, touname);  
	strcat(dtty, totty);          /* build full ttyname */

	if ((fromtty = ttyname(fileno(stderr))) != NULL) {
		stat(fromtty, &sb);
		if ((sb.st_mode & S_IEXEC) == 0)
			fprintf(stderr, "Warning: no reply possible\n");
		fromtty += 5;		/* Skip over `/dev/' */
	}
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
	{   signal(SIGINT, &quit);	   /* catch ints */
	    sigint++;  /* flag we're catching ints */
	}
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
	{   signal(SIGHUP, &quit);  	 /* catch hangups */
	    sighup++;  /* flag */
	}
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
	{  signal(SIGQUIT, &quit);	 /* catch quits */
	   sigquit++;
	}
	ioctl(0, TIOCGETC, &tchars);  /* get char settings */
	setup(touname, dtty);

	if (isatty(0))   /* input a tty? */
	{  istty = TRUE;  /* yes */
	   eofmark = tchars.t_eofc;  /* set EOF marker */
	}
	else
	   eofmark = EOF;  /* set EOF marker */

	if (!(crtmode = dcrtmode&ucrtmode))  /* check for both crt's */
	   if (!ucrtmode && istty)	     /* input not crt and not tty? */
	      fprintf(stderr, crtwarn);      /* yes, warn user */
	   
	conopen = TRUE;               /* flag connection open */
	sgttyb.sg_flags |= CBREAK;    /* want cbreak mode */
	stty(0, &sgttyb);             /* set new mode (no flush) */	
	
	/*
	 * Turn off setuid privileges.
	 */
	setuid(getuid());
	fprintf(tf, "\r\n\7Message from %s (%s) ...\r\n", fromuname, fromtty);
	printf("\r\nUser %s has been notified ...\r\n", touname);
	while ((c = getchar()) != eofmark) {  /* continue until end of file */
		if (nlf && c=='!') {
		   callsys();
		   continue;
		}
		if (c == '\n')
		   nlf = 1;
		else
		   nlf = 0;
		putc(c, tf);
		if (c == '\b' && crtmode)    /* backspace and crt's? */
		{  putc(' ', stdout);  /* erase local char */ 
		   putc('\b', stdout);  		   
		   putc(' ', tf);      /* erase dest char */
		   putc('\b', tf);
		}
	}
	quit();
}

callsys()
{
	char sysline[NSYSC];
	register char *cp;
	register int c;

	cp = sysline;
	while ((c = getchar()) != '\n')
		if (cp < &sysline[NSYSC-1])
			*cp++ = c;
	*cp = '\0';
	signal(SIGINT, SIG_IGN);   /* ignore ints */	
   	signal(SIGHUP, SIG_IGN);   /* hups */
	signal(SIGQUIT, SIG_IGN);  /* and quits */
	stty(0, &sgttys);	   /* restore original modes */
	system(sysline);           /* fork child command */
	stty(0, &sgttyb);	   /* back to special modes */
	if (sigint)		   /* restore int status */
	   signal(SIGINT, &quit);
	if (sighup)		   /* hup */
	   signal(SIGHUP, &quit);
	if (sigquit)		   /* quit */
	   signal(SIGQUIT, &quit);
	printf("!\n");
}

/*
 * Check whether or not writes are
 * allowed to this terminal.
 */
setup(u, t)
char *u, *t;
{				  
	register char abortf = 0;
	register char c;
	
	if ((tf = fopen(t, "w")) == NULL)
		fprintf(stderr, "write: cannot open %s\n", t);
	if (gtty(fileno(tf), &sgttyd) >= 0)  /* stat dest tty */
	   if (sgttyd.sg_flags & CRT)        /* crt mode on dest tty? */
	      dcrtmode++;
	if (fstat(fileno(tf), &sb) < 0) {
		fprintf(stderr, "write: can't write to %s\n", u);	
		quiterr();	
	}
	if ((sb.st_mode&S_IEXEC) == 0) {
		if (getuid() == 0)  /* is our real uid su? */
		{  fprintf(stderr, 
		      "User \"%s\" is denying messages.  Override? ", u);
		   if ((c = getchar()) != 'y')
		      abortf++;  /* flag abort */  
		   while (c != EOF && c != '\n')  /* flush remaining input */
		      c = getchar();
		   if ( ! abortf)  /* override requested? */
		      return;  /* allow write */
		   else
		      quit();
		}
		fprintf(stderr, "write: no permission to write to %s\n", u);
		quiterr();
	}
}

/*
 * Return the tty for the specified
 * username and possibly tty
 * from the utmp file.
 */
char *
getutmp(t, u)
char *t, *u;
{
	register FILE *fp;
	static struct utmp ut;


	if ((fp = fopen("/etc/utmp", "r")) == NULL) {
		fprintf(stderr, "write: /etc/utmp not accessible\n");
		quiterr();
	}
	
	while (fread(&ut, sizeof ut, 1, fp) == 1) {
		if (strncmp(ut.ut_name, u, DIRSIZ) != 0)
			continue;
		if (t != NULL)
			if (strncmp(ut.ut_line, t, 8) == 0) {
				fclose(fp);
				return (t);
			} else
				continue;
		ut.ut_line[8] == '\0';
		fclose(fp);
		return (ut.ut_line);
	}
	if (t == NULL)
		fprintf(stderr, "write: %s not logged in\n", u);
	else
		fprintf(stderr, "write: %s not logged onto %s\n", u, t);
	quiterr();
}

quit()
{
	if (conopen)          /* if connection open */
	{  stty(0, &sgttys);  /* normal mode */
	   fprintf(tf, "EOT\n");
	   fclose(tf);        /* close connection */
	}
	exit(status);  
}

quiterr()
{
	status = 1;  /* exit with error status */
	quit();
}

usage()
{
	fprintf(stderr, "Usage: write username [ttyname]\n");
	quit(1);
}


