/*
 * dcp.h
 *
 * Header File for dcp -- "dcp" a uucp clone
 *
 * copyright (c) richard h. lamb 1985, 1986, 1987 
 * changes copyright (c) stuart lynne may/1987 
 * changes (massive) copyright (c) 1989-1991 by Mark Williams Company
 */

#include <stdio.h>
#include <sys/dir.h>

/*
 *  Various definitions of Global Strings and Values
 */

#define VERSION		"4.0.03"
#define PUBDIR		"/usr/spool/uucppublic"
#define SPOOLDIR	"/usr/spool/uucp"

#define TRACE	printf

#define SITELEN		DIRSIZ	/* Max number of Characters in Sitename	*/
#define	SITESIG		7	/* Significant Characters of Sitename	*/
#define	LOCKSIG		9	/* Signific Chars of Lockable Resource	*/
#define	CTLFLEN		64	/* Max Length of UUCP Control File Name	*/
#define LOGFLEN		64	/* Max Length of UUCP Log File Name	*/
#define	LOKFLEN		64	/* Max Length of UUCP Lock File Name	*/

#define MSGTIME		40	/* Timeout to receive a message		*/
#define DLE		0x10	/* ASCII DLE Character (^P)		*/

#define	SLAVE		0	/* States of Variable 'role' in dcp.c	*/
#define	MASTER		1


#define XQTFILE		"X.%s"

#define PATHLEN		256
#define MAXPACK         256



/* log messages */
#define M_LOG		0	/* log regardless of debuglevel		*/
#define	M_FATAL		0	/* log fatal messages, too		*/
#define M_CALL		1	/* call and pre-protocol negotiation	*/
#define M_CALLMSG	2	/* messages sent during call/pre-proto	*/
#define M_SPOOL		3	/* spool files				*/
#define M_TRANSFER	4	/* file transfer			*/
#define M_CONVERSE	5	/* conversation level			*/
#define M_HIGHPROTO	6	/* high-level protocol			*/
#define M_MEDPROTO	7	/* med-level protocol			*/
#define M_LOWPROTO	8	/* low-level protocol (framing, et al)	*/
#define M_DATA		9	/* actual packet data			*/
#define M_INFO		10	/* reading config files			*/
#define	M_DEBUG		100	/* Highest debug level			*/

/*
 *  Various external function declarations
 */

extern	char *ttyname();
extern	char *visib(), *visbuf();
extern	char *myname(), *uucpname();
extern	char *index(), *rindex();

/*
 *  Various external data declarations for Global Variables
 */

extern	int	abort_cico;	/* Indicates Process Abort Signalled	*/
extern	int	sysended;	/* Indicates sysend() was called	*/
extern	int	processid;	/* Currect Process Id (uucico)		*/
extern	int	debuglevel;	/* User specified Debug Level		*/
extern	int	fpfd;		/* File Decriptor used for send&receive	*/
extern	int	role;		/* Our role, either MASTER or SLAVE	*/
extern	char	*sysname;	/* Command line -[sS]sysname argument	*/
extern	char	*rmtname;	/* Remote System being processed now	*/
extern	int	forcecall;	/* Ignore L.sys spec for time to call	*/
extern	int	terminatelevel;	/* Indicates return code for one call	*/
extern	char	cfile[CTLFLEN];	/* Current C.* Control Work File Name	*/
extern	FILE	*cfp;		/* FILE Pointer for C.* files		*/
extern	char	*devname;	/* Communications Device Name Connected	*/
extern	char	*rdevname;	/* Remote Device Name Connected	*/
extern	char	*nodename;	/* UUCP node name (or MYNAME, perhaps)	*/
extern	char	*version;	/* Version Character String		*/
extern	int	stripflg;	/* Flag to strip chars to 7bits on read */

extern int pktsize;		/* packet size for this protocol */
extern int findwork;
extern char xfromfile[BUFSIZ];	/* source of copy */
extern char xtofile[BUFSIZ];	/* destiation of copy */
extern	char	*clinep[10];	/* pointer to fields from line from C. file */
extern	char	cline [BUFSIZ];	/* line from C. file			*/
extern	char *fromfilep, *tofilep, *usernamep, *optionp, *spoolfilep, *modep,
		 *notifyp;
extern	int	nclinep;	/* number tokens in cline		*/
extern int size;		/* nbytes in buff */
extern FILE *fsys;
	

extern char s_systems[64];

extern unsigned int checksum();
extern char *malloc();

extern	int	total_errors;
extern	long	bytecount;
