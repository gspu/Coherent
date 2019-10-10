/* $Header: /src386/STREAMS/coh.386/RCS/ker_data.c,v 2.1 93/08/09 13:35:41 bin Exp Locker: bin $ */
/*
 * This file contains definitions for the functions which support the Coherent
 * internal binary-compatibility scheme. We select _SYSV3 to get some old
 * definitions like makedev () visible.
 */
/*
 * $Log:	ker_data.c,v $
 * Revision 2.1  93/08/09  13:35:41  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  14:55:28  nigel
 * Nigel's R80
 * 
 */

#define	_SYSV3		1

#include <common/ccompat.h>
#include <common/_tricks.h>
#include <common/_gregset.h>
#include <kernel/sigproc.h>
#include <kernel/v_types.h>
#include <sys/debug.h>
#include <string.h>

/*
 * These two pull in all the old-style trash.
 */

#include <sys/coherent.h>
#include <sys/uproc.h>
#include <sys/proc.h>

/*
 * Stubs for the multiprocessor locking functions.
 */

#define	PROC_STATE_LOCK(proc)		sphi ()
#define	PROC_STATE_UNLOCK(proc,pl)	spl (pl)
#define	PROC_ASSERT_LOCKED(proc)	((void) 0)


/*
 * Initialize a freshly-allocated process structure to a copy of the current
 * process. As a special case, if there is no current process, the new process
 * is given some default values.
 */

#if	__USE_PROTO__
void PROC_INIT (__proc_t * process)
#else
void
PROC_INIT (process)
__proc_t      *	process;
#endif
{
	process->p_event = NULL;	/* Wakeup event channel */
	process->p_alarm = 0;		/* Timer for alarms */

	process->p_utime = 0L;		/* User time (HZ) */
	process->p_stime = 0L;		/* System time */
	process->p_cutime = 0L;		/* Sum of childs user time */
	process->p_cstime = 0L;		/* Sum of childs system time */
	process->p_exit = 0;		/* Exit status */
	process->p_polls = NULL;	/* Enabled polls */

	/* Poll timer */
	process->p_polltim.t_next = NULL;
	process->p_polltim.t_last = NULL;
	process->p_polltim.t_lbolt = 0L;
	process->p_polltim.t_func = NULL;
	process->p_polltim.t_farg = NULL;

	/* Alarm timer */
	process->p_alrmtim.t_next = NULL;
	process->p_alrmtim.t_last = NULL;
	process->p_alrmtim.t_lbolt = 0L;
	process->p_alrmtim.t_func = NULL;
	process->p_alrmtim.t_farg = NULL;

	process->p_prl = NULL;		/* Pending record lock */
	process->p_semu = NULL;		/* Semaphore undo */

	process->p_flags = PFCORE;
	process->p_state = PSRUN;
	process->p_ttdev = NODEV;

	process->p_pid = 0;

	__SIGSET_EMPTY (process->p_pending_signals);

	if (SELF == NULL) {
		process->p_ppid = 0;
		process->p_nice = 0;
		process->p_schedPri = 0;

		process->p_uid = 0;
		process->p_group = 0;
		process->p_ruid = 0;
		process->p_rgid = 0;

		/*
		 * Set ttdev to null so that we do not accidentally set a tty
		 * for init.
		 */
		process->p_ttdev = makedev (0, 0);

		__SIGSET_EMPTY (process->p_signal_mask);
		__SIGSET_EMPTY (process->p_queued_signals);

		/*
		 * The default action for the following signals is to ignore
		 * them.
		 */

#if	_SIGNAL_MAX <= __LONG_BIT
		process->p_ignored_signals._sigbits [0] =
			__SIGSET_MASK (SIGCHLD) | __SIGSET_MASK (SIGPWR) |
			__SIGSET_MASK (SIGWINCH) | __SIGSET_MASK (SIGURG) |
			__SIGSET_MASK (SIGPOLL) | __SIGSET_MASK (SIGCONT);
#else
		__SIGSET_EMPTY (process->p_ignored_signals);
		__SIGSET_CLRBIT (process->p_ignored, SIGCHLD);
		__SIGSET_CLRBIT (process->p_ignored, SIGPWR);
		__SIGSET_CLRBIT (process->p_ignored, SIGWINCH);
		__SIGSET_CLRBIT (process->p_ignored, SIGURG);
		__SIGSET_CLRBIT (process->p_ignored, SIGPOLL);
		__SIGSET_CLRBIT (process->p_ignored, SIGCONT);
#endif

		process->p_sigflags = 0;
	} else {
		process->p_ppid = SELF->p_pid;
		process->p_nice = SELF->p_nice;
		process->p_schedPri = SELF->p_schedPri;

		process->p_uid = SELF->p_uid;
		process->p_group = SELF->p_group;
		process->p_ruid = SELF->p_ruid;
		process->p_rgid = SELF->p_rgid;

		process->p_ttdev = SELF->p_ttdev;

		process->p_signal_mask = SELF->p_signal_mask;
		process->p_ignored_signals = SELF->p_ignored_signals;
		process->p_queued_signals = SELF->p_queued_signals;

		process->p_sigflags = SELF->p_sigflags;
	}
}


/*
 * Wake a process from a sleep state, with the indicated "reason" code so that
 * the process can know whether sleep was interrupted or not.
 */

#if	__USE_PROTO__
void PROC_WAKE (__proc_t * process, int reason)
#else
void
PROC_WAKE (process, reason)
__proc_t      *	process;
int		reason;
#endif
{
	PROC_ASSERT_LOCKED (process);

	/*
	 * Dequeue the process from the hash list used for old-style sleep ().
	 */

	process->p_lback->p_lforw = process->p_lforw;
	process->p_lforw->p_lback = process->p_lback;

	/*
	 * Change the process state to runnable and queue the process on the
	 * appropriate run queue.
	 */

	setrun (process);
}

#define	PROC_WAKE_SIGNAL(proc)	PROC_WAKE (proc, PROCESS_SIGNALLED)


/*
 * Internal version of sigaction (), which allows retrieval and optional
 * replacement of the current signal information.
 *
 * Note that we take a copy of the current information in a temporary before
 * trying to fiddle the new information; this allows the "act" and "oact"
 * pointers to point to the same space.
 */

#if	__USE_PROTO__
void curr_signal_action (int signal, __CONST__ __sigaction_t * act,
			 __sigaction_t * oact)
#else
void
curr_signal_action (signal, act, oact)
int		signal;
__CONST__ __sigaction_t
	      *	act;
__sigaction_t *	oact;
#endif
{
	__sigaction_t *	proc_act;

	ASSERT (oact != NULL || act != NULL);
	ASSERT (signal > 0 && signal <= _SIGNAL_MAX);

	proc_act = & u.u_sigact [signal - 1];

	if (act != NULL) {
		__sigmask_t	mask = __SIGSET_MASK (signal);
		__sigaction_t	temp;

		/*
		 * Exchange the "action" data.
		 */

		temp = * proc_act;
		* proc_act = * act;
		if (oact != NULL)
			* oact = temp;

		/*
		 * The SIGILL, SIGTRAP and SIGPWR signals do not have their
		 * handlers reset to SIG_DFL even if the user requests this.
		 * Again, this is "silently enforced" so it happens at this
		 * layer.
		 */

#if	_SIGNAL_MAX <= __LONG_BIT
		if ((mask & (__SIGSET_MASK (SIGILL) | __SIGSET_MASK (SIGTRAP) |
			     __SIGSET_MASK (SIGPWR))) == 0)
#else
		if (signal == SIGILL || signal == SIGTRAP || signal == SIGPWR)
#endif
			proc_act->sa_flags &= ~ __SA_RESETHAND;

		/*
		 * Set the ignore flags to match handler. Note that we do not
		 * clear the pending signals for the current process here;
		 * that will happen on the way out of the system call we are
		 * in during the check for signals to deliver.
		 */

		if (proc_act->sa_handler == SIG_IGN)
			__SIGSET_ADDMASK (SELF->p_ignored_signals, signal,
					  mask);
		else
			__SIGSET_CLRMASK (SELF->p_ignored_signals, signal,
					  mask);

		/*
		 * Existing System V, Release 4 systems zero the mask if the
		 * signal is defaulted or ignored, so we copy them.
		 */

		if (proc_act->sa_handler == SIG_IGN ||
		    proc_act->sa_handler == SIG_DFL)
			__SIGSET_EMPTY (proc_act->sa_mask);

		/*
		 * The default action for the following signals is for them to
		 * be ignored: SIGCHLD, SIGPWR, SIGWINCH, SIGURG, SIGPOLL,
		 * SIGCONT.
		 */

#if	_SIGNAL_MAX <= __LONG_BIT
		if (((__SIGSET_MASK (SIGCHLD) | __SIGSET_MASK (SIGPWR) |
		      __SIGSET_MASK (SIGWINCH) | __SIGSET_MASK (SIGURG) |
		      __SIGSET_MASK (SIGPOLL) | __SIGSET_MASK (SIGCONT))
		     & mask) != 0
#else
		if ((signal == SIGCHLD || signal == SIGPWR ||
		     signal == SIGWINCH || signal == SIGURG ||
		     signal == SIGPOLL || signal == SIGCONT)
#endif
		    && proc_act->sa_handler == SIG_DFL)
			__SIGSET_ADDMASK (SELF->p_ignored_signals, signal,
					  mask);

		if ((proc_act->sa_flags & __SA_SIGINFO) != 0)
			__SIGSET_ADDMASK (SELF->p_queued_signals, signal,
					  mask);
	} else
		* oact = * proc_act;
}


/*
 * Obtain and possibly change the current process's signal mask. Note that
 * we capture the current mask to a temporary before beginning, thus allowing
 * "mask" and "omask" to point to the same storage.
 */

#if	__USE_PROTO__
void curr_signal_mask (__CONST__ __sigset_t * mask, __sigset_t * omask)
#else
void
curr_signal_mask (mask, omask)
__CONST__ __sigset_t
	      *	mask;
__sigset_t    *	omask;
#endif
{
	ASSERT (omask != NULL || mask != NULL);

	if (mask != NULL) {
		__sigset_t	temp;

		/*
		 * The caller of this function should arrange for collateral
		 * actions such as checking for newly-unmasked signals in such
		 * as way as to ensure that signal actions are dealt with
		 * promptly and to ensure that locking is not needed.
		 *
		 * For instance, at this point signals may be briefly held
		 * which in fact "cannot" be. We don't care about this; the
		 * above constraint on our callers will ensure that this has
		 * no effect.
		 */

		temp = SELF->p_signal_mask;
		SELF->p_signal_mask = * mask;
		if (omask != NULL)
			* omask = temp;

		/*
		 * SIGKILL and SIGSTOP may never be blocked or caught. Here we
		 * enforce the blocking part, which as the SVR4 PRM says is
		 * "silently enforced" by the system.
		 */

		__SIGSET_CLRBIT (SELF->p_signal_mask, SIGKILL);
		__SIGSET_CLRBIT (SELF->p_signal_mask, SIGSTOP);
	} else
		* omask = SELF->p_signal_mask;
}


/*
 * Test the signal mask of the given process to determine whether the given
 * signal is masked.
 */

#if	__USE_PROTO__
int proc_signal_masked (__proc_t * process, int signal)
#else
int
proc_signal_masked (process, signal)
__proc_t      *	process;
int		signal;
#endif
{
	ASSERT (process != NULL && signal > 0 && signal <= _SIGNAL_MAX);
	return __SIGSET_TSTBIT (process->p_signal_mask, signal);
}


/*
 * Return a zero value if there are no signals pending, otherwise return the
 * number of the first unmasked, non-ignored signal.
 *
 * NOTE: The code here interacts with curr_signal_action () and
 * proc_send_signal () in some ways that need comment; specifically, the
 * treatment of ignored signals may seem sloppy. However, the System V usage
 * of allowing ignored signals to be blocked, and then caught if a handler is
 * subsequently attached means that the approach of clearing pending, ignored
 * signals at this point is much simpler than dealing with them at all the
 * other points (arrival, unblocking, handler attach).
 */

#if	__USE_PROTO__
int curr_signal_pending (void)
#else
int
curr_signal_pending ()
#endif
{
	int		signo;

	/*
	 * If any signals have arrived, but which are not held, figure out
	 * what they are. This takes a little doing.
	 */

	for (signo = 0 ;
	     signo < __ARRAY_LENGTH (SELF->p_pending_signals._sigbits) ;
	     signo ++) {
		__sigmask_t	mask;

		mask = (SELF->p_pending_signals._sigbits [signo] &=
			~ SELF->p_ignored_signals._sigbits [signo]) &
				~ SELF->p_signal_mask._sigbits [signo];
		if (mask == 0)
			continue;

		/*
		 * There is at least one signal.  Extract its number
		 * from the signal bits.
		 */

		T_HAL(8, printf ("curr_signal_pending () = %d ",
				 signo * sizeof (__sigmask_t) * __CHAR_BIT +
					__SIGSET_FIRSTBIT (mask)));

		return signo * sizeof (__sigmask_t) * __CHAR_BIT +
				__SIGSET_FIRSTBIT (mask);
	}
	return 0;
}


#if	_SIGNAL_MAX <= __LONG_BIT
/*
 * For optimizing handling of job-control signals.
 */

#define	STOP_SIGNALS	(__SIGSET_MASK (SIGSTOP) | __SIGSET_MASK (SIGTSTP) |\
			 __SIGSET_MASK (SIGTTIN) | __SIGSET_MASK (SIGTTOU))
#endif

/*
 * Add the given signal to the indicated process's pending-signal mask and if
 * the process requires some notification (such as being woken if asleep, or
 * interrupted if scheduled on another processor) then perform that.
 *
 * This function can only be called from base level, but we must be aware of
 * the fact that process state-changes can happen at interrupt level and that
 * we can be interacting with multiple processors.
 */

#if	__USE_PROTO__
void proc_send_signal (__proc_t * process, __CONST__ __siginfo_t * siginfo)
#else
void
proc_send_signal (process, siginfo)
__proc_t      *	process;
__CONST__ __siginfo_t
	      *	siginfo;
#endif
{
	__sigmask_t	mask;
	pl_t		prev_pl;

	ASSERT (process != NULL && siginfo != NULL);
	ASSERT (siginfo->__si_signo > 0 && siginfo->__si_signo <= _SIGNAL_MAX);

	mask = __SIGSET_MASK (siginfo->__si_signo);

	if (siginfo->__si_signo == SIGCONT) {
		/*
		 * When a SIGCONT is received, all pending stop signals are
		 * discarded and if the process is stopped, it is continued.
		 */

#if	_SIGNAL_MAX <= __LONG_BIT
		__SIGSET_CLRMASK (process->p_pending_signals, 0,
				  STOP_SIGNALS);
#else
		__SIGSET_CLRBIT (process->p_pending_signals, SIGSTOP);
		__SIGSET_CLRBIT (process->p_pending_signals, SIGTSTP);
		__SIGSET_CLRBIT (process->p_pending_signals, SIGTTIN);
		__SIGSET_CLRBIT (process->p_pending_signals, SIGTTOU);
#endif

#ifdef	PROC_IS_STOPPED
		if (PROC_IS_STOPPED (process))
			PROC_CONTINUE (process);
#endif
	}

#if	_SIGNAL_MAX <= __LONG_BIT
	if ((mask & STOP_SIGNALS) != 0) {
#else
	if (siginfo->__si_signo == SIGSTOP ||
	    siginfo->__si_signo == SIGTSTP ||
	    siginfo->__si_signo == SIGTTIN ||
	    siginfo->__si_signo == SIGTTOU) {
#endif
		/*
		 * When a stop signal is received, any pending SIGCONT signal
		 * is discarded.
		 */

		__SIGSET_CLRBIT (process->p_pending_signals, SIGCONT);
	}


#if	0
	if (__SIGSET_TSTMASK (process->p_queued_signals, siginfo->__si_signo,
			      mask) != 0) {
		/*
		 * Reliable signal-queueing is in effect, so we must stash
		 * away a copy of the structure pointed at by "siginfo" so we
		 * can deliver the information as an additional argument to
		 * the signal-catching function.
		 */
		ASSERT ("Not implemented" == NULL);
	}
#endif	/* NOT IMPLEMENTED */


	/*
	 * Here we are about to modify some shared per-process state; now is a
	 * good time to take a lock, so we structure the code so it can be
	 * released. We require a basic lock for the state-change code, so we
	 * use that lock to cover the process-level guard as well. In a uni-
	 * processor system, the process-level guard is not necessary, but the
	 * extra area covered is small enough that it doesn't matter.
	 */

	prev_pl = PROC_STATE_LOCK (process);

	__SIGSET_ADDMASK (process->p_pending_signals,
			  siginfo->__si_signo, mask);

	if (! __SIGSET_TSTMASK (process->p_signal_mask,
				siginfo->__si_signo, mask) &&
	    ! __SIGSET_TSTMASK (process->p_ignored_signals,
				siginfo->__si_signo, mask) &&
	    process->p_state == PSSLSIG) {
		/*
		 * The process is sleeping and wants to be woken by
		 * signals, arrange for it to come out of sleep so it
		 * can get to user-level and process the signal.
		 */

		PROC_WAKE_SIGNAL (process);
	}

	PROC_STATE_UNLOCK (process, prev_pl);

	T_HAL (8, printf (" ign = %x hold = %x pend = %x ",
			  process->p_ignored_signals._sigbits [0],
			  process->p_signal_mask._sigbits [0],
			  process->p_pending_signals._sigbits [0]));
}


/*
 * Obtain and/or modify the current process's miscellaneous signal-flags. The
 * previous flags are returned, making "curr_signal_misc (0, 0)" the fetch.
 */

#if	__USE_PROTO__
__sigmiscfl_t curr_signal_misc (__sigmiscfl_t clear, __sigmiscfl_t set)
#else
__sigmiscfl_t
curr_signal_misc (clear, set)
__sigmiscfl_t	clear;
__sigmiscfl_t	set;
#endif
{
	__sigmiscfl_t	temp = SELF->p_sigflags;

	SELF->p_sigflags = (temp & ~ clear) | set;
	return temp;
}


/*
 * Return the given process's miscellaneous signal information. Note that we
 * don't try and deal with synchronization issues here, so whatever code tests
 * this data has to find some way of being multi-processor safe (concretely,
 * it means that the uses of the signal information here are not atomic at the
 * system-call level).
 */

#if	__USE_PROTO__
__sigmiscfl_t proc_signal_misc (__proc_t * process)
#else
__sigmiscfl_t
proc_signal_misc (process)
__proc_t      *	process;
#endif
{
	return process->p_sigflags;
}


/*
 * Remove a signal from the pending signal mask.
 */

#if	__USE_PROTO__
void proc_unkill (__proc_t * process, int signal)
#else
void
proc_unkill (process, signal)
__proc_t      *	process;
int		signal;
#endif
{
	__SIGSET_CLRBIT (process->p_pending_signals, signal);
}


/*
 * Perform a register dump from a gregset_t, plus any useful data from the
 * current process.
 */

#if	__USE_PROTO__
void curr_register_dump (gregset_t * regsetp)
#else
void
curr_register_dump (regsetp)
gregset_t     *	regsetp;
#endif
{
	printf ("\neax=%x  ebx=%x  ecx=%x  edx=%x\n", regsetp->_i386._eax,
		regsetp->_i386._ebx, regsetp->_i386._ecx, regsetp->_i386._edx);
	printf ("esi=%x  edi=%x  ebp=%x  esp=%x\n", regsetp->_i386._esi,
		regsetp->_i386._edi, regsetp->_i386._ebp, regsetp->_i386._esp);
	printf ("cs=%x  ds=%x  es=%x  ss=%x  fs=%x  gs=%x\n",
		regsetp->_i386._cs, regsetp->_i386._ds, regsetp->_i386._es,
		regsetp->_i386._ss, regsetp->_i386._fs, regsetp->_i386._gs);
	printf ("err #%d eip=%x  uesp=%x  cmd=%s\n", regsetp->_i386._err,
		regsetp->_i386._eip, regsetp->_i386._uesp, u.u_comm);
	printf ("efl=%x  ", regsetp->_i386._eflags);

	if ((regsetp->_i386._cs & R_USR) != R_USR) {
		/*
		 * This register set is indicating something that happened in
		 * the kernel, so do a call backtrace. To make this a little
		 * more robust, we only go upwards in the stack from the frame
		 * where we are, so that a smashed %ebp or frame causes no
		 * problems.
		 */

		__ulong_t     *	ebp = (__ulong_t *) regsetp->_i386._ebp;
		__ulong_t     *	prev = & regsetp->_i386._ebp;
		while (ebp > prev) {
			printf (" -> %x", * (ebp + 1));
			ebp = (__ulong_t *) * (prev = ebp);
		}
		printf ("\n");
	}
}

