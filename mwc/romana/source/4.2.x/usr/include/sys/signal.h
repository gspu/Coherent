/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
#ifndef	__SYS_SIGNAL_H__
#define	__SYS_SIGNAL_H__

#include <common/feature.h>
#include <common/_signal.h>

/*
 * Signal numbers, to be used by DDI/DKI device drivers, as described in
 * signals(D5DK). The numeric values are taken from the System V, Release 4
 * "Programmer's Reference Manual".
 *
 * If the values in this header are to be imported from <signal.h>, then the
 * signal-number constants are required to be macros expanding to positive
 * integral constants as per the ISO C standard (ISO/IEC 9989:1990).
 *
 * This header file should not be directly used by user-level code.
 */

#define	SIGHUP		1	/*
				 * Hangup: the device has been disconnected
				 * (DDI/DKI).
				 * Default action: terminate.
				 */
#define	SIGINT		2	/*
				 * Interrupt: the interrupt character has been
				 * received (DDI/DKI).
				 * Default action: terminate.
				 */
#define	SIGQUIT		3	/*
				 * Quit: the quit character has been received
				 * (DDI/DKI).
				 * Default action: terminate with core image.
				 */
#define	SIGILL		4	/*
				 * Illegal instruction.
				 * Default action: terminate with core image.
				 */
#define	SIGTRAP		5	/*
				 * Trace/Breakpoint trap.
				 * Default action: terminate with core image.
				 */
#define	SIGABRT		6	/*
				 * Abort.
				 * Default action: terminate with core image.
				 */
#define	SIGIOT		SIGABRT	/* Obsolete */
#define	SIGEMT		7	/*
				 * Emulation trap.
				 * Default action: terminate with core image.
				 */
#define	SIGFPE		8	/*
				 * Arithmetic exception.
				 * Default action: terminate with core image.
				 */
#define	SIGKILL		9	/*
				 * Kill. It is an error to request a non-
				 * default disposition for this signal. This
				 * signal cannot be blocked.
				 * Default action: terminate.
				 */
#define	SIGBUS		10	/*
				 * Bus error.
				 * Default action: terminate with core image.
				 */
#define	SIGSEGV		11	/*
				 * Segmentation violation.
				 * Default action: terminate with core image.
				 */
#define	SIGSYS		12	/*
				 * Bad system call.
				 * Default action: terminate with core image.
				 */
#define	SIGPIPE		13	/*
				 * Broken pipe. A write was attempted to a
				 * pipe with no readers.
				 * Default action: terminate.
				 */
#define	SIGALRM		14	/*
				 * Alarm timer expired.
				 * Default action: terminate.
				 */
#define	SIGTERM		15	/*
				 * Terminate.
				 * Default action: terminate.
				 */
#define	SIGUSR1		16	/*
				 * User signal 1.
				 * Default action: terminate.
				 */
#define	SIGUSR2		17	/*
				 * User signal 2.
				 * Default action: terminate.
				 */
#define	SIGCHLD		18	/*
				 * Child status changed.
				 * Default action: ignore.
				 */
#define	SIGCLD		SIGCHLD	/*
				 * Obsolete alias for SIGCHLD.
				 */
#define	SIGPWR		19	/*
				 * Power-failure/restart.
				 * Default action: ignore.
				 */
#define	SIGWINCH	20	/*
				 * Window size changed (DDI/DKI).
				 * Default action: ignore.
				 */
#define	SIGURG		21	/*
				 * Urgent data are available (DDI/DKI).
				 * Default action: ignore.
				 */
#define	SIGPOLL		22	/*
				 * Pollable event. A process must specifically
				 * register to receive this signal (DDI/DKI).
				 * Default action: terminate.
				 */
#define	SIGIO		SIGPOLL	/* Obsolete BSD definition */
#define	SIGSTOP		23	/*
				 * Stop. Discards pending SIGCONT.
				 * Default action: stop.
				 */
#define	SIGTSTP		24	/*
				 * Interactive stop. Discards pending SIGCONT.
				 * DDI/DKI character drivers must not send
				 * this signal.
				 * Default action: stop.
				 */
#define	SIGCONT		25	/*
				 * Continue. Dicards pending SIGSTOP, SIGTSTP,
				 * SIGTTIN, and SIGTTOU signals, and continues
				 * the process if it was stopped.
				 * Default action: ignore.
				 */
#define	SIGTTIN		26	/*
				 * Job control on input. Discards pending
				 * SIGCONT.
				 * Default action: stop.
				 */
#define	SIGTTOU		27	/*
				 * Job control on output. Discards pending
				 * SIGCONT.
				 * Default action: stop.
				 */
#define	SIGVTALRM	28	/*
				 * Virtual timer expired.
				 * Default action: terminate.
				 */
#define	SIGPROF		29	/*
				 * Profiling timer expired.
				 * Default action: terminate.
				 */
#define	SIGXCPU		30	/*
				 * CPU time limit exceeded.
				 * Default action: terminate with core image.
				 */
#define	SIGXFSZ		31	/*
				 * File size limit exceeded.
				 * Default action: terminate with core image.
				 */

#if	! _STDC_SOURCE && ! _POSIX_C_SOURCE
#define NSIG		(_SIGNAL_MAX + 1)	/* Number of signals */
#endif

/*
 * Special arguments to signal.
 */

#define SIG_DFL		((__sighand_t *) 0)	/* Default */
#define SIG_IGN		((__sighand_t *) 1)	/* Ignore */

#if	! _STDC_SOURCE

#define SIG_ERR		((__sighand_t *) -1)	/* Error */
#define SIG_HOLD	((__sighand_t *) 2)	/* Hold */

#if	_SYSV4 && ! _SYSV3

typedef	n_sigset_t	sigset_t;

struct sigaction {
	int		sa_flags;
	__sighand_t   *	sa_handler;
	sigset_t	sa_mask;
	int		__NON_POSIX (sa_resv) [2];
};

#define	SA_NOCLDSTOP	((long) __SF_NOCLDSTOP) << 16)

#if	! _POSIX_C_SOURCE

#define	SA_ONSTACK	__SA_ONSTACK
#define	SA_RESETHAND	__SA_RESETHAND
#define	SA_RESTART	__SA_RESTART
#define	SA_SIGINFO	__SA_SIGINFO
#define	SA_NODEFER	__SA_NODEFER
#define	SA_NOCLDWAIT	((long) __SF_NOCLDWAIT << 16)

#endif

#else	/* ! _SYSV4 */

typedef	o_sigset_t	sigset_t;

struct sigaction {
	__sighand_t   *	sa_handler;
	sigset_t	sa_mask;
	int		sa_flags;
};

#define	SA_NOCLDSTOP		1

#endif	/* ! _SYSV4 */

/*
 * "how" flags for sigprocmask ()
 */

#define	SIG_SETMASK		0
#define	SIG_BLOCK		1
#define	SIG_UNBLOCK		2

#endif	/* ! _STDC_SOURCE */

#endif	/* ! defined (__SYS_SIGNAL_H__) */
