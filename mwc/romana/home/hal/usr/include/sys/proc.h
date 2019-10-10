/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_PROC_H__
#define	__SYS_PROC_H__

/*
 * Process information.  PROC structs, unlike UPROC structs, are visible
 * whether the process is active or not.
 */

#include <common/feature.h>
#include <common/__pid.h>
#include <common/__cred.h>
#include <common/_uid.h>
#include <kernel/timeout.h>
#include <sys/inline.h>
#include <sys/poll.h>
#include <sys/seg.h>
#include <sys/ksynch.h>
#include <sys/signal.h>

#if	! _KERNEL
# error	You must be compiling the kernel to use this header
#endif


/*
 * In core information about a process.
 * The `p_lforw' and `p_lback' entries must not move as they must be
 * in the same place as in the `plink' structure.
 */

#define	__PROC_VERSION	0x0103

typedef	struct proc	PROC;

struct proc {
	PROC	      *	p_lforw;	/* Working forward pointer */
	PROC	      *	p_lback;	/* Working backward pointer */
	PROC	      *	p_nforw;	/* Next forward pointer */
	PROC	      *	p_nback;	/* Next backward pointer */

	struct __mcon *	p_sysconp;	/* System context save */
	struct sr	p_segl [NUSEG];	/* User segment descriptions */

	__pid_t		p_pid;		/* Process id */
	__pid_t		p_ppid;		/* Process id of parent */
	__pid_t		p_group;	/* Process group ID */
	__pid_t		p_sid;		/* Session ID */

	unsigned	p_state;	/* Scheduling state */
	unsigned	p_flags;	/* Flags */
	__cred_t      *	p_credp;	/* user credentials */
	char	      *	p_sleep;	/* reason for sleeping */

	__sigset_t	p_pending_signals;
	__sigset_t	p_ignored_signals;
	__sigset_t	p_signal_mask;
	__sigset_t	p_queued_signals;
	__sigmiscfl_t	p_sigflags;	/* General signal flags */

	char	 *p_event;		/* Wakeup event channel */
	unsigned p_alarm;		/* Timer for alarms */
	o_dev_t	 p_ttdev;		/* Controlling terminal */
	unsigned p_nice;		/* Nice value */

	int	 p_schedPri;		/* will index into table in sys/ts.h */

	long	 p_utime;		/* User time (HZ) */
	long	 p_stime;		/* System time */
	long	 p_cutime;		/* Sum of childs user time */
	long	 p_cstime;		/* Sum of childs system time */
	int	 p_exit;		/* Exit status */
	event_t *p_polls;		/* Enabled polls */
	TIM	 p_polltim;		/* Poll  timer */
	TIM	 p_alrmtim;		/* Alarm timer */


	char		p_comm [10];	/* Command name */

	struct rlock  *	p_prl;		/* Pending record lock */
	struct sr	p_shmsr[NSHMSEG];	/* Shared Memory Segments */
	struct sem_undo * p_semu;	/* Sem. undo link list */
	struct {
		char	_space [32];
	} p_ddi_space;
	long   p_foodstamp;		/* Starvation inhibitor */
};


/*
 * Status of process (p_state).
 */

#define PSSLEEP	1	/* Sleeping, signals do not interrupt	*/
#define PSRUN	2	/* Running				*/
#define PSDEAD	3	/* Exiting				*/
#define PSSLSIG	4	/* Sleeping, signals interrupt		*/

#define ASLEEP(pp)	(pp->p_state == PSSLEEP || pp->p_state == PSSLSIG)


/*
 * Flags (p_flags).
 */

#define PFCORE	0x0001			/* Process is in core */
#define PFLOCK	0x0002			/* Process is locked in core */
#define PFSWIO	0x0004			/* Swap I/O in progress */
#define PFSWAP	0x0008			/* Process is swapped out */
#define PFWAIT	0x0010			/* Process is stopped (not waited) */
#define PFSTOP	0x0020			/* Process is stopped (waited on) */
#define PFTRAC	0x0040			/* Process is being traced */
#define PFKERN	0x0080			/* Kernel process */
#define PFAUXM	0x0100			/* Auxiliary segments in memory */
#define PFDISP	0x0200			/* Dispatch at earliest convenience */
#define PFNDMP	0x0400			/* Command mode forbids dump */
#define	PFEXEC	0x0800			/* process has done exec () */


/*
 * Values of nice.
 */

#define MINNICE	0			/* Minimum value of nice */
#define DEFNICE	20			/* Default nice value */
#define MAXNICE	40			/* Maximum nice value */


#define SELF		cprocp


/*
 * Global variables.
 */

extern	int	quantum;		/* Current quantum */
extern	int	disflag;		/* Dispatch soon as possible */
extern	PROC	procq;			/* Process queue */
extern	PROC	*eprocp;		/* Init process struct */
extern	PROC	*cprocp;		/* Current process */


/*
 * Note that COHERENT so far has used an idle process; this is unsuitable for
 * future kernel development, so the notion of whether the system is in fact
 * idle should be checked through the following macros.
 */

#if	1

#define	_USE_IDLE_PROCESS	1
#define	__IS_SYSTEM_IDLE()	(SELF == iprocp)
extern	PROC	*iprocp;		/* Idle process struct */

#else

#define	__IS_SYSTEM_IDLE()	(SELF == NULL)

#endif


/*
 * In the case of the global below, introduce some new macros so as not to
 * export it everywhere.
 */

extern	__DUMB_GATE	__pnxgate;		/* Gate for p_next */

#define	__GLOBAL_LOCK_PROCESS_TABLE(where) \
		(__GATE_LOCK (__pnxgate, "lock : pnxgate " where))
#define	__GLOBAL_UNLOCK_PROCESS_TABLE() \
		(__GATE_UNLOCK (__pnxgate))


/*
 * Stubs for the multiprocessor locking functions; it isn't worth doing these
 * right until the process-table iterators have been finalized.
 */

#define	PROC_STATE_LOCK(proc)		splhi ()
#define	PROC_STATE_UNLOCK(proc,pl)	splx (pl)
#define	PROC_ASSERT_LOCKED(proc)	((void) 0)


/*
 * To underscore the fact that attempting to access the process table
 * ties you to one release of the COHERENT system and one only, we put the
 * following into any object file that touches this header.
 */

#define	__PROC_V_SYM(v)	__CONCAT (__proc_v_, v)

extern	char		__PROC_V_SYM (__PROC_VERSION) [];
static	char	      *	__need_proc = __PROC_V_SYM (__PROC_VERSION);

#endif	/* ! defined (__SYS_PROC_H__) */
