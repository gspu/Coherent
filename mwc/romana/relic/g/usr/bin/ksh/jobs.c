/*
 * Process and job control
 */

static char *RCSid = "$Header: jobs.c,v 3.3 88/11/05 11:56:24 egisin Exp $";

/*
 * based on version by Ron Natalie, BRL
 *
 * TODO:
 *	change Proc table to Job table, with array of pids.
 *	make %+ be jobs, %- be jobs->next.
 *	do not JFREE members of pipeline.
 *	consider procs[] related critical sections.
 *	signal(SIGCHLD, j_sigchld) should be
 *	sigaction(SIGCHLD, sigchld, NULL),
 *	with sigchld.sa_flags = SA_RESTART.
 *	There is a simple work-around if there is no SA_RESTART.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/times.h>
#include <wait.h>
#if JOBS
#if _BSD
#include <sys/ioctl.h>
#else
#include "termios.h"
#endif
#endif
#include "sh.h"
#include "tree.h"

#ifndef WIFCORED
#define	WIFCORED(x)	(!!((x)&0x80)) /* non-standard */
#endif

/* as of P1003.1 Draft 12.3:
 *	pid_t getpgrp(void);		// Get process group id
 *	pid_t setsid(void);		// Create session and Set process group id
 *	int setpgid(pid_t pid, pid_t pgid); // Set process group id for job control
 */

#if JOBS
#if _BSD			/* _BSD 4.* */
#define	setpgid(p, pg)	setpgrp(p, pg)
#define	getpgid(p)	getpgrp(p)
#define	tcsetpgrp(fd,p)	ioctl(fd, TIOCSPGRP, &(p))
#else				/* POSIX-compatible */
#define	getpgid(p)	getpgrp() /* 1003.1 stupidity */
#define	killpg(p, s)	kill(-(p), s)
#endif
#endif

#ifndef	SIGCHLD
#define	SIGCHLD	SIGCLD
#endif

typedef struct Proc Proc;
struct Proc {
	Proc   *next;		/* `procs' list link */
	int	job;		/* job number: %n */
	short	Volatile state;	/* proc state */
	short	Volatile notify; /* proc state has changed */
	Proc   *prev;		/* prev member of pipeline */
	pid_t	proc;		/* process id */
	pid_t	pgrp;		/* process group if flag[FMONITOR] */
	short	flags;		/* execute flags */
	int	status;		/* wait status */
	clock_t	utime, stime;	/* user/system time when JDONE */
	char	com [48];	/* command */
};

/* proc states */
#define	JFREE	0		/* unused proc */
#define	JRUN	1		/* foreground */
#define JEXIT	2		/* exit termination */
#define	JSIGNAL	3		/* signal termination */
#define	JSTOP	4		/* stopped */

static	Proc *procs = NULL;	/* job/process table */

clock_t	j_utime, j_stime;	/* user and system time for last job a-waited */
#if JOBS
static	int	sm_default, sm_sigchld;	/* signal masks */
static	int	our_pgrp;		/* shell's pgrp */
#endif
static	Proc   *j_lastj;		/* last proc created by exchild */
static	int	j_lastjob = 0;		/* last created job */
static	int	j_current = 0;		/* current job */
static	int	j_previous = 0;		/* previous job */

static	int	j_newjob ARGS((void));
static	void	j_print ARGS((Proc *j));
static	Proc   *j_search ARGS((int job));
static	int	j_waitj ARGS((Proc *j, int intr));
static	void	j_sigchld ARGS((int sig));
	
/* initialize job control */
void
j_init()
{
#if JOBS
#ifdef NTTYDISC
	int ldisc = NTTYDISC;	/* BSD brain damage */

	if (ttyfd >= 0)
		ioctl(ttyfd, TIOCSETD, &ldisc);
#endif
	our_pgrp = getpgid(0);
	sm_default = 0;
	sm_sigchld = sigmask(SIGCHLD);
#endif
}

/* job cleanup before shell exit */
void
j_exit()
{
	register Proc *j;
	int killed = 0;

#if JOBS
	/* kill stopped jobs */
	for (j = procs; j != NULL; j = j->next)
		if (j->state == JSTOP) {
			killed ++;
			killpg(j->pgrp, SIGHUP);
			killpg(j->pgrp, SIGCONT);
		}
	if (killed)
		sleep(1);
#endif
	j_notify();

#if JOBS
	if (flag[FMONITOR]) {
		flag[FMONITOR] = 0;
		j_change();
	}
#endif
}

#if JOBS
/* turn job control on or off according to flag[FMONITOR] */
void
j_change()
{
	static handler_t old_tstp, old_ttin, old_ttou;

	if (flag[FMONITOR]) {
		if (ttyfd < 0) {
			flag[FMONITOR] = 0;
			shellf("job control requires tty\n");
			return;
		}
#if !COHERENT
		(void) signal(SIGCHLD, j_sigchld);
		sigtraps[SIGCHLD].sig_dfl = 1; /* restore on fork */
		old_tstp = signal(SIGTSTP, SIG_IGN);
		sigtraps[SIGTSTP].sig_dfl = 1;
		old_ttin = signal(SIGTTIN, SIG_IGN);
		sigtraps[SIGTTIN].sig_dfl = 1;
		old_ttou = signal(SIGTTOU, SIG_IGN);
		sigtraps[SIGTTOU].sig_dfl = 1;
#endif
		sigsetmask(sm_default);
		tcsetpgrp(ttyfd, our_pgrp);
	} else {
		(void) signal(SIGCHLD, SIG_DFL);
		(void) signal(SIGTSTP, old_tstp);
		sigtraps[SIGTSTP].sig_dfl = 0;
		(void) signal(SIGTTIN, old_ttin);
		sigtraps[SIGTTIN].sig_dfl = 0;
		(void) signal(SIGTTOU, old_ttou);
		sigtraps[SIGTTOU].sig_dfl = 0;
	}
}
#endif

/* execute tree in child subprocess */
int
exchild(t, flags)
	struct op *t;
	int flags;
{
	register int i;
	register Proc *j;
	int rv = 0;

	flags &= ~XFORK;
	if ((flags&XEXEC))
		return execute(t, flags);

	/* get free Proc entry */
	for (j = procs; j != NULL; j = j->next)
		if (j->state == JFREE)
			goto Found;
	j = alloc(sizeof(Proc), APERM);
	j->next = procs;
	j->state = JFREE;
	procs = j;
  Found:

	j->prev = ((flags&XPIPEI)) ? j_lastj : NULL;
	j->proc = j->pgrp = 0;
	j->flags = flags;
	j->job = (flags&XPIPEI) ? j_lastjob : j_newjob();
	snptreef(j->com, sizeof(j->com), "%T", t); /* save proc's command */
	j->com[sizeof(j->com)-1] = '\0';
	j->state = JRUN;

	/* stdio buffer must be flushed and invalidated */
	for (i = 0; i < NUFILE; i++)
		flushshf(i);

	/* create child process */
	if ((i = fork()) < 0) {
		/* todo: try a few times with exp-incr delay */
		j->state = JFREE;
		errorf("cannot fork - try again\n");
	}
	j->proc = (i != 0) ? i : getpid();

#if JOBS
	/* job control set up */
	if (flag[FMONITOR] && !(flags&XXCOM)) {
		j->pgrp = !(flags&XPIPEI) ? j->proc : j_lastj->pgrp;
		/* do in both parent and child to avoid fork race condition */
		if (!(flags&XBGND))
			tcsetpgrp(ttyfd, j->pgrp); /* could be trouble */
		setpgid(j->proc, j->pgrp);
	}
#endif
	j_lastj = j;

	if (i == 0) {		/* child */
		e.oenv = NULL;
		if (flag[FTALKING])
			restoresigs();		/* set back to default */
		if ((flags&XBGND) && !flag[FMONITOR]) {
			signal(SIGINT, SIG_IGN);
			signal(SIGQUIT, SIG_IGN);
			if (flag[FTALKING])
				signal(SIGTERM, SIG_DFL);
#if COHERENT
			if (!(flags & XPIPEI)) {
				i = open("/dev/null", 0);
				(void) dup2(i, 0);
				close(i);
			}
#endif
		}
		for (j = procs; j != NULL; j = j->next)
			j->state = JFREE;
		ttyfd = -1;
		flag[FMONITOR] = flag[FTALKING] = 0;
		cleartraps();
		execute(t, flags|XEXEC); /* no return */
		/* NOTREACHED */
	}

	/* shell (parent) stuff */
	if ((flags&XBGND)) { /* async statement */
		async = j->proc;
		j_previous = j_current;
		j_current = j->job;
		if (flag[FTALKING])
			j_print(j);
	} else {		/* sync statement */
		if (!(flags&XPIPE))
			rv = j_waitj(j, 0);
	}

	return rv;
}

/* wait for last job: pipeline or $() sub-process */
int
waitlast()
{
	return j_waitj(j_lastj, 0);
}

/* wait for job to complete or change state */
static int
j_waitj(aj, intr)
	Proc *aj;
	int intr;		/* interruptable */
{
	register Proc *j;
	int rv = 1;
	int ttysig = 0;

#if JOBS
	if (flag[FMONITOR])
		sigsetmask(sm_sigchld);
#endif
	/* wait for all members of pipeline */
	for (j = aj; j != NULL; j = j->prev) {
		/* wait for job to finish, stop, or ^C of built-in wait */
		while (j->state == JRUN) {
#if JOBS
			if (flag[FMONITOR])
				sigpause(sm_default);
			else
#endif
#if COHERENT
				j_sigchld(-1);
#else
				j_sigchld(SIGCHLD);
#endif
			if (sigtraps[SIGINT].set && intr)
				goto Break;
		}
		if (j->state == JEXIT) { /* exit termination */
			if (!(j->flags&XPIPEO))
				rv = WEXITSTATUS(j->status);
			j->notify = 0;
		} else
		if (j->state == JSIGNAL) { /* signalled to death */
			if (WTERMSIG(j->status) == SIGINT ||
			    WTERMSIG(j->status) == SIGPIPE && (j->flags&XPIPEO))
				j->notify = 0;
			if (WTERMSIG(j->status) == SIGINT ||
			    WTERMSIG(j->status) == SIGQUIT)
				ttysig = 1;
		} else
#if JOBS
		if (j->state == JSTOP)
			if (WSTOPSIG(j->status) == SIGTSTP)
				ttysig = 1;
#else
		;
#endif
	}

	/* compute total child time for time statement */
	for (j = aj; j != NULL; j = j->prev)
		j_utime += j->utime, j_stime += j->stime;

	/* find new current job */
#if JOBS
	if (aj->state == JSTOP) {
		j_previous = j_current;
		j_current = aj->job;
	} else {
#else
	if (1) {
#endif
		int hijob = 0;

		/* todo: this needs to be done in j_notify */
		/* todo: figure out what to do with j_previous */
		j_current = 0;
		for (j = procs; j != NULL; j = j->next)
			if ((j->state == JRUN || j->state == JSTOP)
			    && j->job > hijob) {
				hijob = j->job;
				j_current = j->job;
			}
	}

  Break:
#if JOBS
	if (flag[FMONITOR]) {
		/* reset shell job control state */
		sigsetmask(sm_default);
		tcsetpgrp(ttyfd, our_pgrp);
	}
#endif
	if (ttysig)
		fputc('\n', shlout);
	j_notify();

	return rv;
}

/* SIGCHLD handler to reap children */
static void
j_sigchld(sig)
	int sig;
{
	int errno_ = errno;
	struct tms t0, t1;

	(void) times(&t0);
	do {
		register Proc *j;
		int pid, status;
#if JOBS
		if (flag[FMONITOR])
			pid = waitpid(-1, &status, (WNOHANG|WUNTRACED));
		else
#endif
			pid = wait(&status);
		if (pid <= 0)	/* return if would block (0) ... */
			break;	/* ... or no children or interrupted (-1) */
		(void) times(&t1);

		for (j = procs; j != NULL; j = j->next)
			if (j->state != JFREE && pid == j->proc)
				goto Found;
		continue;

	  Found:
		j->notify = 1;
		j->status = status;
#if JOBS
		if (WIFSTOPPED(status))
			j->state = JSTOP;
		else
#endif
		if (WIFEXITED(status))
			j->state = JEXIT;
		else
		if (WIFSIGNALED(status))
			j->state = JSIGNAL;

		/* compute child's time */
		/* todo: what does a stopped job do? */
		j->utime = t1.tms_cutime - t0.tms_cutime;
		j->stime = t1.tms_cstime - t0.tms_cstime;
		t0 = t1;
#if JOBS
	} while (flag[FMONITOR]);
#else
	} while (0);		/* only once if wait()ing */
#endif

	errno = errno_;
}

/* wait for child, interruptable */
int
waitfor(job)
	int job;
{
	register Proc *j;

	if (job == 0 && j_current == 0)
		errorf("no current job\n");
	j = j_search((job == 0) ? j_current : job);
	if (j == NULL)
		errorf("no such job: %d\n", job);
	if (flag[FTALKING])
		j_print(j);
	if (e.interactive) {	/* flush stdout, shlout */
		fflush(shf[1]);
		fflush(shf[2]);
	}
	return j_waitj(j, 1);
}

/* kill (built-in) a job */
void
j_kill(job, sig)
	int job;
	int sig;
{
	register Proc *j;

	j = j_search(job);
	if (j == NULL)
		errorf("cannot find job\n");
	if (j->pgrp == 0) {	/* !flag[FMONITOR] */
		if (kill(j->proc, sig) < 0) /* todo: all member of pipeline */
			errorf("kill: %s\n", strerror(errno));
#if JOBS
	} else {
		if (sig == SIGTERM || sig == SIGHUP)
			(void) killpg(j->pgrp, SIGCONT);
		if (killpg(j->pgrp, sig) < 0)
			errorf("killpg: %s\n", strerror(errno));
#endif
	}
}

#if JOBS

/* fg and bg built-ins */
int
j_resume(job, bg)
	int job;
	int bg;
{
	register Proc *j; 
	
	j = j_search((job == 0) ? j_current : job);
	if (j == NULL)
		errorf("cannot find job\n", job);
	if (j->pgrp == 0)
		errorf("job not job-controlled\n");

	j->state = JRUN;
	j_print(j);
	flushshf(2);

	if (!bg)
  		tcsetpgrp(ttyfd, j->pgrp); /* attach shell to job */
	if (killpg(j->pgrp, SIGCONT) < 0)
		errorf("cannot continue job %%%d\n", job);
	if (!bg)
		return j_waitj(j, 0);
	return 0;
}

#endif

/* list jobs for jobs built-in */
void
j_jobs()
{
	register Proc *j; 

	for (j = procs; j != NULL; j = j->next)
		if (j->state != JFREE)
			j_print(j);
}

/* list jobs for top-level notification */
void
j_notify()
{
	register Proc *j; 

	for (j = procs; j != NULL; j = j->next) {
		if (j->state == JEXIT && !flag[FTALKING])
			j->notify = 0;
		if (j->state != JFREE && j->notify)
			j_print(j);
		if (j->state == JEXIT || j->state == JSIGNAL)
			j->state = JFREE;
		j->notify = 0;
	}
}

static void
j_print(j)
	register Proc *j;
{
	char buf [64], *s = buf;

	switch (j->state) {
	  case JRUN:
		s = "Running";
		break;

#if JOBS
	  case JSTOP:
		strcpy(buf, "Stopped ");
		s = strchr(sigtraps[WSTOPSIG(j->status)].mess, '(');
		if (s != NULL)
			strcat(buf, s);
		s = buf;
		break;
#endif

	  case JEXIT: {
		int rv;
		rv = WEXITSTATUS(j->status);
		sprintf(buf, "Done (%d)", rv);
		if (rv == 0)
			*strchr(buf, '(') = 0;
		j->state = JFREE;
		} break;

	  case JSIGNAL: {
		int sig = WTERMSIG(j->status);
#if COHERENT
		char *n = NULL;
		int i;
		for (i = 0; i < SIGNALS; ++i)
			if (sigtraps[i].signal == sig) {
				n = sigtraps[i].mess;
				break;
			}
#else
		char *n = sigtraps[sig].mess;
#endif
		if (n != NULL)
			sprintf(buf, "%s", n);
		else
			sprintf(buf, "Signal %d", sig);
		if (WIFCORED(j->status))
			strcat(buf, " - core dumped");
		j->state = JFREE;
		} break;

	  default:
		s = "Hideous job state";
		j->state = JFREE;
		break;
	}
	shellf("%%%-2d%c %5d %-20s %s%s\n", j->job,
	       (j_current==j->job) ? '+' : (j_previous==j->job) ? '-' : ' ',
	       j->proc, s, j->com, (j->flags&XPIPEO) ? "|" : "");
}

/* convert % sequence to job number */
int
j_lookup(cp)
	char *cp;
{
	register Proc *j;
	int len, job = 0;

	if (*cp == '%')		/* leading % is optional */
		cp++;
	switch (*cp) {
	  case '\0':
	  case '+':
		job = j_current;
		break;

	  case '-':
		job = j_previous;
		break;

	  case '0': case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9': 
		job = atoi(cp);
		break;

	  case '?':		/* %?string */
		for (j = procs; j != NULL; j = j->next)
			if (j->state != JFREE && strstr(j->com, cp+1) != NULL)
				job = j->job;
		break;

	  default:		/* %string */
		len = strlen(cp);
		for (j = procs; j != NULL; j = j->next)
			if (j->state != JFREE && strncmp(cp, j->com, len) == 0)
				job = j->job;
		break;
	}
	if (job == 0)
		errorf("%s: no such job\n", cp);
	return job;
}

/* are any stopped jobs ? */
#if JOBS
int
j_stopped()
{
	register Proc *j; 

	for (j = procs; j != NULL; j = j->next)
		if (j->state == JSTOP)
			return 1;
	return 0;
}
#endif

/* create new job number */
static int
j_newjob()
{
	register Proc *j; 
	register int max = 0;
	
	j_lastjob ++;
	for (j = procs; j != NULL; j = j->next)
		if (j->state != JFREE && j->job)
			if (j->job > max)
				max = j->job;
	if (j_lastjob > max)
		j_lastjob = max + 1;
	return j_lastjob;
}

/* search for job by job number */
static Proc *
j_search(job)
	int job;
{
	register Proc *j;

	for (j = procs; j != NULL; j = j->next)
		if (j->state != JFREE && job == j->job && !(j->flags&XPIPEO))
			return j;
	return NULL;
}

