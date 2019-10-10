/* (-lgl
 * 	COHERENT Version 5.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Scheduling parameters.
 * These are identical to the pdp11 parameters except that there is no MAP
 * parameter for claiming the unibus map.
 */

#ifndef	__SYS_SCHED_H__
#define	__SYS_SCHED_H__

#if	_I386

#include <kernel/pri.h>
#include <sys/ksynch.h>

/*
 * sleep priorities
 */
enum {
	slpriNoSig = 0,		/* signals may not interrupt sleep	*/
	slpriSigLjmp = 1,	/* signals cause longjmp (EINTR)	*/
	slpriSigCatch = 2	/* signals are caught			*/
};

/*
 * Update parameters.  All values are in ticks.  The processor value
 * update interval is always 1.
 */
#define	NCRTICK	(HZ/10)		/* Processor time slice */

/*
 * Values.
 */
#define CVNOSIG	256		/* Lower priorities can interrupt */
#define CVCLOCK	1		/* Core value update */

#define CVCHILD	32767		/* Initial child core value		*/
#define	CVSWAP	256		/* Swapper.				*/
#define CVBLKIO	32767 		/* Waiting for block I/O to complete.	*/
#define	CVGATE	16384		/* Waiting for a gate to open.		*/
#define	CVTTOUT	256		/* Terminal output.			*/
#define CVCLIST	256		/* Waiting for free clists.		*/
#define CVPTSET	256		/* Process trace.			*/
#define CVPTRET	256		/* Process trace stop.			*/
#define CVPIPE	256		/* Waiting for a pipe.			*/
#define CVTTIN	255		/* Terminal input.			*/
#define CVPAUSE	0		/* Pause.				*/
#define CVWAIT	128		/* Wait.				*/

#else	/* if ! _I386 */

/*
 * Update parameters.  All values are in ticks.  The processor value
 * update interval is always 1.
 */
#define	NCRTICK	(HZ/10)			/* Processor time slice */
#define	NSUTICK	(HZ/6)			/* Swap value update interval */
#define	NSRTICK	(HZ/2)			/* Swap run update interval */

/*
 * Values.
 */
#define CVNOSIG	256			/* Lower priorities can interrupt */
#define CVCLOCK	1			/* Core value update */
#define	SVCLOCK	16			/* Swap value update */

/*
 * Child process.
 */
#define CVCHILD	32767			/* Initial child core value */
#define IVCHILD	16			/* Importance */
#define SVCHILD	4096			/* Initial child swap value */
#define RVCHILD	0			/* Response value */

/*
 * Swapper.
 */
#define	CVSWAP	256
#define	IVSWAP	0
#define	SVSWAP	0

/*
 * Waiting for block I/O to complete.
 */
#define CVBLKIO	32767
#define IVBLKIO	32767
#define	SVBLKIO	0

/*
 * Waiting for a gate to open.
 */
#define	CVGATE	16384
#define IVGATE	3
#define SVGATE	0

/*
 * Terminal output.
 */
#define	CVTTOUT	256
#define	IVTTOUT	0
#define SVTTOUT	0

/*
 * Waiting for free clists.
 */
#define CVCLIST	256
#define IVCLIST	0
#define SVCLIST	0

/*
 * Process trace.
 */
#define CVPTSET	256
#define IVPTSET	0
#define SVPTSET	0

/*
 * Process trace stop.
 */
#define CVPTRET	256
#define IVPTRET	0
#define SVPTRET	0

/*
 * Waiting for a pipe.
 */
#define CVPIPE	256
#define IVPIPE	0
#define SVPIPE	0

/*
 * Terminal input.
 */
#define CVTTIN	255
#define IVTTIN	1
#define SVTTIN	32767

/*
 * Pause.
 */
#define CVPAUSE	0
#define IVPAUSE	-64
#define SVPAUSE	0

/*
 * Wait.
 */
#define CVWAIT	128
#define IVWAIT	-128
#define SVWAIT	4096

#endif	/* ! _I386 */

#endif	/* ! defined (__SYS_SCHED_H__) */

