#ifndef	__SYS_KSYNCH_H__
#define	__SYS_KSYNCH_H__

/*
 * Support for basic locks.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__VOID__
 *		__PROTO ()
 *	<kernel/_lock.h>
 *		__lock_t
 *	<kernel/x86lock.h>
 *		atomic_uchar_t
 *	<sys/types.h>
 *		bool_t
 *		pl_t
 */

#if	_DDI_DKI

#include <common/ccompat.h>
#include <kernel/_lock.h>
#include <kernel/x86lock.h>
#include <sys/types.h>

/*
 * We can provide a greater level of error checking/reporting if we know we
 * are running on a uniprocessor. Use this feature-test macro to control
 * compilation of code for uniprocessors.
 *
 * Note that many of the traditional simplifying assumptions about
 * uniprocessor systems may not apply if basic-lock acquisition can schedule
 * interrupt contexts (ie, if the system is designed such that interrupts
 * need not be turned off). Such as system would behave effectively like a
 * multiprocessor, although most of the extra checking could still be valid
 * with information gained from the interrupt-handler scheduling system. 
 */

#define	__UNIPROCESSOR__


/*
 * For greater scalability, a "ticket lock" should be preferred over a basic
 * test-and-set or test-and-test-and-set spin lock, since it guarantees
 * FIFO ordering and reduces the level of memory contention (important for
 * multiprocessor shared-memory cache performance). See the paper cited at the
 * top of this file for more details.
 *
 * However, ticket locks take slightly more memory and are slightly slower
 * that basic test-and-set spin locks, so we only use them in situations
 * where we expect them to be needed.
 */

#ifdef	__UNIPROCESSOR__
#define	__TICKET_LOCK__
#endif


/*
 * Even if ticket locks are used, in the absence of a fetch-and-increment
 * atomic operation a basic test-and-set style lock must be used to control
 * the "ticket gate", so the style of test-and-set lock used will always have
 * consequences. A simple test-and-set spin lock reduces delay, while a
 * test-and-test-and-set spin reduces contention.
 */

#define	__TEST_AND_TEST__


/*
 * Information structure used for holding debugging information about
 * lock structures.
 */

struct lock_info {
	__CONST__ char * lk_name;	/* name for statistics gathering */
	int		lk_flags;	/* must be zero */
	int		lk_pad;
};

typedef	struct lock_info lkinfo_t;


/*
 * No lock statistics structure is currently defined, but we reserve the name.
 *
 * For the other lock structures, we publish only their names, and leave the
 * types incomplete.
 */

typedef	__VOID__		lkstat_t;
typedef	__lock_t		lock_t;
typedef struct synch_var	sv_t;
typedef struct sleep_lock	sleep_t;
typedef struct readwrite_lock	rwlock_t;


__EXTERN_C_BEGIN__

/* Basic lock functions */

pl_t		LOCK 		__PROTO ((lock_t * _lockp, pl_t _pl));
lock_t	      *	LOCK_ALLOC	__PROTO ((__lkhier_t _hierarchy,
					  pl_t _min_pl, lkinfo_t * _lkinfop,
					  int _flag));
void		LOCK_DEALLOC	__PROTO ((lock_t * _lockp));
pl_t		TRYLOCK		__PROTO ((lock_t * _lockp, pl_t _pl));
void		UNLOCK		__PROTO ((lock_t * _lockp, pl_t _pl));


/* Synchronisation variable functions */

sv_t	      *	SV_ALLOC	__PROTO ((int _flag));
void		SV_BROADCAST	__PROTO ((sv_t * _svp, int _flags));
void		SV_DEALLOC	__PROTO ((sv_t * _svp));
void		SV_SIGNAL	__PROTO ((sv_t * _svp, int _flags));
void		SV_WAIT		__PROTO ((sv_t * _svp, int _priority,
					  lock_t * _lkp));
bool_t		SV_WAIT_SIG	__PROTO ((sv_t * _svp, int _priority,
					  lock_t * _lkp));


/* Sleep locks */

sleep_t	      *	SLEEP_ALLOC	__PROTO ((int _arg, lkinfo_t * _lkinfop,
					  int _flag));
void		SLEEP_DEALLOC	__PROTO ((sleep_t * _lockp));
void		SLEEP_LOCK	__PROTO ((sleep_t * _lockp, int _priority));
bool_t		SLEEP_LOCKAVAIL	__PROTO ((sleep_t * _lockp));
bool_t		SLEEP_LOCKOWNED	__PROTO ((sleep_t * _lockp));
bool_t		SLEEP_LOCK_SIG	__PROTO ((sleep_t * _lockp, int _priority));
bool_t		SLEEP_TRYLOCK	__PROTO ((sleep_t * _lockp));
void		SLEEP_UNLOCK	__PROTO ((sleep_t * _lockp));


/* Read/write locks */

rwlock_t      *	RW_ALLOC	__PROTO ((__lkhier_t _hierarchy,
					  pl_t _min_pl, lkinfo_t * _lkinfop,
					  int _flag));
void		RW_DEALLOC	__PROTO ((rwlock_t * _lockp));
pl_t		RW_RDLOCK	__PROTO ((rwlock_t * _lockp, pl_t _pl));
pl_t		RW_TRYRDLOCK	__PROTO ((rwlock_t * _lockp, pl_t _pl));
pl_t		RW_TRYWRLOCK	__PROTO ((rwlock_t * _lockp, pl_t _pl));
void		RW_UNLOCK	__PROTO ((rwlock_t * _lockp, pl_t _pl));
pl_t		RW_WRLOCK	__PROTO ((rwlock_t * _lockp, pl_t _pl));

__EXTERN_C_END__

#endif	/* _DDI_DKI */

/*
 * This type is used in the old Coherent source as a primitive form of sleep
 * lock. It should go ASAP, but there's a limit to how much people will let
 * me get away with changing... this type is used in a structure which cannot
 * alter because the "configuration" mechanism for Coherent kernels is to
 * stuff values into the linked object file, in particular the device switch
 * table!
 */

#if	! _I386

typedef	unsigned char	GATE [2];

#else	/* if _I386 */

typedef	struct {
	unsigned char	_lock [2];
	unsigned	_count;
	char	      *	_name;
} GATE [1];

#define	__GATE_INIT(g, name)	((g)->_lock [1] = (g)->_lock [0] = 0, \
				 (g)->_name = (name), (g)->_count = 0)
#define	__GATE_LOCK_COUNT(g)	((g)->_count ++)
#define __GATE_LOCKED(gate)	((gate)->_lock [0])

#endif	/* ! _I386 */

#endif	/* ! defined (__SYS_KSYNCH_H__) */
