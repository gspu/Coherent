/*
 * cronpipe.c 
 * Special version of popen, that allows to cron keep track about
 * children.
 */
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include "cron.h"

#define	R	0	/* Pipe read descriptor index */
#define	W	1	/* Pipe write descriptor index */

extern child_id	*add_entry();	/* Add an new entry to a link list */
extern int	ifmail();

extern char	acRealUser[MAX_UNAME];
extern time_t	clock;
extern child_id *current;	/* Pointer to the current child id */
extern int	flag320;
extern int	mailFlag;
extern int	set_uid_flag;

static	int	poppid[_NFILE];

FILE *
cronpipe(command, type)
char *command, *type;
{
	register mode;
	register fd;
	int pd[2];

	if (pipe(pd) < 0)
		return (NULL);
	if (*type == 'w') {
		mode = W;
		fd = pd[W];
	} else {
		mode = R;
		fd = pd[R];
	}
	if ((poppid[fd] = fork()) < 0) {
		close(pd[R]);
		close(pd[W]);
		return (NULL);
	}
	if (poppid[fd] == 0) {		/* Child */
		/* Set uid to user which crontab is going to be executed.
	 	 * In case of COHERENT3.2.0 set uid to daemon (set_uid_flag
		 * is TRUE).
	 	 */
		Dprint("Set user ID to '%s'\n", acRealUser);
		if (set_uid_flag == FALSE) {
			/* If file name is not user name ignore it */
			if ((set_uid_flag = set_uid(acRealUser)) != TRUE) {
				/* During testing were user traps
					from smail. So they may come from here.
				if (ifmail(acRealUser)) {
					char mailBuf[80];
					sprintf(mailBuf,"smail %s < %s" 
					  "cron: cannot find/set user '%s'\n", 
					   acRealUser, acRealUser);
					system(mailBuf);
				}*/
				exit(0);
			} 
		}
		if (mode == W) {
			close(pd[W]);
			close(fileno(stdin));
			dup(pd[R]);
			close(pd[R]);
		} else {
			close(pd[R]);
			close(fileno(stdout));
			dup(pd[W]);
			close(pd[W]);
		}
		Dprint("cronpipe: before execl %s\n", command);
		execl("/bin/sh", "sh", "-c", command, NULL);
		exit(1);
	} 
	/* Parent */
	/* We will keep track about keeds only in SV mode */
	if (flag320 == FALSE) {
		Dprint("cronpipe before fork: Name %s\t", acRealUser);
		Dprint("                      Pid %d\t", poppid[fd]);
		Dprint("                      Command %s\n", command);
		current = add_entry(poppid[fd], acRealUser, clock, command);
		Dprint("cronpipe: after add. Pid is %d\n", current->pid);
	}
	if (mode == W) {
		close(pd[R]);
		return (fdopen(pd[W], "w"));
	} else {
		close(pd[W]);
		return (fdopen(pd[R], "r"));
	}
}

pclose(stream)
FILE *stream;
{
	register fd;
	register pid, wpid;
	int status;
	int (*hupfun)(), (*intfun)(), (*quitfun)();

	fd = fileno(stream);
	pid = poppid[fd];
	poppid[fd] = 0;
	if (pid==0 || fclose(stream)==EOF)
		return (-1);
	hupfun = signal(SIGHUP, SIG_IGN);
	intfun = signal(SIGINT, SIG_IGN);
	quitfun = signal(SIGQUIT, SIG_IGN);
	while ((wpid = wait(&status))!=pid && wpid>=0)
		;
	if (wpid < 0)
		status = -1;
	signal(SIGHUP, hupfun);
	signal(SIGINT, intfun);
	signal(SIGQUIT, quitfun);
	return (status);
}
