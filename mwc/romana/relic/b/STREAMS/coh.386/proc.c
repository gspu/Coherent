/* $Header: /src386/STREAMS/coh.386/RCS/proc.c,v 2.3 93/08/09 13:35:59 bin Exp Locker: bin $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 3.x, 4.x
 *	Copyright (c) 1982, 1993.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Process handling and scheduling.
 *
 * $Log:	proc.c,v $
 * Revision 2.3  93/08/09  13:35:59  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  15:53:52  nigel
 * Nigel's R80
 * 
 */

#include <common/_wait.h>
#include <kernel/_sleep.h>
#include <kernel/ker_data.h>
#include <kernel/sigproc.h>
#include <sys/wait.h>
#include <stddef.h>

#include <sys/coherent.h>
#include <sys/acct.h>
#include <sys/errno.h>
#include <sys/inode.h>
#include <sys/ptrace.h>
#include <sys/sched.h>
#include <signal.h>
#include <sys/stat.h>

/*
 * Initialization.
 * Set up the hash table queues.
 */
pcsinit()
{
	register PROC *pp;
	register PLINK *lp;

	/*
	 * Explicitly initialize everything in the first process. We use the
	 * kernel initialize routine with SELF set to NULL so that the first
	 * process gets started out with default values for fields that are
	 * normally inherited.
	 */

	pp = & procq;
	procq.p_nforw = pp;
	procq.p_nback = pp;
	procq.p_lforw = pp;
	procq.p_lback = pp;

	/* Segments are initialized in mchinit() and eveinit().	*/
	/* procq is static, so p_shmsr[] initializes to nulls.	*/

	PROC_INIT (& procq);

	for (lp = linkq ; lp < linkq + NHPLINK ; lp ++) {
		lp->p_lforw = lp;
		lp->p_lback = lp;
	}

	/*
	 * After we have set things up, we can have a current process.
	 */

	SELF = pp;
}

/*
 * Initiate a process.
 */

static PROC *
process ()
{
	register PROC *pp1;
	register PROC *pp;

	if ((pp = kalloc (sizeof (PROC))) == NULL)
		return NULL;

	PROC_INIT (pp);

	lock (pnxgate);
next:

	/*
	 * Pick the next process id.
	 */
	if (++ cpid >= NPID)
		cpid = 2;
	pp->p_pid = cpid;


	/*
	 * Make sure that process id is not in use.
	 */

	pp1 = & procq;
	while ((pp1 = pp1->p_nforw) != & procq) {
		if (pp1->p_pid < pp->p_pid)
			break;
		if (pp1->p_pid == pp->p_pid)
			goto next;
	}

	/*
	 * We've got a valid pid, so let's put this process into
	 * the process table.
	 */

	pp->p_nback = pp1->p_nback;
	pp1->p_nback->p_nforw = pp;
	pp->p_nforw = pp1;
	pp1->p_nback = pp;
	unlock (pnxgate);
	return pp;
}

/*
 * Remove a process from the next queue and release and space.
 */
relproc(pp)
register PROC *pp;
{
	register SEG * sp;

	/*
	 * Child process still has a user-area.
	 */
	if (sp = pp->p_segp[SIUSERP]) {

		/*
		 * Detach user-area from child process.
		 */
		pp->p_segp[SIUSERP] = NULL;

		/*
		 * Child process is swapped out.
		 */
		if (pp->p_flags & PFSWAP)
			sp->s_lrefc++;

		/*
		 * Release child's user-area.
		 */
		sfree(sp);
	}

	/*
	 * Remove process from doubly-linked list of all processes.
	 * Release space allocated for proc structure.
	 */
	lock(pnxgate);
	pp->p_nback->p_nforw = pp->p_nforw;
	pp->p_nforw->p_nback = pp->p_nback;
	unlock(pnxgate);
	kfree(pp);
}

/*
 * Create a clone of ourselves.
 *	N.B. - consave(&mcon) returns twice; anything not initialized
 *	in automatic storage before the call to segadup() will not be
 *	initialized when the second return from consave() commences.
 */
pfork()
{
	register PROC *cpp;
	register PROC *pp;
	register int s;
	MCON mcon;

	if ((cpp = process ()) == NULL) {
		SET_U_ERROR( EAGAIN, "no more process table entries" );
		return -1;
	}

	s = sphi();	/* put current interrupt level into s before segadup */
	spl(s);

	/*
	 * As stated above, no auto variable may be changed between calls
	 * to segadup() and consave().
	 */

	if (segadup(cpp) == 0) {
		SET_U_ERROR( EAGAIN, "can not duplicate segments" );
		relproc(cpp);
		return -1;
	}

	shmDup(cpp);	/* copy shared memory info & update ref counts */

	if (u.u_rdir)
		u.u_rdir->i_refc ++;
	if (u.u_cdir)
		u.u_cdir->i_refc ++;
	fdadupl ();

	sphi ();		/* s = sphi() was done before segadup() */
	consave (& mcon);
	spl (s);

	/*
	 * Parent process.
	 */

	if ((pp = SELF) != cpp) {
		segfinm (cpp->p_segp [SIUSERP]);
		dmaout (sizeof (mcon),
			MAPIO (cpp->p_segp [SIUSERP]->s_vmem,
			       U_OFFSET + offsetof (struct uproc, u_syscon)),
			(char *) & mcon);
		s = sphi ();
		setrun (cpp);
		spl (s);

		u.u_rval2 = 0;
		return cpp->p_pid;
	} else {
		/*
		 * Child process.
		 */

		u.u_btime = timer.t_time;
		u.u_flag = AFORK;

#ifdef UPROC_VERSION
		u.u_version = UPROC_VERSION;
#endif /* UPROC_VERSION */
		u.u_sleep [0] = '\0'; /* We are not sleeping to start with.  */
		sproto (0);
		segload ();
		u.u_rval2 = SELF->p_ppid;
		return 0;
	}
}

/*
 * Die.
 */
pexit(s)
{
	register PROC *pp1;
	register PROC *pp;
	register SEG  *sp;
	register int n;
	PROC	      *	parent = NULL;

	pp = SELF;
	T_PIGGY( 0x1, printf("%s:pexit(%x)", u.u_comm, s));

	ndpEndProc ();

	/*
	 * Cancel alarm and poll timers [if any].
	 */
	timeout(&pp->p_alrmtim, 0, NULL, 0);
	timeout(&pp->p_polltim, 0, NULL, 0);

	/*
	 * Write out accounting directory and close all files associated with
	 * this process.
	 */
	setacct();
	if (u.u_rdir)
		ldetach(u.u_rdir);
	if (u.u_cdir)
		ldetach(u.u_cdir);
	fdaclose();

	/*
	 * Free all segments in reverse order, except for user-area.
	 */
	for (n = NUSEG; --n > 0;) {
		if (sp = pp->p_segp[n]) {
			pp->p_segp[n] = NULL;
			sfree(sp);
		}
	}

	/* Detach remaining shared memory segments. */
	shmAllDt();

	/* Adjust(undo) all semaphores. */
	semAllAdjust(pp);

	/*
	 * Wakeup our parent.  If we have any children, init will become the
	 * new parent.  If there are any children we are tracing who are
	 * waiting for us, we wake them up.
	 */
	pp1 = &procq;

	/* pp1 runs through the list of all processes */
	while ((pp1=pp1->p_nforw) != &procq) {

		/* if pp1 points to parent of the current process...*/
		if (pp1->p_pid == pp->p_ppid) {
			parent = pp1;	/* Remember our parent.  */
			if (ASLEEP(pp1) && pp1->p_event==(char *)pp1)
				wakeup((char *)pp1);
		}

		/* if pp1 points to child of the current process...*/
		if (pp1->p_ppid == pp->p_pid) {
			pp1->p_ppid = 1;
			if (pp1->p_state == PSDEAD)
				wakeup((char *)eprocp);
			if (pp1->p_flags&PFTRAC)
				wakeup((char *)&pts.pt_req);
		}
	}

	/*
	 * Mark us as dead and give up the processor forever.
	 */
	pp->p_exit = s;
	pp->p_state = PSDEAD;

	/*
	 * If this is a process group leader, inform all members of the group
	 * of the recent death with a HUP signal.
	 */
	if (pp->p_group == pp->p_pid)
		ukill(-pp->p_pid, SIGHUP);

	/*
	 * If the parent is ignoring SIGCLD, 
	 * remove the zombie right away.
	 */

	if (parent == NULL)
		panic ("%d (@ %x) has no parent!\n", SELF->p_pid, SELF);

	if ((proc_signal_misc (parent) & __SF_NOCLDWAIT) != 0) {
		/*
		 * The parent has requested that no zombie processes be
		 * created out of childen.
		 */

		parent->p_cutime += pp->p_utime + pp->p_cutime;
		parent->p_cstime += pp->p_stime + pp->p_cstime;
		relproc (pp);
	} else {
		/*
		 * Send parent a notification of our demise.
		 */
		__siginfo_t	sigchld;

		sigchld.__si_signo = SIGCHLD;
		sigchld.__si_errno = 0;
		if (__WIFEXITED (s)) {
			sigchld.__si_code = __CLD_EXITED;
			sigchld.__si_status = __WEXITSTATUS (s);
		} else {
			sigchld.__si_code = __WCOREDUMP (s) ? __CLD_DUMPED :
							      __CLD_KILLED;
			sigchld.__si_status = __WTERMSIG (s);
		}
		sigchld.__si_pid = SELF->p_pid;
		proc_send_signal (parent, & sigchld);
	}

	dispatch();
}

/*
 * x_sleep()
 *
 * Surrender CPU while awaiting some event or resource.
 *
 * Arguments:
 *	event:		key value; so wakeup() can find this sleep
 *	schedPri:	prilo/primed/prihi/pritape/pritty/pridisk/prinet
 *			just copied into proc struct for scheduler to use.
 *			(see sys/v_types.h)
 *	sleepPri:	slpriNoSig	- signals may not interrupt sleep
 *			slpriSigLjmp	- signals cause longjmp (EINTR)
 *			slpriSigCatch	- signals are caught
 *			(see sys/sched.h)
 *	reason:		up to 10 chars of text for ps command "event"
 *
 * Return values:
 *	PROCESS_NORMAL_WAKE	wakeup received
 *	PROCESS_SIGNALLED	signal (other than SIGSTOP/SIGCONT) received
 *	PROCESS_CONTINUED	SIGSTOP/SIGCONT (unimplemented now)
 *
 * If longjmp occurs, won't return from x_sleep!
 */

__sleep_t
x_sleep(event, schedPri, sleepPri, reason)
char * event;
int schedPri;
int sleepPri;
char * reason;
{
	int i;
	register PROC *bp;
	register PROC *fp;
	register PROC *pp;
	register int s;

	/*
	 * The descriptive string may be at most 10 characters long.
	 * It will only be NUL terminated if it has 9 or fewer characters.
	 */
	for (i = 0; i < U_SLEEP_LEN; ++i) {
		if ('\0' == (u.u_sleep[i] = reason[i])) {
			break;
		}
	}

	pp = SELF;

	/*
	 * Get ready to go to sleep and do so.
	 */
	s = sphi();
	pp->p_state = (sleepPri == slpriNoSig) ? PSSLEEP : PSSLSIG;
	pp->p_schedPri = schedPri;
	pp->p_event = event;
	fp = &linkq[hash(event)];
	bp = fp->p_lback;
	pp->p_lforw = fp;
	fp->p_lback = pp;
	pp->p_lback = bp;
	bp->p_lforw = pp;
	spl(s);

	/* Here is sleep if signals may *not* interrupt. */
	if (sleepPri == slpriNoSig) {
		dispatch ();
		u.u_sleep [0] = '\0';
		return PROCESS_NORMAL_WAKE;
	}

	/* Here is sleep if signals *may* interrupt. */
	/* Don't sleep at all if there is already a signal pending. */

	if (curr_signal_pending () == 0) {
		dispatch ();
		u.u_sleep [0] = '\0';
		if (curr_signal_pending () == 0)
			return PROCESS_NORMAL_WAKE;
	}

	/* The process has been interrupted from sleep by a signal. */

	if (sleepPri == slpriSigCatch)
		return PROCESS_SIGNALLED;

	/* Do longjmp to beginning of system call. */
	T_HAL(8, printf("[%d]Ljmps ", SELF->p_pid));
	sphi();
	envrest(&u.u_sigenv);
}

/*
 * Defer function to wake up all processes sleeping on the event `e'.
 */
wakeup(e)
char *e;
{
	void dwakeup();

#ifdef TRACER
	/*
	 * In diagnostic kernel, keep return addr on queue as well.
	 */
	int *r=(int*)(&e);
	defer0(dwakeup, e, *(r-1));
#else
	defer(dwakeup, e);
#endif
}

/*
 * Wake up all processes sleeping on "event".
 */
void
dwakeup(event)
char *event;
{
	register PROC *pp;
	register PROC *pp1;
	register int s;

	/*
	 * Identify event queue to check.
	 * Disable interrupts.
	 */
	pp1 = &linkq[hash(event)];
	pp = pp1;
	s = sphi();

	/*
	 * Traverse doubly-linked circular event-queue.
	 */
	while ((pp = pp->p_lforw) != pp1) {

		/*
		 * Process is waiting on event 'event'.
		 */
		if (pp->p_event == event) {
			/*
			 * Remove process from event queue.
			 * Update process priority.
			 * Insert process into run queue.
			 */
			pp->p_lback->p_lforw = pp->p_lforw;
			pp->p_lforw->p_lback = pp->p_lback;
			setrun(pp);

			/*
			 * Enable interrupts.
			 * Restart search at start of event queue.
			 * Disable interrupts.
			 */
			spl(s);
			pp = pp1;
			s = sphi();
		}
	}
	spl(s);
}

/*
 * Select next process for execution, working backward from iprocp.
 * If it is not the idle process, delete it from the run queue.
 * If it is not the current process, consave the current process and
 * conrest the selected process.
 */
dispatch()
{
	register PROC *pp;
	register int s;

	s = sphi();
	pp = iprocp->p_lback;
	if (pp != iprocp) {
		pp->p_lforw->p_lback = pp->p_lback;
		pp->p_lback->p_lforw = pp->p_lforw;
	}
	spl(s);

	quantum = NCRTICK;
	disflag = 0;
	if (pp != SELF) {
		/*
		 * Consave() returns twice.
		 * 1st time is after our context is saved in u.u_syscon,
		 *	whereupon we should restore other proc's context.
		 * 2nd time is after our context is restored by another proc.
		 * Conrest() forces a context switch to a new process.
		 */
		s = sphi ();
		SELF = pp;
		if (consave (& u.u_syscon) == 0) {
			conrest (* pp->p_segp [SIUSERP]->s_vmem,
				 & u.u_syscon);
		}

		if (SELF->p_pid) {	/* init is special! */
			ndpConRest ();
			segload ();
		}

		spl (s);
	}
}

/*
 * Add a process to the run queue, just forward of iprocp.
 * This routine must be called at high priority.
 */
setrun(pp)
register PROC *pp;
{
	pp->p_lback = iprocp;
	pp->p_lforw = iprocp->p_lforw;
	pp->p_lback->p_lforw = pp;
	pp->p_lforw->p_lback = pp;
	pp->p_state = PSRUN;
}

/*
 * Wait for the gate `g' to unlock, and then lock it.
 */
lock(g)
register GATE g;
{
	register int s;

	s = sphi ();
	while (g->_lock [0]) {
#ifdef	TRACER
		if (g->_lock [0] != 1)
			panic ("Uninitialised gate");
#endif
		g->_lock [1] = 1;
		x_sleep ((char *) g, primed, slpriNoSig, "lock");
		/* Waiting for a gate to unlock.  */
	}
	g->_lock [0] = 1;
	__GATE_LOCK_COUNT (g);
	spl (s);
}

/*
 * Unlock the gate `g'.
 */
unlock(g)
register GATE g;
{
#ifdef	TRACER
	if (g->_lock [0] == 0)
		panic ("Gate not locked!");
#endif
	g->_lock [0] = 0;
	if (g->_lock [1]) {
#ifdef	TRACER
		if (g->_lock [1] != 1)
			panic ("Uninitialised gate");
#endif
		g->_lock [1] = 0;
		disflag = 1;
		wakeup ((char *)g);
	}
}
