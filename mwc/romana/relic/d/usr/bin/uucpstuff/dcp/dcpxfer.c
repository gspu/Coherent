/*
 * dcpxfer.c 
 *
 * Revised edition of dcp 
 *
 * Stuart Lynne May/87 
 *
 * Copyright (c) Richard H. Lamb 1985, 1986, 1987 Changes Copyright (c) Stuart
 * Lynne 1987 
 *
 * "DCP" a uucp clone. Copyright Richard H. Lamb 1985,1986,1987
 * file send routines
 */

#include "dcp.h"
#include <signal.h>
#include <ctype.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <access.h>
#include "perm.h" 

extern int (*getpkt)(), (*sendpkt)(), (*openpk)(), (*closepk)();

extern	int errno;
extern	char *sys_errlist[];
extern	char	*strtok();
extern	char	*rebuildfn();
static	char	tempname[] =	"/usr/spool/uucp/TM.XXXXXX";
static	unsigned char rpacket[MAXPACK];
static	unsigned char spacket[MAXPACK];
char	reason [80];
static	char	S_record [BUFSIZ];
static	int	lstat;
extern	int	canwrite();
#define	BIGBUFSZ (BUFSIZ * 20)
static	char	bigreadbuf [BIGBUFSZ];

struct timeb	transferstart;	/* transfer start time */
struct timeb	transferstop;	/* transfer stop time */

/*
 * s d a t a 
 *
 * Send File Data 
 */
sdata()
{
	for (;;) {
		if ((size = read(fpfd, spacket, pktsize)) <= 0) /* Get data */
			return 'Z';		/* If EOF set state to that */
		bytecount += size;
		if ((*sendpkt) (spacket, size, 0))
			return cantsend();		/* send data */
	}
}

/*
 * s b r e a k 
 *
 */
sbreak()
{
	if (sendmsg("H") < 0) 
		return cantsend();
	if (getmsg(spacket) < 0) {
		plog(M_CONVERSE, "Cannot get reply to H (sbreak)");
		terminatelevel++;
		return 'Y';	/* was return 0 */
	}
	printmsg(M_CONVERSE, "Switch modes: %s", spacket);
	if (spacket[1] == 'N') 
		return 'G';
	return 'Y';
}

/*
 * s e o f 
 *
 * Send End-Of-File. 
 */
seof()
{
	double	elapsed;

	close(fpfd);
	fpfd = -1;
	printmsg(M_TRANSFER, "seof");
	if ((*sendpkt) (spacket, 0, 0) < 0)
		return cantsend();	/* length zero packet indicates EOF */
	if (getmsg(spacket) < 0)
		return cantread();	/* was plog; termlev++, no ret*/
	printmsg(M_TRANSFER, "seof: spacket is %s.", spacket);
	if (strncmp(spacket, "CY", 2) != 0) {
		sprintf(reason, "Remote would not accept file %s", fromfilep);
		plog(M_TRANSFER, reason);
		notifystatus(fromfilep, tofilep, 1, 0, lstat);
		return 'F';
	}
	if (lstat && (index(optionp, 'c') == NULL)) {
		unlink(xfromfile);
		/* plog("unlinking %s", xfromfile);	 */
		printmsg(M_TRANSFER, "unlinking %s.", xfromfile);
	}
	printmsg(M_TRANSFER, "seof: Transfer of %s completed.",
		fromfilep);
	ftime(&transferstop);
	printmsg(M_TRANSFER, "Stop time is  %ld.%03d\n",
		transferstop.time, transferstop.millitm);
		elapsed =
			(double)transferstop.time - (double)transferstart.time;
		elapsed += 
			((double)transferstop.millitm -
			(double)transferstart.millitm) / 1000.0;
	if (total_errors > 0) 
		plog(M_TRANSFER,
		     "S (OK) %ld bytes, %5.2f seconds, %5.1f Bps, %d error%c", 
			bytecount, elapsed, (double) bytecount / elapsed,
			total_errors, total_errors == 1 ? ' ' : 's'); 
	else
		plog(M_TRANSFER, "S (OK) %ld bytes, %5.2f seconds, %5.1f Bps", 
			bytecount, elapsed, (double) bytecount / elapsed); 
	notifystatus(fromfilep, tofilep, 1, 1, lstat);
	return 'F';		/* go get the next file to send */
}

/*
 * s f i l e 
 *
 * Send File Header. 
 * This needs to respond to:
 *	1) commands from a C. file
 *	2) requests from remote.
 */
sfile()
{
	printmsg(M_SPOOL, "Sending %s as %s,\n\tspool %s",
		fromfilep, tofilep, xfromfile);
	plog(M_SPOOL, "S (%s)->(%s)", fromfilep, tofilep);
	bytecount = 0;
	total_errors = 0;
	ftime(&transferstart);
	printmsg(M_TRANSFER, "Start time is %ld.%03d\n",
		transferstart.time, transferstart.millitm);
	return 'D';
}

/*
 * s i n i t 
 *
 * Send Initiate: send this host's parameters and get other side's back. 
 */
sinit()
{
	if ((*openpk) ())
		return 'A';
	return ('B');
}

/*
 * cdotcmd
 * read commands from C. file, and dispatch accordingly.
 * This routine and the corresponding routine rmtcmd need to set up
 * the following global variables.
 *	fromfilep	pointer to file name that is source.
 *	tofilep		pointer to file name that is destination.
 *	xfromfile	file name that is actual source,  eg, spool file.
 *	optionp		pointer to option string.
 *	modep		pointer to octal mode string.
 *	notifyp		pointer to user to be notified
 * These control the action of sendf, sfile and seof.
 *
 */
cdotcmd()
{
	char	*cp;

	lstat	= 1;
	printmsg(M_SPOOL, "cdotcmd enter");
	if (fpfd != -1) {		/* If not already open, */
		plog(M_SPOOL, "File already open");
		terminatelevel++;
		return 'A';	/* If something's already open, we' trouble */
	}
	printmsg(M_SPOOL, "looking for next file...");
	if (getcline()) {	/* get next file from current work */
		fclose(cfp);
		printmsg(M_SPOOL, "unlinking %s", cfile);
		unlink(cfile); /* close and delete completed workfile */
		cfp = NULL;
		return 'B';	/* end controlling session */
	}
	if (*clinep [1] == 'R') {	/* ask for file */
		int	permerr = 0;
		printmsg(M_SPOOL, "getcline gave us an 'R' rec");
		if ((cp = rebuildfn(tofilep)) == NULL) {
			sprintf(reason, "No such user as referenced in \"%s\"",
				tofilep);
			permerr = 1;
		} else
			strcpy(xtofile, cp);
		if (!permerr && !perm_write(xtofile)) {
			sprintf(reason, "No permission to write file \"%s\"",
				xtofile);
			permerr = 1;
		}
		if (!permerr && !canwrite(xtofile)) {
			sprintf(reason, "Unable to create \"%s\"; %s",
				xtofile, sys_errlist[errno]);
			permerr = 1;
		}
		if (permerr) {
			plog(M_SPOOL, reason);
			notifystatus(fromfilep, xtofile, 0, 0, lstat);
			return 'F';	/* Give up if can't */
		}			
		if (sendmsg(S_record) < 0)	/* really S&R_record */
			return cantsend();
		if (getmsg(spacket) < 0)
			return cantread();
		printmsg(M_SPOOL, "spacket is %s", spacket);
		if ((spacket [0] != 'R') || (spacket [1] != 'Y')) {
			sprintf(reason, "Remote won't send file \"%s\".",
				fromfilep);
			plog(M_SPOOL, reason);
			notifystatus(fromfilep, xtofile, 0, 0, lstat);
			return 'F';	/* was return 'A' */
		}
		return rfile();
	}
	if (strcmp(clinep [1], "S") == 0) {
		int	noperm;
		noperm = 0;
		printmsg(M_SPOOL, "Opening \"%s\" for sending.", xfromfile);
		if ((strcmp(perm_value(sendfiles_e), "call") == 0) ||
			!perm_read(xfromfile)) {
			if (role != MASTER) {
				sprintf(reason, 
				"No permission to send as remote \"%s\"",
					xfromfile);
				noperm = 1;
			}
		} else if ((strcmp(perm_value(sendfiles_e), "yes") != 0) ||
			!perm_read(xfromfile)) {
			sprintf(reason,"No permission to send file\"%s\"",
				xfromfile);
			noperm = 1;
		}
		if (noperm) {
			plog(M_TRANSFER, reason);
			notifystatus(xfromfile, xtofile, 0, 0, lstat);
			return 'F';
		}
		fpfd = open(xfromfile, 0);	/* open the file to be sent */
		if (fpfd == -1) { 
			sprintf(reason, "Unable to read \"%s\"; %s",
				xfromfile, sys_errlist[errno]);
			plog(M_TRANSFER, reason);
			notifystatus(xfromfile, xtofile, 0, 0, lstat);
			return 'F';
		}
		/* send 'S fromfile tofile user - tofile <perms>'. */
		if (sendmsg(S_record) < 0)
			return cantsend();	/* was return 0 */
		if (getmsg(spacket) < 0)
			return cantread();	/* was return 0 */
		printmsg(M_SPOOL, "spacket is %s", spacket);
		if (spacket[1] != 'Y') {
			sprintf(reason, "Remote won't accept file \"%s\"",
				xfromfile);
			plog(M_TRANSFER, reason);
			notifystatus(xfromfile, xtofile, 1, 0, lstat);
			close(fpfd);
			fpfd = -1;
			return 'F';
		}
		return sfile();
	}
	plog(M_SPOOL, "unknown record in \"%s\" of \"%s\"", cfile, clinep[1]);
	return 'Y';
}

/*
 *	getcline()
 *	get one line from the C. file and split it apart.
 */
getcline()
{
	char	*p;
	static char line[BUFSIZ];
	if (fgets(line, BUFSIZ, cfp) == (char *) NULL)
		return 1;
	if ((p = index(line, '\n')) != NULL)
		*p = '\0';
	strcpy(cline, line);
	return sepcline();

}

sepcline()
{
	char	*sp;
	int	i;

	printmsg(M_SPOOL, "sepcline: line is %s", cline);
	strcpy(S_record, cline);
	sp = cline;
	for (i=0; i<10; i++)
		clinep[i] = NULL;
	for (i=1; i<10; i++) {
		if ( (clinep[i]=strtok(sp, " \t\n")) == NULL ){
			break;
		}
		printmsg(M_SPOOL, "cline[%d]:\t%\"%s\"", i, clinep[i]);
		sp = NULL;
	}

	printmsg(M_SPOOL,"PERMISSIONS WATCH: cline[%d] is %o",i, clinep[i]);

	/* If the last field of a parsed command line is NOT null, then
	 * there is something wrong with the line parsed. Leave a  message
	 * to this affect in the logs, and print the offending line there
	 * as well. Abort processing this file.

	 * It has not yet been determined what affects this will have.
	 * It may keep other requests from the site which is expecting to 
	 * receive files from receving all of the files it expects.

	 * the following is a short description of how this was called:

		sendf() -> calls cdotcmd() until 'complete'	(dcp.c)
		cdotcmd() -> calls getcline() until a non-zero
			     value is returned. When a non zero
			     value is returned, the C. file being
			     read is closed and deleted. A 'B' is
			     then returned back to sendf()
		getcline() -> calls sepcline (breaks out the C. fields)

	 * Bob Hemedinger 01/27/92
	 */

	/* 01/30/92: removed the return statement. We WANT to continue
	 * if we don't see the NULL terminator for compatibility with
	 * other variants.
	 */

	if (clinep[9] != NULL) {
		plog(M_SPOOL, "Error parsing command 'C.' file");
		plog(M_SPOOL, "last sepcline field not null");
		plog(M_SPOOL, "Actually parsed: ");
		plog(M_SPOOL, " %s %s %s %s %s %s %s %s %s",
				clinep[1], clinep[2], clinep[3],
				clinep[4], clinep[5], clinep[6],
				clinep[7], clinep[8], clinep[9]);
	}
	nclinep = i;
	fromfilep = clinep[2];
	tofilep = clinep[3];
	usernamep = clinep[4];
	optionp = clinep[5];
	spoolfilep = clinep[6];
	modep = clinep[7];
	notifyp = clinep[8];

	if (strcmp(clinep[1], "S") == 0) {
		sprintf(xfromfile, "%s/%s/%s", SPOOLDIR, rmtname, spoolfilep);
		if (index(optionp, 'c') != NULL)	/* this looks weak */
			strcpy(xfromfile, fromfilep);
	} else if (strcmp(clinep[1], "R") == 0) {
		strcpy(xfromfile, fromfilep);
	} else if (*clinep[1] == 'H') {
		;
	} else {
		plog(M_SPOOL, "Unrecog record type %s", cline);
		printmsg(M_SPOOL, "Unrecog type %s %s %s", cline[1],
			cline[2], cline[3]);
		return 1;
	}
	return 0;
}

/*********************** MISC SUB SUB PROTOCOL *************************/

/*
 *
 * schkdir
 * scan the dir 
 */
schkdir()
{
	char c;

	c = scandir();

	if (c == 'Q') {
		return ('Y');
	}
	if (c == 'S') {
		sprintf(rpacket, "HN");
		if ((*sendpkt) (rpacket, 0, 1))
			return cantsend();
	}
	return ('B');
}

/* 
 * endp() end protocol 
 *
 */
endp()
{

	(void) sendmsg("HY");
	(*closepk) ();
	return 'P';
}



/***********************RECEIVE PROTOCOL**********************/

/*
 * r d a t a 
 *
 * Receive Data 
 */
rdata()
{
	int	len;

	if ((*getpkt) (rpacket, &len))
		return cantread();
	if (len == 0) 
		return reof();
	if (write(fpfd, rpacket, len) != len)	/* Write the data to the file */
		return 'Y';
	bytecount += len;
	return 'J';			/* Remain in data state */
				/* changed to J */
}

/*
 *	reof
 *	handle eof on read.
 *	try to link the temp file to the new name;
 *	if not successful, then copy it.
 */
reof()
{
	int	tfpfd;
	int	noperm;
	double	elapsed;
	int	mode;
	int	bytes;

	noperm = 0;
	tfpfd = -1;
	close(fpfd);
	fpfd = -1;
	unlink(xtofile);
	if (link(tempname, xtofile) == -1) {
		if ((tfpfd = open(tempname, 0)) == -1) {
			sprintf(reason, "Unable to reread \"%s\"; %s",
				tempname, sys_errlist[errno]);
			noperm = 1;
		} else if ((fpfd = creat(xtofile, 0644)) == -1) {
			sprintf(reason, "Unable to create \"%s\"; %s (rdata)",
				xtofile, sys_errlist[errno]);
			noperm = 1;
		} 
		if (noperm) {
			plog(M_TRANSFER, reason);
			notifystatus(fromfilep, tempname, 0, 0, lstat);
			if (sendmsg("CN") < 0) {
				plog(M_SPOOL, "Message CN refused");
				return cantsend();
			}
			if (tfpfd != -1)
				close(tfpfd);
			if (fpfd != -1)
				close(fpfd);
			return 'F';
		}
		while ((bytes = read(tfpfd, bigreadbuf, BIGBUFSZ)) > 0)
			write(fpfd, bigreadbuf, bytes);
		close(tfpfd);
		close(fpfd);
		fpfd = -1;
	}
	unlink(tempname);

/* this problem showed up in 4.0. When we request a file, nothing specifies
   the file permissions. This little ditty will take care of the problem.
   Now we check the value of nclinep, as it holds the place where we
   exitted the loop which parses the command received. If we broke out at 7,
   which is where the file permissions are stored, then we end up with garbage
   permissions in modep (which is clinep[7]). For these cases, we will default
   to permissions of 0644 and pray that the customers can live with it. 

   Bob H. 08/26/92 */

	if ((strlen(modep) > 0) && (mode = getoct(modep)) != 0 &&
		(chmod(xtofile, (nclinep == 7 ? 0644:getoct(modep)) ) == -1)) {
		printmsg(M_TRANSFER, "Unable to change permission");
		plog(M_TRANSFER, 
			"Unable to change permission to \"%s\" on file \"%s\"",
			modep, xtofile);
	}
	/* now, do the copy. rewind the above file and reread it */
	/* we ought to do the copy in a temp file, and move it here */
	printmsg(M_TRANSFER, "transfer complete");
	ftime(&transferstop);
	printmsg(M_TRANSFER, "Stop time is  %ld.%03d\n",
		transferstop.time, transferstop.millitm);
	elapsed =
		(double)transferstop.time -
			(double)transferstart.time;
	elapsed += 
		((double)transferstop.millitm -
		(double)transferstart.millitm) / 1000.0;
	if (total_errors > 0)
		plog(M_TRANSFER, 
		     "R (OK) %ld bytes, %5.2f seconds, %5.1f Bps, %d error%c", 
			bytecount, (double)elapsed, bytecount / elapsed,
			total_errors, total_errors == 1 ? ' ' : 's'); 
	else
		plog(M_TRANSFER, "R (OK) %ld bytes, %5.2f seconds, %5.1f Bps", 
			bytecount, (double)elapsed, bytecount / elapsed); 
	notifystatus(fromfilep, xtofile, 0, 1, lstat);
	printmsg(M_TRANSFER, "returning from rdata");
	if (sendmsg("CY") < 0) {
		strcpy(reason, "Message CY refused.");
		plog(M_TRANSFER, reason);
		return cantsend();
	} 
	return 'F';
}

/*
 *	Analog of cdotcmd.  This takes 'S' records or 'R' records
 *	from the other end and dispatches them properly.
 */
rmtcmd()
{
	static	char	buf[256];
	char	*cp;
	int	noperm;

	lstat = 0;
	if (getmsg(buf) < 0) {
		printmsg(M_LOWPROTO, "in rmtcmd, getmsg says -1");
		return cantread();	/* was return 0 */
	}
	printmsg(M_TRANSFER, "buf[0] is %c", buf[0]);
	strcpy(cline, buf);
	sepcline();
	if (*clinep [1] == 'H') {	/* used to be buf [0] */
		printmsg(M_TRANSFER, "rmtcmd got H, returning C");
		return 'C';
	}
	printmsg(M_TRANSFER, "rmtcmd: buf %d \"%s\"", strlen(S_record),
		S_record);
	if (strcmp (clinep [1], "R") == 0) {
		printmsg(M_INFO, "rmdcmd: send file R");
		printmsg(M_SPOOL, "Opening %s for sending.", xfromfile);
		if ((strcmp(perm_value(request_e), "yes") != 0) ||
			!perm_read(xfromfile)) {
			sprintf(reason, "Request permission denied: \"%s\"",
				xfromfile);
			plog(M_TRANSFER, reason);
			sendmsg("RN");
			notifystatus(fromfilep, xtofile, 0, 0, lstat);
			return 'F';
		} else {
			fpfd = open(xfromfile, 0);
				/* open the file to be sent */
			if (fpfd == -1) { /* If bad file pointer, give up */
				sprintf(reason,
					"Cannot open file \"%s\".", xfromfile);
				plog(M_TRANSFER, reason);
				sendmsg("RN");
				notifystatus(fromfilep, xtofile, 0, 0, lstat);
				return  'F';	/* was return 'A' */
			}
			sendmsg("RY");
			return sfile();
		}
	}
	printmsg(M_INFO, "rmtcmd: receive file \"%s\"", tofilep);
	if ((cp = index(optionp, 'c')) != NULL) 	/* oops, not allowd*/
 		*cp = ' ';
	noperm = 0;
	if ((cp = rebuildfn(tofilep)) == NULL) {
		noperm = 1;
		sprintf(reason, "No such user as referenced in \"%s\"",
			tofilep);
	} else
		strcpy(xtofile, rebuildfn(tofilep));
	if (!noperm && !perm_write(xtofile)) {
		sprintf(reason, "No permission to write file \"%s\"", xtofile);
		noperm = 1;
	}
	if (!noperm && !canwrite(xtofile)) {
		sprintf(reason, "rmtcmd: Unable to create file \"%s\": %s",
	 		xtofile, sys_errlist[errno]);
		noperm = 1;
	}
	if (noperm) {
		plog(M_TRANSFER, reason);
		notifystatus(fromfilep, xtofile, 0, 0, lstat);
		if (sendmsg("SN") < 0) 
			return cantsend();
		return 'F';
	}
	if (sendmsg("SY") < 0) {
		plog(M_TRANSFER, "Can't send SY");
		return 'Y';
	}
	return rfile();
}

/*
 * r f i l e 
 *
 * Receive File Header 
 */
rfile()
{
	printmsg(M_TRANSFER, "Receiving %s as %s", fromfilep, xtofile);
	/* create temp file name, open it for output */
	if ((fpfd = creat(mktemp(tempname), 0644)) == -1) {
		sprintf(reason, "Unable to create \"%s\"; %s",
			tempname, sys_errlist[errno]);
		plog(M_TRANSFER, reason);
		notifystatus(fromfilep, xtofile, 0, 0, lstat);
		return 'F';	/* Give up if can't */
	}
	plog(M_TRANSFER, "R (%s)<-(%s)", xtofile, fromfilep);
	bytecount = 0;
	total_errors = 0;
	ftime(&transferstart);
	printmsg(M_TRANSFER, "Start time is %ld.%03d\n",
		transferstart.time, transferstart.millitm);
	return 'J';		/* Switch to data state */
}

/*
 * r i n i t 
 *
 * Receive Initialization 
 */
rinit()
{
	if ((*openpk) ()) {
		plog(M_TRANSFER, "Unable to get opening packet");
		return 'Y';
	}
	return ('F');
}

/*
 * |getmsg()| recieves a null-terminated "conversation-level" message
 * from the communications channel. This may require one or more packets,
 * but all of them will be "long-data" packets containing a full 64 bytes.
 */
int getmsg(dest)
char *dest;
{
	int len;

	while(1) {
		if ((*getpkt)(dest, &len) < 0)
			return(-1);
		*(dest + len) = '\0';		/* make sure it's terminated */
		if (strlen(dest) != len)
			break;			/* we reached the terminator */
		dest += len;
	}
	return( 0 );
}

/*
 * |sendmsg(message)| sends a null-terminated "conversation-level" message.
 */
int sendmsg(message)
char *message;
{
	int len;
	len = strlen(message) + 1;	/* total length including '\0' */

	while(1) {
		if ((*sendpkt)(message, 0, 1) < 0) /* send with padding */
			return -1;
		if ((len -= pktsize) <= 0)
			break;
		message += pktsize;
	}
	return 0;
}

