/* (lgl-
 *	md.c
 *
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
 * Coherent 386
 *	IBM PC
 * Machine dependent stuff.
 *
 */

#include <kernel/reg.h>

#include <sys/coherent.h>
#include <sys/clist.h>
#include <sys/errno.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/uproc.h>
#include <sys/buf.h>

/*
 * Given an irq level (1..15) and an irq function pointer, try to hook the
 * function into the desired interrupt.  Do not allow resetting of the
 * clock interrupt, irq 0.
 *
 * On success, return 1.
 * If the level number is invalid or the interrupt is already in use,
 * do nothing but return 0.
 *
 * Make an entry in the "vecs" table, for use by the assist.
 * Make sure that the channel on the 8259 is armed.
 * Interrupt vectors 2 and 9 are mapped into channel 9.
 */
int
setivec(level, fun)
unsigned int	level;
int		(*fun)();
{
	register int	picm;
	extern	 int	(*vecs[])();
	extern	 int	vret();

	if (level >= NUM_IRQ_LEVELS)
		return 0;
	if (level == 2)
		level = 9;
	if (level==0 || vecs[level]!=&vret)
		return 0;

	vecs[level] = fun;

	/*
	 * NIGEL: The original code here (and matching code below in clrivec ())
	 * takes pains to correctly manipulate the slave PIC chain mask bit in
	 * the master PIC. This is redundant, and unnecessarily complex, so I
	 * have removed it to aid the process of adding a more rational system
	 * of interrupt handling for the DDI/DKI.
	 *
	 * Now the slave PIC chain mask bit is enabled (via a modification to
	 * code in "i386/as.s") at startup, and it should never be disabled.
	 *
	 * The rational interrupt scheme requires that the implementations of
	 * the DDI/DKI functions know the base-level mask value so that they
	 * can correctly (and quickly) manipulate masks to raise and lower
	 * priority levels even when deeply nested inside interrupts (see the
	 * implementations of those functions for a deeper discussion of the
	 * issues involved). This function cooperates with the new system via
	 * the DDI_BASE_..._MASK () macro, which either manipulates the mask
	 * register as the old code did or passes responsibility over to the
	 * new DDI/DKI scheme if it has been enabled.
	 *
	 * The new macro-calls are defined in <kernel/reg.h>
	 */

	if ( level >= LOWEST_SLAVE_IRQ ) {
		picm = inb(SPICM);
		picm &= ~(0x01 << (level-LOWEST_SLAVE_IRQ));
		DDI_BASE_SLAVE_MASK (picm);
	} else {
		picm = inb(PICM);
		picm &= ~(0x01 << level);
		DDI_BASE_MASTER_MASK (picm);
	}
	return 1;
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
		printf("clrivec: level=%d", level);
	vecs[level] = &vret;

	/*
	 * NIGEL: This code has been modified to match the changes made to the
	 * setivec () routine above. See the comment there for details.
	 */

	if (level >= LOWEST_SLAVE_IRQ) {
		picm = inb(SPICM);
		picm |= (0x01 << (level-LOWEST_SLAVE_IRQ));
		DDI_BASE_SLAVE_MASK (picm);
	} else {
		picm = inb(PICM);
		picm |= (0x01 << level);
		DDI_BASE_SLAVE_MASK (picm);
	}
}


/*
 * Convert an array of filesystem 3 byte
 * numbers to longs. This routine, unlike the old one,
 * is independent of the order of bytes in a long.
 * Bytes have 8 bits, though.
 */
l3tol(lp, cp, nl)
register long *lp;
register unsigned char *cp;
register unsigned nl;
{
	register long l;

	if (nl != 0) {
		do {
			l  = (long)cp[0] << 16;
			l |= (long)cp[1];
			l |= (long)cp[2] << 8;
			cp += 3;
			*lp++ = l;
		} while (--nl);
	}
}
/*
 * Convert an array of longs into an array
 * of filesystem 3 byte numbers. This routine, unlike
 * the old one, is independent of the order of bytes in
 * a long. Bytes have 8 bits.
 */
ltol3(cp, lp, nl)
register char *cp;
register long *lp;
register unsigned nl;
{
	register long l;

	if (nl != 0) {
		do {
			l = *lp++;
			cp[0] = l >> 16;
			cp[1] = l;
			cp[2] = l >> 8;
			cp += 3;
		} while (--nl);
	}
}


/*
 * Given a port number and a bit value, write the bit value into
 * the tss iomap.
 *
 * Bit value of 0 enables user I/O for that port.
 * Bit value of 1 disables user I/O for that port.
 *
 * Return 1 if port number is valid for the bitmap, else 0.
 */
int
kiopriv(port, bit)
unsigned int port, bit;
{
	extern int tssIoMap;
	extern int tssIoEnd;
	int ret = 0;
	int * ip;
	unsigned int offset = port >> 5;
	int shift = port & 0x1f;
	int mask = 1 << shift;
	int val = (bit & 1) << shift;

	if (offset >= 0 && offset < (&tssIoEnd - &tssIoMap)) {
		ip = (& tssIoMap) + offset;
		*ip &= ~mask;		/* clear old bit value */
		*ip |= val;		/* or in desired new bit value */
		ret = 1;
	}
	return ret;
}

/*
 * Given a 32 bit mask and a word offset into the tss io map,
 * bitwise or the mask into the map.
 * Offset of 0 covers ports 0..31, offset of 1 covers ports 32..63, etc.
 * Current valid range for offset is 0..63, covering ports 0..7ff.
 *
 * Return the new map word.
 */
int
iomapOr(val, offset)
int val, offset;
{
	extern int tssIoMap;
	extern int tssIoEnd;
	int ret;
	int * ip;

	if (offset >= 0 && offset < (&tssIoEnd - &tssIoMap)) {
		ip = (& tssIoMap) + offset;
		ret = *ip |= val;
	}
	return ret;
}

/*
 * Given a 32 bit mask and a word offset into the tss io map,
 * bitwise and the mask into the map.
 * Offset of 0 covers ports 0..31, offset of 1 covers ports 32..63, etc.
 * Current valid range for offset is 0..63, covering ports 0..7ff.
 *
 * Return the new map word.
 */
int
iomapAnd(val, offset)
int val, offset;
{
	extern int tssIoMap;
	extern int tssIoEnd;
	int ret;
	int * ip;

	if (offset >= 0 && offset < (&tssIoEnd - &tssIoMap)) {
		ip = (& tssIoMap) + offset;
		ret = *ip &= val;
	}
	return ret;
}
