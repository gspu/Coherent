#ifndef	__LOOP_H__
#define	__LOOP_H__

/*
 * Definitions for the loop-around driver.
 */

/*
 * I_STR ioctl () command to connect a pair of streams.
 */

#define	LOOP_SET	(('l' << 8) | 1)


#if	_DDI_DKI

/*
 * Require definitions for queue_t and lock_t.
 */

#include <sys/ksynch.h>
#include <sys/stream.h>


/*
 * Definitions common to the "Driver.o" and "Space.c" file, for kernel
 * compiles only.
 */

struct loop {
	queue_t	      *	qptr;		/* back pointer to write queue */
	queue_t	      *	oqptr;		/* pointer to connected read queue */
	lock_t	      *	lck;
};

extern	struct loop	loop_loop [];
extern	int		loop_cnt;

#endif	/* _DDI_DKI */


#endif	/* ! defined (__LOOP_H__) */
