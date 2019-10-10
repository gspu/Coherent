/*
 * coh.386/misc.c
 *
 * Miscellaneous routines.
 *
 * Revised: Thu Jul 15 14:18:13 1993 CDT
 */
#include <sys/coherent.h>
#include <sys/acct.h>
#include <sys/errno.h>
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
		return 0;
	}
	u.u_flag |= ASU;
	return 1;
}

/*
 * Make sure we are the gived `uid' or the super user.
 */

owner(uid)
{
	if (u.u_uid == uid)
		return 1;
	if (u.u_uid == 0) {
		u.u_flag |= ASU;
		return 1;
	}
	u.u_error = EPERM;
	return 0;
}

/*
 * Use printf to generate a call-trace.
 */

void backtrace (dummy)
long		dummy;
{
	long	      *	bp = (& dummy) - 2;

	printf ("Call backtrace: ");
	do {
		bp = (long *) * bp;
		printf (" -> %x", * (bp + 1));
	} while (* bp != NULL);
	printf ("\n");
}

/*
 * Panic.
 */
void
panic(a1)
char *a1;
{
	static panflag;

	sphi ();

#ifdef TRACER
	if (t_piggy & 0x80) {
		if (panflag ++ == 0) {
			printf ("Panic: %r", &a1);
			putchar ('\n');
			usync ();
		}
		printf ("relax! It really isn't so bad.\n");
	} else {
		if (panflag ++ == 0) {
			if (paging ()) {
				printf ("Panic: %r", &a1);
				putchar ('\n');
			} else {
				strchirp ("Panic: ");
				strchirp (a1);
			}
			backtrace (0);
			for (;;)
				/* DO NOTHING */ ;
			usync ();
		}
		halt ();
	}
#else
	if (panflag ++ == 0) {
		printf ("Panic: %r", &a1);
		putchar ('\n');
		for (;;)
			/* DO NOTHING */ ;
		usync ();
	}
	halt ();
#endif /* TRACER */

	-- panflag;
}

/*
 * Print a message from a device driver.
 */
devmsg(dev, a1)
dev_t dev;
char *a1;
{
	printf ("(%d,%d): %r", major(dev), minor(dev), &a1);
	printf ("\n");
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
	int p0 = read_t0 () < THRESH ? 0 : 1;
	int p1;

	for (;;) {
		if (fn && (* fn) ())
			return 1;

		/* did we change halves of counter cycle? */
		p1 = read_t0 () < THRESH ? 0 : 1;
		if (p0 != p1) {
			p0 = p1;
			flips ++;

			/* two phase flips make a tick */
			if (flips >= 2) {
				flips = 0;
				tickCt ++;
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
busyWait2 (fn, counts)
int (*fn)();
unsigned int counts;
{
	/*
	 * ct0 is previous t0 reading, ct1 is current reading.
	 * We have timer rollover when ct1 < ct0.
	 */

	unsigned int totCt = 0;
	unsigned int ct0 = read_t0 ();
	unsigned int ct1;

	for (;;) {
		if (fn && (* fn) ())
			return 1;

		ct1 = read_t0 ();
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
