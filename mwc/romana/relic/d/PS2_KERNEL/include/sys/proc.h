/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/proc.h
 * Process information.
 */

#ifndef _PROC_H
#define	_PROC_H

#include <sys/types.h>
#include <sys/poll.h>
#include <sys/timeout.h>

/*
 * Number of user segments.
 */
#ifdef _I386
#define NUSEG	4
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
	struct	 seg  *p_segp[NUSEG+1];	/* Segments */
	unsigned p_pid;			/* Process id */
	unsigned p_ppid;		/* Process id of parent */
	unsigned p_uid;			/* Effective uid */
	unsigned p_ruid;		/* Real uid */
	unsigned p_rgid;		/* Real gid */
	unsigned p_state;		/* Scheduling state */
	unsigned p_flags;		/* Flags */
	sig_t	 p_ssig;		/* Signals which have been set */
#ifdef _I386
	sig_t	 p_dfsig;		/* Signals which are defaulted */
	sig_t	 p_hsig;		/* Signals which are being held */
	sig_t	 p_dsig;		/* Signals which are being deferred */
#endif
	sig_t	 p_isig;		/* Signals which are being ignored */
	char	 *p_event;		/* Wakeup event channel */
	unsigned p_alarm;		/* Timer for alarms */
	unsigned p_group;		/* Process group */
	dev_t	 p_ttdev;		/* Controlling terminal */
	unsigned p_nice;		/* Nice value */
	unsigned p_cval;		/* Cpu schedule value */
	unsigned p_sval;		/* Swap schedule value */
	int	 p_ival;		/* Importance value */
	unsigned p_rval;		/* Response value */
	unsigned p_lctim;		/* Last time cval was updated */
	long	 p_utime;		/* User time (HZ) */
	long	 p_stime;		/* System time */
	long	 p_cutime;		/* Sum of childs user time */
	long	 p_cstime;		/* Sum of childs system time */
	int	 p_exit;		/* Exit status */
	event_t *p_polls;		/* Enabled polls */
	TIM	 p_polltim;		/* Poll  timer */
	TIM	 p_alrmtim;		/* Alarm timer */
#ifdef _I386
	struct	 rlock	*p_prl;		/* Pending record lock */
#endif
} PROC;

/*
 * Segment indices.
 */
#ifdef _I386
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
#endif

/*
 * Status of process (p_state).
 */
#define PSSLEEP	1			/* Sleeping */
#define PSRUN	2			/* Running */
#define PSDEAD	3			/* Process is exiting */

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
#ifdef _I386
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
#endif

#ifdef KERNEL

#define SELF		cprocp
#define locked(gate)	((gate)[0])
#ifndef _I386
#define p_u		p_segp[SIUSERP]
#endif
/*
 * Functions.
 */
extern	int	idle();
extern	PROC	*process();
extern	int	wakeup();

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
#ifdef _I386
#define hash(e)	((unsigned)(e) % NHPLINK)
#else
#define hash(e)	((((unsigned)(e))>>1)%NHPLINK)
#endif
#endif

/* end of sys/proc.h */
