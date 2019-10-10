/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_V_PROC_H__
#define	__KERNEL_V_PROC_H__

/*
 * This function describes a high-level abstract interface to the low-level
 * scheduling system that is suitable for multiprocessor use. The design of
 * this interface has been primarily motivated by the multiprocessor DDI/DKI
 * sleep lock and synchronization variable functions, which impose a rather
 * different model of sleep incompatible with the old-style sleep () and
 * wakeup () functions. See "ksynch.c" for a detailed discussion of this.
 *
 * This interface is private to this DDI/DKI implementation, and is not
 * part of the DDI/DKI itself. Portable code should not rely on the contents
 * of this header at all.
 */

#include <common/ccompat.h>
#include <kernel/_sleep.h>
#include <kernel/ddi_proc.h>


/*
 * Flag values to pass to MAKE_SLEEPING ().
 */

enum {
	SLEEP_NO_SIGNALS,
	SLEEP_INTERRUPTIBLE
};


/*
 * The following structure type defines the information that a client of
 * MAKE_SLEEPING is required to keep.
 *
 * If the underlying implementation uses an adress-based hash-list like the
 * old-style sleep () and wakeup () functions, then this item need not contain
 * any data. However, making at least one byte in size gives the property that
 * the address of such an item is unique. While this would normally contain
 * pointers threading process-table entries together, an implementation based
 * on old-style broadcast wakeup is plausible (where this structure would
 * contain counters to implement the one-at-a-time wakeup).
 *
 * Since this stuff is used to implement sleep locking, it is justified in not
 * using basic locks but going instead to the really primitive operations.
 */

typedef struct proc_list plist_t;

struct proc_list {
	atomic_uchar_t	pl_locked;	/* control list manipulation */
	pnode_t	      *	pl_head;	/* first waiting process info */
};


#define	PLIST_INIT(l)		((l)->pl_head = NULL, \
				 (void) ATOMIC_CLEAR_UCHAR ((l)->pl_locked))
#define PLIST_DESTROY(l)	((void) 0)
#define PLIST_LOCK(l,n)		(TEST_AND_SET_LOCK ((l)->pl_locked, plhi, \
						    (n)))
#define	PLIST_UNLOCK(l,p)	(ATOMIC_CLEAR_UCHAR ((l)->pl_locked), \
						     (void) splx (p))
#define	PLIST_ASSERT_LOCKED(l)	ASSERT (ATOMIC_FETCH_UCHAR ((l)->pl_locked) \
					!= 0)


/*
 * External function definitions. The curious may like to note that the use
 * of the __PROTO () macro below has a useful side-effect with respect to
 * macro-expansion of MAKE_SLEEPING (). The ISO C preprocessor scanning rules
 * prevent MAKE_SLEEPING () below from being expanded because the next right
 * token is not a left parenthesis. The rescanning rules mean that even through
 * __PROTO () actually expands to something beginning with a left parenthesis,
 * the MAKE_SLEEPING token will get copied to the output without expansion,
 * because it will not be considered again.
 *
 * This feature may be of some use if one has a non-ISO C compiler but can
 * get the source ISO-preprocessed. Many K&R compilers do not understand the
 * ISO macro-suppression idiom of enclosing a macro name within parentheses
 * (this suppresses macro-expansion because the next token will be a right
 * parenthesis) in function declarations due to weaknesses in the grammars
 * used. Use of an identity macro to enclose the parameter list ensures that
 * the preprocessor doesn't perform the expansion but leaves the function
 * name unadorned by unnecessary parentheses. Note that enclosing the function
 * name does not have the suppressing effect!
 */

__EXTERN_C_BEGIN__

__sleep_t	MAKE_SLEEPING	__PROTO ((plist_t * _plistp, int _priority,
					  int _flag));
void		RUN_NEXT	__PROTO ((void));
__VOID__      *	WAKE_ONE	__PROTO ((plist_t * _plistp));
void		WAKE_ALL	__PROTO ((plist_t * _plistp));

__VOID__      *	PROC_HANDLE	__PROTO ((void));

__EXTERN_C_END__


#endif	/* ! defined (__KERNEL_V_PROC_H__) */
