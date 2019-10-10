/*
 * dcp.c 
 *
 * revised edition of dcp -- "dcp" a uucp clone
 *
 * copyright (c) richard h. lamb 1985, 1986, 1987 
 * changes copyright (c) stuart lynne may/1987 
 * changes copyright (c) peter housel nov/1988
 * changes (massive) copyright (c) 1989-1991 by Mark Williams Company
 *
 * this program implements a uucico type file transfer and remote execution
 * type protocol. 
 *
 * Usage:  uucico [-xn] [-r0]		   slave mode
 * 	   uucico [-xn] [-r1] -{sS}host    call host
 * 	   uucico [-xn] [-r1] -{sS}all	   call all known hosts
 * 	   uucico [-xn] [-r1] -{c}all	   call all known hosts with queued files
 * 	   uucico [-xn] -r1		   call uutouch queued hosts
 */

#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include "dcp.h"
#include "perm.h"
#include "alarm.h"

int	pktsize;		/* packet size for pro */
char	xfromfile[BUFSIZ];	/* source of copy */
char	xtofile[BUFSIZ];	/* destination of copy */
char	*clinep[10];		/* pointer to fields from line from C. file */
char	cline [BUFSIZ];		/* line from C. file			*/

char *fromfilep, *tofilep, *usernamep, *optionp, *spoolfilep, *modep, *notifyp;

int	nclinep;		/* number tokens in cline		*/
long	bytecount;		/* transfer byte count	*/

int	size;			/* nbytes in buff */
char proto[5];			/* list of protocols */
int	bad_count;
int	total_errors;
unsigned int checksum();

/*
 *  Global Variables
 */

int	checkfirst = 0;		/* check spooldir before calling	*/
int	abort_cico = 0;		/* Indicates Process Abort Signalled	*/
int	sysended = 0;		/* Indicates sysend() was called	*/
int	processid;		/* Current Process Id (uucico)		*/
int	fpfd = -1;		/* File Decriptor used for send&receive	*/
int	role = SLAVE;		/* Our role, either MASTER or SLAVE	*/
char	*sysname = NULL;	/* Command line -[sS]sysname argument	*/
char	*rmtname = NULL;	/* Remote System being processed now	*/
char	*logname = NULL;	/* In SLAVE mode, the login name	*/
int	forcecall = 0;		/* Ignore L.sys spec for time to call	*/
int	terminatelevel = 0;	/* Indicates return code for one call	*/
char	cfile[CTLFLEN];		/* Current C.* Control Work File Name	*/
FILE	*cfp = NULL;		/* FILE Pointer for C.* files		*/
char	*nodename;		/* UUCP node name (or MYNAME, perhaps)	*/
char	*version;		/* Version Character String		*/
int	stripflg = 0;		/* Flag to strip chars to 7bits on read */

/*
 *  Extern Function Declarations
 */

extern	int catchhup(), catchquit(), catchterm(), catchsegv();
extern	char *getenv();

/*
 *  Local Variables
 */

static char	state;			/* system state */

main(argc, argv)
int argc;
char *argv[];
{
	time_t now;
	static char buf[16];
	umask(077);
	sprintf(buf, "%.14s%s", VERSION,
#if SGTTY
		"S");
#elif TERMIO
		"T");
#elif BBS
		"SB");
#endif
	version = &buf[0];

	while(--argc) {
		if (**++argv != '-')
			usage();
		switch(argv[0][1]) {
		case 'x':
			debuglevel = atoi(&argv[0][2]);  break;
		case 'c':
			checkfirst = 1;
			role = MASTER;
			sysname = &argv[0][2];
			break;
		case 'S':
			forcecall = 1;
		case 's':
			role = MASTER;
			sysname = &argv[0][2];  break;
		case 'r':
			role = atoi(&argv[0][2]);  break;
		case 'v':
		case 'V':
			fatal("uucico: Version %s", version);
		default:
			usage();  break;
		}
	}
	if ( ((role!=MASTER) && ((role!=SLAVE) || (sysname!=NULL))) ||
	     ((role==MASTER) && (sysname==NULL)) )
		usage();

	open_debug("uucico", role==SLAVE);
	if ( debuglevel && !lsys_access() ) {
		printmsg(M_LOG, "unauthorized debuglevel; reset to zero.");
		debuglevel = 0;
	}
	processid = getpid();
	timedout  = 0;
	INITALRM();
	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  catchhup);
	signal(SIGQUIT, catchquit);
	signal(SIGTERM, catchterm);
	signal(SIGSEGV, catchsegv);

	if (role == MASTER) {
		time(&now);
		printmsg(M_CALL,
			"System Name \"%s\", debug=%d (%.24s) (V%s)",
			sysname, debuglevel, ctime(&now), version);
		lsys_open();
		state = 'I';
		while(!abort_cico) {
			printmsg(M_DEBUG, "Mstate = %c", state);
			switch(state) {
			case 'I':
				terminatelevel = 0;
				if ( (state=getsystem()) != 'S' )
					rmtname = NULL;
				break;
			case 'S':

		/* If our check first flag is NOT set, then we call regardless
		 * of if there files pending for transfer from this site.
		 * If our check flag is set, then we only call when a file is
		 * pending for transfer.
		*/
				if(checkfirst == 0){
					state = callup();
					break;
				}
				if((checkfirst != 0) && (scandir() == 'S')){
					plog(M_CALL,"Attempting to call, checking for queued files.");
					state = callup();
				}
				else{
					state = 'Y';
					plog(M_CALL,"No Files pending");
				}
				break;

			case 'P':
				perm_get(rmtname, NULL);
				nodename = myname();
				fixline();
				state = startup();
				break;
			case 'D':
				if ( (state=master()) != 'A' )
					break;
			case 'A':
				terminatelevel++;
			case 'Y':
				state = sysend();  break;
			}
			if (state == 'A')
				break;
		}
		lsys_close();
	} else {
		state = '0';
		while(!abort_cico) {
			printmsg(M_DEBUG, "Sstate = %c", state);
			switch(state) {
			case '0':
				state = initline() ? 'I': 'Y';
				break;
			case 'I':
				if ( (logname=getenv("USER")) == NULL )
					fatal("Can't getenv USER");
				time(&now);
				printmsg(M_CALL,
				    "Logname \"%s\", debug=%d (%.24s) (V%s)",
				     logname, debuglevel, ctime(&now), version);
				perm_get(NULL, logname);
				nodename = myname();
				state = startup();
				break;
			case 'R':
				if ( (state=slave()) != 'A' )
					break;
			case 'A':
				terminatelevel++;
			case 'Y':
				state = sysend();
				break;
			}
			if (state == 'A')
				break;
		}
	}
	if ( !sysended )
		sysend();
	exec_xqt();
	exit(0);
}

/*
 * master
 */
master()
{
	state = 'I';
	while(!abort_cico) {
		printmsg(M_CONVERSE, "Top level state (master mode) %c", state);
		switch(state) {
		case 'I':	state = sinit();			break;
		case 'B':	state = scandir();			break;
		case 'S':	state = sendf();			break;
		case 'Q':	state = sbreak();			break;
		case 'G':	state = recvf();			break;
		case 'C':	state = 'Y';				break;
		case 'Y':	state = endp();				break;
		case 'P':			return ('Y');
		case 'A':			return ('A');
		default:			return ('A');
		}
	}
	return('A');
}

/*
 * slave
 */
slave()
{
	state = 'I';
	while(!abort_cico) {
		printmsg(M_CONVERSE, "Top level state (slave mode) %c", state);
		switch(state) {
		case 'I':	state = rinit();			break;
		case 'F':	state = recvf();			break;
		case 'C':	state = schkdir();			break;
		case 'T':	state = 'B';				break;
		case 'B':	state = scandir();			break;
		case 'S':	state = sendf();			break;
		case 'Q':	state = sbreak();			break;
		case 'G':			return ('Y');
		case 'Y':	state = endp();				break;
		case 'P':			return ('Y');
		case 'A':			return ('A');
		default:			return ('A');
		}
	}
	return('A');
}

/*
 * r e c v f
 *
 * This is the state table switcher for receiving files. 
 */

recvf()
{
	state = 'F';			/* Receive-Init is the start state */
	while(!abort_cico) {
		printmsg(M_DEBUG, " recvf state: %c", state);
		switch(state) {	
		case 'F':	state = rmtcmd();	break;	/* Rcv File */
		case 'J':	state = rdata();	break;	/* Rcv Data */
		case 'D':	state = sdata();	break;
		case 'Z':	state = seof();		break;
		case 'C':		return ('C');	/* Complete state */
		case 'A':		return ('Y');	/* Abort state */
		default:		return ('Y');	/* Abort state */
		}
	}
	return('A');
}


/*
 * s e n d f
 *
 * Sendsw is the state table switcher for sending files. It loops until either
 * it finishes, or an error is encountered.  The routines called by sendsw
 * are responsible for changing the state. 
 * -
 * This routine is more properly called the command file reader.
 * It needs to dispatch to sfile and rfile based upon decoding of records in
 * the C.<> file.
 *
 */
sendf()
{
	fpfd = -1;			/* reset file getter/opener */
	state = 'F';			/* Send initiate is the start state */
	while(!abort_cico) {		/* Do this as long as necessary */
		printmsg(M_DEBUG, "sendf state: %c", state);
		switch(state) {
		case 'F':	state = cdotcmd();	break;	/* Send-File */
		case 'J':	state = rdata();	break;
		case 'D':	state = sdata();	break;	/* Send-Data */
		case 'Z':	state = seof();		break;	/* Send-EoF */
		case 'B':		return ('B');		/* Complete */
		case 'A':		return ('Y');		/* Abort */
		default:		return ('Y');	/* Unknown, fail */
		}
	}
	return('A');
}
