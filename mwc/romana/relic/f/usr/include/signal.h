/*
 * /usr/include/signal.h
 *
 * COHERENT signal support.
 *
 * Revised: Wed May 12 07:56:47 1993 CDT
 */
#ifndef	 __SIGNAL_H__
#define	 __SIGNAL_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/__pid.h>

#if	_I386

#include <sys/signal.h>

typedef	long	sig_atomic_t;


__EXTERN_C_BEGIN__

__sigfunc_t	signal		__PROTO ((int _sig, __sigfunc_t _func));
int		raise		__PROTO ((int _sig));

#if	! _STDC_SOURCE

int		kill		__PROTO ((__pid_t _pid, int _sig));
int		sigaction	__PROTO ((int _sig,
					  __CONST__ struct sigaction * _act,
					  struct sigaction * _oact));
int		sigaddset	__PROTO ((sigset_t * _set, int _signo));
int		sigdelset	__PROTO ((sigset_t * _set, int _signo));
int		sigemptyset	__PROTO ((sigset_t * _set));
int		sigfillset	__PROTO ((sigset_t * _set));
int		sigismember	__PROTO ((__CONST__ sigset_t * _set,
					  int _signo));
int		sigpending	__PROTO ((sigset_t * _set));
int		sigprocmask	__PROTO ((int _how,
					  __CONST__ sigset_t * _set,
					  sigset_t * _oset));
int		sigsuspend	__PROTO ((__CONST__ sigset_t * _sigmask));

#if	_SYSV3

/*
 * Pre-SVR4 systems make these available as inlines. For us, this is optional
 * but still legal. For SVR4 systems, we require proper error checking. For
 * this to work, we depend on __SIGSET_UNIT (ss, n) not evaluating "n".
 */

#define	sigfillset(set)		((set)->_sigbits [0] = -1UL, 0)
#define	sigemptyset(set)	((set)->_sigbits [0] = 0)
#define	sigismember(set, signo)	((set)->_sigbits [0] & __SIGSET_MASK (signo))
#define sigaddset(set, signo)	((set)->_sigbits [0] |= \
					__SIGSET_MASK (signo), 0)
#define	sigdelset(set, signo)	((set)->_sigbits [0] &= \
					~ __SIGSET_MASK (signo), 0)

#endif	/* _SYSV3 */

#endif	/* ! _STDC_SOURCE */

__EXTERN_C_END__

#else	/* if ! _I386 */

#include <sys/msig.h>

#define SIGHUP	1			/* Hangup */
#define	SIGINT	2			/* Interrupt */
#define SIGQUIT	3			/* Quit */
#define SIGALRM	4			/* Alarm */
#define SIGTERM	5			/* Software termination signal */
#define SIGREST	6			/* Restart */
#define SIGSYS	7			/* Bad argument to system call */
#define	SIGPIPE	8			/* Write to pipe with no readers */
#define SIGKILL	9			/* Kill */
#define SIGTRAP	10			/* Breakpoint */
#define	SIGSEGV	11			/* Segmentation violation */

#endif	/* ! _I386 */

#endif	/* ! defined (__SIGNAL_H__) */
