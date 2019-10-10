/*
 * For the benefit of user programs.
 *
 * /usr/include/sys/signal.h
 */

#ifndef	 SIGNAL_H
#define	 SIGNAL_H	SIGNAL_H

#define SIGHUP	1			/* Hangup */
#define	SIGINT	2			/* Interrupt */
#define SIGQUIT	3			/* Quit */
#define SIGILL	4			/* Illegal instruction */
#define	SIGTRAP	5			/* Trace trap */	
#define SIGIOT	6			/* IOT instruction */
#define SIGABRT	6			/* replace by SIGIOT in the future */
#define	SIGEMT	7			/* emulator trap */
#define	SIGFPE	8			/* floating point exception */
#define SIGKILL	9			/* Kill */
#define	SIGBUS	10			/* bus error */
#define	SIGSEGV	11			/* Segmentation violation */
#define SIGSYS	12			/* Bad argument to system call */
#define	SIGPIPE	13			/* Write to pipe with no readers */
#define SIGALRM	14			/* Alarm */
#define SIGTERM	15			/* Software termination signal */
#define	SIGUSR1	16
#define	SIGUSR2	17
#define	SIGCLD	18			/* Death of a child - not done yet */
#define	SIGCHLD	18			/* Death of a child - not done yet */
#define SIGPWR	19			/* Restart */
#define	SIGWINCH 20			/* window change */
#define	SIGPOLL	22			/* polled event in stream */

#define NSIG	23			/* Number of signals */
#define	MAXSIG	32

/*
 * Special arguments to signal.
 */
#define SIG_DFL (void(*)())0		/* Default */
#define SIG_ERR (void(*)())-1		/* Error */
#define SIG_IGN (void(*)())1		/* Ignore */
#define SIG_HOLD (void(*)())2		/* Hold */

extern void	(*signal())();
extern void	(*sigset())();

#ifdef	KERNEL
#define	SIGDEFER	0x100
#define	SIGHOLD		0x200
#define	SIGRELSE	0x400
#define	SIGIGNORE	0x800
#define	SIGPAUSE	0x1000
#define	SIGDEFAULT	0x8000
#endif

#endif
