/*
 *	Header file for the mail system.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <time.h>
#include <pwd.h>
#include <utmp.h>
#include <access.h>
#include <signal.h>
#include <ctype.h>

#define MAXLONG		LONG_MAX

#define	SPOOLDIR	"/usr/spool/mail/"
#define PUBKEYDIR	"/usr/spool/pubkey/"
#define XDELIVER	"exec /bin/rxmail"	/* Delivery secret agent.  */
#define DELIVER		"exec /bin/rmail"	/* Delivery agent.  */

#define	NLINE	512		/* Longest line in a message		*/
#define	NCLINE	256		/* Length of an interactive command	*/
#define	MAXRECIP  512		/* Maximum Number of Actual Recipients	*/
#define CMDLINE	NCLINE
#define CNULL	((char) NULL)	/* Lint-safe string terminator */
#define MSGSEP "\1\1\1\1\n"	/* message separator */

extern	char	*index(), *rindex();
extern	char	*myalloc();
extern	char	**listcat();
extern	char	msgline[];

#if HPCOHERENT
typedef long fsize_t;
#endif
