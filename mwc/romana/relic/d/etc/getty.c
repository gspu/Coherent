/*
 * Rec'd from Lauren Weinstein, 7-16-84.
 * Read a user name from a terminal.
 * Attempt to determine the proper line
 * speed and mode. Call login. This version
 * explicitly sets all sgtty and tchars values
 * to allow proper resetting of the console device.
 * 
 * Responds to explicitly set erase and kill chars,
 * and ALSO to (erase == '#') and (kill == '@').
 *
 * Compile -s -n -i
 */

#include <stdio.h>
#include <ctype.h>
#include <sgtty.h>
#if NEWTTYS
#include <sys/tty.h>
#endif
#include <sys/deftty.h>
#include <signal.h>
#include <sys/stat.h>

#define FALSE	0
#define TRUE	1

#define MAXTIME 60			/* Maximum attempt time in seconds */
#define	NTYPE	(sizeof stypes/sizeof stypes[0])
#define	GMODE	(ECHO|XTABS|CRT|CRMOD)
#define	LA36M	GMODE			/* Should have LA36 delays w/a */
#define	MAXNAME	50			/* Maximum characters in a name */
#define	LOGMSG	"/etc/logmsg"		/* alternate login msg file */

char	defm[] = "\n\r\7Coherent login: "; /* default login prompt */

int	xflags;				/* Extra flags to set */
char	remote = FALSE;			/* TRUE for remote tty line */
int 	timeout();			/* forward declaration */

/*
 * Structure for each speed table.
 * The 's_nindex' entry is the next
 * indent in the current table to
 * use (for fixed-speed entries, it
 * should be always 0).
 */
struct	stable	{
	char	s_nindex;
	int	s_mode;
	char	s_ispeed;
	char	s_ospeed;
	char	*s_lmsg;
};

/*
 * Fixed-speed tables.
 */
struct	stable	sA[] = {0, GMODE, B50, B50, defm};
struct	stable	sB[] = {0, GMODE, B75, B75, defm};
struct	stable	sC[] = {0, GMODE, B110, B110, defm};
struct	stable	sD[] = {0, GMODE, B134, B134, defm};
struct	stable	sE[] = {0, GMODE, B150, B150, defm};
struct	stable	sF[] = {0, GMODE, B200, B200, defm};
struct	stable	sG[] = {0, GMODE, B300, B300, defm};
struct	stable	sH[] = {0, GMODE, B600, B600, defm};
struct	stable	sI[] = {0, GMODE, B1200, B1200, defm};
struct	stable	sJ[] = {0, GMODE, B1800, B1800, defm};
struct	stable	sK[] = {0, GMODE, B2000, B2000, defm};
struct	stable	sL[] = {0, GMODE, B2400, B2400, defm};
struct	stable	sM[] = {0, GMODE, B3600, B3600, defm};
struct	stable	sN[] = {0, GMODE, B4800, B4800, defm};
struct	stable	sO[] = {0, GMODE, B7200, B7200, defm};
struct	stable	sP[] = {0, GMODE, B9600, B9600, defm};
struct	stable	sQ[] = {0, GMODE, B19200, B19200, defm};
struct	stable	sR[] = {0, GMODE, EXTA, EXTA, defm};
struct	stable	sS[] = {0, GMODE, EXTB, EXTB, defm};

/*
 * Variable-speed tables.
 */
struct	stable	sdash[] = {	/* TTY 33 */
	0,	GMODE,	B110,	B110,	defm
};

struct	stable	s0[] = {	/* Default--various terminals */
	1,	GMODE,	B300,	B300,	defm,
	2,	GMODE,	B1200,	B1200,	defm,
	3,	GMODE,	B150,	B150,	defm,
	0,	GMODE,	B110,	B110,	defm
};

struct	stable	s1[] = {	/* 150 baud TTY 37 */
	0,	GMODE,	B150,	B150,	defm
};

struct	stable	s2[] = {	/* 9600 baud - Tek 4104 */
	0,	GMODE,	B9600,	B9600,	defm
};

struct	stable	s3[] = {	/* v22.bis & 212 datasets (2400,1200,300) */
	1,	GMODE,	B2400,	B2400,	defm,
	2,	GMODE,	B1200,	B1200,	defm,
	0,	GMODE,	B300,	B300,	defm
};

struct	stable	s5[] = {	/* Reverse of 's3' */
	1,	GMODE,	B300,	B300,	defm,
	0,	GMODE,	B1200,	B1200,	defm
};

struct	stable	s4[] = {	/* LA36 (needs its delays) */
	0,	LA36M,	B300,	B300,	defm
};

struct  stable  s9[] = {	/* 212 with auto speed select via CTS */
	1,	GMODE,	B1200,	B1200,	defm,  /* 1200 */
	2,	GMODE,  B300,	B300,	defm,  /* then 300 */
	0,	GMODE,	B110, 	B110,	defm   /* then 110 */
};

/*
 * This table is used to map
 * a speed character argument, passed
 * to "getty" from "init", into a speed
 * table base.
 */
struct	stypes	{
	int	s_name;
	struct	stable	*s_ptr;
}	stypes[] = {
	'-',	sdash,
	'0',	s0,
	'1',	s1,
	'2',	s2,
	'3',	s3,
	'4',	s4,
	'5',	s5,
	'9',	s9,	/* 212 with auto speed select */
	'A',	sA,
	'B',	sB,
	'C',	sC,
	'D',	sD,
	'E',	sE,
	'F',	sF,
	'G',	sG,
	'H',	sH,
	'I',	sI,
	'J',	sJ,
	'K',	sK,
	'L',	sL,
	'M',	sM,
	'N',	sN,
	'O',	sO,
	'P',	sP,
	'Q',	sQ,
	'R',	sR,
	'S',	sS
};

/*
 * Default sgtty and tchars settings.
 */

struct	sgttyb	defsgt = {		/* Initial sgtty */
	DEF_SG_ISPEED,
	DEF_SG_OSPEED,
	DEF_SG_ERASE,
	DEF_SG_KILL,
	DEF_SG_FLAGS
};
	
struct	tchars	deftch = {		/* Initial tchars */
	DEF_T_INTRC,
	DEF_T_QUITC,
	DEF_T_STARTC,
	DEF_T_STOPC,
	DEF_T_EOFC,
	DEF_T_BRKC
};


main(argc, argv)
char *argv[];
{
	register struct stable	*sp;
	register struct stable	*stpp;
	register struct stypes  *stp;
	register int index;
	char name[MAXNAME];
	char *ttyn;	/* pointer to current tty name */
	extern char *ttyname();

	if (argc != 2) {
		write(1, "Usage: /etc/getty baudtype\r\n", 28);
		slexit(1);		  /* error exit */
	}

	signal(SIGINT, SIG_IGN);	  /* ignore int and quit */
	signal(SIGQUIT, SIG_IGN);

	if ((ttyn = ttyname(1)) == NULL)  /* get current tty name */
	{  write(1, "getty: can't get tty name!\r\n", 28);
	   slexit(1);  /* error exit */
	}
	if (chmod(ttyn, 04000) < 0)  /* setuid bit to indicate line in use */
	{  write(1, "getty: can't chmod!\r\n", 21);
	   slexit(1);  /* error exit */
 	}

#if NEWTTYS
	if (argv[0][0] == '-' && argv[0][1] == 'r') {
		remote = TRUE;
		signal(SIGALRM, &timeout);     /* catch alarm timeouts */
		alarm(MAXTIME);		  /* timeout interval */
	}
#endif

	stpp = s0;
	if (argc > 1) {
		for (stp = stypes; stp < &stypes[NTYPE]; stp++)
			if (*argv[1] == stp->s_name) {
				stpp = stp->s_ptr;
				break;
			}
	}
	for (index=0; ; index=sp->s_nindex) {
		sp = &stpp[index];
		xflags = 0;
		/* a '9' indicates autospeed mode for first index only */
		dostty(sp, RAW, 0, (index == 0 && stp->s_name == '9'));
		if (readname(name, sp) != 0)
			break;
	}
#if NEWTTYS
	alarm(0);  /* turn off login alarm timeout */
#endif
	dostty(sp, 0, RAW, 0);		/* set sgtty modes */
	ioctl(1, TIOCSETC, &deftch);	/* set tchars modes */
	execl("/bin/login", (remote) ? "-r" : "-", name, NULL);
	slexit(1);
}

/*
 * Do a "stty" on the terminal.
 * The erase and kill characters come
 * from the defaults in "defsgt".
 * The speeds and the basic mode come
 * from the "stable" pointed to by
 * "sp". The "on" and "off" masks
 * change the modes.
 * If "automode" is non-zero, 212 auto speed
 * mode has been selected.
 */
dostty(stabp, on, off, automode)
register struct stable *stabp;
{
#if NEWTTYS
	unsigned msr;

	if (automode) 		               /* 212 auto speed mode? */
	   if (ioctl(1, TIOCRMSR, &msr) == 0)  /* get modem status register */
	      stabp->s_ispeed = stabp->s_ospeed 
	         = ((msr & MSRCTS) ? B1200 : B300);  /* CTS high means 1200 */  
#endif

	defsgt.sg_ispeed = stabp->s_ispeed;
	defsgt.sg_ospeed = stabp->s_ospeed;
	defsgt.sg_flags  = stabp->s_mode;
	defsgt.sg_flags |= xflags|on;
	defsgt.sg_flags &= ~off;
	ioctl(1, TIOCSETP, &defsgt);	      /* set modes */
}

/*
 * Read a name from the terminal.
 * Return true if you get a name, or false
 * if the speed should be cycled. Use the
 * editing characters from the initial
 * defaults.
 */
readname(s, sp)
char	*s;
struct	stable	*sp;
{
	register char	*cp;
	register int	c;
	register int	seenupper;
	register int	seenlower;
	FILE		*msgfp;
	char		last;

loop:
	seenlower = 0;
	seenupper = 0;
	xflags  = 0;
	if ((msgfp = fopen(LOGMSG, "r")) != NULL) {
		/* Read from login message file. */
		for (last = '\n'; (c = fgetc(msgfp)) != EOF; last = c) {
			if (last == '\n')
				write(1, "\r", 1);
			write(1, &last, 1);
		}
		if (last != '\n')
			write(1, &last, 1);
		fclose(msgfp);
	} else   
		write(1, sp->s_lmsg, strlen(sp->s_lmsg));  /* use standard msg */

	cp = s;
	for (;;) {
		if ((c = getchar() & 0x7F) == 0) 
	   	   return(0);    	      /* break advances speed table */
		if (c==' ' || c=='\t' || c=='\f')
		   continue;                  /* ignore this char */
		if (c == deftch.t_eofc)      /* EOF returns to init */
		   slexit(1);		
		if (c == defsgt.sg_kill || c == '@')  /* kill this line */
		{	write(1, "\r\n", 2);
			seenupper = seenlower = 0;
			xflags = 0;
			cp = s;
			continue;
		}
		if (c == defsgt.sg_erase || c == '#')  /* erase this char */
		{	if (sp != s)
			   --cp;
			continue;
		}
		if (c == '\n') 
		{	putchar('\r');
			break;
		}
		if (c == '\r')
		{	putchar('\n');
			xflags |= CRMOD;
			break;
		}
		if (isupper(c))
			seenupper = 1;
		else if (islower(c))
			seenlower = 1;
		if (cp < s+MAXNAME)
			*cp++ = c;
	}
	if (cp == s)
		goto loop;
	*cp = 0;
	if (seenupper!=0 && seenlower==0) 
	{	xflags |= LCASE;
		for (cp=s; *cp!=0; ++cp) 
		{   if (isupper(*cp))
		       *cp = tolower(*cp);
		}
	}
	return(1);
}

/* login attempt timeout routine */
timeout()
{
	printf("\r\n"); 	/* neatness */
	slexit(1); 		/* terminate and try force hangup */
}

/* sleep to make sure output has flushed, then exit */
slexit(status)
{
    sleep(3);
    exit(status);
}
