/* $Header: /x/usr/src/sys/coh/RCS/clock.c,v 1.2 91/06/20 14:12:44 hal Exp $ */
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
 * Clock.
 * The clock comes in two parts.  There is the routine `clock' which
 * gets called every tick at high priority.  It does the minimum it
 * can and returns as soon as possible.  The second routine, `stand',
 * gets called whenever we are about to return from an interrupt to
 * user mode a low priority.  It can look at flags that the clock set
 * and do the things the clock really wanted to do but didn't have time.
 * Stand is truly the kernel of the system.
 *
 * 90/08/13	Hal Snyder		/usr/src/sys/coh/clock.c
 * Add external altclk to allow polled device drivers.
 * (extern'ed in coherent.h)
 * 
 * 87/10/26	Allan cornish		/usr/src/sys/coh/clock.c
 * Timed functions are now invoked with TIM * tp as second argument.
 * This facilitates the use of timed functions within loadable drivers.
 *
 * 87/07/07	Allan Cornish		/usr/src/sys/coh/clock.c
 * Clocks static variable added - incremented by clock, decremented by stand().
 * Lbolt variable added - clock ticks since startup - incremented by stand().
 * Support for multiple timing queues ported from RTX.
 *
 * 87/01/05	Allan Cornish		/usr/src/sys/coh/clock.c
 * stand() now only wakes &stimer if swap timer is active.
 *
 * 86/11/24	Allan Cornish		/usr/src/sys/coh/clock.c
 * Added support for new t_last field in tim struct.
 *
 * 86/11/19	Allan Cornish		/usr/src/sys/coh/clock.c
 * Stand() calls defend() to execute functions deferred from interrupt level.
 */
#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/stat.h>
#include <sys/timeout.h>
#include <sys/uproc.h>
#include <sys/mdata.h>

int (*altclk)();	/* pointer to higher-speed clock function */
int altsel;	/* if nonzero, CS for LOADABLE driver owning altclk() */

static int clocks;

/*
 * This routine is called once every tick (1/HZ seconds).
 * It gets called with the programme counter that was interrupted
 * a flag telling whether we were in user or kernel mode and the
 * previous priority we were in.
 */
clock(pc, umode)
vaddr_t pc;
{
	register PROC *pp;
	/*
	 * Ignore clock interrupts till we are ready.
	 */
	if (batflag == 0)
		return;

	/*
	 * Hook for alternate clock interrupt;
	 * Call polling function ("altclk") if there is one.
	 *
	 * For near function, "altsel" is 0 and "altclk" is offset.
	 * For far function, "altsel" is the CS selector and "altclk"
	 * is the offset.
	 *
	 * Since the polling function ends with a near rather than
	 * far return, far invocation is via ld_call() (ldas.s) which uses
	 * the despatch routine at CS:4 (ld.s) in any loadable driver.
	 */
	if (altclk) {
		if (altsel) {	/* will do far call to altclk fn */
			if (ld_call(altsel, altclk))
				return;
		} else
			if ((*altclk)())
				return;
	}

	/*
	 * Update timers.  Decrement time slice.
	 */
	utimer += 1;
	clocks += 1;
	timer.t_tick += 1;
	quantum -= 1;

	/*
	 * Give processes their schedule values per tick.
	 */
	if (procq.p_lforw->p_cval > CVCLOCK) {
		procq.p_lforw->p_cval -= CVCLOCK;
		procq.p_cval += CVCLOCK;
	}

	/*
	 * Tax current process and update his times.
	 */
	pp = SELF;
	pp->p_cval >>= 1;
	if (umode == 0)
		pp->p_stime++;
	else {
		pp->p_utime++;
		u.u_ppc = pc;
	}
}

/*
 * Do everything the clock wanted to do but couldn't as it would have
 * taken too long.
 * Also perform any system bookkeeping required at regular intervals.
 */
stand()
{
	int s;

	u.u_error = 0;

	/*
	 * Update the clock.
	 */
	while (timer.t_tick >= HZ) {
		timer.t_time++;
		timer.t_tick -= HZ;
		outflag = 1;
	}

	/*
	 * Check expiration of quantum.
	 */
	if (quantum <= 0) {
		quantum = 0;
		disflag = 1;
	}

	/*
	 * Check the timed function queue if necessary.
	 */
	if ( clocks > 0 )
	do {
		register TIM * np;
		register TIM * tp;

		/*
		 * Update [serviced] clock ticks since startup.
		 */
		lbolt++;

		/*
		 * Remove timing list from queue, creating new temporary queue.
		 */
		tp = (TIM *) &timq[ lbolt % nel(timq) ];
		s  = sphi();

		/*
		 * Scan timing list.
		 */
		for ( np = tp->t_next; tp = np; ) {

			/*
			 * Remember next function in timing list.
			 * NOTE: Must be done before function is invoked,
			 *	 since it may start a new timer.
			 */
			np = tp->t_next;

			/*
			 * Function has not timed out: leave it on timing list.
			 */
			if ( tp->t_lbolt != lbolt )
				continue;

			/*
			 * Remove function from timing list.
			 */
			if ( tp->t_last->t_next = tp->t_next )
				tp->t_next->t_last = tp->t_last;
			tp->t_last = NULL;

			/*
			 * Invoke function.
			 */
			spl(s);
			(*tp->t_func)( tp->t_farg, tp );
			sphi();
		}

		spl( s );

	} while ( --clocks > 0 );

	/*
	 * Timeout any devices.
	 */
	if (outflag) {
		register int n;

		outflag = 0;
		for (n=0; n<drvn; n++) {
			if (drvl[n].d_time == 0)
				continue;
			s = sphi();
			dtime((dev_t)makedev(n, 0));
			spl(s);
		}
	}

	/*
	 * Do profiling.
	 */
	if (u.u_pscale != 0) {
		register unsigned p;
		register vaddr_t a;

		p = u.u_pscale;
		a = (int *)u.u_pbase +
		    pscale(u.u_ppc-u.u_pofft, p/sizeof (int));
		if (a < u.u_pbend)
			putuwd(a, getuwd(a)+1);
	}

	/*
	 * Check for signals and execute them.
	 */
	if (SELF->p_ssig)
		actvsig();

	/*
	 * Execute deferred functions.
	 */
	defend();

	/*
	 * Should we dispatch?
	 */
	if ((SELF->p_flags&PFDISP) != 0) {
		SELF->p_flags &= ~PFDISP;
		disflag = 1;
		if ( stimer.t_last != 0 )
			wakeup((char *)&stimer);
	}

#ifdef QWAKEUP
	/*
	 * Dispatch pending wakeups.
	 */
	while (ntowake)
		wakeup2();

#endif
	/*
	 * Redispatch.
	 * This used to be a function call in tsave,
	 * expanded in line here.
	 */
	if (disflag) {
		register PROC *pp;

#ifndef QWAKEUP
		s=sphi();
#endif
		if ((pp=SELF)!=iprocp)
			setrun(pp);
		dispatch();
#ifndef QWAKEUP
		spl(s);
#endif
	}
}
