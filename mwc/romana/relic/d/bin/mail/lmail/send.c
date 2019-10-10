/*
 *	The code that basically handles the sending of 
 *	an individual message in the mail system.
 */

#include "mail.h"

#define SITENAMELEN	32	/* max length of sitename */

extern	char	*strtok();
FILE	*aliasfp;
char	aliasbuf [BUFSIZ];
static	char	**tolist;

extern	char	*temp;
extern	int	myuid;		/* User-id of mail user */
extern	int	mygid;		/* Group-id of mail user */
extern	char	cmdname[];	/* Command for x{en,de}code filter */
				/* and for tail recursion to uumail */
				/* and for editor recursion */
extern	char	*editname;	/* name of editor	   */
extern	char	*askcc;		/* Ask for CC: list? (YES/NO) */
extern	int	callmexmail;	/* Xmail modifier present */
extern	char	wrerr[];
extern	char	myname[];	/* User name */
extern	char	myfullname[];	/* full user name */
extern	int	mflag;		/* `You have mail.' message to recipient */
extern	int	verbflag;	/* verbose flag */
extern	char	templ[];	/* Temp file name template */
extern	char	nosave[];
extern	int	callmermail;

static	int	senderr;
struct	tm	*tp;
char	toerr[] = "Cannot create temporary file\n";
char	header[BUFSIZ];		/* Message header */
char	boxname[256];		/* Destination mailbox */
char	remotefrom [32];	/* "remote from uucp" */

char	nosend[] = "Can't send mail to '%s'\n";
char	nopubk[] = "Can't send xmail to '%s'\n";

/*
 * Send the message found on
 * the file pointer to the list
 * of people (argv style) with
 * a NULL pointer at the end.
 * The message is copied to a temp-file
 * from position `start' to `end' (or EOF).
 */
send2(fp, users, start, end, asksubj)
FILE *fp;
register char **users;
fsize_t start, end;
int	asksubj;
{
	char	**getcc();
	FILE 	*xfp, *tfp, *sigfp;
	time_t	curtime;

	senderr = 0;
	temp = templ;

	if ((tfp = fopen(temp, "w")) != NULL) {
		fclose(tfp);
		if ((tfp = fopen(temp, "r+w")) == NULL)
			merr(toerr);
	} else
		merr(toerr);
	chown(temp, myuid, mygid);
	unlink(temp);
	temp = NULL;
	fseek(fp, start, 0);
	end -= start;

	if (intcheck()) {
		fclose(tfp);
		return(1);
	}

	for (;;) {
		if (fgets(msgline, NLINE, fp) == NULL)
			break;
		fputs(msgline, tfp);
		if ( (end-=strlen(msgline)) <= 0 )
			break;
	}

	if (intcheck()) {
		fclose(tfp);
		return(1);
	}

	/*
	 * if empty message, bug out.
	 */

	if (ftell(tfp) == 0) {
		fclose(tfp);
		return(1);
	}

	/*
	 * Now send the message.
	 */

	time(&curtime);
	tp = localtime(&curtime);

	if (callmexmail)
		xsend(users, tfp);
	else
		usend(users, tfp);

	return( senderr );
}

usend(users, tfp)
char **users;
FILE *tfp;
{
	FILE *xfp;
	char	*cp, *name;
	char	**ulist;
	register struct passwd *pwp;

	for (ulist = users; (name=*ulist) != NULL; ulist++) {
		rewind(tfp);


		sprintf(boxname, "%s%s", SPOOLDIR, name);
		if ((pwp = getpwnam(name)) == NULL) {
			/* RFC 822 (Internet) addresses are case independent,
			 * so valid local addresses must should be lower case.
			 */
			lcase(name);
			sprintf(boxname, "%s%s", SPOOLDIR, name);
			if ((pwp = getpwnam(name)) == NULL) {
				mmsg(nosend, name);
				logdump(nosend, name);
				senderr = 1;
				continue;
			}
		}

		mlock(pwp->pw_uid);
		if ((xfp = fopen(boxname, "a")) == NULL) {
			mmsg(nosend, name);
			logdump(nosend, name);
			senderr = 1;
			munlock();
			continue;
		}
		chown(boxname, pwp->pw_uid, pwp->pw_gid);
	        mcopy(tfp, xfp, ftell(tfp), (fsize_t)MAXLONG, 0);
		fprintf(xfp, "\n\1\1\n");
		fclose(xfp);
		munlock();
		advise(name);
	}

	if (senderr && callmermail) {
		mmsg("error sending mail, text of message follows:\n\n");
		mmsg("------------------------\n\n");
		rewind(tfp);
		mcopy(tfp, stdout, ftell(tfp), (fsize_t)MAXLONG, 0);
		mmsg("\n------------------------\n\n");
	}
		
	fclose(tfp);
}

xsend(users, tfp) char **users; FILE *tfp;
{
	register char **ulist;
	register char *cp;
	register struct passwd *pwp;
	FILE *xfp;

	for (ulist = users; *ulist!=NULL; ulist++) {
		rewind(tfp);
		sprintf(boxname, "%s%s", SPOOLDIR, *ulist);
		sprintf(cmdname, "xencode %s >> %s", *ulist, boxname);
		if (index(*ulist, '!') != NULL
		 || (pwp = getpwnam(*ulist)) == NULL) {
			mmsg(nosend, *ulist);
			continue;
		}
		if (xaccess(*ulist) == 0) {
			mmsg(nopubk, *ulist);
			continue;
		}
		mlock(pwp->pw_uid);
		if ((xfp = fopen(boxname, "a")) == NULL) {
			mmsg(nosend, *ulist);
			munlock();
			continue;
		}
		chown(boxname, pwp->pw_uid, pwp->pw_gid);
		fprintf(xfp, "From xmail %s %s\n", cp,
			tzname[tp->tm_isdst ? 1 : 0]);
		fclose(xfp);
		if ((xfp = popen(cmdname, "w")) == NULL) {
			mmsg("Can't pipe to xencode\n");
			continue;
		}
		if (fwrite(header, strlen(header), 1, xfp) != 1
		 || mcopy(tfp, xfp, (fsize_t)0, (fsize_t)MAXLONG, 0)) {
			merr(wrerr, cmdname);
		}
		pclose(xfp);
		munlock();
	}
	fclose(tfp);
}

/*
 * If the `-m' option is specified, advise
 * the recipient of the presence of mail.
 */
advise(recipient)
char *recipient;
{
	register FILE *fp;
	register FILE *tfp;
	struct utmp ut;
	char tty[30];
	struct stat sb;

	if (!mflag)
		return;
	if ((fp = fopen("/etc/utmp", "r")) == NULL)
		return;
	while (fread(&ut, sizeof ut, 1, fp) == 1)
		if (strncmp(ut.ut_name, recipient, DIRSIZ) == 0) {
			sprintf(tty, "/dev/%s", ut.ut_line);
			if (stat(tty, &sb)<0 || (sb.st_mode&S_IEXEC)==0)
				continue;
			if ((tfp = fopen(tty, "w")) != NULL) {
				fprintf(tfp, "\7%s: you have mail.\n", myname);
				fclose(tfp);
			}
		}
	fclose(fp);
}
