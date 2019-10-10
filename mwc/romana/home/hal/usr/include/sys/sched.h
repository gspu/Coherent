/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_SCHED_H__
#define	__SYS_SCHED_H__

#include <kernel/pri.h>
#include <sys/ksynch.h>

/*
 * sleep priorities
 */
enum {
	slpriNoSig = 0,		/* signals may not interrupt sleep */
	slpriSigLjmp = 1,	/* signals cause longjmp (EINTR) */
	slpriSigCatch = 2	/* signals are caught */
};

/*
 * Update parameters.  All values are in ticks.  The processor value
 * update interval is always 1.
 */

#define	NCRTICK	(HZ/10)		/* Processor time slice */
#define DEFAULT_QUANTUM ((NCRTICK)) /* Food for thought */
#define SCHED_SCALE 100		/* Scale usage factor */

#endif	/* ! defined (__SYS_SCHED_H__) */

