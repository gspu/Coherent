/*
 * cron.h
 */
#define VERSION "3.2"		/* Ignore junk file in /usr/spool/cron/crontabs
				 * Restored make dependencies. 9-11-92 
				 */

/* 3.1 Do fork only to execute next job 2-28-92 
 * opendir took out from the main loop.
 * check FIFO before cron bacame a daemon.
 * Minor bugs fixed.
 */
/* 1.3 Beta  Cron will not die when cannot fork 2-26-92 
 * 1.2 Beta  Added stuff to be a real deamon.
 */
#include	<sys/types.h>
/*
 * File's and directory's definitions.
 */
#define D_MAIN	"/usr/lib/cron"			/* main cron directory */
#define D_SPOOL "/usr/spool/cron/crontabs"	/* spool area */
#define F_LOG	"/usr/lib/cron/log"		/* accounting information */
#define F_LOCK	"/usr/lib/cron/FIFO"		/* lock file (named pipe) */
#define	F_DENY	"/usr/lib/cron/cron.deny"	/* list of denied users */
#define	F_ALLOW	"/usr/lib/cron/cron.allow"	/* list of allowed users */
#define F_LOCK	"/usr/lib/cron/FIFO"		/* Lock pipe */
#define F_DEF 	"/etc/default/cron"		/* user config. file */

/* Old crontab entry. We will use it for the backword compatibility. */
#define F_CRON	"/usr/lib/crontab"	

#define MAX_UNAME	21		/* Maximum user name */
#define	MAX_STR_LEN	512		/* Maximum length of the command line */


#if DEBUG
# define	Dprint(x,y)	printf((x),(y));
#else
# define	Dprint(x,y)	;
#endif

#define	TRUE	1
#define FALSE	0
#define DAEMON	"daemon"

extern char	*malloc();

/*
 * We want to keep track about children.
 */
typedef	struct	_child_id {
	struct _child_id	*next, *prev;		/* links */
	int			pid;			/* Process ID */
	char			name[MAX_UNAME];	/* User name */
	time_t			time;			/* Time */
	char			command[1];		/* Command */
} child_id;

