/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_DDI_CPU_H__
#define	__KERNEL_DDI_CPU_H__

/*
 * This internal header file defines structures and an access procedure for
 * DDI/DKI data that is global per CPU. In a multiprocessor system, each
 * CPU would have a data region that was separate from all others.
 */

#include <common/ccompat.h>
#include <common/__size.h>
#include <common/_intmask.h>
#include <kernel/_cpuid.h>
#include <kernel/_lock.h>
#include <kernel/ddi_data.h>
#include <sys/debug.h>

/*
 * Many DDI/DKI routines specify constraints on the circumstances in which
 * they may be called, for example.
 *	Base level only, eg SV_WAIT ().
 *	Pass interrupt priority level greater than the current level,
 *		eg LOCK ().
 *	Not called from within a streams service routine, eg putbq () with
 *		a high-priority STREAMS message.
 *
 * These assertions are important for maintaining system integrity, but it can
 * be difficult to detect when these constraints a violated under normal
 * circumstances, especially when the probability of failure is expected to
 * be low. Therefore, it is important for routines which specify such
 * constraints to be able to umambiguously detect constraint violations with
 * the highest level of probability possible.
 *
 * However, the testability of these assertions depends on cooperation from
 * a wide range of kernel facilities. In a highly multithreaded system with
 * either multiple processors or the ability to suspend interrupt contexts,
 * or both, it is necessary to be able to make detailed inquiries about the
 * system state to re-establish some of the certainty about system state that
 * is lost in the move to gain extra concurrency.
 */

/*
 * Returns true if we are at base level or if the system cannot distinguish
 * between base and interrupt level; if the system is definitely not at base
 * level, returns false.
 */

#define	IS_BASE_LEVEL()		(ddi_cpu_data ()->dc_int_level == 0)

/*
 * Return true if we are at interrupt level or if the system cannot
 * distinguish between base and interrupt level; if the system is definitely
 * at base level, return false.
 */

#define	IS_INTERRUPT_LEVEL()	(ddi_cpu_data ()->dc_int_level != 0)

/*
 * Enter/exit an interrupt context, maintaining the state variables checked
 * for above.
 */

#define	__DDI_DKI_ENTER_INTERRUPT() \
		((void) (ddi_cpu_data ()->dc_int_level ++))
#define	__DDI_DKI_LEAVE_INTERRUPT() \
		((void) (ddi_cpu_data ()->dc_int_level --))


/*
 * Most of the time we want to include the above in simple assertions.
 */

#define	ASSERT_BASE_LEVEL()	ASSERT (IS_BASE_LEVEL ())


/*
 * Here is a description of the per-CPU data we wish to record.
 *
 * To deal with lock-hierarchy assertions, we use a table of
 * counters instead of maintaining a list of held locks. We do this because
 * (1) the nature of shared read/write locks permits even a single CPU to hold
 * the same lock multiple times, and (2) although TRYLOCK () and UNLOCK ()
 * allow out-of-order acquisition and release we want LOCK () to check
 * rigidly the hierarchy assertions.  The array of counters is mostly O(1), a
 * desirable property.
 *
 * Although a trace of held locks is not unreasonable, it is difficult to set a
 * fixed upper bound on the number of simultaneous locks. Even if we use a
 * counter for multiple acquisitions of shared locks, we can potentially need
 * as many trace records as allocated locks.
 */

typedef struct ddi_cpu_data	dcdata_t;

struct ddi_cpu_data {
	/*
	 * Interrupt-related data are at the front of this structure for easy
	 * access by hand-coded assembly-language support routines.  Same for
	 * the defer-table stuff.
	 */

	processorid_t	dc_cpuid;		/* who are we for? */

	intmask_t	dc_base_mask;		/* interrupt masks */

	unsigned char	dc_int_level;		/* processing interrupts */
	unsigned char	dc_user_level;		/* user level/kernel level */
	unsigned char	dc_ipl;			/* current ipl */
	atomic_uchar_t	dc_run_timeouts;	/* run deferred timeouts */


	defer_t		dc_defint;		/* interrupt-deferred ops */
	defer_t		dc_defproc;		/* process-deferred ops */

	unsigned long	dc_reserved_1 [4];	/* RESERVED */

	__lkhier_t	dc_max_hierarchy;	/*
						 * For basic-lock hierarchy
						 * assertions
						 */
	__lkhier_t    *	dc_hierarchy_cnt;

	struct pollwait	*
			dc_pollwait;		/* task performing a poll */

	char	      *	dc_dynalloc;		/* for getting per-cpu data */
	char	      *	dc_dynend;		/* end of free per-cpu data */

	unsigned long	dc_idle_ticks;		/* idle ticks */
	unsigned long	dc_idle_time;		/* reserved */
	unsigned char	dc_is_idle;		/* non-zero if system idle */

	unsigned char	dc_reserved_2 [3];	/* RESERVED */
	unsigned long	dc_reserved_3 [4];	/* RESERVED */
};


__EXTERN_C_BEGIN__

dcdata_t      *	ddi_cpu_data	__PROTO ((void));

#if	_DDI_DKI_IMPL
	/*
	 * Functions for the implementation only.
	 */
__VOID__      * ddi_cpu_alloc	__PROTO ((__size_t _size));
dcdata_t      *	ddi_cpu_ref	__PROTO ((processorid_t _cpu));
void		ddi_cpu_unref	__PROTO ((dcdata_t * _data));
processorid_t	ddi_cpu_id	__PROTO ((void));

#endif

__EXTERN_C_END__


extern	dcdata_t	__ddi_cpu_data [];
extern	char		__ddi_cpu_dynarea [512];

#define	ddi_cpu_data()		(__ddi_cpu_data)

#define	_USE_IDLE_PROCESS	1

#define	__IS_CPU_IDLE()		(ddi_cpu_data ()->dc_is_idle)
#define	__MARK_CPU_IDLE()	((void) (ddi_cpu_data ()->dc_is_idle = 1))
#define	__CLEAR_CPU_IDLE()	((void) (ddi_cpu_data ()->dc_is_idle = 0))

#endif	/* ! defined (__KERNEL_DDI_CPU_H__) */
