#ifndef __KERNEL_STRMLIB_H__
#define __KERNEL_STRMLIB_H__

/*
 * This ^^^^^^^^^^^^^^^^^ symbol is used in the DDI/DKI header <sys/ddi.h> to
 * determine which #undef directives it is required to perform, on the basis
 * that it should avoid touching namespaces unless they have been reserved
 * by the inclusion of a header which reserves such classes of names.
 */

/*
 * This file contains definitions that are be used for the implementation of
 * the STREAMS standard library routines and scheduling code. Some details
 * of the interfaces between STREAMS and the rest of the system will also
 * be dealt with here so as to insulate the STREAMS implementation from the
 * details of that interface.
 *
 * Note that as the contents of this header are for private, system internal
 * use only, names do not begin with underscores.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__PROTO ()
 *	<common/xdebug.h>
 *		__LOCAL__
 *	<common/__size.h>
 *		__size_t
 *	<common/__clock.h>
 *		__clock_t
 *	<common/__pid.h>
 *		__pid_t
 *	<kernel/st_alloc.h>
 *		_ST_HEAP_CONTROL
 *	<kernel/defer.h>
 *		defer_int_any ()
 *	<sys/ksynch.h>
 *		lock_t
 *		sv_t
 *	<sys/uio.h>
 *		uio_t
 */

#include <common/ccompat.h>
#include <common/xdebug.h>
#include <common/__size.h>
#include <common/__clock.h>
#include <common/__pid.h>
#include <kernel/st_alloc.h>
#include <kernel/defer.h>
#include <sys/inline.h>
#include <sys/ksynch.h>
#include <sys/uio.h>

#include <common/_stream.h>
#include <kernel/ddi_lock.h>
#include <kernel/ddi_glob.h>

/*
 * In order to be able to declare prototypes that refer to structures that are
 * declared in other headers, we supply incomplete declarations at top-level
 * to avoid some scoping problems.
 */

struct stroptions;
struct strbuf;


/*
 * In onder for freezestr () to work as defined in the Multiprocessor DDI/DKI,
 * it cannot simply be implemented in terms of the high-level locking
 * operations defined in <sys/ksynch.h> (the reason being that it is specified
 * as raising the processor priority level, whereas the "pl" parameter to the
 * high-level locking functions is specified as setting the level, with a
 * caution that it not cause the level to be lowered).
 *
 * Stream queue freezing cannot really be implemented in terms of high-level
 * basic locks because of the hierarchy mechanism; the relative priority of
 * stream head locks is determined by relative position in a stream so that
 * following the "q_next" member yields increasing (virtual) hierarchy values,
 * with the additional constraint that a given context may only hold a lock on
 * one side of a stream at a time.
 */

#define	SFREEZE_INIT(q)		((void) ATOMIC_CLEAR_UCHAR ((q)->q_locked))
#define	SFREEZE_DESTROY(q)	((void) 0)
#define	SFREEZE_LOCK(q,name)	TEST_AND_SET_LOCK ((q)->q_locked, plstr, \
						   (name))
#define	SFREEZE_UNLOCK(q,pl)	(ATOMIC_CLEAR_UCHAR ((q)->q_locked), \
				 (void) splx (pl))
#define	SFREEZE_ASSERT_FROZEN(q) ASSERT (ATOMIC_FETCH_UCHAR ((q)->q_locked))


/*
 * The implementation of allocb () and freeb () deals in terms of "triples"
 * consisting of an mblk_t, a dblk_t, and the actual data being managed.
 * (Of course, under esballoc () the data belongs to another subsystem, but
 * such a double should also be managed by the same system).
 *
 * Be aware that changing the definition of any of these things may
 * invalidate assumptions made by the code in allocb (), freeb (), dupb ()
 * and esballoc ().
 */

/*
 * Various ways to map between the components of a triple.
 */

#define	MB_TO_DB(mp)		((dblk_t *) (mp + 1))
#define	DB_TO_MB(db)		(((mblk_t *) db) - 1)
#define	DB_TO_DATA(db)		((unsigned char *) (db + 1))
#define	DATA_TO_DB(d)		(((dblk_t *) d)) - 1)


/*
 * Some useful predicates for determining whether various items are actually
 * parts of triples or not, since there are exceptional cases where the
 * individual elements of a triple have been scattered to the four winds.
 *
 * Note that the SET_MB_TRIPLE () and SET_MB_FREE () exist as a pair, since
 * they only relate to message blocks that are part of a triple.
 * SET_MB_TRIPLE () is used when a message block is instantiated, and
 * SET_MB_FREE () is used when a message block is deallocated but the rest
 * of the triple is still in use.
 */

#define	IS_DB_USER_DATA(db)	(db->db_base != DB_TO_DATA (db))
						/*
						 * Test to see whether the
						 * data referenced by this
						 * block is user-controlled.
						 */

#define	IS_TRIPLE_TOGETHER(mp)	(mp->b_datap == MB_TO_DB (mp))
						/*
						 * Test to see whether the
						 * data block referenced by
						 * this message block forms
						 * part of the same triple.
						 */

#define	SET_MB_TRIPLE(mp)	(void) ((mp)->b_flags =\
				 ((mp)->b_flags | MSGTRIPLE) & ~ MSGFREE)
						/*
						 * Mark the message block as
						 * part of a triple.
						 */

#define	IS_MB_TRIPLE(mp)	(((mp)->b_flags & MSGTRIPLE) != 0)
						/*
						 * Test whether the message
						 * block is part of a triple
						 * or is a floating block.
						 */

#define	SET_MB_FREE(mp)		(void) ((mp)->b_flags |= MSGFREE)
						/*
						 * Indicate that the message
						 * block is no longer in
						 * use. Only for message
						 * blocks in triples.
						 */

#define	IS_MB_FREE(mp)		((mp->b_flags & MSGFREE) != 0)
						/*
						 * Test whether the message
						 * block is free or not.
						 */

/*
 * STREAMS Local extension: scheduling
 */

/*
 * The methods used in the multiplexing examples in the STREAMS Programmer's
 * Guide for System V Release 2 are quite inefficient. The following general
 * STREAMS queue scheduling structures and routines were defined in a visible
 * manner since
 *
 *   (i) It served to better-document the way in which queues are serviced
 *	 within this STREAMS implementation,
 *
 *  (ii) Multiplexing drivers (and device drivers which multiplex several
 *	 minor numbers on a single I/O bus, such as an Ethernet or SCSI
 *	 driver) need effcient mechanisms for managing multiple request
 *	 channels,
 *
 * (iii) It allows drivers to leverage the multiprocessor locking that is
 *	 built into these primitives.
 *
 * Note that allowing drivers access to this facility causes a significant
 * departure from the standard version of STREAMS, but that driver use of the
 * macros and data structures below permits other implementations.
 *
 * As this system is used internally by this STREAMS implementation, it can
 * use the "q_link" member of a queue to hold a link to the next STREAM on
 * a schedule. In addition, some "q_flag" bits can be used to control
 * whether or not a queue is currently threaded on any schedule. A portable
 * implementation that does not depend on any reserved parts of STREAMS
 * data structures may not use these fields, and so may operate under a
 * different set of constraints. However, since in this case the drivers and
 * STREAMS itself share common scheduling code, the following restrictions
 * must be observed:
 *
 *   (i) A queue may be requested to be scheduled more than once with no
 *	 effect. If a queue is requested to be scheduled on different
 *	 schedules, then this is probably an error, most likely to occur
 *	 if you define both a STREAMS service routine and a schedule for
 *	 a driver queue; this should not be done under the common scheme.
 *
 *  (ii) A queue which has noenable () set on it will still be scheduled.
 *	 The manner in which qenable ()/noenable () work has no bearing on
 *	 this mechanism.
 *
 * (iii) Schedules should only be created or destroyed with the schedule
 *	 allocation/deallocation routines in order to guarantee that the
 *	 schedule is multi-processor addressable and is the correct size for
 *	 the selected run-time environment.
 *
 *  (iv) When a driver is closed, the queue must not be on any schedule in
 *	 order to prevent list corruption and the dire effects that can
 *	 result from such corruption. A portable implementation of scheduling
 *	 has no way to do this automatically for the driver since the
 *	 close () call is managed by STREAMS.
 *
 * Note that in this common implementation, conditions (i), (iii), and (iv)
 * can be checked by the scheduling system if compiled such that debugging
 * code is inserted.
 */

/*
 * STREAMS scheduler private per-schedule global data structure.
 */

struct __streams_schedule {
	queue_t	      *	ss_head;	/* head of list of scheduled queues */
	queue_t	      *	ss_tail;	/* tail of list of scheduled queues */

	lock_t	      *	ss_locked;	/* lock for schedule */
};


extern lkinfo_t	__stream_schedule_lkinfo;

#define	SCHLOCK_INIT(s,flag) \
	((s)->ss_locked = LOCK_ALLOC (stream_schedule_hierarchy, plstr, \
				      & __stream_schedule_lkinfo, (flag)))

#define	SCHLOCK_DESTROY(s)	LOCK_DEALLOC ((s)->ss_locked)

#define	SCHLOCK_LOCK(s,n)	LOCK ((s)->ss_locked, plstr)

#define	SCHLOCK_UNLOCK(s,p)	UNLOCK ((s)->ss_locked, p)

#define	SCHLOCK_ASSERT_LOCKED(s) \
		ASSERT (TRYLOCK ((s)->ss_locked, plstr) == invpl)

#define SCHED_INIT(s,flag)	((s)->ss_head = (s)->ss_tail = NULL, \
				 SCHLOCK_INIT (s, flag))


/*
 * I use these everywhere that I know which queue side I've got. These
 * macros duplicate the old-style RD () and WR () functionality, which has
 * been changed along the path from System V Release 3.2 to System V Release
 * 4 Multi-Processor. When compiling under the DDI/DKI, these macros are also
 * converted to functions.
 */

#define W(q)    	((q) + 1)
#define R(q)    	((q) - 1)


/*
 * A utility function, for use with STRMEM_ALLOC (), to calculate the size
 * required for a message triple with "n" bytes of associated data buffer.
 */

#define	MSGB_SIZE(n)	((n) + sizeof (mblk_t) + sizeof (dblk_t))


/*
 * The following data type is used to hold registration information for the
 * SIGPOLL signal. Every process that registers for SIGPOLL signals allocates
 * one of these structures and threads it on the stream head.
 */

typedef struct sigpoll	sigpoll_t;


/*
 * The following is the internal structure (an "event cell" in STREAMS)
 * used by the bufcall ()/esbbcall () mechanism to record the necessary
 * information for calling the user back.
 *
 * Note that the real System V bufcall ()/esbbcall () takes a a pointer to
 * a function with undefined parameters, yet also accepts a parameter which
 * it will then pass on to the callback. The only way to implement this at
 * all portably is to use some version of the ISO <stdarg.h> mechanism to
 * allow bufcall () to take any argument in it's "natural" form and then
 * pass a maximum-sized chunk of stack into the callback in order to capture
 * all of the information that the client wanted to pass.
 *
 * Of course, doing this is opening a wide door to all kinds of problems,
 * since while there are some sleazy ways to restrict the size of the extra
 * argument to something reasonable, there is no way that we can make sure
 * that the shapes of the argument and the callback parameter really do
 * match (eg, the callback takes a long but bufcall () is given an int).
 *
 * Of course, this is all no worse than the situation AT&T have under the
 * DDI/DKI, where bufcall () is required to take the callback parameter as
 * a "long" and clients assume that pointers have the same shape as longs
 * (if you look at the example in bufcall (D3DK) that is exactly what A&T
 * seem to be suggesting - blech).
 *
 * There is no entirely satisfactory way to get around this evil in plain C,
 * although we can get at least a partial solution through supplying a range
 * of supplementary definitions that (at least in an ISO environment) ensure
 * that the function argument types and argument types will be coerced to
 * something reasonable.
 *
 * Event management is made a little more complex by the availability of the
 * unbufcall () procedure, which requires us to assign a numeric ID code to
 * each event cell. It would be nice if this ID code also included some kind
 * of generation field, and mapped instantly to the desired value.
 *
 * Note that this is exactly the same problem faced by the timeout () routine
 * in implementing untimeout (), and there are numerous possible solutions,
 * each of which maximizes some desirable property at the cost of losing some
 * other property.
 */

#define	_TOID_MEMBER	1

typedef void (* se_funcptr_t)	__PROTO ((long _arg));

typedef struct __stream_event sevent_t;

struct __stream_event {
	sevent_t      *	se_next;     	/* next event in chain */
	sevent_t      *	se_prev;	/* previous event in chain */
	long    	se_arg;		/* argument for function */
	se_funcptr_t	se_func;	/* function to call */
	unsigned int	se_size;	/* optional size information */
#if _TOID_MEMBER
	toid_t		se_id;		/* timeout id for item */
#endif
};


/*
 * Of course, we have to thread the above events on a list. We'll create a
 * structure for this to abstract the details of the locking scheme we'll use
 * for maintaining the consistency of the event lists.
 *
 * We store a generator for the timeout ID codes in the list header so that
 * the ID generation step can be protected by the same lock as the list
 * manipulation code. This means that at initialisation time each list header
 * should have the initial ID code set to its number and that the increment
 * value used to step between codes is one greater than the total number of
 * list headers (so that code 0 never appears).
 *
 * We have a wide range of possible policies for managing event cells. We need
 * to get some performance data on each to make a final decision.
 */

#define	_FIFO_BUFCALL	1

typedef struct __stream_event_list selist_t;

struct __stream_event_list {
	lock_t	      *	sl_locked;
	sevent_t      *	sl_head;
#if _FIFO_BUFCALL
	sevent_t      *	sl_tail;
#endif
#if _TOID_MEMBER
	toid_t		sl_id;
#endif
};


extern lkinfo_t	__stream_event_lkinfo;

#define	SELIST_INIT(s,flag) \
	((s)->sl_head = NULL, \
	 (s)->sl_locked = LOCK_ALLOC (stream_event_hierarchy, plstr, \
				      & __stream_event_lkinfo, (flag)))

#define SELIST_DESTROY(s)	LOCK_DEALLOC ((s)->sl_locked)

#define	SELIST_LOCK(s)		LOCK ((s)->sl_locked, plstr)

#define	SELIST_UNLOCK(s,pl)	UNLOCK ((s)->sl_locked, (pl))

#define	SELIST_ASSERT_LOCKED(s)	\
		ASSERT (TRYLOCK ((s)->sl_locked, plstr) == invpl)


/*
 * Some handy requirements for timeout ID generation. We have to define these
 * numbers such that each list above generates a non-overlapping sequence that
 * overflows into the same sequence, eg.
 *	1, k + 1, 2k + 1, ... ik + 1, 1, ...
 *	2, k + 2, 2k + 2, ... ik + 2, 2, ...
 *
 * Since each list generates a distinct and identifiable sequence we can map
 * from the ID to the list in one step, which somewhat ameliorates the cost
 * of having to search the list to find the given event.
 */

#define	TOID_INCREMENT		(N_PRI_LEVELS + 1)
#define	TOID_MODULUS		(TOID_MAX - TOID_MAX % TOID_INCREMENT)
#define	TOID_TO_PRI(id)		((id) % TOID_INCREMENT - 1)


/*
 * Watermark control structures:
 *
 * We record here the amount of memory held by each allocation band and the
 * maximum amount we will permit it to hold. In addition, we record a thread
 * of structures used by bufcall () and esbbcall () to record information
 * for callbacks to drivers when memory becomes available.
 */
/*
 * A note on bufcall ()/esbbcall () structures;
 *
 * I am not able to conceive of a comprehensive strategy for dealing with
 * these things that works well under all circumstances. For now, it seems
 * that keeping a few around in preallocated event cells is a good idea for
 * when we run out of memory. However, as long as possible the system will
 * always attempt to satisfy a request for an event cell with a newly
 * allocated cell, since we expect the usual reason for being unable to
 * satisfy a request is that memory is merely fragmented rather than fully
 * occupied.
 *
 * Of course, allocating bufcall cells within the heap may increase the
 * level of fragmentation - which is why we prefer to allocate them from
 * the "long-term" heap (and indeed, why we have two heaps at all).
 */

#define	N_PRI_LEVELS		3	/*
					 * Number of buffer priority levels
					 */
#define MAP_PRI_LEVEL(p)	p	/*
					 * Map from BPRI_xxx to index.
					 */

/* #define SPLIT_STREAMS_MEMORY */	/*
					 * Control whether there are separate
					 * arenas for messages and other
					 * allocations.
					 */

/*
 * We keep several lists of stream heads according to the category of the
 * stream; device, pipe, and possibly FIFO. Various general operations require
 * iterating over several of the lists, and so we define an array of list
 * head pointers to simplify things since we don't have the C++ "pointer to
 * member" types and operations available.
 */

typedef	struct __stream_head shead_t;

typedef enum slist_id {
	DEV_SLIST,
	PIPE_SLIST,
	SLIST_MAX
} slist_id_t;

typedef	int		muxid_t;


/*
 * This structure grew to its present form before the <kernel/ddi_glob.h>
 * header took shape. Most of it should eventually be moved there, with
 * appropriate thought given to what should be fixed and what should be
 * variable allocations.
 */


struct streams_mem {
	atomic_uchar_t	sm_init;	/* primitive lock for startup */

	__size_t	sm_used;	/* memory used in the message heap */
	__size_t	sm_max [N_PRI_LEVELS];
					/*
					 * Maximum memory we let a level use
					 */

	toid_t		sm_bcid;	/* id generator for bufcall events */
	selist_t	sm_bcevents [N_PRI_LEVELS];
					/*
					 * Bufcall event linked-list head.
					 */
	ssched_t	sm_sched [1];	/* List of enabled queues. */

	lock_t	      *	sm_seq_lock;	/* Lock sequence number registers. */
	unsigned long	sm_err_seq;	/* error log sequence number */
	unsigned long	sm_trc_seq;	/* trace log sequence number */
	unsigned long	sm_con_seq;	/* console log sequence number */

	queue_t	      *	sm_log_rq;	/* STREAMS logger device */

	lock_t	      *	sm_proc_lock;	/* Basic lock for qprocsoff (). */
	sv_t	      *	sm_proc_sv;	/*
					 * Synchronization variable for
					 * qprocsoff ().
					 */
	queue_t	      *	sm_log_drvr;	/* Log-driver read side. */

	rwlock_t      *	sm_head_lock;	/* Lock for stream head list */
	shead_t	      *	sm_streams [SLIST_MAX];
					/* Chains of stream heads */

	__size_t	sm_maxctlsize;	/* max. size of control message */
	__size_t	sm_maxdatasize;	/* max. size of data message part */


	/*---------- Locked by sm_msg_lock ------------*/
	lock_t	      *	sm_msg_lock;	/* basic lock for message heap */
	_ST_HEAP_CONTROL_P sm_msg_heap;	/* Memory heap for STREAMS messages */
	sv_t	      *	sm_msg_sv;	/*
					 * synchronization variable for
					 * waiting for free memory.
					 */
	__size_t	sm_msg_needed;	/* Level of free memory required */

#ifdef	SPLIT_STREAMS_MEMORY
	/*---------- Locked by sm_other_lock ------------*/

	lock_t	      *	sm_other_lock;	/* basic lock for "other" heap */

	_ST_HEAP_CONTROL_P sm_other_heap;/* Memory heap for long-term info */

	sv_t	      *	sm_other_sv;	/*
					 * synchronization variable for
					 * waiting for free memory.
					 */
	__size_t	sm_other_needed;/* Level of free memory required */
#else
# define	sm_other_heap	sm_msg_heap
# define	sm_other_lock	sm_msg_lock
# define	sm_other_sv	sm_msg_sv
# define	sm_other_needed	sm_msg_needed
#endif
};


/*
 * These should be the standard priorities to be used with the locks for
 * controlling access to the various memory pools.
 */

#ifdef	SPLIT_STREAMS_MEMORY

# define	str_msg_pl	plstr
# define	str_other_pl	plhi

#else

# define	str_msg_pl	plhi
# define	str_other_pl	plhi

#endif


/*
 * When someone thinks it might be a good idea to see about running some of
 * the bufcall routines, we defer a routine to deal with the checking. A
 * global flag bit protects
 */

#define SCHEDULE_BUFCALLS() \
   (ATOMIC_FETCH_AND_STORE_UCHAR (ddi_global_data ()->dg_run_bufcalls, 1) \
			== 1 ? (void) 0 : (void) defer_int_any (RUN_BUFCALLS))


/*
 * If all allocations are coming from the same pool, non-message allocations
 * need to be counted in with the allocation-priority information.
 *
 * The "other" pool (which will really be the message pool) should be locked
 * when these functions are called.
 */

#ifdef	SPLIT_STREAMS_MEMORY

# define	OTHER_ALLOCED(size)
# define	OTHER_FREED(size)

#else

# define	OTHER_ALLOCED(size)	(void) (str_mem->sm_used += size)
# define	OTHER_FREED(size)	((void) (str_mem->sm_used -= size), \
					 SCHEDULE_BUFCALLS ())

#endif


/*
 * We need access to a global instance of the above for managing STREAMS
 * memory in a consistent manner.
 */

extern struct streams_mem str_mem [];


/*
 * Category flags for the sh_lock_mask member. This is a collection of flag
 * bits which correspond to operations on the "sh_wait_sv" synchronization
 * variable.
 */

typedef enum category {
	SH_NONE		= 0,		/* use for paranoid checking */
	SH_OPENCLOSE	= 1,
	SH_IOCTL_LOCK	= 2,
	SH_READ_LOCK	= 4,		/* NOT NORMALLY USED */
	SH_WRITE_LOCK	= 8,		/* NOT NORMALLY USED */
	SH_PEEK_LOCK	= 16,

	SH_LOCK_MASK	= 31,		/* Mask for lock bits */


	SH_READ_WAIT	= 32,
	SH_WRITE_WAIT	= 64,
	SH_IOCTL_WAIT	= 128,
	SH_DRAIN_WAIT	= 256,
	SH_PEEK_WAIT	= 512,

	SH_WAIT_MASK	= 992,		/* Mask for wait bits */


	/*
	 * Not a lock mask; just use the extra flag space.
	 */

	SH_TIMEFLAG	= 8192
} cat_t;


/*
 * Stream head control structure.
 *
 * Be careful with locking the stream head; for many operations, it may be
 * sufficient to freeze the stream head.
 */

struct __stream_head {
	n_dev_t		sh_dev;		/* key for lookup */
	shead_t	      *	sh_next;	/* next stream head on chain */

	queue_t       *	sh_head;	/* read queue of stream head */
	struct streamtab
		      *	sh_tab;		/* initialisation data for driver */

	__clock_t	sh_cltime;	/* time to wait for close to drain */

	struct pollhead
		      *	sh_pollhead;	/* for polling support */

	pid_t		sh_pgrp;	/* foreground TTY process group */
	__VOID__      *	sh_controller;	/* reference to controlling process */

	int		sh_rerrcode;	/* error code from M_ERROR */
	int		sh_werrcode;	/* error code from M_ERROR */

	unsigned short	sh_flags;	/* miscellaneous flags */

	short		sh_readopt;	/* read options */
	short		sh_wropt;	/* write options */

	unsigned short	sh_wroff;	/* write offset */

	sigpoll_t     *	sh_sigs;	/* processes registered for SIGPOLL */

	toid_t		sh_read_bufcall;/* ID for read bufcall () */

	shead_t	      *	sh_linked;	/* head of stream we are linked to */
	muxid_t		sh_muxid;	/* our ID below that stream */


	lock_t	      *	sh_basic_lockp;	/* */

	/*---- For process-level locking and timeout operations ----*/

	sv_t	      *	sh_wait_sv;	/* for process lock operations */

	int		sh_lock_mask;	/* which categories are locked */

	short		sh_ref_count;	/* references to stream memory */
	short		sh_open_count;	/* successful open count */
	short		sh_attach_count;/* attachments to stream */
	short		sh_lock_count;	/* processes queued or locked */
	short		sh_time_count;	/* count of timeout checks */

	__clock_t	sh_timeout_tick;/* current soonest timeout */
	toid_t		sh_timeout_id;	/* ID of current active timeout */

	int		sh_ioc_seq;	/* ioctl () sequence number */
	mblk_t	      *	sh_ioc_msg;	/* ioctl () messages */
};


/*
 * A collection of functions for dealing with the stream head basic lock.
 */

#define	SHEAD_LOCK(sheadp)	LOCK ((sheadp)->sh_basic_lockp, plstr)
#define	SHEAD_UNLOCK(sheadp,pl)	UNLOCK ((sheadp)->sh_basic_lockp, pl)
#define	SHEAD_ASSERT_LOCKED(sheadp) \
		ASSERT ((sheadp) != NULL && \
			TRYLOCK ((sheadp)->sh_basic_lockp, plstr) == invpl)


/*
 * Masks for use with the "sh_flags" member.
 */

enum {
	SH_READMSG	= 0x0001,	/* generate M_READ messages */
	SH_NDELAY	= 0x0002,	/* non-STREAMS O_NDELAY semantics */
	SH_TTY		= 0x0004,	/* stream is acting as a tty */
	SH_TOSTOP	= 0x0008,	/* job control on background writes */


	SH_HANGUP	= 0x0010,	/* stream has been hung up */

	SH_PLINK	= 0x0020,	/* stream is permanently linked */

	/*
	 * Permanent flags set when the stream is created. We do not need a
	 * flag to indicate a pipe, because pipes have a NULL "sh_tab" entry
	 * to mark them.
	 */

	SH_MASTER	= 0x0100,	/* master end of stream pipe */


	/*
	 * This flag enables serialization of reads and writes. This is a
	 * local, experimental extension to STREAMS.
	 */

	SH_RWLOCKING	= 0x0200
};


/*
 * For stream pipes there is a master/slave relationship between the stream
 * heads at each end. The following macros define some handy relationships
 * between them to allow quick mapping from one to another.
 */

#define	SHEAD_M2SLAVE(m)	((m) + 1)
#define	SHEAD_SLAVE2M(s)	((s) - 1)
#define	SHEAD_MASTER(sheadp)	(((sheadp)->sh_flags & SH_MASTER) == 0 ? \
				 SHEAD_SLAVE2M (sheadp) : (sheadp))
#define	SHEAD_OTHER(sheadp)	(((sheadp)->sh_flags & SH_MASTER) == 0 ? \
				 SHEAD_SLAVE2M (sheadp) : \
				 SHEAD_M2SLAVE (sheadp))

#define	SHEAD_IS_PIPE(sheadp)	((sheadp)->sh_tab == NULL)

#define	SHEAD_HANGUP(sheadp)	(((sheadp)->sh_flags & SH_HANGUP) != 0)

#define	SHEAD_READMSG(sheadp)	(((sheadp)->sh_flags & SH_READMSG) != 0)


/*
 * For working in with the abstract filesystem layer, we define a format for
 * our opaque information.
 */

typedef	struct {
	shead_t	      *	sheadp;
} scookie_t;



__EXTERN_C_BEGIN__

ssched_t      *	QSCHED_ALLOC	__PROTO ((void));
void		SSCHED_FREE	__PROTO ((ssched_t * _sched));
int		QSCHED_SCHEDULE	__PROTO ((queue_t * _q, ssched_t * _sched));
void		QSCHED_UNSCHEDULE
				__PROTO ((queue_t * _q, ssched_t * _sched));
queue_t	      *	QSCHED_GETFIRST	__PROTO ((ssched_t * _sched));

pl_t		QFREEZE_TRACE	__PROTO ((queue_t * _q,
					  __CONST__ char * _name));
void		QFROZEN_TRACE	__PROTO ((queue_t * _q,
					  __CONST__ char * _name));
void		QUEUE_TRACE	__PROTO ((queue_t * _q,
					  __CONST__ char * _name));
void		QUNFREEZE_TRACE	__PROTO ((queue_t * _q, pl_t _pl));

void		QUEUE_BACKENAB	__PROTO ((queue_t * _q));
queue_t	      *	QUEUE_NEXT	__PROTO ((queue_t * _q));

mblk_t	      *	STRMEM_ALLOC	__PROTO ((__size_t _size, int _pri,
					  int _flag));
void		STRMEM_FREE	__PROTO ((mblk_t * _bp, __size_t _size));

mblk_t	      *	MSGB_ALLOC	__PROTO ((__size_t _size, int _pri,
					  int _flag));

void		SHEAD_WAKE	__PROTO ((shead_t * _sheadp, cat_t _flag));
void		SHEAD_SIGNAL	__PROTO ((shead_t * _sheadp, uchar_t _sig));
int		SHEAD_SRDOPT	__PROTO ((shead_t * _sheadp, int _flag));
void		QBAND_SETOPT	__PROTO ((queue_t * _q,
					  struct stroptions * _so));

qband_t	      *	QUEUE_BAND	__PROTO ((queue_t * _q, uchar_t _pri));
qband_t	      *	QBAND_PREV	__PROTO ((queue_t * _q, qband_t * _qbandp));

void		RUN_BUFCALLS	__PROTO ((void));
void		RUN_STREAMS	__PROTO ((void));

shead_t	      *	SHEAD_FIND	__PROTO ((n_dev_t _dev, slist_id_t _id));

int		STREAMS_OPEN	__PROTO ((n_dev_t * _devp,
					  struct streamtab * _stabp,
					  int _mode, cred_t * _credp));
int		STREAMS_CLOSE	__PROTO ((shead_t * _sheadp, int _mode,
					  cred_t * _credp));

int		STREAMS_READ	__PROTO ((shead_t * _sheadp, uio_t * _uiop));
int		STREAMS_WRITE	__PROTO ((shead_t * _sheadp, uio_t * _uiop));
int		STREAMS_IOCTL	__PROTO ((shead_t * _sheadp, int _cmd,
					  _VOID * _arg, int _mode,
					  cred_t * _credp, int * _rvalp));

int		STREAMS_CHPOLL	__PROTO ((shead_t * _sheadp, short _events,
					  int _anyyet, short * _reventsp,
					  struct pollhead ** _phpp));

int		STREAMS_GETPMSG	__PROTO ((shead_t * _sheadp,
					  struct strbuf * _ctlbuf,
					  struct strbuf * _databuf,
					  int * _bandp, int * _flagsp,
					  int _mode, int * _rvalp));
int		STREAMS_PUTPMSG	__PROTO ((shead_t * _sheadp,
					  struct strbuf * _ctlbuf,
					  struct strbuf * _databuf, int _band,
					  int _flags, int _mode,
					  int * _rvalp));

__EXTERN_C_END__


/*
 * This internal definition is the dual to QFREEZE_TRACE (), being the
 * internal implementation of unfreezestr ().
 */

#define	QUNFREEZE_TRACE(q,pl)	SFREEZE_UNLOCK (q, pl)


/*
 * Dealing with queue priority bands can be a little tricky; in particular,
 * the definition of the linked-list structure of priority bands has some
 * characteristics that may be unsuitable for some implementations.
 * Specifically, the "qband" structure apparently is designed to function as
 * a member of a singly-linked list rooted in the "queue" structure. This is
 * flexible, but if there are several priority bands the time spent in
 * dereferencing the link pointers might outweigh any extra cost involved in
 * keeping the "band" structures in a vector [While the allocation cost is
 * neglible in terms of time, there is a potentially high cost in terms of
 * increasing the likelihood of a failure to allocate a band structure].
 *
 * The following internal functions have been defined to abstract away the
 * details of how band structures are accessed from the queue, allowing a
 * more time-efficient vector scheme to be used, or even a hybrid scheme
 * where vectors are preferred but a linked-list fallback is available.
 *
 * The SVR4 DDI/DKI used to document the band structure, but that structure is
 * no longer documented as of the SVR4 MP DDI/DKI.
 */

#define	VECTOR_BANDS
#define	VECTOR_BANDS_TEST		/* test mode for VECTOR_BANDS */

#ifdef	VECTOR_BANDS

#define	QUEUE_BAND(q,pri)	((pri) > (q)->q_nband ? NULL : \
					& (q)->q_bandp [pri - 1])

#define	QBAND_PREV(q,bandp)	((bandp) > (q)->q_bandp ? (bandp) - 1 : NULL)

#endif	/* defined (VECTOR_BANDS) */


#endif /* ! __KERNEL_STRMLIB_H__ */
