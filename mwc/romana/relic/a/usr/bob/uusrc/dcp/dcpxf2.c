/*
 * $Header: /newbits/usr/lib/uucp/dcp/RCS/dcpxf2.c,v 1.4 91/08/15 13:24:56 bin Exp Locker: bin $
 * $Log:	dcpxf2.c,v $
 * Revision 1.4  91/08/15  13:24:56  bin
 * changes by epstein for 7bit sites
 * 
 * Revision 1.6	90/03/29  10:54:13 	wgl
 * Add check of the -d option and fail to create directory if option absent.
 * 
 * Revision 1.5	90/03/26  13:45:37 	wgl
 * Add routine 'rebuildfn' to rebuild file name as needed on receive.
 * 
 * Revision 1.4	90/03/23  09:30:39 	wgl
 * Add routines for cheking read and write permissions.
 * 
 * Revision 1.3	90/03/22  12:11:08 	wgl
 * Add 'canwrite'.
 * 
 * Revision 1.2	90/03/22  08:59:23 	wgl
 * Fixed error in from/to file reporting in status mail message.
 * 
 */

#include "dcp.h"
#include <signal.h>
#include <ctype.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <access.h>
#include <pwd.h>
#include "perm.h" 

static	char	uuxbuf [64];
static	char	allowed [80];
static	char	disallowed[80];
static	char	*successmsg [2] = {
	"FAILED",
	"Successfully"
};

static char	tmpfilename[BUFSIZ];	/* Holds the converted file name */
extern	char	reason[];
extern	char	*strtok();

notifystatus(fromfilep, xtofile, sending, success, cdotctl)
char	*fromfilep, *xtofile;
int	success, cdotctl, sending;
{
	if (cdotctl) {
		if (index(optionp, 'm') != NULL) 
			l3statmail(fromfilep, xtofile, sending, success);
	} else {
		if (index(optionp, 'n') != NULL)
			notify2mail(fromfilep, xtofile, success);
	}
}

l3statmail(from, to, sending, success)
char	*from;
char	*to;
int	sending;
int	success;
{
	FILE	*fmp;

	(void) signal(SIGPIPE, SIG_IGN);
	sprintf(uuxbuf, "mail -auucp %s", usernamep);
	plog(M_TRANSFER, "Posting local status mail to user \"%s\"", usernamep);
	if ((fmp = popen(uuxbuf, "w")) == NULL)
		plog(M_TRANSFER, "Cannot send status mail");
	else {
		static char	*succp;
		fprintf(fmp, "From: UUCP V%s\n", version);
		fprintf(fmp, "Subject: UUCP file transfer status\n\n");
		succp = successmsg [success];
		if (sending) {
			fprintf (fmp, "Sent file:\t%s\n", from);
			fprintf (fmp, "       to:\t%s!%s\n%s\n",
				rmtname, to, succp);
		} else {
			fprintf (fmp,"Received file:\t%s\n", to);
			fprintf (fmp, "        from:\t%s!%s\n%s\n",
				rmtname, from, succp);
		}
		if (!success)
			fprintf(fmp, "Reason: %s\n", reason);
		if (pclose(fmp) != 0)
			plog(M_TRANSFER, "Status mail failed.");
	}
}

notify2mail(from, to, success)
char	*from;
char	*to;
int	success;
{
	FILE	*fmp;
	int	status;

	(void) signal(SIGPIPE, SIG_IGN);
	sprintf(uuxbuf, "mail -auucp %s", notifyp);
	plog(M_TRANSFER, "Notifying user \"%s\".", notifyp);
	if ((fmp = popen(uuxbuf, "w")) == NULL)
		plog(M_TRANSFER, "Cannot send notify mail (popen)");
	else {
		fprintf(fmp, "From: UUCP V%s\n", version);
		fprintf(fmp, "\nThe file:\t%s\nReceived from:\t%s!%s\n",
			to, rmtname, from);
		fprintf(fmp, "%s\n", successmsg [success]);
		if (!success)
			fprintf(fmp, "Reason: %s\n", reason);
		if ((status = pclose(fmp)) != 0) {
			plog(M_TRANSFER, 
				"Notify mail fails (pclose), code %d", status);
		}
	}
}

getoct(s)
char	*s;
{
	char	*cp;
	char	c;
	int	n;

	cp = s;
	n = 0;
	while ((c = *cp) != '\0') {
		n = (n * 8) + *cp++ -'0';
	}
	return n;
}
		
int
cantsend()
{
	plog(M_TRANSFER, "Cannot send packet");
	printmsg(M_TRANSFER, "Cannot send packet (cantsend)");
	terminatelevel++;
	return 'Y';
}

int
cantread()
{
	plog(M_TRANSFER, "Cannot read packet");
	printmsg(M_TRANSFER, "Cannot read packet (cantread)");
	terminatelevel++;
	return 'Y';
}

int
canwrite(fn)
char	*fn;
{
	static	dirname [120];
	struct	stat	statbuf;	
	char	*cp;

	strcpy(dirname, fn);
	if ((cp = rindex(dirname, '/')) != NULL) {
		*cp = '\0';
		if (index(optionp, 'd') != NULL) {
			if (!ckdir(dirname)) {
				plog(M_SPOOL, "unable to create directory %s", 
								dirname);
				return 0;
			}
		}
	}
	if (stat(fn, &statbuf) == -1)
		return 1;
	if (access(fn, 0) == 0)
		return 1;
	return 0;
}

perm_write(fn)
char	*fn;
{
	strcpy(allowed, perm_value(write_e));
	strcpy(disallowed, perm_value(nowrite_e));
	return allowed_or_not(fn);
}

perm_read(fn)
char	*fn;
{
	strcpy(allowed, perm_value(read_e));
	strcpy(disallowed, perm_value(noread_e));
	return allowed_or_not(fn);
}

static int
allowed_or_not(fn)
char	*fn;
{
	char	*sp, *cp;

	strcat(allowed, ":/usr/spool/uucp");
	sp = disallowed;
	while ((cp = strtok(sp, ":")) != NULL) {
		sp = NULL;
		if (strncmp(fn, cp, strlen(cp)) == 0)
			return 0;
	}
	sp = allowed;
	while ((cp = strtok(sp, ":")) != NULL) {
		sp = NULL;
		if (strncmp(fn, cp, strlen(cp)) == 0)
			return 1;
	}
	return 0;
}

/*
 *	rebuildfn
 *	rebuild the destination file name for a transfer, handling
 *	directories and so on.
 */
char	*
rebuildfn(destfile)
char	*destfile;
{
	struct	passwd	*pwp;
	struct	stat	statbuf;
	char	*cp;

	if (*destfile == '~') {
		if (*(destfile + 1) == '/')
			sprintf(tmpfilename, "%s/%s", PUBDIR, destfile + 2);
		else {
			static	char	user [20];
			strcpy(tmpfilename, destfile + 1);
			if ((cp = strtok(tmpfilename, "/\n")) == NULL)
				cp = tmpfilename;
			strcpy(user, tmpfilename);
			if ((pwp = getpwnam(user)) == NULL)
				return NULL;
			if ((cp = index(destfile, "/")) == NULL)
				cp = "";
			sprintf(tmpfilename, "%s/%s", pwp->pw_dir, cp);
		}
	} else if (*destfile == '/')
		sprintf(tmpfilename, "%s", destfile);
	else
		sprintf(tmpfilename, "%s/%s/%s", SPOOLDIR, rmtname, destfile);
	if ((cp = index(tmpfilename, '\n')) != NULL)
		*cp = '\0';
	if (((stat(tmpfilename, &statbuf) != -1) &&
		((statbuf.st_mode & S_IFDIR) != 0)) ||
		(tmpfilename[strlen(tmpfilename) - 1] == '/')) {
		if ((cp = rindex(fromfilep, '/')) == NULL)
			cp = fromfilep;
		else
			cp++;
		if (tmpfilename[strlen(tmpfilename) - 1] != '/')
			strcat(tmpfilename, "/");
		strcat(tmpfilename, cp);
	}
	return tmpfilename;
}
