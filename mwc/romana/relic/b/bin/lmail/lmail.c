static	char	*rcsrev = "$Revision: 1.3 $";
static	char	*rcshdr =
	"$Header: /src386/bin/lmail/RCS/lmail.c,v 1.3 92/11/03 09:06:26 bin Exp Locker: bin $";
/*
 * $Header: /src386/bin/lmail/RCS/lmail.c,v 1.3 92/11/03 09:06:26 bin Exp Locker: bin $
 * $Log:	lmail.c,v $
 * Revision 1.3  92/11/03  09:06:26  bin
 * Bob H: restructures routines to lock and write mailboxes and wait for mailbox to become available.
 * 
 * Revision 1.1  92/03/13  12:19:15  bin
 * Initial revision
 * 
 * Revision 1.2  92/01/07  08:46:20  bin
 * Piggy changes for so that if an all caps sitename fails, it will try
 * lower case.
 * 
 * Revision 2.13	90/03/30  16:16:19 	wgl
 * Correct seek pointer work within readmail.
 * 
 * Revision 2.12	90/03/12  14:19:08 	wgl
 * New version of send doing aliases in a different place.
 * 
 * Revision 2.11	90/03/08  16:41:15 	wgl
 * Add code to get signature file.
 * 
 * Revision 2.10	90/03/02  10:41:59 	wgl
 * Add the -a flag so that uucp does not look like it is coming from
 * somewhere else.
 * 
 * Revision 2.9	90/03/01  11:17:19 	wgl
 * Final conversion to rely soley on ^A^A as separators.
 * Effectively delete concept of m_hsize.
 * 
 * Revision 2.8	90/03/01  10:14:31 	wgl
 * Go buy separators, not "From_" lines.
 * 
 * Revision 2.7	90/02/28  17:11:41 	wgl
 * Add version print, add some changes to the message-id field.
 * 
 * Revision 2.6	90/02/28  16:42:09 	wgl
 * Version number fakeout.
 * 
 * Revision 2.5	90/02/28  16:19:44 	wgl
 * Try again to get versioin number right.
 * 
 * Revision 2.4	90/02/28  16:18:56 	wgl
 * Get version number right.
 * 
 * Revision 2.3	90/02/28  16:04:59 	wgl
 * Many changes; ripped out send.
 * Changes are to make it work with uucp and internet mail.
 * 
 * Revision 1.6	89/02/22  05:34:00 	bin
 * Changes by rec to integrate with lauren's uumail.
 * 
 * Revision 1.5	88/09/01  14:49:01	bin
 * Source administration: Re-install declaration of getenv. 
 * It was inserted after epstein made his copy.
 * 
 * Revision 1.4	88/09/01  14:44:49	bin
 * Mark Epsteins changes for ASKCC and for message scrolling, and interrupt
 * handling during processing.
 * 
 * Revision 1.3	88/09/01  14:27:41	bin
 * declare getenv to get rid of integer pointer pun error message.
 * 
 * Revision 1.2	88/09/01  11:02:23	bin
 * Remove extra declaration of header which had rcs stuff in it.
 * 
 * Revision 1.1	88/09/01  10:55:34	bin
 * Initial revision
 * 
 */
/*
 * The mail command.
 * Coherent electronic postal system.
 * (NOTE: this command is written in such a way that
 * it assumed that it is setuid on execution to `root'.
 * All file accession is checked on this basis).
 * Modifications by rec january 1986 to include xmail.
 * 		 by epstein november 1987 to include CC:
 *		 by epstein november 1987 to allow ^C exit to leave you in
 *					  mail command processor
 *		 by epstein november 1987 to substitute /usr/games/fortune
 *					  for printing encrypted messages
 *		by rec february 1989 to tail to lauren weinstein's
 *			mail for alias expansion and uucp queuing.
 *		by wgl January 1990 to handle uucp mail more directly.
 */

char	isummary[] = "\
\
Command summary:\n\
	d		Delete current message and print the next message\n\
	m [user ...]	Mail current message to each named 'user'\n\
	p		Print current message again\n\
	q		Quit and update the mailbox\n\
	r		Reverse direction of scan\n\
	s [file ...]	Save message in each named 'file'\n\
	t [user ...]	Mail standard input to each named 'user'\n\
	w [file ...]	Save message in each named 'file' without its header\n\
	x		Exit without updating mailbox\n\
	newline		Print the next message\n\
	.		Print current message again\n\
	+		Print the next message\n\
	-		Print the previous message\n\
	EOF		Put undeleted mail back into mailbox and quit\n\
	?		Print this command summary\n\
	!command	Pass 'command' to the shell to execute\n\
If no 'file' is specified, 'mbox' in user's home directory is default.\n\
If no 'user' is specified, the invoking user is default.\n\
If the 'm', 'p', 't' commands are followed by an 'x',\n\
then the public key cryptosystem is applied to the message.\n\
\
";

#include "mail.h"

#define	NARGS	64		/* Maximum # args to interactive command */

extern	char	*getenv();
extern	int	optind;
extern	char	*optarg;
extern	char	getopt();
char	*revnop();

char BOBerrmsg[50];

int	mflag;			/* `You have mail.' message to recipient */
int	rflag;			/* Reverse order of print */
int	qflag;			/* Exit after interrrupts */
int	pflag;			/* Print mail */
int	verbflag;		/* verbose */
int	callmexmail;		/* Xmail modifier present */
int	callmermail;		/* rmail modifier present */
struct	msg {
	struct msg *m_next;		/* Link to next message */
	struct msg *m_prev;		/* Link to previous message */
	int	m_flag;			/* Flags - non-zero if deleted */
	int	m_hsize;		/* Size of header of message */
	fsize_t	m_seek;			/* Start position of message */
	fsize_t	m_end;			/* End of message */
};
struct msg	*m_first = NULL;	/* First message */
struct msg	*m_last = NULL;		/* Last message */

struct	tm	*tp;

char	iusage[] = "Bad command--type '?' for command summary\n";
char	nombox[] = "No mailbox '%s'.\n";
char	nomail[] = "No mail.\n";
char	noperm[] = "Mailbox '%s' access denied.\n";
char	moerr[] = "Cannot open mailbox '%s'\n";
char	wrerr[] = "Write error on '%s'\n";
char	nosave[] = "Cannot save letter in '%s'\n";

FILE	*mfp;				/* Mailbox stream */
int	myuid;				/* User-id of mail user */
int	mygid;				/* Group-id of mail user */
char	myname[25];			/* User name */
char	myfullname[50];
char	mymbox[256];			/* $HOME/mbox		*/
char	spoolname[50] = SPOOLDIR;
char	*mailbox = spoolname;
char	cmdname[1024];		/* Command for x{en,de}code filter */
				/* and for tail recursion to uumail */
				/* and for editor recursion */

char	*args[NARGS];			/* Interactive command arglist */
char	msgline[NLINE];
char	cline[NCLINE] = "+\n";
char	*temp;				/* Currently open temp file	*/
char	templ[] = "/tmp/mailXXXXXX";	/* Temp file name template	*/
char	*editname;			/* name of editor		*/
char	*scatname;			/* name of scat filter		*/
char	*askcc;				/* Ask for CC: list? (YES/NO)	*/

fsize_t	ftell();
char	*getlogin();
char	*mktemp();
int	catchintr();
int	catchpipe();
void	catchchild();
int	close_fds();
char	asuser [32];

main(argc, argv)
char *argv[];
{
	register char *ap;
	char	c, *foo;

	close_fds(); 
	logopen();

	ap = argv[0];
	if ( (foo=rindex(ap, '/')) != NULL )
		ap = foo+1;
	callmermail = (strcmp(ap, "rmail") == 0);
	callmexmail = (strcmp(ap, "xmail") == 0);

	if (callmermail)
		logdump("argv0 = rmail\n");

	asuser [0] = '\0';
	signal(SIGCLD, catchchild);
	signal(SIGINT, catchintr);
	signal(SIGPIPE, catchpipe);
	while ((c = getopt(argc, argv, "a:f:mpqrv")) != EOF) {
		switch(c) {
		case 'f':
			if (!callmermail)
				mailbox = optarg;
			break;
		case 'a':
			strcpy(asuser, optarg);
			break;
		case 'm':
			mflag++;
			break;
		case 'p':
			pflag++;
			break;
		case 'q':
			qflag++;
			break;
		case 'r':
			rflag++;
			break;
		case 'v':
			fprintf(stderr, "mail, ver. %s\n", revnop());
			verbflag++;
			break;
		default:
			usage();
		}
	}
	if (callmermail) 
		verbflag = mflag = pflag = rflag = 0;
	setname();
	if (optind < argc) {
		qflag = 1;
		if ( send2(stdin, &argv[optind], (fsize_t)0,
						 (fsize_t)MAXLONG, 1) != 0 )
			rmexit(1);
	} else {
		if ( ! pflag)
			callmexmail = 0;
	}
	rmexit(0);
}

/*
 * Setup all the identities for the current user.
 */
setname()
{
	register struct passwd *pwp;
	register char *np;
	extern struct passwd * getpwnam();

	if (strlen(asuser) > 0) {
		if((pwp = getpwnam(asuser)) == NULL)
			merr("No such user %s.\n", asuser);
		myuid = pwp -> pw_uid;
	} else 		
		myuid = getuid();
	if ((pwp = getpwuid(myuid)) == NULL)
		merr("Who are you?\n");
	np = pwp->pw_name;
	mygid = pwp->pw_gid;
	strcat(spoolname, np);
	strcpy(myname, np);
	strcpy(myfullname, pwp->pw_gecos);
	strcpy(mymbox, pwp->pw_dir);
	strcat(mymbox, "/mbox");
	mktemp(templ);

	if ((editname=getenv("EDITOR"))==NULL)
		editname = "/bin/ed";

	if ( ((scatname=getenv("PAGER")) != NULL) &&
	     (strlen(scatname) == 0) )
		scatname = NULL;

	if ((askcc=getenv("ASKCC")) != NULL)
		if ( strcmp(askcc, "YES") )
			askcc = NULL;
}

/*
 * Lock the appropriately-numbered mailbox
 * (numbered by user-number) from multiple
 * accesses. There is a (small) race here
 * which will be overlooked for now.
 */
char	*lockname;

mlock(uid)
int uid;
{
	register int fd;
	static char lock[32];

	lockname = lock;
	sprintf(lock, "/tmp/maillock%d", uid);

	while ( (access(lockname, AEXISTS)) == 0 ){
		sprintf(BOBerrmsg, "{%d} sleeping(1)\n",getpid());
		logdump(BOBerrmsg);
		sleep(1);
	}

	if ((fd = creat(lockname, 0)) >= 0){
		close(fd);
		return(0);
	}else{
		return(1);
	}
}

/*
 * Unlock the currently-set lock (by `mlock')
 * also called from rmexit.
 */
munlock()
{
	if (lockname != NULL)
		unlink(lockname);
	lockname = NULL;
}

/*
 * Errors, usage, and exit removing
 * any tempfiles left around.
 */
mmsg(x)
{
	fprintf(stderr, "%r", &x);
}

merr(x, s)
char *x, *s;
{
	mmsg(x, s);
	logdump("merr: ");
	logdump(x, s);
	rmexit(1);
}

rmexit(s)
int s;
{
	if (temp != NULL)
		unlink(temp);

	/* only unlock the mailbox if we are exiting normally. Remember,
	 * the box may have been locked by another process.
	 */

	if(s == 0)
		munlock(); 

	sprintf(BOBerrmsg,"{%d} About to exit, status = 0x%04x\n", getpid(),s);
	logdump(BOBerrmsg);
	logclose();

	exit(s);
}

/*
 * Catch interrupts, taking the
 * appropriate action based on
 * the `-q' option.
 */
int	intrflag;		/* On when interrupt sent	*/
int	pipeflag;		/* On when broken pipe caught	*/

catchintr()
{
	logdump("Caught SIGINT\n");
	if (qflag)
		rmexit(1);
	intrflag = 1;
	signal(SIGINT, catchintr);
}

void catchchild()
{
	logdump("Caught SIGCLD\n");
	signal(SIGCLD, catchchild);
}

catchpipe()
{
	logdump("Caught SIGPIPE\n");
	pipeflag = 1;
	signal(SIGPIPE, catchpipe);
}

intcheck()
{
	if (intrflag || pipeflag) {
		if (intrflag){
			putc('\n', stdout);
			sprintf(BOBerrmsg,"{%d}	lmail caught interrupt signal\n",
				getpid());
		}else{
			sprintf(BOBerrmsg,"{%d} lmail caught broken pipe signal", 
				getpid());
		}

		logdump(BOBerrmsg);

		intrflag = pipeflag = 0;
		return (1);
	}
	return (0);
}

char	*
revnop()
{
	register	char *cp;
	register	char c;
	static		char revnobuf[32];

	if ((cp = index(rcsrev, ' ')) != NULL) {
		while (((c = *++cp) == ' ') && (c != '\0'))
			;
		strcpy(revnobuf, cp);
		if ((cp = index(revnobuf, ' ')) != NULL)
			*cp = '\0';
		return (revnobuf);
	} else
		return("OOPS");
}


/* close any unwanted file descriptors that we may have inheritted */

close_fds()
{

int i;

	for(i = fileno(stderr) +1  ; i < _NFILE ; i++){
		close(i);
	}
}
