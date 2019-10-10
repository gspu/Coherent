/* $Header: /kernel/kersrc/ldrv/RCS/dmalock.c,v 1.1 92/07/17 15:27:50 bin Exp Locker: bin $
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
 *	Routines to lock/unlock the DMA controller chip from a loadable driver.
 *
 * $Author: bin $
 *
 * $Creation: June 29, 1989 $
 *
 * $Log:	dmalock.c,v $
 * Revision 1.1  92/07/17  15:27:50  bin
 * Initial revision
 * 
 * Revision 1.1	89/06/30  16:29:52 	src
 * Initial revision
 * 
 */

#include <sys/coherent.h>

typedef void (* vfp_t)();		/* Void function pointer type.	     */

/*
 * External functions.
 */
extern void Kdmalock();
extern void Kdmaunlock();
extern void Kldtimcall();
extern saddr_t getcs();

/*
 * int
 * dmalock( tp, fun, arg )
 * TIM * tp;
 * vfp_t fun;
 * int	 arg;
 *
 *	Inputs:	tp  = Deferred function structure pointer.
 *		fun = Function to call if request is deferred.
 *		arg = Argument to pass to function.
 *
 *	Action:	Calls kernel dmalock() routine.
 *
 *	Return:	0 = Lock granted or -1 = Lock deferred.
 *
 *	Notes:	DMA controller locking was introduced to cure a bug on the
 *		NCR DMA controller, where overlapped DMA caused problems.
 *		No action is taken if DMA locking is disabled.
 */

int
dmalock( tp, fun, arg )
register TIM  * tp;
vfp_t		fun;
int		arg;
{
	/*
	 * Define loadable driver interface.
	 * Kldtimcall will be invoked when a deferred function is executed.
	 * It will in turn invoke FP_SEL(tp->t_ldrv):4 (the calling drivers
	 * function call entry point), passing FP_OFF(tp->t_ldrv) (the function
	 * to call) in AX.
	 */
	FP_SEL(tp->t_ldrv) = getcs();
	FP_OFF(tp->t_ldrv) = fun;
	return( kcall( Kdmalock, tp, Kldtimcall, arg ) );
}

/*
 * void
 * dmaunlock( tp )
 * TIM * tp;
 *
 *	Inputs:	tp = Deferred function structure pointer.
 *
 *	Action:	Calls the kernel dmaunlock().
 */

void
dmaunlock( tp )
register TIM * tp;
{
	kcall( Kdmaunlock, tp );
}

