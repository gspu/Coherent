/* $Header: /y/coh.386/RCS/sys1.c,v 1.7 92/07/16 16:33:34 hal Exp $ */
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
 * Revision 1.7  92/07/16  16:33:34  hal
 * Kernel #58
 * 
 * Revision 1.4  92/01/27  12:38:52  hal
 * Forgot to check flag in upgrp().
 * 
 * Revision 1.3  92/01/24  21:29:35  hal
 * Kernel version 29.
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
#include <sys/times.h>

/*
 * Send alarm signal to specified process - function timed by ualarm()
 */
sigalrm(pp)
register PROC * pp;
{
	sendsig(SIGALRM, pp);
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
	if (pp->p_alrmtim.t_last != NULL)
		s = (pp->p_alrmtim.t_lbolt - lbolt + HZ - 1) / HZ;

	/*
	 * Cancel previous alarm [if any], start new alarm [if n != 0].
	 */
	timeout2(&pp->p_alrmtim, (long) n * HZ, sigalrm, pp);

	/*
	 * Return time left before previous alarm timeout.
	 */
	return(s);
}


/*
 * Change the size of our data segment.
 */
char *
ubrk(cp)
vaddr_t cp;
{
	register SEG *sp;
	register vaddr_t sb;
	register SR	*stack_sr;
	vaddr_t top_of_stack;

	T_HAL(0x8000, printf("%s:ubrk(%x) ", u.u_comm, cp));

	/*
	 * Pick up the segment handle for our data segment.
	 */
	sp = SELF->p_segp[SIPDATA];

	/*
	 * Extract the starting virtual address for our data segment,
	 * as it is currently mapped into the memory space.
	 */
	sb = u.u_segl[SIPDATA].sr_base;

	/*
	 * We can not move the top of the data segment below the
	 * start of the data segment.
	 */
	if (cp < sb) {
		SET_U_ERROR(ENOMEM,
		    "Requested brk address is below start of data segment.");
		return 0;
	}

	/*
	 * Would the request cause a collision with the stack segment?
	 *
	 * Since the stack grows downward, its top is below its base :-).
	 */
	stack_sr = &u.u_segl[SISTACK];
	top_of_stack = (stack_sr->sr_base) - (stack_sr->sr_size);

	if (btoc(cp) >= btoc(top_of_stack)) {
		SET_U_ERROR(ENOMEM,
		    "Requested brk address would collide with stack segment.");
 		return 0;
	}

	/*
	 * Attempt to establish the segment with the newly requested size.
	 */
	segsize(sp, (cp - sb));

	/*
	 * Be sure to return the true new top of data segment.
	 */
	sb += sp->s_size;

	T_HAL(0x8000, printf("=%x ", sb));
	return sb;
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
 * Get group id.
 * Get effective group id.
 */
ugetgid()
{
	u.u_rval2 = u.u_gid;
	return u.u_rgid;
}

/*
 * Get user id.
 * Get effective user id.
 */
ugetuid()
{
	u.u_rval2 = u.u_uid;
	return u.u_ruid;
}

/*
 * Get process group.
 * Set the process group.
 *
 * This is System V type setpgrp().
 * Set process group equal to process id (make process its own group leader).
 * If process was NOT already a group leader, lose its controlling terminal.
 */
upgrp(fl)
{
	register PROC * pp = SELF;
	
	if (fl) {
		if (pp->p_group != pp->p_pid)
			pp->p_ttdev = NODEV;
		pp->p_group = pp->p_pid;
	}
	return pp->p_group;
}

/*
 * Get process id.
 */
ugetpid()
{
	register PROC *pp = SELF;

	u.u_rval2 = pp->p_ppid;
	return pp->p_pid;
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

	if (sig > NSIG) {
		u.u_error = EINVAL;
		return;
	}
	sigflag = 0;
	lock(pnxgate);
	if (pid > 0) {	/* send to matching process */
		for (pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_pid == pid) {
				sigflag = 1;
				if (sig) {
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
		for (pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw) {
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
		for (pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw) {
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
		for (pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_pid == 0)
				continue;
			if (pp->p_pid == 1)
				continue;
			if (pp->p_flags & PFKERN)
				continue;
			sigflag = 1;
			if (sig && super())
				sendsig(sig, pp);
		}
	}
	unlock(pnxgate);
	if (sigflag == 0)
		u.u_error = ESRCH;
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
		v_sleep((char *)&u, CVPAUSE, IVPAUSE, SVPAUSE, "pause");
	/* The pause system call.  */
}

/*
 * Start/stop profiling.
 *
 * buff:	address in user data of an array of shorts
 * bufsiz:	number of bytes in the area at buff
 * offset:	address in user text of start of profiling area
 * scale:	0 or 1 - turn off profiling
 *		other - treat as 16 bit scale factor
 *
 * For purposes of compatibility with System 5, scale values work as follows:
 *	0xFFFF	profile buffer is same length as text being profiled.
 *	0x7FFF  profile buffer is half as long as text being profiled.
 *	0x4000	profile buffer is one fourth as long as text profiled.
 *		(each short in the buffer covers 8 bytes of text)
 *	...	...
 *	0x0002	each short in the buffer covers 64K bytes of text.
 *
 * Values 0xFFFF and 0x7FFF are used, for historical reasons, when 0x10000
 * and 0x8000, respectively, should be used.  To clean up the ensuing
 * arithmetic, there is an upward rounding kluge below.
 *
 * Each clock interrupt, take (pc - offset) * scale * (2**-16) as a byte
 * offset into pbase.  Add 1 to the short at or below the given address
 * when profiling.
 */
uprofil(buff, bufsiz, offset, scale)
short * buff;
int bufsiz, offset, scale;
{
	u.u_pbase = buff;
	u.u_pbend = buff + bufsiz;
	u.u_pofft = offset;
	u.u_pscale = scale & 0xffff;	/* scale is really unsigned short */

	/* round up kluge - see above */
	if ((scale & 0xfff) == 0xfff)
		u.u_pscale++;
}

/*
 * Process trace.
 */
uptrace(req, pid, add, data)
int *add;
{
	if (req == 0) {
		SELF->p_flags |= PFTRAC;
		return 0;
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
	return 0;
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
	return 0;
}

/*
 * Load a device driver.
 */
usload(np)
char *np;
{
	return pload(np);
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
	return 0;
}

/*
 * Return process times.
 */
utimes(tp)
struct tms *tp;
{
	register PROC *pp;
	struct tms tbuffer;

	pp = SELF;
	tbuffer.tms_utime = pp->p_utime;
	tbuffer.tms_stime = pp->p_stime;
	tbuffer.tms_cutime = pp->p_cutime;
	tbuffer.tms_cstime = pp->p_cstime;
	kucopy(&tbuffer, tp, sizeof(tbuffer));
	return 0;
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
	return 0;
}


/*
 * Wait for a child to terminate.
 */
uwait()
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
				u.u_rval2 = 0177;
T_PIGGY(0x100,
	printf("<uwait(WAIT): pid: %d ppid: %d rval2: 0x%x, signo: %d>",
	       pp->p_pid, pp->p_ppid, u.u_rval2, u.u_signo);
);
				return (pp->p_pid);
			}
			if (pp->p_state == PSDEAD) {
				ppp->p_cutime += pp->p_utime + pp->p_cutime;
				ppp->p_cstime += pp->p_stime + pp->p_cstime;
				u.u_rval2 = pp->p_exit;
				pid = pp->p_pid;
				unlock(pnxgate);
				relproc(pp);
T_PIGGY(0x100,
	printf("<uwait(DEAD): pid: %d ppid: %d rval2: 0x%x, signo: %d>",
	       pp->p_pid, pp->p_ppid, u.u_rval2, u.u_signo);
);
				return (pid);
			}
			cpp = pp;
		}
		unlock(pnxgate);
		if (cpp == NULL) {
			u.u_error = ECHILD;
#if 0 /* This error happens so often it tends to run away.  */
			SET_U_ERROR( ECHILD,
				     "there are no children to wait for" );
#endif /* 0 */
			T_PIGGY( 0x100, printf(";"); );
			return;
		}
		v_sleep((char *)ppp, CVWAIT, IVWAIT, SVWAIT, "wait");
		/* Wait for a child to terminate.  */
	}
}
