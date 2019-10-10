/* $Header: /kernel/kersrc/i286/RCS/md2.c,v 1.1 92/07/17 15:21:32 bin Exp Locker: bin $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * 8086/8088 Coherent.
 * IBM PC.
 *
 * $Log:	md2.c,v $
 * Revision 1.1  92/07/17  15:21:32  bin
 * Initial revision
 * 
 * Revision 1.1	88/03/24  17:33:38	src
 * Initial revision
 * 
 * 87/10/26	Allan Cornish		/usr/src/sys/i8086/ibm_at/md2.c
 * Clrivec() now properly resets the interrupt vector for intr 2 in slot 9.
 */
#include <sys/coherent.h>
#include <sys/i8086.h>
#include <sys/clist.h>
#include <errno.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <signal.h>

int nirqslave;
/*
 * Set an interrupt vector.
 * Make an entry in the "vecs" table, for
 * use by the assist. Make sure that the channel
 * on the 8259 is armed.
 * Note that interrupt vectors 2 and 9 are mapped into channel 9.
 */
setivec(level, fun)
register int	level;
int		(*fun)();
{
	register int	picm;
	extern	 int	(*vecs[])();
	extern	 int	vret();

	if ((level &= 0x0F) == 2)
		level = 9;
	if (level==0 || vecs[level]!=&vret) {
		u.u_error = EDBUSY;
		return;
	}
	vecs[level] = fun;
	if ( level >= 8 ) {
		++nirqslave;
		picm = inb(SPICM);
		picm &= ~(0x01 << (level-8));
		outb(SPICM, picm);
		level = 2;
	}
	picm = inb(PICM);
	picm &= ~(0x01 << level);
	outb(PICM, picm);
}

/*
 * Clear an interrupt vector.
 */
clrivec(level)
register int	level;
{
	register int	picm;
	extern	 int	(*vecs[])();
	extern	 int	vret();

	if ((level &= 0x0F) == 2)
		level = 9;
	if (level == 0)
		panic("clrivec: level=%d", level);
	vecs[level] = &vret;
	if (level >= 8) {
		--nirqslave;
		picm = inb(SPICM);
		picm |= (0x01 << (level-8));
		outb(SPICM, picm);
		level = 2;
	}
	if ((level != 2) || (nirqslave == 0)) {
		picm = inb(PICM);
		picm |= (0x01 << level);
		outb(PICM, picm);
	}
}
