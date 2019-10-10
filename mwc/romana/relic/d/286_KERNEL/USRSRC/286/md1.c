/* $Header: /usr/src/sys/i8086/src/RCS/md1.c,v 1.2 88/08/05 15:34:26 src Exp $ */
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
 * All machines.
 * Machine dependent stuff.
 *
 * $Log:	/usr/src/sys/i8086/src/RCS/md1.c,v $
 * Revision 1.2	88/08/05  15:34:26	src
 * mproto(), segload(), and msetsys() functions simplified.
 * 
 * Revision 1.1	88/03/24  17:39:43	src
 * Initial revision
 * 
 * 88/03/10	Allan Cornish		/usr/src/sys/coh/proc.c
 * Numerous temporary fixes due to AMD 286 chip being buggy in protected mode.
 * These partial fixes will be removed once all CPU's are replaced.
 *
 * 88/02/13	Allan Cornish		/usr/src/sys/i8086/src/md1.c
 * segload() now checks for regl[OIP] being system code segment.
 * iAPX-286 protected mode interrupt gates enable interrupts momentarily.
 *
 * 88/01/21	Allan Cornish		/usr/src/sys/i8086/src/md1.c
 * Segments are now de-associated from processes before freeing the segment.
 *
 * 87/11/05	Allan Cornish		/usr/src/sys/i8086/src/md1.c
 * New seg struct now used to allow extended addressing.
 *
 * 87/09/21	Allan Cornish	/usr/src/sys/i8086/src/md1.c
 * mproto() and setload() modified to support loadable driver processes.
 */
#include <sys/coherent.h>
#include <sys/i8086.h>
#include <sys/clist.h>
#include <errno.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/uproc.h>

/*
 * Calculate segmentation for a
 * new program. If there is a stack segment
 * present merge it into the data segment and
 * relocate the argument list.
 * Make sure that the changes are reflected in the u.u_segl array
 * which sproto sets up.
 */
mproto()
{
	register PROC	*pp;
	register SEG	*dsp;
	register SEG	*ssp;
	register SEG	*csp;
	fsize_t		ds;
	fsize_t		ss;
	unsigned int	so;
	unsigned int	*up;
	unsigned int	v;

	pp = SELF;

	dsp = pp->p_segp[SIPDATA];

	if ( ((pp->p_flags & PFKERN) == 0)
	  && ((ssp=pp->p_segp[SISTACK]) != NULL) ) {
		ds = dsp->s_size;
		ss = ssp->s_size;
		so = ds + ss;
		if (seggrow(dsp, (fsize_t)so) == 0)
			return (0);
		plrcopy( ssp->s_paddr, dsp->s_paddr + ds, ss);
		pp->p_segp[SISTACK] = NULL;
		sfree(ssp);
		u.u_sproto.mp_svb = ds;
		u.u_sproto.mp_svl = so;
		if (u.u_argp != NULL) {
			abase(FP_SEL(dsp->s_faddr));
			up = u.u_argp += so;
			--up;
			while (ageti(--up) != NULL)
				;
			up -= 2+u.u_argc;
			while ((v=ageti(up)) != NULL)
				aputi(up++, v+so);
			while ((v=ageti(++up)) != NULL)
				aputi(up, v+so);
		}
		return (sproto());	/* Recurse to fix u.u_segl */
	}

	/*
	 * Shared code, private code, or kernel code [csp == NULL].
	 * NOTE: Combined code/data no longer supported.
	 */
	if ( (csp = pp->p_segp[SISTEXT]) == NULL )
		csp = pp->p_segp[SIPTEXT];

	/*
	 * Special case if no code/data segment specified.
	 */
	u.u_sproto.mp_cbp = (csp != NULL) ? &FP_SEL(csp->s_faddr) : &scs;
	u.u_sproto.mp_dbp = (dsp != NULL) ? &FP_SEL(dsp->s_faddr) : &sds;
	u.u_sproto.mp_csl = (csp != NULL) ? csp->s_size - 1 : 0;
	u.u_sproto.mp_dsl = (dsp != NULL) ? dsp->s_size - 1 : 0;

	return (1);
}

/*
 * Load up segmentation registers.
 */
segload()
{
	register unsigned *ip;
	register unsigned s;

	ucs = *u.u_sproto.mp_cbp;
	uds = *u.u_sproto.mp_dbp;
	ucl =  u.u_sproto.mp_csl;
	udl =  u.u_sproto.mp_dsl;

	ip = regl;
	ip[OCS] = ucs;
	ip[ODS] = s = uds;
	ip[OES] = s;
	ip[OSS] = s;
}

/*
 * Set up a new process.
 */
msetusr(ip, sp)
vaddr_t sp;
vaddr_t ip;
{
	regl[OIP] = ip;
	regl[OSP] = sp + u.u_sproto.mp_svl;
}

/*
 * Set up initial context for a system process.
 * System processes run at depth 1, just like any
 * user process. This is necessary to make sure that
 * the machine state save is correctly handled, just
 * in case the clock hits a kernel process executing
 * out of the IBM ROM.
 */
msetsys(mp, fn, us)
register MCON *mp;
int (*fn)();
saddr_t us;
{
	mp->mc_sp = (int)(&u) + UPASIZE - 32;
	mp->mc_pc = fn;
	mp->mc_fw = 0;
	mp->mc_depth = 1;
}

/*
 * Set the given address in the user area to the given value if it is
 * okay to do so.
 */
msetuof(a, v)
register int a;
{
	if (a<UPASIZE+OBP*2 || a>UPASIZE+OFW*2)
		return (0);
	if (a == UPASIZE+OCS*2)
		return (0);
	if (a == UPASIZE+ODS*2)
		return (0);
	if (a == UPASIZE+OES*2)
		return (0);
	if (a == UPASIZE+OSS*2)
		return (0);
	if (a == UPASIZE+OID*2)		/* Protect trap id */
		return (0);
	if (a == UPASIZE+ORA*2)
		return (0);		/* Protect trap return link */
	*((int *)((int)&u+a)) = v;
	return (1);
}

/*
 * Cause a signal routine to be executed.
 */
msigint(n, f)
{
	register int *usp;

	usp = regl[OSP];
	putuwd(--usp, regl[OFW]);
	putuwd(--usp, regl[OIP]);
	putuwd(--usp, n);
	regl[OFW] &= ~MFTTB;
	regl[OIP] = f;
	regl[OSP] = usp;
	if (n != SIGTRAP)
		u.u_sfunc[n-1] = SIG_DFL;
}

/*
 * Cause the next instruction to single step.
 */
msigsin()
{
	regl[OFW] |= MFTTB;
}

/*
 * Fix up context.
 */
mfixcon(pp)
PROC *pp;
{
}

/*
 * Idle kernel process.
 */
idle()
{
	for (;;) {
		disflag = 1;
		_idle();
	}
}
