#ifndef	__KERNEL_DDI_LOCK_H__
#define	__KERNEL_DDI_LOCK_H__

/*
 * This is an internal header file, not intended for general use.
 *
 * This is a registry of value and constants for the lock hierarchy values
 * recommended for use in internal DDI/DKI basic and read/write locking
 * operations. Many of these hierarchy values are outside the range that
 * are permitted to DDI/DKI-compliant drivers, and a special value in the
 * "lk_flags" member of the "lkinfop" structure passed to LOCK_ALLOC () is
 * necessary to permit access to this range of hierarchy values.
 *
 * We also specify a recommended minimum priority value here which should be
 * used with the indicated lock whenever a specific value is not indicated.
 */

enum {
	INTERNAL_LOCK = 1,		/* access to reserved hierarchies */

	/*
	 * For polling, the global lock nests outside the "pollhead" lock
	 * which nests outside the "pollwait" lock.
	 */

	pollwait_hierarchy = 40,
#define	pollwait_priority 	plhi

	pollhead_hierarchy = 39,
#define	pollhead_priority	plhi

	poll_global_hierarchy = 38,
#define	poll_global_priority	plhi


	/*
	 * For the timeout system. The timeout code calls defer_int_cpu ().
	 */

	timeout_global_hierarchy = 39,
#define	timeout_global_priority	plhi


	/*
	 * For the defer system.
	 */

	defer_hierarchy = 40,
#define	defer_priority		plhi


	/*
	 * Process-related locks.
	 */

	proc_global_hierarchy = 40,
#define	proc_global_priority	plhi


	/*
	 * For streams, the stream directory lock nests outside the stream
	 * head lock, which nests outside STREAMS memory allocation. STREAMS
	 * locks are all at "plstr" by definition *except* for the streams
	 * heap locks, which have a priority conditional on various factors.
	 * Both of the streams heap locks call defer... () routines.
	 */

	stream_heap_hierarchy = 39,
	stream_head_hierarchy = 38,
	stream_dir_hierarchy = 37,


	/*
	 * The STREAMS schedule, bufcall event, qprocsoff (), and trace-
	 * sequence locks are all disjoint.
	 */

	stream_schedule_hierarchy = 40,
	stream_event_hierarchy = 40,
	stream_proc_hierarchy = 40,
	stream_seq_hierarchy = 40

};

#endif	/* ! defined (__KERNEL_DDI_LOCK_H__) */
