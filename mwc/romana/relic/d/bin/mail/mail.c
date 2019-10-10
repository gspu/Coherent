static	char	*rcsrev = "$Revision: 1.1 $";
static	char	*rcshdr =
	"$Header: /newbits/bin/mail/mail.c,v 1.1 91/02/04 10:42:40 bin Exp $";
/*
 * $Header: /newbits/bin/mail/mail.c,v 1.1 91/02/04 10:42:40 bin Exp $
 * $Log:	/newbits/bin/mail/mail.c,v $
 * Revision 1.1	91/02/04  10:42:40 	bin
 * Initial revision
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

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <access.h>
#include <signal.h>
#include <sys/mdata.h>
#include <sys/stat.h>
#include "mail.h"

#define	NARGS	64		/* Maximum # args to interactive command */

extern	char	*getenv();
extern	int	optind;
extern	char	*optarg;
extern	char	getopt();
char	*revnop();

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
char	mymbox[256];			/* $HOME/mbox */
char	mydead[256];			/* $HOME/dead.letter */
char	mysig[256];			/* $HOME/dead.letter */
char	spoolname[50] = SPOOLDIR;
char	*mailbox = spoolname;
char	cmdname[1024];		/* Command for x{en,de}code filter */
				/* and for tail recursion to uumail */
				/* and for editor recursion */

char	*args[NARGS];			/* Interactive command arglist */
char	msgline[NLINE];
char	cline[NCLINE] = "+\n";
char	*temp;				/* Currently open temp file */
char	templ[] = "/tmp/mailXXXXXX";	/* Temp file name template */
char	*editname;			/* name of editor	   */
char	*askcc;				/* Ask for CC: list? (YES/NO) */

fsize_t	ftell();
char	*getlogin();
char	*mktemp();
int	catchintr();
char	*malloc();
extern	char	*rindex();
char	asuser [32];
main(argc, argv)
char *argv[];
{
	register char *ap;
	char	c;

	ap = argv[0];
	if (rindex (ap, '/') != NULL)
		++ap;
	callmermail = (strcmp(ap, "rmail") == 0);
	callmexmail = (strcmp(ap, "xmail") == 0);
	asuser [0] = '\0';
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, catchintr);
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
		send2(stdin, &argv[optind], (fsize_t)0, (fsize_t)MAXLONG, 1);
	} else {
		if ( ! pflag)
			callmexmail = 0;
		commands();
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
	strcpy(mydead, pwp->pw_dir);
	strcat(mydead, "/dead.letter");
	strcpy(mysig, pwp->pw_dir);
	strcat(mysig, "/.sig.mail");
	mktemp(templ);

	if ((editname=getenv("EDITOR"))==NULL)
		editname = "/bin/ed";

	if ((askcc=getenv("ASKCC")) != NULL)
		if ( strcmp(askcc, "YES") || !isatty(fileno(stdin)) )
			askcc = NULL;
}

/*
 * Process mail's interactive commands
 * for reading/deleting/saving mail.
 */
commands()
{
	register struct msg *mp;
	struct msg *dest;
	register char **fnp;
	register FILE *fp;
	fsize_t seek;

	readmail();
	mprint(mp = rflag ? m_last : m_first);
	for (;;) {
		readmail();
		intcheck();
		if ( ! pflag) {
			callmexmail = 0;
			mmsg("? ");
			if (fgets(cline, sizeof cline, stdin) == NULL) {
				if (intcheck())
					continue;
				break;
			}
		}
		switch (cline[0]) {
		case 'd':
			if (cline[1] != '\n')
				goto usage;
			mp->m_flag += 1;
			goto advance;

		case 'm':
		case 't':
			if (csplit(cline, args) == 1) {
				args[1] = myname;
				args[2] = NULL;
			}
			callmexmail = (cline[1] == 'x');
			if (cline[0] == 'm') {
				send2(mfp, args+1, mp->m_seek, mp->m_end - 3, 0);
				fseek(mfp, mp->m_seek, 0);
			} else
				send2(stdin, args+1, 0L, (fsize_t)MAXLONG, 1);
			break;

		case '.':
		case 'p':
			if (cline[1] == 'x') {
				callmexmail = 1;
				if (cline[2] != '\n')
					goto usage;
			} else if (cline[1] != '\n')
				goto usage;
			if (mprint(mp))
				break;
			goto advance;

		case 'q':
			if (cline[1] != '\n')
				goto usage;
			mquit();
			/* NOTREACHED */

		case 'r':
			if (cline[1] != '\n')
				goto usage;
			rflag = ! rflag;
			break;
		case 's':
		case 'w':
			if (csplit(cline, args) == 1) {
				args[1] = mymbox;
				args[2] = NULL;
			}
			seek = mp->m_seek;
			if (cline[0] == 'w')
				seek += mp->m_hsize;
			for (fnp = &args[1]; *fnp != NULL; fnp++) {
				fp = NULL;
				if (maccess(*fnp) < 0
				 || (fp = fopen(*fnp, "a")) == NULL
				 || mcopy(mfp, fp, seek, mp->m_end - 3, 0))
					mmsg(nosave, *fnp);
				if (fp != NULL) {
					fclose(fp);
					chown(*fnp, myuid, mygid);
				}
			}
			break;

		case 'x':
			if (cline[1] != '\n')
				goto usage;
			rmexit(0);
			/* NOTREACHED */

		case '!':
			if (system(cline+1) == 127)
				mmsg("Try again\n");
			else
				mmsg("!\n");
			break;

		case '?':
			mmsg(isummary);
			break;

		case '-':
			if (cline[1] != '\n')
				goto usage;
			dest = rflag ? m_last : m_first;
			goto nextmsg;

		case '+':
			if (cline[1] != '\n')
				goto usage;
		case '\n':
		advance:
			dest = rflag ? m_first : m_last;
		nextmsg:
			do {
				if (mp == dest) {
					if (pflag)
						return;
					mmsg("No more messages.\n");
					break;
				}
				mp = (dest==m_last) ? mp->m_next : mp->m_prev;
			} while (mprint(mp) == 0);
			break;

		default:
		usage:
			mmsg(iusage);
			break;
		}
	}
	putc('\n', stderr);
	mquit();
}

/*
 * Read through the mail-box file
 * constructing list of letters.
 * On subsequent calls, append any additional mail
 * and notify the user.
 */
readmail()
{
	register struct msg *mp;
	struct stat sb;
	static long m_last_end;
	fsize_t	first;		/* to become ffsize_t */
	int datasw;

	if (m_first == NULL) {
		if (stat(mailbox, &sb) < 0)
			merr(nombox, mailbox);
		if (sb.st_size == 0)
			merr(nomail);
		if (access(mailbox, AREAD) < 0)
			merr(noperm, mailbox);
		if ((mfp = fopen(mailbox, "r")) == NULL)
			merr(moerr, mailbox);
		m_last_end = 0;
	} else {
		fstat(fileno(mfp), &sb);
		if (sb.st_size == m_last_end)
			return;
		mmsg("More mail received.\n");
	}

	for(mlock(myuid);;) {
		fseek(mfp, m_last_end, 0);
		first = ftell(mfp);
		for (datasw = 0;
		     fgets(msgline, sizeof msgline, mfp) != NULL; ) {
			if(!(datasw = strcmp("\1\1\n", msgline))) {
				/* found message seperator */
				mp = (struct msg *)malloc(sizeof(*mp));
				mp->m_next = NULL;
				mp->m_prev = m_last;
				mp->m_flag = mp->m_hsize = 0;
				mp->m_seek = first;
				mp->m_end = first = m_last_end = ftell(mfp);
				if (m_first == NULL)
					m_last = m_first = mp;
				else
					m_last = m_last->m_next = mp;
			}
		}

		if(!datasw)
			break;

		/* unterminated data, patch and retry */
		fclose(mfp);
		if ((mfp = fopen(mailbox, "a")) == NULL)
			merr(moerr, mailbox);
		fprintf(mfp, "\1\1\n"); /* terminate it */
		fclose(mfp);
		if ((mfp = fopen(mailbox, "r")) == NULL)
			merr(moerr, mailbox);
		mmsg("Incomplete message found.\n");
	}
	munlock();
}

/*
 * Split a command line up into
 * argv (passed) and argc (returned).
 */
int
csplit(command, args)
char *command;
char **args;
{
	register char *cp;
	register char **ap;
	register int c;

	cp = command;
	ap = args;
	for (;;) {
		while ((c = *cp)==' ' || c=='\t')
			*cp++ = '\0';
		if (*cp == '\n')
			*cp = '\0';
		if (*cp == '\0')
			break;
		*ap++ = cp;
		while ((c = *cp)!=' ' && c!='\t' && c!='\n' && c!='\0')
			cp++;
	}
	*ap = NULL;
	return (ap - args);
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
	while (access(lockname, 0) == 0)
		sleep(1);
	if ((fd = creat(lockname, 0)) >= 0)
		close(fd);
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
 * Exit, after rewriting the
 * mailbox
 */
mquit()
{
	register struct msg *mp;
	register FILE *nfp;
	struct stat sb;

	readmail();
	mlock(myuid);
	if (mailbox != spoolname && maccess(mailbox) < 0)
		merr(noperm, mailbox);
	fstat(fileno(mfp), &sb);
	signal(SIGINT, SIG_IGN);
	unlink(mailbox);
	if ((nfp = fopen(mailbox, "w")) == NULL)
		merr("Cannot re-write '%s'\n", mailbox);
	chown(mailbox, sb.st_uid, sb.st_gid);
	chmod(mailbox, sb.st_mode&0777);
	for (mp = m_first; mp != NULL; mp = mp->m_next) {
		if (mp->m_flag == 0) {
			if (mcopy(mfp, nfp, mp->m_seek, mp->m_end, 0)) 
				merr(wrerr, mailbox);
/*			else
				fprintf(nfp, "\1\1\n"); */
		}
	}
	fclose(nfp);
	fclose(mfp);
	munlock();
	rmexit(0);
}

/*
 * Print the current message, given by
 * the message pointer (`mp').
 */
mprint(mp)
register struct msg *mp;
{
	FILE *xfp;
	if (mp->m_flag)
		return 0;
	if (callmexmail) {
		sprintf(cmdname, "xdecode");
		if ((xfp = popen(cmdname, "w")) == NULL) {
			return 0;
		}
		mcopy(mfp, xfp, mp->m_seek+mp->m_hsize, mp->m_end, 1);
		pclose(xfp);
	} else {
		fseek(mfp, mp->m_seek, 0);
		if (fgets(msgline, sizeof msgline, mfp) != NULL &&
		    strncmp(msgline, "From xmail", 10) == 0) {
			printf("From xmail\n");
			system("/usr/games/fortune");
			return(1);
		}
		mcopy(mfp, stdout, mp->m_seek, mp->m_end - 3, 1);
	}
	return (1);
}

/*
 *	Get additional users to receive message (CC:)
 */

char **
getcc(users)
register char **users;
{
	register char **ulist = args;
	static	char names[NCLINE];

	mmsg("CC: ");
	if ( fgets(names, sizeof names, stdin) == NULL )
		return(users);

	while (*users != NULL)
		*ulist++ = *users++;

	csplit(names, ulist);
	return(args);
}

	
/*
 * Errors, usage, and exit removing
 * any tempfiles left around.
 */
mmsg(x, s)
char *x, *s;
{
	fprintf(stderr, x, s);
}

merr(x, s)
char *x, *s;
{
	mmsg(x, s);
	rmexit(1);
}

rmexit(s)
int s;
{
	if (temp != NULL)
		unlink(temp);
	munlock();
	exit(s);
}

/*
 * Catch interrupts, taking the
 * appropriate action based on
 * the `-q' option.
 */
int	intflag;		/* On when interrupt sent */

catchintr()
{
	signal(SIGINT, SIG_IGN);
	if (qflag)
		rmexit(1);
	intflag = 1;
	signal(SIGINT, catchintr);
}

intcheck()
{
	if (intflag) {
		intflag = 0;
		putc('\n', stderr);
		return (1);
	}
	return (0);
}

/*
 * Call the system to execute a command line
 * which is passed as an argument.
 * Change the user id and group id.
 */
system(line)
char *line;
{
	int status, pid;
	register wpid;
	register int (*intfun)(), (*quitfun)();

	if ((pid = fork()) < 0)
		return (-1);
	if (pid == 0) {		/* Child */
		setuid(myuid);
		setgid(mygid);
		execl("/bin/sh", "sh", "-c", line, NULL);
		exit(0177);
	}
	intfun = signal(SIGINT, SIG_IGN);
	quitfun = signal(SIGQUIT, SIG_IGN);
	while ((wpid = wait(&status))!=pid && wpid>=0)
		;
	if (wpid < 0)
		status = wpid;
	signal(SIGINT, intfun);
	signal(SIGQUIT, quitfun);
	return (status);
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
