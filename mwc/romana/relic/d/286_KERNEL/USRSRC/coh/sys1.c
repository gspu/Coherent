/* $Header: /newbits/286_KERNEL/USRSRC/coh/RCS/sys1.c,v 1.2 92/01/13 09:00:16 bin Exp $ */
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
 * General system calls.
 *
 * $Log:	sys1.c,v $
 * Revision 1.2  92/01/13  09:00:16  bin
 * update by hal to properly support process id groups
 * 
 * Revision 1.2  92/01/13  08:42:37  hal
 * setpgrp() - detach controlling terminal if process not group leader
 * 
 * Revision 1.1	88/03/24  16:14:27	src
 * Initial revision
 * 
 * 87/11/05	Allan Cornish		/usr/src/sys/coh/sys1.c
 * New seg struct now used to allow extended addressing.
 *
 * 87/10/21	Allan Cornish		/usr/src/sys/coh/sys1.c
 * ukill() no longer signals kernel processes if pid is -1.
 * usload() changed to new loadable driver format.
 *
 * 87/08/14	Allan Cornish		/usr/src/sys/coh/sys1.c
 * utick() system call added. Returns elapsed clock ticks since system startup.
 *
 * 87/07/23	Allan Cornish		/usr/src/sys/coh/sys1.c
 * ualarm2() now takes the delay interval as a long instead of an unsigned.
 *
 * 87/07/08	Allan Cornish		/usr/src/sys/coh/sys1.c
 * ualarm() modified to use timed functions to send alarm signal.
 * ualarm2() added to allow alarm times in clock ticks rather than seconds.
 *
 * 85/07/25	Allan Cornish
 * ukill() modified to allow a signal of 0 to check process existence.
 *
 * 85/07/9	Allan Cornish
 * ukill() modified to allow signals to be sent to other process groups.
 * usetpgrp() modified to be System V compatible (group set to pid).
 * ugetpgrp() system call added.
 */
#include <sys/coherent.h>
#include <acct.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <sys/uproc.h>

/*
 * Send alarm signal to specified process - function timed by ualarm()
 */
static
sigalrm( pp )
register PROC * pp;
{
	sendsig( SIGALRM, pp );
}

/*
 * Send a SIGALARM signal in `n' seconds.
 */
ualarm(n)
unsigned n;
{
	register PROC * pp = SELF;
	register unsigned s;

	/*
	 * Calculate time left before current alarm timeout.
	 */
	s = 0;
	if ( pp->p_alrmtim.t_last != NULL )
		s = (pp->p_alrmtim.t_lbolt - lbolt + HZ - 1) / HZ;

	/*
	 * Cancel previous alarm [if any], start new alarm [if n != 0].
	 */
	timeout2( &pp->p_alrmtim, (long) n * HZ, sigalrm, pp );

	/*
	 * Return time left before previous alarm timeout.
	 */
	return( s );
}

/*
 * Send a SIGALARM signal in `n' clock ticks.
 */
long
ualarm2(n)
long n;
{
	register PROC * pp = SELF;
	long s;

	/*
	 * Calculate time left before current alarm timeout.
	 */
	s = 0;
	if ( pp->p_alrmtim.t_last != NULL )
		s = pp->p_alrmtim.t_lbolt - lbolt;

	/*
	 * Cancel previous alarm [if any], start new alarm [if n != 0].
	 */
	timeout2( &pp->p_alrmtim, (long) n, sigalrm, pp );

	/*
	 * Return time left before previous alarm timeout.
	 */
	return( s );
}

/*
 * Change the size of our data segment.
 */
char *
ubrk(cp)
char *cp;
{
	register SEG *sp;
	register vaddr_t sb;

	sp = SELF->p_segp[SIPDATA];
	sb = u.u_segl[SIPDATA].sr_base;
	if (cp != NULL)
		segsize(sp, (vaddr_t)cp-sb);
#ifdef	OLD
	return (0);
#else
	sb += sp->s_size;
	return ((char *)sb);
#endif
}

/*
 * Execute a l.out.
 */
uexece(np, argp, envp)
char *np;
char *argp[];
char *envp[];
{
	pexece(np, argp, envp);
}

/*
 * Exit.
 */
uexit(s)
{
	pexit(s<<8);
}

/*
 * Fork.
 */
ufork()
{
	return (pfork());
}

/*
 * Return date and time.
 */
uftime(tbp)
struct timeb *tbp;
{
	register int s;
	struct timeb timeb;

	timeb.time = timer.t_time;
	/* This should be a machine.h macro to avoid
	 * unnecessary long arithmetic and roundoff errors
	 */
	timeb.millitm = timer.t_tick*(1000/HZ);
	timeb.timezone = timer.t_zone;
	timeb.dstflag = timer.t_dstf;
	s = sphi();
	kucopy(&timeb, tbp, sizeof(timeb));
	spl(s);
}

/*
 * Get effective group id.
 */
ugetegid()
{
	return (u.u_gid);
}

/*
 * Get effective user id.
 */
ugeteuid()
{
	return (u.u_uid);
}

/*
 * Get group id.
 */
ugetgid()
{
	return (u.u_rgid);
}

/*
 * Get process id.
 */
ugetpid()
{
	return (SELF->p_pid);
}

/*
 * Get user id.
 */
ugetuid()
{
	return (u.u_ruid);
}

/*
 * Get process group.
 */
ugetpgrp()
{
	return (SELF->p_group);
}

/*
 * Set the process group.
 * When process group is 0 and a terminal is
 * opened, this process is made the base of
 * processes associated with that terminal.
 */
usetpgrp()
{
	register PROC * pp = SELF;

	if (pp->p_group != pp->p_pid)
		pp->p_ttdev = NODEV;
	return(pp->p_group = pp->p_pid);
}

/*
 * Send the signal `sig' to the process with id `pid'.
 */
ukill(pid, sig)
int pid;
register unsigned sig;
{
	register PROC *pp;
	register int sigflag;

	if ( sig > NSIG ) {
		u.u_error = EINVAL;
		return;
	}
	sigflag = 0;
	lock(pnxgate);
	if (pid > 0) {	/* send to matching process */
		for ( pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw ) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_pid == pid) {
				sigflag = 1;
				if ( sig ) {
					if (sigperm(sig, pp))
						sendsig(sig, pp);
					else
						u.u_error = EPERM;
				}
				break;
			}
		}
	}
	else if (pid < -1) {
		pid = -pid;
		for ( pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw ) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_group == pid) {
				sigflag = 1;
				if (sig) {
					if (sigperm(sig, pp))
						sendsig(sig,pp);
					else
						u.u_error = EPERM;
				}
			}
		}
	}
	else if (pid == 0) {
		for ( pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw ) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_group == SELF->p_group) {
				sigflag = 1;
				if (sig && sigperm(sig, pp))
					sendsig(sig, pp);
			}
		}
	}
	else if (pid == -1) {
		for ( pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw ) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_pid == 0)
				continue;
			if (pp->p_pid == 1)
				continue;
			if ( pp->p_flags & PFKERN )
				continue;
			sigflag = 1;
			if (sig && super())
				sendsig(sig, pp);
		}
	}
	unlock(pnxgate);
	if (sigflag == 0)
		u.u_error = ESRCH;
	return (0);
}

/*
 * See if we have permission to send the signal, `sig' to the process, `pp'.
 */
sigperm(sig, pp)
register PROC *pp;
{
	if (u.u_uid == pp->p_uid)
		return (1);
	if (u.u_ruid == pp->p_ruid) {
		if (sig == SIGHUP
		||  sig == SIGINT
		||  sig == SIGQUIT
		||  sig == SIGTERM)
			return (1);
	}
	if (u.u_uid == 0) {
		u.u_flag |= ASU;
		return (1);
	}
	return (0);
}

/*
 * Lock a process in core.
 */
ulock(f)
{
	if (super() == 0)
		return;
	if (f)
		SELF->p_flags |= PFLOCK;
	else
		SELF->p_flags &= ~PFLOCK;
	return (0);
}

/*
 * Change priority by the given increment.
 */
unice(n)
register int n;
{
	n += SELF->p_nice;
	if (n < MINNICE)
		n = MINNICE;
	if (n > MAXNICE)
		n = MAXNICE;
	if (n<SELF->p_nice && super()==0)
		return;
	SELF->p_nice = n;
	return (0);
}

/*
 * Non existant system call.
 */
unone()
{
	u.u_error = EFAULT;
}

/*
 * Null system call.
 */
unull()
{
}

/*
 * Pause.  Go to sleep on a channel that nobody will wakeup so that only
 * signals will wake us up.
 */
upause()
{
	for (;;)
		sleep((char *)&u, CVPAUSE, IVPAUSE, SVPAUSE);
}

/*
 * Start profiling.  `bp' is the profile buffer, `n' is the size, `off'
 * is the offset in the users programme and `scale' is the scaling
 * factor.
 */
uprofil(bp, n, off, scale)
register char *bp;
{
	u.u_pbase = bp;
	u.u_pbend = bp + n;
	u.u_pofft = off;
	u.u_pscale = scale;
}

/*
 * Process trace.
 */
uptrace(req, pid, add, data)
int *add;
{
	if (req == 0) {
		SELF->p_flags |= PFTRAC;
		return (0);
	}
	return (ptset(req, pid, add, data));
}

/*
 * Set group id.
 */
usetgid(gid)
register int gid;
{
	if (u.u_gid!=gid && super()==0)
		return;
	u.u_gid = gid;
	u.u_rgid = gid;
	SELF->p_rgid = gid;
	return (0);
}

/*
 * Set user id.
 */
usetuid(uid)
register int uid;
{
	if (uid!=u.u_ruid && super()==0)
		return;
	u.u_uid = uid;
	u.u_ruid = uid;
	SELF->p_uid = uid;
	SELF->p_ruid = uid;
	return (0);
}

/*
 * Set up the action to be taken on a signal.
 */
int *
usignal(sig, f)
register int sig;
int (*f)();
{
	register PROC *pp;
	register sig_t s;
	register int (*o)();

	pp = SELF;
	if (sig<=0 || sig>NSIG || sig==SIGKILL) {
		u.u_error = EINVAL;
		return;
	}
	s = (sig_t)1 << --sig;
	o = u.u_sfunc[sig];
	/* This order is critical to isig's use */
	if (f == SIG_IGN) {
		pp->p_isig |= s;
		u.u_sfunc[sig] = f;
	} else {
		u.u_sfunc[sig] = f;
		pp->p_isig &= ~s;
	}
	pp->p_ssig &= ~s;
	return (o);
}

/*
 * Load a device driver.
 */
usload( np )
char *np;
{
	return( pload( np ) );
}

/*
 * Set time and date.
 */
ustime(tp)
register time_t *tp;
{
	register int s;

	if (super() == 0)
		return;
	s = sphi();
	ukcopy(tp, &timer.t_time, sizeof(*tp));
	spl(s);
	return (0);
}

/*
 * Return elapsed ticks since system startup.
 */
long
utick()
{
	return( lbolt );
}

/*
 * Return process times.
 */
utimes(tp)
struct tbuffer *tp;
{
	register PROC *pp;
	struct tbuffer tbuffer;

	pp = SELF;
	tbuffer.tb_utime = pp->p_utime;
	tbuffer.tb_stime = pp->p_stime;
	tbuffer.tb_cutime = pp->p_cutime;
	tbuffer.tb_cstime = pp->p_cstime;
	kucopy(&tbuffer, tp, sizeof(tbuffer));
	return (0);
}

/*
 * Unload a device driver.
 */
usuload(m)
register int m;
{
	if (super() == 0)
		return;
	puload(m);
	return (0);
}


/*
 * Wait for a child to terminate.
 */
uwait(stp)
int *stp;
{
	register PROC *pp;
	register PROC *ppp;
	register PROC *cpp;
	register int pid;

	ppp = SELF;
	for (;;) {
		lock(pnxgate);
		cpp = NULL;
		pp = &procq;
		while ((pp=pp->p_nforw) != &procq) {
			if (pp == ppp)
				continue;
			if (pp->p_ppid != ppp->p_pid)
				continue;
			if ((pp->p_flags&PFSTOP) != 0)
				continue;
			if ((pp->p_flags&PFWAIT) != 0) {
				pp->p_flags &= ~PFWAIT;
				pp->p_flags |= PFSTOP;
				unlock(pnxgate);
				if (stp != NULL)
					putuwd(stp, 0177);
				return (pp->p_pid);
			}
			if (pp->p_state == PSDEAD) {
				ppp->p_cutime += pp->p_utime + pp->p_cutime;
				ppp->p_cstime += pp->p_stime + pp->p_cstime;
				if (stp != NULL)
					putuwd(stp, pp->p_exit);
				pid = pp->p_pid;
				unlock(pnxgate);
				relproc(pp);
				return (pid);
			}
			cpp = pp;
		}
		unlock(pnxgate);
		if (cpp == NULL) {
			u.u_error = ECHILD;
			return;
		}
		sleep((char *)ppp, CVWAIT, IVWAIT, SVWAIT);
	}
}
