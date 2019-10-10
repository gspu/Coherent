/* $Header: /src386/kernel/coh.386/RCS/misc.c,v 1.8 93/06/14 13:34:48 bin Exp Locker: bin $ */
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
 * Coherent.
 * Miscellaneous routines.
 *
 * $Log:	misc.c,v $
 * Revision 1.8  93/06/14  13:34:48  bin
 * Hal: kernel 78 update
 * 
 * Revision 1.4  93/04/14  10:06:36  root
 * r75
 * 
 * Revision 1.3  92/11/09  17:10:53  root
 * Just before adding vio segs.
 * 
 * Revision 1.2  92/01/06  11:59:45  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:14:01	src
 * Initial revision
 * 
 * 87/05/08	Allan Cornish		/usr/src/sys/coh/misc.c
 * System code and data segments no longer reported in panic messages.
 *
 * 87/02/17	Allan Cornish		/usr/src/sys/coh/misc.c
 * Panic message now includes system code and data segments.
 */
#include <sys/coherent.h>
#include <sys/acct.h>
#include <errno.h>
#include <sys/ino.h>
#include <sys/stat.h>

#ifdef TRACER
extern unsigned t_piggy;
#endif

/*
 * Make sure we are the super user.
 */
super()
{
	if (u.u_uid) {
		u.u_error = EPERM;
		return (0);
	}
	u.u_flag |= ASU;
	return (1);
}

/*
 * Make sure we are the gived `uid' or the super user.
 */
owner(uid)
{
	if (u.u_uid == uid)
		return (1);
	if (u.u_uid == 0) {
		u.u_flag |= ASU;
		return (1);
	}
	u.u_error = EPERM;
	return (0);
}

/*
 * Panic.
 */
void
panic(a1)
char *a1;
{
	static panflag;
	sphi();

#ifdef TRACER
	if ( t_piggy & 0x80 ) {
		if (panflag++ == 0) {
			printf("Panic: %r", &a1);
			putchar('\n');
			usync();
		}
		printf("relax! It really isn't so bad.\n");
	} else {
		if (panflag++ == 0) {
			if (paging()) {
				printf("Panic: %r", &a1);
				putchar('\n');
			} else {
				strchirp("Panic: ");
				strchirp(a1);
			}
			for (;;);
			usync();
		}
		halt();
	}
#else
	if (panflag++ == 0) {
		printf("Panic: %r", &a1);
		putchar('\n');
		for (;;);
		usync();
	}
	halt();
#endif /* TRACER */

	--panflag;
}

/*
 * Print a message from a device driver.
 */
devmsg(dev, a1)
dev_t dev;
char *a1;
{
	printf("(%d,%d): %r", major(dev), minor(dev), &a1);
	printf("\n");
}

/*
 * Wait up to "ticks" clock ticks for an event to occur.
 * A tick is 1/HZ seconds (10 msec).
 * Works whether interrupts are enabled or not.
 * Busy-waits the system.
 * The event occurs when (*fn)() returns a nonzero value.
 * If fn is NULL, delay unconditionally.
 *
 * Return 0 if timeout occurred, 1 if the desired event occurred.
 */

#define THRESH (T0_RATE/2)	/* half of 11932 */

int
busyWait(fn, ticks)
int (*fn)();
int ticks;
{
	/*
	 * p0, p1 are 0 if in low half of counting cycle, else 1.
	 * flips counts the number of changes from low-to-high or vice versa.
	 * tickCt counts the number of clock ticks at rate HZ.
	 */

	int tickCt = 0;
	int flips = 0;
	int p0 = (read_t0() < THRESH)?0:1;
	int p1;

	for (;;) {
		if (fn && (*fn)())
			return 1;

		/* did we change halves of counter cycle? */
		p1 = (read_t0() < THRESH)?0:1;
		if (p0 != p1) {
			p0 = p1;
			flips++;

			/* two phase flips make a tick */
			if (flips >= 2) {
				flips = 0;
				tickCt++;
				if (tickCt > ticks)
					return 0;
			}
		}
		
	}
}

/*
 * busyWait2() has finer granularity than busyWait().
 *
 * Wait up to "counts" clock counts for an event to occur.
 * A count is 1/(11932*HZ) seconds (about 0.84 usec).
 * Works whether interrupts are enabled or not.
 * Busy-waits the system.
 * The event occurs when (*fn)() returns a nonzero value.
 * If fn is NULL, delay unconditionally.
 *
 * Return 0 if timeout occurred, 1 if the desired event occurred.
 */

int
busyWait2(fn, counts)
int (*fn)();
unsigned int counts;
{
	/*
	 * ct0 is previous t0 reading, ct1 is current reading.
	 * We have timer rollover when ct1 < ct0.
	 */

	unsigned int totCt = 0;
	unsigned int ct0 = read_t0();
	unsigned int ct1;

	for (;;) {
		if (fn && (*fn)())
			return 1;

		ct1 = read_t0();
		if (ct1 > ct0) {
			/* no timer 0 rollover */
			totCt += ct1 - ct0;
		} else {
			/* timer 0 rollover */
			totCt += ct1 + T0_RATE - ct0;
		}
		if (totCt > counts)
			return 0;
		ct0 = ct1;
	}
}
