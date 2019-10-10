/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Signals.
 */

#ifndef	 SIG_H
#define	 SIG_H	SIG_H

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

#ifndef	KERNEL
/*
 * For the benefit of user programs.
 */
int	(*signal())();

#endif

#endif
