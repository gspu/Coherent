/* $Header: /usr/src/sys/ldrv/RCS/timeout.c,v 1.1 88/03/24 16:31:21 src Exp $ */
/*
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1987
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

/*
 * Loadable Driver - Timed functions.
 *
 * $Log:	/usr/src/sys/ldrv/RCS/timeout.c,v $
 * Revision 1.1	88/03/24  16:31:21	src
 * Initial revision
 * 
 * 87/12/08	Allan Cornish	/usr/src/sys/ldrv/timeout.c
 * Timed loadable driver functions now supported.
 */
#include <sys/coherent.h>

/*
 * External functions.
 */
extern void Ktimeout();
extern void Kldtimcall();
extern saddr_t getcs();

/*
 * Given a pointer to a timeout structure, `tp', call the function `f'
 * with integer argument `a' in `n' ticks of the clock. The list is
 * searched to see if the specified timeout structure is already in a
 * list, and it is removed if already there.
 * This module is specific to loadable drivers.
 */
timeout( tp, n, f, a )
register TIM * tp;
int n;
void (*f)();
int a;
{
	register int s;

	/*
	 * Cancel existing timer.
	 */
	if ( (f == NULL) || (n <= 0) ) {
		kcall( Ktimeout, tp, 0, NULL, 0 );
		return;
	}

	/*
	 * Define loadable driver interface.
	 * Kldtimcall will be invoked when timeout occurs.
	 * It will in turn invoke FP_SEL(tp->t_ldrv):4,
	 *	passing FP_OFF(tp->t_ldrv) in AX.
	 */
	s = sphi();
	FP_SEL(tp->t_ldrv) = getcs();
	FP_OFF(tp->t_ldrv) = f;
	kcall( Ktimeout, tp, n, Kldtimcall, a );
	spl(s);
}
