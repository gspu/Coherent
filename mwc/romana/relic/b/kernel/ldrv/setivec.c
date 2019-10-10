/* $Header: /kernel/kersrc/ldrv/RCS/setivec.c,v 1.1 92/07/17 15:27:57 bin Exp Locker: bin $ */
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
 * Loadable Driver - Enable/Disable Interrupts.
 *
 * $Log:	setivec.c,v $
 * Revision 1.1  92/07/17  15:27:57  bin
 * Initial revision
 * 
 * Revision 1.1	88/03/24  16:31:02	src
 * Initial revision
 * 
 * 87/12/03	Allan Cornish		/usr/src/sys/ldrv/setivec.c
 * Initial version.
 */
#include <sys/coherent.h>
#include <errno.h>
#include <sys/con.h>
#include <sys/uproc.h>

/*
 * Interrupt entry points [within kernel] for loadable drivers.
 */
extern void (*ldrvint[16])();

/*
 * Interrupt handlers [within driver] for loadable drivers.
 *	ldrvics[n]:	Interrupt handler code segment.
 *	ldrvipc[n]:	Interrupt handler program counter.
 */
extern saddr_t ldrvics[16];
extern void  (*ldrvipc[16])();

setivec( level, func )
int level;
void (*func)();
{
	extern void Ksetivec();
	extern saddr_t ucs;

	u.u_error = 0;
	level &= 15;

	/*
	 * Ensure interrupt is not already in use.
	 */
	if ( (ldrvics[level] != 0) || (ldrvipc[level] != NULL) ) {
		u.u_error = EDBUSY;
		return;
	}

	/*
	 * Record interrupt function BEFORE enabling interrupt.
	 */
	ldrvipc[level] = func;
	ldrvics[level] = ucs;

	/*
	 * Attempt to enable interrupt.
	 */
	kcall( Ksetivec, level, ldrvint[level] );

	/*
	 * Interrupt is in use by a resident driver.
	 */
	if ( u.u_error ) {
		ldrvipc[level] = NULL;
		ldrvics[level] = 0;
	}
}

clrivec( level )
register int level;
{
	extern void Kclrivec();
	extern saddr_t ucs;

	level &= 15;

	/*
	 * Ensure interrupt belongs to our process.
	 */
	if ( ldrvics[level] != ucs ) {
		u.u_error = EPERM;
		return;
	}

	/*
	 * Disable interrupt.
	 */
	kcall( Kclrivec, level );

	/*
	 * Erase interrupt function AFTER disabling interrupt.
	 */
	ldrvipc[level] = NULL;
	ldrvics[level] = 0;
}
