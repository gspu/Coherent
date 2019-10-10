/*
 * dcpsys.c
 *
 * System File Parsing and Initial Login Negotiation Routines for uucico
 */

/* #include <sys/dir.h> */
#include "dirent.h"
#include <ctype.h>
#include "dcp.h"
#include "lsys.h"

/*
 *  Communication Protocol Declarations and Definitions
 */

#define PROTOS  "g"
int (*getpkt)(), (*sendpkt)(), (*openpk)(), (*closepk)();

extern int ggetpkt(), gsendpkt(), gopenpk(), gclosepk();
#if 0
extern int kgetpkt(), ksendpkt(), kopenpk(), kclosepk();
extern int rgetpkt(), rsendpkt(), ropenpk(), rclosepk();
extern int tgetpkt(), tsendpkt(), topenpk(), tclosepk();
#endif

typedef struct proto {
	char type;		/* One character Protocol Type	*/
	int (*getpkt)();	/* Get Packet Function		*/
	int (*sendpkt)();	/* Send Packet Function		*/
	int (*openpk)();	/* Open Packet Function		*/
	int (*closepk)();	/* Close Packet Function	*/
} PROTO;

PROTO protolst[] =
{
	{ 'g', ggetpkt, gsendpkt, gopenpk, gclosepk },
#if 0
	{ 'k', kgetpkt, ksendpkt, kopenpk, kclosepk },
	{ 'r', rgetpkt, rsendpkt, ropenpk, rclosepk },
	{ 't', tgetpkt, tsendpkt, topenpk, tclosepk },
#endif
	{ '\0' }
};

int	leavelock = 0;		/* Leave the rmtname lock file	*/

/*
 * getsystem()
 * Retrieve and Process an "L.sys" file entry.
 */

getsystem()
{
	if ( lsys_next() == 0 )
		return('A');

	rmtname = lsys_value(sys_e);
	if ( strcmp(rmtname, sysname) ) {
		if ( strcmp(sysname, "all") && strcmp(sysname, "any") )
			return('I');
		if ( scandir() != 'S' )
			return('I');
	}
	printmsg(M_CALL, "Thinking of calling system: %s", rmtname);
	if ( !checktime(lsys_value(sched_e)) )
		return('I');

	open_the_logfile("uucico");
	if (cfp != NULL)
		fclose(cfp);	/* in case matched scandir */
	if ( lockit(rmtname) < 0 ) {
		plog(M_CALL, "Site Locked: %s", rmtname);
		close_logfile();
		return('I');
	} 
	return('S');
}

/*
 * checkrmt()
 * Scan through L.sys file to verify that we know about the given
 * remote system name.
 * Returns: (1) if we found match, (0) if rmtname unknown.
 */

checkrmt()
{
	lsys_open();
	while ( lsys_next() ) {
		if ( strcmp(rmtname, lsys_value(sys_e)) == 0 )
			return(1);
	}
	lsys_close();
	return(0);
}

/*
 *  rmsg(msg, maxlen)  char *msg;  int maxlen;
 *  Read a ^P (DLE) type msg from the remote uucp.  (startup negotiation).
 *
 *  wmsg(msg)  char *msg;
 *  Write a ^P (DLE) type msg to the remote uucp.  (startup negotiation).
 */

#define	MAXDROP	2048	/* Maximum characters to drop waiting for ^P	*/

rmsg(msg, maxlen)
char *msg;
int maxlen;
{
	char ch;
	register char *cp;
	int i;

	for (i=0; i<MAXDROP; i++) {
		if ( sread(&ch, 1, MSGTIME) != 1 )
			goto badrmsg;
		if ( ch == DLE )
			break;
	}
	if ( ch != DLE )
		goto nodle;

	for (cp=msg,i=1; i<maxlen; i++) {
		if ( sread(&ch, 1, MSGTIME) != 1 )
			goto badrmsg;
		if ( (ch == '\0') || (ch == '\r') || (ch == '\n') )
			break;
		*cp++ = ch;
	}
	*cp = '\0';
	printmsg(M_DEBUG, "rmsg {%s}", visib(msg));
	return( strlen(msg) );

badrmsg:
	*cp = '\0';
	printmsg(M_CALL, "Bad received message {%s}", visib(msg));
	return(-1);
nodle:
	*cp = '\0';
	printmsg(M_CALL, "Remote machine not sending DLE startup (uucico)");
	return(-1);
}

wmsg(msg)
char *msg;
{
	char buf[2] = {'\0', DLE};

	printmsg(M_DEBUG, "wmsg {%s}", visib(msg));
	swrite(&buf[0], 2);
	swrite(msg, strlen(msg));
	swrite(&buf[0], 1);
}

/*
 * startup()
 *
 * Initiate communication with a remote uucp.
 */

#define	FAIL1(x)	{errmsg=(x); errarg=NULL; goto failure;}
#define	FAIL2(x, y)	{errmsg=(x); errarg=(y); goto failure;}

startup()
{
	char msg[BUFSIZ];
	char *errmsg, *errarg, *cp, ch;
	static char locbuf[SITELEN+1];
	static char *readymsg = "Ready for transactions";

	sysended = 0;
	leavelock = 0;
	if (role == MASTER) {
		if ( rmsg(msg, BUFSIZ) < 0 )
			FAIL1("Logon failed: 1st msg (broken communication?)");
		printmsg(M_CALLMSG, "1st msg = {%s}", visib(msg));

		if ( strncmp(msg, "Shere", 5) )
			FAIL1("Bad format for 1st msg");
		if ( (msg[5] == '\0') || (msg[5] == '\n') ||
		     (msg[6] == '\0') || (msg[6] == '\n') )
			plog(M_CALL, "Warning: Null sitename contacted");
		else if ( strncmp(&msg[6], rmtname, SITESIG) )
			FAIL2("Incorrect sitename contacted: %s", &msg[6]);

		sprintf(msg, "S%.*s -Q0 -x%d", SITESIG, nodename, debuglevel);
		printmsg(M_CALLMSG, "Reply to 1st msg = {%s}", msg);
		wmsg(msg);

		if ( rmsg(msg, BUFSIZ) < 0 )
			FAIL1("Logon failed: 2nd msg (remote site stale LCK?)");

		printmsg(M_CALLMSG, "2nd msg = {%s}", visib(msg));
		if ( strncmp(msg, "ROK", 2) )
			FAIL1("Bad format for 2nd msg");

		if ( rmsg(msg, BUFSIZ) < 0 )
			FAIL1("Logon failed: 3rd msg (broken communication?)");
		printmsg(M_CALLMSG, "3rd msg = {%s}", visib(msg));
		if ( *msg != 'P' )
			FAIL1("Bad format for 3rd msg");
		ch = 'N';
		for (cp=PROTOS; *cp; cp++)
			if ( index(&msg[1], *cp) != NULL ) {
				ch = *cp;
				break;
			}
		sprintf(msg, "U%c", ch);
		printmsg(M_CALLMSG, "Reply to 3rd msg = {%s}", msg);
		wmsg(msg);
		if ( ch == 'N' )
			FAIL1("No common protocol");
		setproto(ch);
		plog(M_CALL, readymsg);
		return('D');
	} else {
		sprintf(msg, "Shere=%s", nodename);
		printmsg(M_CALLMSG, "1st msg = {%s}", msg);
		wmsg(msg);

		if ( rmsg(msg, BUFSIZ) < 0 )
			FAIL1("Logon failed: 1st reply (wrong sitename?)");
		printmsg(M_CALLMSG, "Reply to 1st msg = {%s}", visib(msg));
		if ( *msg != 'S' )
			FAIL1("Bad format for reply to 1st msg");
		if ( (cp=index(msg+1, ' ')) != NULL )
			*cp = '\0';
		if ( strlen(msg) > (SITELEN+1) )
			FAIL1("Received sitename to long");
		strcpy(locbuf, msg+1);
		rmtname = &locbuf[0];
#ifdef BBS
#define BBSUSER	"bbsuser"

		/*
		 * For BBS, anonymous caller's sitename is replaced by the
 		 * device name, notice we skip over "/dev/".
		 */
		{
			char *tmp;

			if ( (strcmp(rmtname, BBSUSER) == 0) &&
			     ((tmp=ttyname(fileno(stdin))) != NULL) )
				sprintf(locbuf, "U%s", tmp+5);
			printmsg(M_DEBUG, "BBS replaced new rmtname: <%s>",
								 rmtname);
		}
#endif
		open_the_logfile("uucico");
		plog(M_CALL, "Call received from %s {%d} (V%s)",
						rmtname, processid, version);
		plog(M_CALL,"Locking remote site %s",rmtname);
		if ( lockit(rmtname) < 0 ) {
			leavelock = 1;
			FAIL2("Incoming site %s already locked", rmtname);
		}

		if ( !checkrmt() )
			FAIL2("Unknown host %s", rmtname);

		if ( (rdevname=ttyname(fileno(stdin))) == NULL )
			FAIL1("No attached tty device.");
		rdevname += 5;

	/* The incoming device will already have been locked by login.  */
		if (MASTER == role) { 
			if ( locktty(rdevname) < 0 ) {
				cp = rdevname;
				rdevname = NULL;
				FAIL2("Incoming device locked: %s", cp);
			}
		}

		sprintf(msg, "ROK");
		printmsg(M_CALLMSG, "2nd msg = {%s}", msg);
		wmsg(msg);
		sprintf(msg, "P%s", PROTOS);
		printmsg(M_CALLMSG, "3rd msg = {%s}", msg);
		wmsg(msg);
		if ( rmsg(msg, BUFSIZ) < 0 )
			FAIL1("Logon failure: 3rd reply (lost communication?)");
		printmsg(M_CALLMSG, "Reply to 3rd msg = {%s}", visib(msg));
		if ( (msg[0] != 'U') || (index(PROTOS, msg[1]) == NULL) )
			FAIL1("No common communications protocol");
		setproto(msg[1]);
		plog(M_CALL, readymsg);
		return('R');
	}

failure:
	plog(M_CALL, errmsg, errarg);
	terminatelevel++;
	return('Y');
}

/*
 * Assign the global protocol function variables to the specified
 * protocol type.
 */

setproto(type)
char type;
{
	register PROTO *p;

	for(p=&protolst[0]; (p->type!='\0') && (p->type!=type); p++) ;

	if (p->type == '\0')
		fatal("setproto('%c'): Unknown protocol type", type);
	printmsg(M_CALL, "Agreed protocol: %c", type);
	getpkt	= p->getpkt;
	sendpkt	= p->sendpkt;
	openpk	= p->openpk;
	closepk	= p->closepk;

}

/*
 *  expectstr(str, timeout)  char *str; int timeout;
 *
 *  Wait to receive the specified "str" from the serial connection.
 *  Returns: (1) if string received,
 *	     (0) for timeout occurred before string received.
 */

expectstr(str, timeout)
char *str;
int timeout;
{
	register char *cp;
	char buf[BUFSIZ], ch;
	int retval = 0;
	int len;

	printmsg(M_DEBUG, "waiting for {%s}", str);

	if ( strcmp(str, "\"\"") == 0 )
		retval = 1;
	len = strlen(str);

	cp = &buf[0];
	while ( !retval && (sread(&ch, 1, timeout) == 1) ) {
		*cp++ = ch;
		if ( (cp-buf) < len )
			continue;
		*cp = '\0';
		if ( strncmp(str, cp-len, len) == 0 )
			retval = 1;
		if ( cp == &buf[BUFSIZ-1] )
			break;
	}
	printmsg(M_CALL, "actually received {%s}", visbuf(buf, cp-buf));
	if ( retval )
		printmsg(M_CALL, "got that");
	return(retval);
}

/*
 *  sendstr(str) char *str;
 *  Send string of chat script out the communications line.
 */

sendstr(str)
char *str;
{
	register char *cp = str;
	char ch;
	int slash, nocr;

	sleep(1);
	if ( strncmp(str, "\"\"", 2) == 0 )
		str = "";

	nocr = slash = 0;
	while ( (ch=*cp++) ) {
		if ( !slash ) {
			if ( strncmp(cp-1, "EOT", 3) == 0 ) {
				swrite("\004", 1);
				cp += 2;
			} else if ( strncmp(cp-1, "BREAK", 4) == 0 ) {
				sendbrk();
				cp += 4;
			} else if ( ch == '\\' ) {
				slash = 1;
			} else {
				swrite(&ch, 1);
			}
			continue;
		}
		switch( ch ) {
		case 'b':
			swrite("\b", 1);
			break;
		case 'c':
			nocr = 1;
			break;
		case 'd':
			sleep(2);
			break;
		case 'K':
			sendbrk();
			break;
		case 'n':
			swrite("\n", 1);
			break;
		case 'N':
			swrite("\0", 1);
			break;
		case 'p':
			sleep(1);
			break;
		case 'r':
			swrite("\r", 1);
			break;
		case 's':
			swrite(" ", 1);
			break;
		case 't':
			swrite("\t", 1);
			break;
		case '\\':
			swrite("\\", 1);
			break;		
		default:
			if ( !isdigit(ch) ||
			     !isdigit(*cp) ||
			     !isdigit(*(cp+1)) )
				fatal("Can't parse chat script: {%s}", str);
			ch = ((ch-'0')<<6) | ((*cp-'0')<<3) | (*(cp+1));
			swrite(&ch, 1);
			break;
		}
		slash = 0;
	}
	if ( !nocr )
		swrite("\r", 1);
}

/*
 * callup()
 * Initiate the call, and walk through the chat script.
 */

callup()
{
	char *device, *speed, *phone;
	char *expect, *send, *dsh;
	int i;

	plog(M_CALL, "Calling site %s {%d} (V%s)", rmtname, processid, version);
	
	device = lsys_value(device_e);
	speed  = lsys_value(speed_e);
	phone  = lsys_value(phone_e);

	if ( strcmp(device, "ACU") == 0 ) {
		if ( !dcpdial(NULL, speed, phone) )
			goto calluperr;
	} else if ( !dcpdial(device, speed, NULL) )
		goto calluperr;

	for (i=0; (expect=lsys_expect(i)) != NULL; i++) {
		for (;;) {
			printmsg(M_CALL, "callup: expecting {%s}", expect);
			if ( (dsh=index(expect, '-')) != NULL )
				*dsh++ = '\0';
			if ( expectstr(expect, MSGTIME) )
				break;
			if ( dsh == NULL ) {
				plog(M_CALL, "Login failed.");
				goto calluperr;
			}
			if ( (expect=index(dsh, '-')) == NULL )
				fatal("Chat script syntax error: %s", dsh);
			*expect++ = '\0';
			printmsg(M_CALL, "callup: send alternate {%s}", dsh);
			sendstr(dsh);
		}
		send = lsys_send(i);
		printmsg(M_CALL, "callup: sending {%s}", send);
		sendstr(send);
	}
	return('P');

calluperr:
	terminatelevel++;
	return('Y');
}

/*
 *  sysend()
 *  End UUCP session.
 */

sysend()
{

	/* We may have a problem here. Someone else may be removing the remote
	 * device lock file before we call unlocktty. Tests have shown this to
	 * be unimportant because the lock file IS being removed. A warning
	 * will be printed to the log files for users to check for locks.
	 * Hopefully, this isn't going to be a bug.  Bob H. 11/22/91.
	 */

	/* December 4, 1991: Problem solved(?). When we are in master mode,
	 * we call dcpundial, which calls the hangup, which, in turn, calls
	 * undial. Undial removes the locks on the device. Thie following
	 * code USED to call dcpundial if in master mode, and THEN continued
	 * to call the unlocktty function, which had previously been called
	 * by dcpundial. I snuck in an 'else' statement to get around this.
	 * Tests show that this solves the problem described above, but who
	 * knows if this will cause problems when we are NOT in master mode.
	 */

	if ( lockttyexist(rdevname) ) {
		if ( role == MASTER ){
			printmsg(M_DEBUG,"Sysend: calling undial and hangup routines.");
			dcpundial();
		}else{
			plog(M_CALL,"Sysend: Removing remote device lock file.");
			if(unlocktty(rdevname) != 0){
				plog(M_CALL,"sysend: Possible lock file removal failure.");
			}
		}
	}

	/* May 05, 1992 (Bob H.) problems communicating with a 386 based bbs
	 * and a customer complaint from Germany lead to the discovery of a
	 * condition where undial() was never called, which meant that cicio
	 * NEVER hung up the phone! Adding the dcpundial() in the following
	 * if... resolves this.
	 */
	if ( !leavelock && lockexist(rmtname) ){
		dcpundial();
		plog(M_CALL,"Removing remote site lock");
		if(lockrm(rmtname) != 0){
			printmsg(M_LOG,"sysend: Remote site Lock file removal failed!");
			plog(M_CALL,"sysend: Remote site lock file removal failed!");
		}
	}
	plog(M_CALL, terminatelevel ?
		"Abnormal completion {%d}" :
		"Call completing normally {%d}", processid);
	close_logfile();
	sysended = 1;
	return( (role == MASTER) ? 'I': 'A' );
}

/*
 * scandir()
 *
 * scan work dir for "C." files matching current remote host (rmtname)
 * return:
 *	   Y	- can't open file
 *	   S	- ok
 *	   Q	- no files 
 */

extern	DIR *opendir();
extern	struct direct *readdir();

scandir()
{
	char fn[DIRSIZ+1];
	char dirname[CTLFLEN];
	DIR *dirp;
	struct dirent *dp;

	sprintf(dirname, "%s/%s", SPOOLDIR, rmtname); /* build spool dir name */
	printmsg(M_INFO, "Scandir: %s", dirname);

	if ((dirp=opendir(dirname)) == NULL)	/* open spool dir */
		return('Q');

	fn[0] = fn[DIRSIZ] = '\0';
	while ( (dp=readdir(dirp)) != NULL ) {
		printmsg(M_DEBUG, "Scandir: %s", dp->d_name);
		if ( strncmp("C.", dp->d_name, 2) )	/* is this a C. file? */
			continue;
		if ( (fn[0] == '\0') || (strncmp(dp->d_name, fn, DIRSIZ) < 0) )
			strncpy(fn, dp->d_name, DIRSIZ);
	}
	closedir(dirp);

	if (fn[0] != '\0') {
		sprintf(cfile, "%s/%s/%s", SPOOLDIR, rmtname, fn);
		printmsg(M_DEBUG, "Scandir: cfile: %s", cfile);
		if ((cfp = fopen(cfile, "r")) == NULL) {
			plog(M_DEBUG, "Unable to open control file %s", cfile);
			terminatelevel++;
			return('Y');
		}
		return('S');
	}
	return('Q');
}
