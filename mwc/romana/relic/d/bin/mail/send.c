#include <stdio.h>
#include <sys/types.h>
#include <sys/mdata.h>
#include <time.h>
#include <pwd.h>
#include <utmp.h>
#include <sys/stat.h>
#include "mail.h"

#define SITENAMELEN	32	/* max length of sitename */
#define NODENAME	"/etc/uucpname"
#define DOMAINNAME	"/etc/domain"
#define	ALIAS	"/usr/lib/mail/aliases"
char	domain [64];

extern	char	*strtok();
FILE	*aliasfp;
char	aliasbuf [BUFSIZ];
char	*findalias();
static	**gusers;
char mysite[SITENAMELEN];	/* this host's uucpname */

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
extern	char	mydead[];	/* $HOME/dead.letter */
extern	char	mysig[];
extern	char	nosave[];
extern	int	callmermail;

static	int	eflag;		/* Edit this mail */
static	int	senderr;
struct	tm	*tp;
char	toerr[] = "Cannot create temporary file\n";
char	header[BUFSIZ];		/* Message header */
char	boxname[256];		/* Destination mailbox */
char	aftralias [BUFSIZ];		/* name after alias processing */
char	remotefrom [32];	/* "remote from uucp" */

char	nosend[] = "Can't send mail to '%s'\n";
char	nopubk[] = "Can't send xmail to '%s'\n";
static	int fromtty;

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

	uucpname();
	domainname();
	senderr = 0;
	temp = templ;
/*fprintf(stderr, "send2(fp, users, %ld, %ld, %d)\n", start, end, asksubj);*/
	fromtty = isatty(fileno(fp));
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

	eflag = 0;
	if (fromtty && asksubj && !callmermail) {
		fprintf(stdout, "Subject: ");
		fflush(stdout);
		if (fgets(msgline, NLINE, fp) != NULL) {
			if (strlen(msgline) > 1)
				fprintf(tfp, "Subject: %s", msgline);
		}
	}
	for (;;) {
		if (fgets(msgline, NLINE, fp) == NULL)
			break;
		if (fp == stdin)
			if ((strcmp(".\n", msgline)==0))
				break;
			else if ((strcmp("?\n", msgline)==0)) {
				eflag = 1;
				break;
			}
/*		fprintf(stderr, ":%s:", msgline);	*/
		fputs(msgline, tfp);
		end -= strlen(msgline);	/* compiler bug */		
		if (end <= 0) {
			break;
		}
	}
	if (!callmermail && (sigfp = fopen(mysig, "r")) != NULL) {
		fputs("\n", tfp);
		while (fgets(msgline, NLINE, sigfp) != NULL) {
			fputs(msgline, tfp);
		}
		fclose(sigfp);
	}
	/*
	 * If interrupted, bug out.
	 */
	if (intcheck()) {
		fclose(tfp);
		return;
	}
	/*
	 * Now, see if user wants to edit the message
	 */
	if (eflag) {
		xfp = tfp;
		temp = templ;
		if ((tfp = fopen(temp, "wr")) == NULL)
			merr(toerr);
		chown(temp, myuid, mygid);
		mcopy(xfp, tfp, (fsize_t)0, (fsize_t)MAXLONG, 0);
		fclose(xfp);
		sprintf(cmdname, "%s %s", editname, templ);
		system(cmdname);
		unlink(temp);
		temp = NULL;
	}
	/*
	 * Otherwise if empty message, bug out.
	 */
	else if (ftell(tfp) == 0) {
		fclose(tfp);
		return;
	}
	/*
	 * Now see if a copy list is requested.
	 */
	if (askcc)
		users = getcc(users);
	gusers = users;
	/*
	 * Now send the message.
	 */
	time(&curtime);
	tp = localtime(&curtime);
	if (callmexmail)
		xsend(users, tfp);
	else
		usend(users, tfp);
}

usend(users, tfp)
char **users;
FILE *tfp;
{
	FILE *xfp;
	char	*cp, *ap;
	char	**ulist;
	register struct passwd *pwp;

	for (ulist = users; *ulist != NULL; ulist++) {
		rewind(tfp);
		strcpy(aftralias, *ulist);
		if (index(*ulist, '!') == 0) {
			ap = findalias(*ulist);
			if (ap != NULL) {
				strcpy(aftralias, ap);
				if (verbflag && strcmp(aftralias, ap))
					fprintf(stderr,
					"name %s aliased to %s\n",
					*ulist, aftralias);
			}
		}
		if ((cp = index(aftralias, '!')) != NULL) {
			*cp++ = '\0';
			if (rsend(aftralias, cp, tfp) != 0) 
				senderr = 1;
			continue;
		}
		sprintf(boxname, "%s%s", SPOOLDIR, aftralias);
		if ((pwp = getpwnam(aftralias)) == NULL) {
			mmsg(nosend, aftralias);
			senderr = 1;
			continue;
		}
		mlock(pwp->pw_uid);
		if ((xfp = fopen(boxname, "a")) == NULL) {
			mmsg(nosend, aftralias);
			senderr = 1;
			munlock();
			continue;
		}
		chown(boxname, pwp->pw_uid, pwp->pw_gid);
		if (build_header(aftralias, NULL, tfp, xfp) != 1
		  || mcopy(tfp, xfp, ftell(tfp), (fsize_t)MAXLONG, 0)) {
			merr(wrerr, boxname);
			senderr = 1;
		} else
			fprintf(xfp, "\1\1\n");
		fflush(xfp);
		munlock();
		advise(aftralias);
	}
	if (senderr && fromtty && ! callmexmail) {
		if (maccess(mydead) < 0
		 || (xfp = fopen(mydead, "a")) == NULL
		 || mcopy(tfp, xfp, (fsize_t)0, (fsize_t)MAXLONG, 0))
			mmsg(nosave, mydead);
		else
			mmsg("Letter saved in %s\n", mydead);
		if (xfp != NULL) {
			chown(mydead, myuid, mygid);
			fclose(xfp);
		}
	}
	fclose(tfp);
}

rsend(system, user, tfp)
char	*user;
char	*system;
FILE	*tfp;
{
	FILE	*xfp;
	int	i;

	sprintf(cmdname, "uux -r - %s!rmail '(%s)'", system, user);
	if (verbflag)
		fprintf(stderr, "Queueing remote mail to %s!%s\n",
		system, user);
	if ((xfp = popen(cmdname, "w")) == NULL) {
		mmsg("Can't pipe to %s\n", cmdname);
		return 1;
	}
	if (build_header(user, mysite, tfp, xfp) != 1 
	  || mcopy(tfp, xfp, ftell(tfp), (fsize_t)MAXLONG, 0)) {
		merr(wrerr, cmdname);
		return 1;
	}
	if (i = (pclose(xfp) != 0)) {
		mmsg("uux has failed, status %d\n", i);
		return 1;
	}
	return 0;
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

uucpname()
{
	FILE *uufile;

	if (NULL == (uufile = fopen(NODENAME, "r"))) {
		strcpy(mysite, "<unknown>");
		return;
	}
	fgets(mysite, sizeof mysite, uufile);
	mysite[strlen(mysite) - 1] = '\0';	/* remove '\n' */
	fclose(uufile);
	return;
}

domainname()
{
	FILE *domfile;
	if((domfile = fopen(DOMAINNAME, "r")) == NULL) {
		strcpy(domain, ".UNKNOWN");
		return;
	}
	fgets(domain, sizeof domain, domfile);
	domain [strlen(domain) - 1] = '\0';
	fclose (domfile);
	return;
}

char	*
findalias(who)
{
	char	*name, *newname;
	static	whobuf [64];
	int	recurcount;
	int	hit;

	if (aliasfp == NULL) {
		if ((aliasfp = fopen(ALIAS, "r")) == NULL) {
			mmsg("Cannot open alias file\n");
			return who;
		}
	}
	recurcount = 0;
	strcpy(whobuf, who);
	for (; ; ) {
		fseek(aliasfp, 0L, 0);
		hit = 0;
		while (fgets(aliasbuf, BUFSIZ, aliasfp) != NULL) {
			name = strtok(aliasbuf," #:\t");
			newname = strtok(NULL, "#(), \t\n");
			if (strcmp(name, whobuf) == 0) {
				strcpy(whobuf, newname);
				if (recurcount++ > 4) {
					sprintf(whobuf,
					"Too many alias recursions for %s\n",
						who);
					mmsg(whobuf);
					return who;
				}
				hit = 1;
				break;
			}
		}
		if (hit == 0)
			return whobuf;
	}
	return whobuf;
}

char *
arpadate(tp)
struct	tm	*tp;
{
	static	char arpabuf [64];
	static	char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	sprintf(arpabuf, "%d %c%c%c %d %02d:%02d:%02d",
		tp->tm_mday,
		months[tp->tm_mon * 3],
		months[tp->tm_mon * 3 + 1],
		months[tp->tm_mon * 3 + 2],
		tp->tm_year,
		tp->tm_hour, tp->tm_min, tp->tm_sec);
	return arpabuf;
}

char *
msgid(tp)
struct	tm	*tp;
{
	static	char	msgidbuf [32];
	sprintf(msgidbuf, "%02d%02d%02d%02d%02d",
	tp->tm_year, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min);
	return msgidbuf;
}

build_header(user, site, tfp, xfp)
char	*user;
char	*site;
FILE	*tfp;
FILE	*xfp;
{
	char	*cp;
	char	**ulist;
	long	pos;
	int	processid;

	cp = asctime(tp);
	cp[strlen(cp)-1] = 0;
/*	if (!callmermail) {	*/
		sprintf(header, "From %s %s %s", myname, cp,
			tzname[tp->tm_isdst ? 1 : 0]);
		if (site != NULL) {
			strcat(header, " remote from ");
			strcat(header, site);
		}
		strcat(header, "\n");
/*	}	*/
	if (fwrite(header, strlen(header), 1, xfp) != 1)
		return 0;
	pos = ftell(tfp);
	/* scan to end of From_ lines */
	while (fgets(header, NLINE, tfp) != NULL) {
		if ((strncmp (header, "From ", 5) != 0) &&
			(strncmp (header, ">From ", 6) != 0)) {
			break;
		}
		if (fwrite(header, strlen(header), 1, xfp) != 1)
			return 0;
		pos = ftell(tfp);
	}
	fseek(tfp, pos, 0);
	processid = getpid();
	if (callmermail) {
		sprintf(header,
		"Received: by %s (mail v %s)\n\tid AA%d; %s\n",
			mysite, revnop(), processid, arpadate(tp));
		if (fwrite(header, strlen(header), 1, xfp) != 1)
			return 0;
	} else {
		sprintf(header, "Message-Id: <%s.AA%d.V%s.%s@%s>\n",
			msgid(tp), processid, revnop(), mysite, domain);
		if (fwrite(header, strlen(header), 1, xfp) != 1)
			return 0;
		sprintf(header, "Date: %s\nFrom: %s!%s (%s)\n",
			arpadate(tp), mysite, myname, myfullname);
		if (fwrite(header, strlen(header), 1, xfp) != 1)
			return 0;
		sprintf(header, "To:   %s\n", user);
		if (fwrite(header, strlen(header), 1, xfp) != 1)
			return 0;
		if (gusers[0] != NULL && gusers[1] != NULL) {
			strcpy(header, "cc:");
			for (ulist = gusers; *ulist != NULL; ulist++) {
				strcat(header, " ");
				strcat(header, *ulist);
			}
			strcat(header, "\n");
			if (fwrite(header, strlen(header), 1, xfp) != 1)
				return 0;
		}
	}
	return 1;
}

