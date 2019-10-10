/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/proc.h
 *
 * Process information.  PROC structs, unlike UPROC structs, are visible
 * whether the process is active or not.
 *
 * Revised: Wed May 12 09:48:19 1993 CDT
 */

#ifndef __SYS_PROC_H__
#define	__SYS_PROC_H__

#include <common/feature.h>
#include <kernel/timeout.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/seg.h>
#include <sys/ksynch.h>
#include <sys/signal.h>

#if	! __KERNEL__
# error	You must be compiling the kernel to use this header
#endif


/*
 * Number of user segments.
 */

#if	_I386

#define NUSEG	4
#define NSHMSEG	6

#else

#define NUSEG	6

#endif

/*
 * In core information about a process.
 * The `p_lforw' and `p_lback' entries must not move as they must be
 * in the same place as in the `plink' structure.
 */
typedef struct proc {
	struct	 proc *p_lforw;		/* Working forward pointer */
	struct	 proc *p_lback;		/* Working backward pointer */
	struct	 proc *p_nforw;		/* Next forward pointer */
	struct	 proc *p_nback;		/* Next backward pointer */
#if	1
	/*
	 * NIGEL: What is the +1 for ? Who can tell, but it is never, ever,
	 * actually used.
	 */
	struct seg    *	p_segp [NUSEG];	/* Segments */
#else
	struct	 seg  *p_segp[NUSEG+1];	/* Segments */
#endif
	unsigned p_pid;			/* Process id */
	unsigned p_ppid;		/* Process id of parent */
	unsigned p_uid;			/* Effective uid */
	unsigned p_ruid;		/* Real uid */
	unsigned p_rgid;		/* Real gid */
	unsigned p_state;		/* Scheduling state */
	unsigned p_flags;		/* Flags */

/*
 * NIGEL: New deal for signals; the old stuff appears lower in the file.
 */
	__sigset_t	p_pending_signals;
	__sigset_t	p_ignored_signals;
	__sigset_t	p_signal_mask;
	__sigset_t	p_queued_signals;
	__sigmiscfl_t	p_sigflags;	/* General signal flags */

	char	 *p_event;		/* Wakeup event channel */
	unsigned p_alarm;		/* Timer for alarms */
	unsigned p_group;		/* Process group */
	dev_t	 p_ttdev;		/* Controlling terminal */
	unsigned p_nice;		/* Nice value */
#if	_I386
	int	 p_schedPri;		/* will index into table in sys/ts.h */
#else
	unsigned p_cval;		/* Cpu schedule value */
	unsigned p_sval;		/* Swap schedule value */
	int	 p_ival;		/* Importance value */
	unsigned p_rval;		/* Response value */
	unsigned p_lctim;		/* Last time cval was updated */
#endif
	long	 p_utime;		/* User time (HZ) */
	long	 p_stime;		/* System time */
	long	 p_cutime;		/* Sum of childs user time */
	long	 p_cstime;		/* Sum of childs system time */
	int	 p_exit;		/* Exit status */
	event_t *p_polls;		/* Enabled polls */
	TIM	 p_polltim;		/* Poll  timer */
	TIM	 p_alrmtim;		/* Alarm timer */
#if	_I386
	struct	 rlock *p_prl;		/* Pending record lock */
	struct	 sr p_shmsr[NSHMSEG];	/* Shared Memory Segments */
	struct   sem_undo *p_semu;	/* Sem. undo link list */
	struct {
		char	_space [32];
	} p_ddi_space;
#endif
} PROC;

/*
 * Segment indices.
 */

#if	_I386

#define SIUSERP	0			/* User area segment */
#define	SISTEXT	1			/* Shared text segment */
#define SIPDATA	2			/* Private data segment */
#define SISTACK	3			/* Stack segment */
#define SIAUXIL	4			/* Auxiliary segment */
#define	SIBSS	0			/* overlay of SIUSERP [coh/exec.c] */

#else

#define SIUSERP	0			/* User area segment */
#define SISTACK	1			/* Stack segment */
#define	SISTEXT	2			/* Shared text segment */
#define SIPTEXT	3			/* Private text segment */
#define SISDATA	4			/* Shared data segment */
#define SIPDATA	5			/* Private data segment */
#define SIAUXIL	6			/* Auxiliary segment */

#endif	/* ! _I386 */

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

#ifdef QWAKEUP
#define PFWAKE	0x0800			/* Wakeup requested */
extern	int	wakeup2();
extern	int	ntowake;		/* Wakeup pending */
#endif

/*
 * Values of nice.
 */
#define MINNICE	0			/* Minimum value of nice */
#define DEFNICE	20			/* Default nice value */
#define MAXNICE	40			/* Maximum nice value */

/*
 * Number of entries in sleep/wakeup queue.
 */

#if	_I386
#define	NHPLINK	97
#else
#define	NHPLINK	32
#endif

/*
 * Sleep/wakeup queues.
 */
typedef struct plink {
	struct	 proc *p_lforw;		/* Working forward pointer */
	struct	 proc *p_lback;		/* Working backward pointer */
} PLINK;

#if	__KERNEL__

#define SELF		cprocp

#if	! _I386
#define p_u		p_segp[SIUSERP]
#endif

/*
 * Functions.
 */
extern	int	idle();
extern	int	wakeup();
#if	! _I386
extern	PROC	*process();
#endif

/*
 * Global variables.
 */
extern	int	quantum;		/* Current quantum */
extern	int	disflag;		/* Dispatch soon as possible */
extern	int	intflag;		/* We are in an interrupt */
extern	int	cpid;			/* Current process id */
extern	GATE	pnxgate;		/* Gate for p_next */
extern	PROC	procq;			/* Process queue */
extern	PROC	*iprocp;		/* Idle process struct */
extern	PROC	*eprocp;		/* Init process struct */
extern	PROC	*cprocp;		/* Current process */
extern	PLINK	linkq[NHPLINK];		/* Sleep/wakeup hash queue */

#ifndef NOMONITOR
extern	int	swmflag;		/* Monitor swapper */
#endif

/*
 * Function to hash a wakeup channel.
 * Most channels are even.
 */

#if	_I386
#define hash(e)	((unsigned)(e) % NHPLINK)
#else
#define hash(e)	((((unsigned)(e))>>1)%NHPLINK)
#endif

#endif	/* __KERNEL__ */

#endif	/* ! defined (__SYS_PROC_H__) */
