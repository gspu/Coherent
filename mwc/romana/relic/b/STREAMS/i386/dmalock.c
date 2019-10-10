/* $Header: /src386/STREAMS/i386/RCS/dmalock.c,v 2.3 93/08/09 13:39:02 bin Exp Locker: bin $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, Ltd, and is  confidential information.   It is provided
 *	under a license agreement,  and may be copied or disclosed  only
 *	under  the  terms  of  that  agreement.    Any  reproduction  or
 *	disclosure  of  this   material   without  the  express  written
 *	authorization of INETCO Systems, Ltd. or persuant to the license
 *	agreement is unlawful.
 *
 *	Copyright (c) 1989
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 *
 * $Description: $
 *	Routines to lock/unlock the DMA controller chip.
 *
 * $Author: bin $
 *
 * $Creation: June 21, 1989 $
 *
 * $Log:	dmalock.c,v $
 * Revision 2.3  93/08/09  13:39:02  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  13:55:59  nigel
 * Nigel's R80
 * 
 * Revision 1.1  93/04/14  10:26:34  root
 * r75
 * 
 * Revision 1.1	89/06/30  16:21:26 	src
 * Initial revision
 * 
 */

#include <kernel/timeout.h>

typedef void (* vfp_t)();		/* Void function pointer type.	     */

/*
 * If the following variable is non-zero, DMA controller locking is enabled,
 * allowing at access to only one DMA channel at a time.
 */
int DMALCK = 1;

static TIM * dmatail = (TIM *)0;	/* DMA deferred function queue tail. */
static TIM * dmahead = (TIM *)0;	/* DMA deferred function queue head. */

/*
 * int
 * dmalock( dfp, fun, arg )
 * TIM * dfp;
 * vfp_t fun;
 * int	 arg;
 *
 *	Inputs:	dfp  = Deferred function structure pointer.
 *		fun  = Function to call if request is deferred.
 *		arg  = Argument to pass to function.
 *
 *	Action:	Either locks DMA controller immediately or defers function
 *		call until lock can be granted.
 *
 *	Return:	0 = Lock granted or -1 = Lock deferred.
 *
 *	Notes:	DMA controller locking was introduced to cure a bug on the
 *		NCR DMA controller, where overlapped DMA caused problems.
 *		No action is taken if DMA locking is disabled.
 */

int
dmalock( dfp, fun, arg )
register TIM  * dfp;
vfp_t		fun;
int		arg;
{
	register int s;		/* Interrupt mask state. */

	/*
	 * If DMA locking is disabled, allow functions to proceed.
	 */
 	if ( DMALCK == 0 )
		return( 0 );

	/*
	 * Record function and argument to be invoked upon dmaunlock.
	 */
	dfp->t_func = fun;
	dfp->t_farg = arg;
	dfp->t_next = (TIM *)0;

	s = sphi();

	/*
	 * If the queue is empty, put our structure at the head.
	 */
	if ( dmahead == (TIM *)0 ) {
		dmahead = dfp;
		dmatail = dfp;
		spl( s );
		return( 0 );
	}

	/*
	 * PARANOIA:	If our structure is already at the head of the queue,
	 *		print a message and return.
	 */
	if ( dmahead == dfp ) {
		spl( s );
		printf( "dmalock: driver attempting to doubly lock DMA controller.\n" );
		return( 0 );
	}

	/*
	 * Append to tail of DMA deferred function queue.
	 */
	dmatail->t_next = dfp;
	dmatail		= dfp;
	spl( s );
	return( -1 );
}

/*
 * void
 * dmaunlock( dfp )
 * TIM * dfp;
 *
 *	Inputs:	dfp = Deferred function structure pointer.
 *
 *	Action:	Unlocks the DMA controller and calls the next deferred
 *		function, if any.
 *
 *	Notes:	No action is taken if the deferred function structure pointer
 *		is not the same as the one used to lock the DMA controller.
 */

void
dmaunlock( dfp )
register TIM * dfp;
{
	register TIM *	qp;	/* Temporary function queue pointer.	*/
	register int	s;	/* Interrupt mask state.		*/

	s = sphi();

	/*
	 * If the DMA controller is not locked, return.
	 */
	if ( dmahead == (TIM *)0 ) {
		spl( s );
		return;
	}

 	/*
	 * If our lock is not the one holding the DMA controller:
	 */
	if ( dmahead != dfp ) {

		/*
		 * Look for us in queue.
		 */
		for ( qp = dmahead; qp != dmatail; qp = qp->t_next )

			/*
			 * If found, remove us.
			 */
			if ( qp->t_next == dfp ) {
				qp->t_next = dfp->t_next;

				if ( dmatail == dfp )
					dmatail = qp;

				break;
			}

		spl( s );
		return;
	}

	/*
	 * If there are no functions waiting for us, empty queue and return.
	 */
	if ( dmahead == dmatail ) {
		dmahead = (TIM *)0;
		spl( s );
		return;
	}

	/*
	 * Remove us and execute next deferred function.
	 */
	dmahead = dmahead->t_next;
	spl( s );
	(*dmahead->t_func)( dmahead->t_farg, dmahead );
}

