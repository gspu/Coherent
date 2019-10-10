/* $Header: /y/coh.386/RCS/clock.c,v 1.6 92/07/16 16:33:29 hal Exp $ */
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
#include <sys/mdata.h>

int (*altclk)();	/* pointer to higher-speed clock function */

#ifndef _I386
int altsel;	/* if nonzero, CS for LOADABLE driver owning altclk() */
#endif

static int clocks;

/*
 * This routine is called once every tick (1/HZ seconds).
 * It gets called with the program counter that was interrupted
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
#ifndef _I386
		if (altsel) {	/* will do far call to altclk fn */
			if (ld_call(altsel, altclk))
				return;
		} else
#endif
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
	if (umode) {
		pp->p_utime++;
		u.u_ppc = pc;
	} else
		pp->p_stime++;
}

/*
 * stand()
 *
 * Called when there is an interrupt or trap.
 */

stand()
{
	int s;

#ifdef TIMING
	/*
	 * Every 500th call to stand(), dump logged intervals.
	 *
	 * Log a group of events doing
	 * 	LOGIN(label);
	 *	...
	 *	LOGOUT();
	 *
	 * If you want to group several selected events in one total,
	 * do LOGPAUSE() to halt between events and LOGRESUME() to
	 * add in the time for another event.  Event count is displayed.
	 * For example:
	 * 	LOGIN(label);
	 * 	while (...) {
	 *		LOGPAUSE();
	 *		... stuff you don't want timed...
	 *		LOGRESUME();
	 *		... stuff you want timed...
	 *	}
	 *	LOGOUT();
	 *
	 * Display is for the 3 longest intervals seen in latest logging
	 * period.
	 *
	 * Timing between in & out is displayed with ~1 usec resolution.
	 * Overhead per event is ~20 usec.
	 */
	static int call_ct; /* DEBUG */

	call_ct++;
	if (call_ct >= 500) {
		call_ct = 0;
		LOGLIST();
	}
#endif


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
	if (clocks > 0)
	do {
		register TIM * np;
		register TIM * tp;

		/*
		 * Update [serviced] clock ticks since startup.
		 */
		lbolt++;
		clocks--;

		/*
		 * Remove timing list from queue, creating new temporary queue.
		 */
		tp = (TIM *) &timq[ lbolt % nel(timq) ];
		s  = sphi();

		/*
		 * Scan timing list.
		 */
		for (np = tp->t_next; tp = np;) {

			/*
			 * Remember next function in timing list.
			 * NOTE: Must be done before function is invoked,
			 *	 since it may start a new timer.
			 */
			np = tp->t_next;

			/*
			 * Function has not timed out: leave it on timing list.
			 */
			if (tp->t_lbolt != lbolt)
				continue;

			/*
			 * Remove function from timing list.
			 */
			if (tp->t_last->t_next = tp->t_next)
				tp->t_next->t_last = tp->t_last;
			tp->t_last = NULL;

			/*
			 * Invoke function.
			 */
			spl(s);
			(*tp->t_func)(tp->t_farg, tp);
			sphi();
		}

		spl(s);

	} while (clocks);

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
#ifdef _I386	/* profiling */
	if (u.u_pscale & ~1) {	/* if scale is not zero or one */
		/*
		 * Treat u.u_pscale as fixed-point fraction 0xXXXX.YYYY.
		 * Increment the (short) profiling entry at
		 *	base + (pc - offset) * scale
		 */
		register vaddr_t a;

		a = (u.u_pbase + pscale(u.u_ppc-u.u_pofft, u.u_pscale)) & ~1;
		if (a < u.u_pbend)
			putusd(a, getusd(a)+1);
	}
#else		/* profiling */
	if (u.u_pscale) {
		register unsigned p;
		register vaddr_t a;

		p = u.u_pscale;
		a = (int *)u.u_pbase +
		    pscale((int)(u.u_ppc-u.u_pofft), p/sizeof (int));
		if (a < u.u_pbend)
			putuwd(a, getuwd(a)+1);
	}
#endif		/* profiling */

	/*
	 * Check for signals and execute them.
	 */
	if (SELF->p_ssig) {
		actvsig();
	}

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
		if (stimer.t_last != 0)
			wakeup((char *)&stimer);
	}

	/*
	 * Redispatch.
	 */
	if (disflag) {
		register PROC *pp;

		s=sphi();
		if ((pp=SELF)!=iprocp)
			setrun(pp);
		dispatch();
		spl(s);
	}
stand_done:
	return;
}

#ifdef TIMING

#define EIMAX 3

static int label, b_in, t_in;
static int b_pause, t_pause, e_pause, paused, timing;

struct H_event {
	int f; /* timing label */
	int b; /* delta ticks */
	int t; /* delta timer */
	int e; /* event count */
} Ltab[EIMAX];

LOGIN(lab)
{
	paused = 0;
	b_pause = t_pause = 0;
	e_pause = 1;
	label = lab;
	b_in = clocks + lbolt;
	t_in = read_t0();
	timing = 1;
}

LOGOUT()
{
	int i;

	if (!timing)
		return;
	if (!paused) {
		b_pause += clocks + lbolt - b_in;
		t_pause += (t_in - read_t0());
		if (t_pause < 0)
			t_pause += 11932;
	}
	for (i = 0; i < EIMAX; i++) {
		if (b_pause > Ltab[i].b) {
			Ltab[i].b = b_pause;
			Ltab[i].t = t_pause;
			Ltab[i].f = label;
			Ltab[i].e = e_pause;
			break;
		} else if (b_pause == Ltab[i].b) {
			if (t_pause > Ltab[i].t) {
				Ltab[i].t = t_pause;
				Ltab[i].f = label;
				Ltab[i].e = e_pause;
				break;
			} else if (t_pause == Ltab[i].t) {
				break;
			}
		}
	}
	timing = 0;
}

LOGLIST()
{
	int i, printed = 0;

	for (i = 0; i < EIMAX; i++)
		if (Ltab[i].t || Ltab[i].b) {
			if (printed == 0) {
				printf("\npsw=%x ", read_psw());
				printed = 1;
			}
			printf("f=%x b=%d t=%d e=%d  ",
			  Ltab[i].f, Ltab[i].b, Ltab[i].t, Ltab[i].e);
			Ltab[i].f = Ltab[i].b = Ltab[i].t = 0;
		} else
			break;
}

LOGPAUSE()
{
	if (!timing)
		return;
	if (!paused) {
		b_pause += clocks + lbolt - b_in;
		t_pause += (t_in - read_t0());
		if (t_pause < 0)
			t_pause += 11932;
		paused = 1;
	}
}

LOGRESUME()
{
	if (!timing)
		return;
	if (paused) {
		b_in = clocks + lbolt;
		t_in = read_t0();
		paused = 0;
		e_pause++;
	}
}
#endif
