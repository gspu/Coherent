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
 * coh.386/sys1.c
 *
 * Coherent.
 * General system calls.
 *
 * Revised: Tue May 11 11:12:03 1993 CDT
 */

#include <common/_gregset.h>
#include <kernel/sigproc.h>
#include <sys/coherent.h>
#include <sys/acct.h>
#include <sys/con.h>
#include <sys/wait.h>
#include <sys/errno.h>
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
	sendsig (SIGALRM, pp);
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
	timeout2 (& pp->p_alrmtim, (long) n * HZ, sigalrm, pp);

	/*
	 * Return time left before previous alarm timeout.
	 */
	return s;
}


/*
 * Change the size of our data segment.
 */
char *
ubrk(cp)
caddr_t cp;
{
	register SEG *sp;
	register caddr_t sb;
	register SR	*stack_sr;
	caddr_t top_of_stack;

	T_HAL (0x8000, printf ("%s:ubrk(%x) ", u.u_comm, cp));

	/*
	 * Pick up the segment handle for our data segment.
	 */
	sp = SELF->p_segp [SIPDATA];

	/*
	 * Extract the starting virtual address for our data segment,
	 * as it is currently mapped into the memory space.
	 */
	sb = u.u_segl [SIPDATA].sr_base;

	/*
	 * We can not move the top of the data segment below the
	 * start of the data segment.
	 */
	if (cp < sb) {
		SET_U_ERROR (ENOMEM,
		    "Requested brk address is below start of data segment.");
		return 0;
	}

	/*
	 * Would the request cause a collision with the stack segment?
	 *
	 * Since the stack grows downward, its top is below its base :-).
	 */

	stack_sr = & u.u_segl [SISTACK];
	top_of_stack = stack_sr->sr_base - stack_sr->sr_size;

	if (btoc (cp) >= btoc (top_of_stack)) {
		SET_U_ERROR (ENOMEM,
		    "Requested brk address would collide with stack segment.");
 		return 0;
	}

	/*
	 * Attempt to establish the segment with the newly requested size.
	 */
	segsize (sp, cp - sb);

	/*
	 * Be sure to return the true new top of data segment.
	 */
	sb += sp->s_size;

	T_HAL (0x8000, printf ("=%x ", sb));
	return sb;
}


/*
 * Execute an l.out or COFF file.
 */

int
uexece (np, argp, envp, regsetp)
char	      *	np;
char	      *	argp [];
char	      *	envp [];
gregset_t     *	regsetp;
{
	pexece (np, argp, envp, regsetp);
}


/*
 * Exit.
 */

uexit (s)
unsigned	s;
{
	pexit ((s & 0xFF) << 8);
}


/*
 * Fork.
 */

pid_t
ufork ()
{
	return pfork ();
}

/*
 * Get group id.
 * Get effective group id.
 */

gid_t
ugetgid ()
{
	u.u_rval2 = u.u_gid;
	return u.u_rgid;
}


/*
 * Get user id.
 * Get effective user id.
 */

uid_t
ugetuid ()
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

int
upgrp (fl)
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

pid_t
ugetpid()
{
	u.u_rval2 = SELF->p_ppid;
	return SELF->p_pid;
}


/*
 * Send the signal `sig' to the process with id `pid'.
 */

int
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
	lock (pnxgate);

	if (pid > 0) {	/* send to matching process */
		for (pp = procq.p_nforw ; pp != & procq ; pp = pp->p_nforw) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_pid == pid) {
				sigflag = 1;
				if (sig) {
					if (sigperm (sig, pp))
						sendsig (sig, pp);
					else
						u.u_error = EPERM;
				}
				break;
			}
		}
	} else if (pid < -1) {
		pid = -pid;
		for (pp = procq.p_nforw ; pp != & procq ; pp = pp->p_nforw) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_group == pid) {
				sigflag = 1;
				if (sig) {
					if (sigperm (sig, pp))
						sendsig (sig,pp);
					else
						u.u_error = EPERM;
				}
			}
		}
	} else if (pid == 0) {
		for (pp = procq.p_nforw ; pp != & procq ; pp = pp->p_nforw) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_group == SELF->p_group) {
				sigflag = 1;
				if (sig && sigperm (sig, pp))
					sendsig (sig, pp);
			}
		}
	} else if (pid == -1) {
		for (pp = procq.p_nforw ; pp != & procq ; pp = pp->p_nforw) {
			if (pp->p_state == PSDEAD)
				continue;
			if (pp->p_pid == 0)
				continue;
			if (pp->p_pid == 1)
				continue;
			if (pp->p_flags & PFKERN)
				continue;
			sigflag = 1;
			if (sig && super ())
				sendsig (sig, pp);
		}
	}

	unlock (pnxgate);
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
		return 1;

	if (u.u_ruid == pp->p_ruid && (sig == SIGHUP || sig == SIGINT ||
				       sig == SIGQUIT || sig == SIGTERM)) 
		return 1;

	if (u.u_uid == 0) {
		u.u_flag |= ASU;
		return 1;
	}
	return 0;
}

/*
 * Lock a process in core.
 */
ulock(f)
{
	if (super () == 0)
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
	if (n < SELF->p_nice && super () == 0)
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
	x_sleep ((char *) & u, prilo, slpriSigLjmp, "pause");
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
		u.u_pscale ++;
}

/*
 * Process trace.
 */
uptrace(req, pid, add, data)
int *add;
{
	int ret;

#ifdef TRACER
	int readChild = 0;	/* for debug, true if reading child memory */

	if (t_hal & 0x10000) {
		switch (req) {
		case 0:	/* init called by child */
			printf("PSetup: child=%d  ", SELF->p_pid);
			break;
		case 1:	/* parent reads child text */
			printf("PRdT: add=%x ", add);
			readChild = 1;
			break;
		case 2:	/* parent reads child data */
			printf("PRdD: add=%x ", add);
			readChild = 1;
			break;
		case 3:	/* parent reads child u area */
			printf("PRdU: add=%x ", add);
			readChild = 1;
			break;
		case 4:	/* parent writes child text */
			printf("PWrT: add=%x data=%x  ", add, data);
			break;
		case 5:	/* parent writes child data */
			printf("PWrD: add=%x data=%x  ", add, data);
			break;
		case 6:	/* parent writes child u area */
			printf("PWrU: add=%x data=%x ", add, data);
			break;
		case 7:	/* resume child, maybe fake signal to child */
			printf("PResume: sig=%d  ", data);
			break;
		case 8:	/* terminate child */
			printf("PTerm: pid=%d  ", pid);
			break;
		case 9:	/* single-step child, maybe fake signal to child */
			printf("PSStp: sig=%d  ", data);
			break;
		}
	}
#endif

	if (req == 0) {
		SELF->p_flags |= PFTRAC;
		ret = 0;
	} else
		ret = ptset (req, pid, add, data);

#ifdef TRACER
	if (t_hal & 0x10000) {
		if (readChild)
			printf("data=%x  ", ret);
	}
#endif

	return ret;
}

/*
 * Set group id.
 *
 * As in SVID issue 2:
 *
 * if effective gid is superuser
 *	set real, effective, and saved effective gid to argument "gid"
 * else if real gid is same as "gid"
 *	set effective gid to "gid"
 * else if saved effective gid is same as "gid"
 *	set effective gid to "gid"
 */

usetgid(gid)
register int gid;
{
	if (super ()) {
		u.u_gid = u.u_rgid = u.u_egid = gid;
		SELF->p_rgid = gid;
	} else {
		u.u_error = 0;  /* super() sets u_error when it fails */

		if (u.u_rgid == gid || u.u_egid == gid)
			u.u_gid = gid;
		else
			SET_U_ERROR (EPERM, "Illegal gid");
	}
	return 0;
}

/*
 * Set user id.
 *
 * As in SVID issue 2:
 *
 * if effective uid is superuser
 *	set real, effective, and saved effective uid to argument "uid"
 * else if real uid is same as "uid"
 *	set effective uid to "uid"
 * else if saved effective uid is same as "uid"
 *	set effective uid to "uid"
 */

usetuid(uid)
register int uid;
{
	if (super ()) {
		u.u_uid = u.u_ruid = u.u_euid = uid;
		SELF->p_uid = SELF->p_ruid = uid;
	} else {
		u.u_error = 0;  /* super() sets u_error when it fails */

		if (u.u_ruid == uid || u.u_euid == uid)
			SELF->p_uid = u.u_uid = uid;
		else
			SET_U_ERROR(EPERM, "Illegal uid");
	}
	return 0;
}

/*
 * Load a device driver.
 */
usload(np)
char *np;
{
	return pload (np);
}

/*
 * Set time and date.
 *
 * Unlike the libc interface, this routine expects a time_t value
 * as an arg, not a time_t pointer.
 */

int
ustime (newtime)
time_t newtime;
{
	register int s;

	if (super () == 0)
		return;

	s = sphi ();
	timer.t_time = newtime;
	spl (s);
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

	if (tp) {
		pp = SELF;
		tbuffer.tms_utime = pp->p_utime;
		tbuffer.tms_stime = pp->p_stime;
		tbuffer.tms_cutime = pp->p_cutime;
		tbuffer.tms_cstime = pp->p_cstime;
		kucopyS (& tbuffer, tp, sizeof (tbuffer));
	}
	return lbolt;
}

/*
 * Unload a device driver.
 */
usuload(m)
register int m;
{
	if (super () == 0)
		return;
	puload (m);
	return 0;
}

/*
 * Wait for a child to terminate. If this function is called from i286 code,
 * then we will have a "statp" argument, where we stash the exit status, but
 * for the i386 code we put a value in u.u_rval2.
 *
 * NIGEL: The idiom for looping over the process table must be changed to
 * work right ASAP.
 */

int
uwait (statp)
short	      *	statp;
{
	register PROC *pp;
	register PROC *ppp;
	register PROC *cpp;
	register int pid;

	/* Wait for a child to stop or die. */

	T_HAL (8, printf ("[%d]waits ", SELF->p_pid));
	ppp = SELF;

	for (;;) {
		/* Look at all processes. */
again:
		lock (pnxgate);
		cpp = NULL;
		pp = & procq;
		while ((pp = pp->p_nforw) != &procq) {

			/* Ignore the current process. */
			if (pp == ppp)
				continue;
			/*
			 * Ignore processes that aren't children of the
			 * current one.
			 */
			if (pp->p_ppid != ppp->p_pid ||
			    (pp->p_flags & PFSTOP) != 0)
				continue;

			/* Here is a child that hit a breakpoint. */
			if (pp->p_flags & PFWAIT) {
				int work;	/* virtual click number */
				int childUseg;	/* system global addr */
				UPROC * uprc;
				SEG * sp;

				pp->p_flags &= ~PFWAIT;
				pp->p_flags |= PFSTOP;

				/* fetch u.u_signo from the child */

				/* Find u area for child process pp */
				sp = pp->p_segp [SIUSERP];
				childUseg = MAPIO (sp->s_vmem, U_OFFSET);
				work = workAlloc ();
				ptable1_v [work] =
				  sysmem.u.pbase [btocrd (childUseg)] | SEG_RW;
				uprc = (UPROC *) (ctob (work) + U_OFFSET);

				if (statp != NULL)
					putusd (statp, (uprc->u_signo << 8) |
							__WSTOPFLG);
				else
					u.u_rval2 = (uprc->u_signo << 8) |
							__WSTOPFLG;

				workFree (work);

				unlock (pnxgate);
				T_HAL (8,
				       printf ("[%d]ends waiting, %d stopped ",
					       SELF->p_pid, pid));
				return pp->p_pid;
			}
			if (pp->p_state == PSDEAD) {
				ppp->p_cutime += pp->p_utime + pp->p_cutime;
				ppp->p_cstime += pp->p_stime + pp->p_cstime;

				if (statp != NULL)
					putusd (statp, pp->p_exit);
				else
					u.u_rval2 = pp->p_exit;
				pid = pp->p_pid;
				unlock (pnxgate);
				relproc (pp);

				if ((proc_signal_misc (ppp) &
				     __SF_NOCLDWAIT) != 0)
					goto again;

				T_HAL (8, printf ("[%d]ends waiting, %d died ",
						  SELF->p_pid, pid));
				return pid;
			}
			cpp = pp;
		}
		unlock (pnxgate);
		if (cpp == NULL) {
			T_HAL (8, printf ("[%d]ends waiting, no children ",
					  SELF->p_pid));
			u.u_error = ECHILD;
			return;
		}
		(void) x_sleep ((char *) ppp, prilo, slpriSigLjmp, "wait");
		/* Wait for a child to terminate.  */
	}
}

/*
 * waitpid() and wait() share the same system call number under BCS.
 *
 * pid argument:
 *	>  0	wait for child whose process matches pid
 *	=  0	wait for any child in current process group
 *	= -1	wait for any child - same as wait()
 *	< -1	wait for any child in group given by -pid
 *
 * The only waitpid() options supported are WNOHANG and WUNTRACED.
 *
 */
int
uwaitpid(opid, stat_loc, options)
register pid_t opid;
int	*stat_loc, options;
{
	register PROC *pp;
	register PROC *ppp;
	register PROC *cpp;
	register int pid;

	if (options & WUNTRACED) {	
                printf("waitpid(%d,%d, WUNTRACED): unsupported\n", opid,
								     stat_loc);
		u.u_error = EINVAL;
		return;
	}

	/* Wait for a child to stop or die. */
	ppp = SELF;
	for (;;) {
		/* Look at all processes. */
again:
		lock (pnxgate);
		cpp = NULL;
		pp = & procq;
		while ((pp=pp->p_nforw) != &procq) {

			/* Ignore the current process. */
			if (pp == ppp)
				continue;
			/*
			 * Ignore processes that aren't children of the
			 * current one.
			 */
			if (pp->p_ppid != ppp->p_pid ||
			    (pp->p_flags & PFSTOP) != 0)
				continue;

			/* If opid == 0 we want to match gids */
			/* If opid>0, want to match opid to child pid */
			/* If opid<-1, want to match -opid to child gid */
			if ((opid == 0 && pp->p_group != ppp->p_group) ||
			    (opid > 0 && opid != pp->p_pid) ||
			    (opid < -1 && - opid != pp->p_group))
				continue;

			/* if opid == -1, then any child is acceptable */

			/* Here is an acceptable child that hit a breakpoint. */
			if (pp->p_flags & PFWAIT) {
				int work;	/* virtual click number */
				int childUseg;	/* system global addr */
				UPROC * uprc;
				SEG * sp;

				pp->p_flags &= ~PFWAIT;
				pp->p_flags |= PFSTOP;

				/* fetch u.u_signo from the child */

				/* Find u area for child process pp */
				sp = pp->p_segp [SIUSERP];
				childUseg = MAPIO (sp->s_vmem, U_OFFSET);
				work = workAlloc ();
				ptable1_v [work] =
				  sysmem.u.pbase [btocrd (childUseg)] | SEG_RW;
				mmuupd ();
				uprc = (UPROC *) (ctob (work) + U_OFFSET);
				u.u_rval2 = (uprc->u_signo << 8) | __WSTOPFLG;
				workFree (work);

				unlock (pnxgate);
				return pp->p_pid;
			}

			/* Here is an acceptable child that is a zombie. */
			if (pp->p_state == PSDEAD) {
				ppp->p_cutime += pp->p_utime + pp->p_cutime;
				ppp->p_cstime += pp->p_stime + pp->p_cstime;
				u.u_rval2 = pp->p_exit;
				pid = pp->p_pid;
				unlock (pnxgate);
				relproc (pp);

				if ((proc_signal_misc (ppp) &
				     __SF_NOCLDWAIT) != 0)
					goto again;

				return pid;
			}
			cpp = pp;
		}
		unlock (pnxgate);
		if (cpp == NULL) {
			u.u_error = ECHILD;
			return;
		}

		if (options & WNOHANG) {
               		u.u_rval2 = 0;
			return 0;
		}

		/* Wait for a child to terminate. */
		(void) x_sleep ((char *) ppp, prilo, slpriSigLjmp,
				"waitpid");
	}
}

/*
 * Wait for a child to terminate.
 *
 * iBCS2 says the same system call number is wait() and waitpid(), the
 * distinction being in how the psw is set on entry.
 *
 * iBCS2 fails to mention that when wait() or waitpid() report status
 * by writing into the pointer supplied, the status is put into %edx by
 * the kernel, and moved from there into user space by the function in
 * libc.a.  uwait() and uwaitpid() specify a value for %edx by writing
 * to u.u_rval2.
 *
 * Do wait() unless (ZF|PF|SF|OF) (=WPMASK) are set in psw.
 */
#define	WPMASK	0x8C4

uwait386(arg1, arg2, arg3, regsetp)
unsigned long	arg1;
unsigned long	arg2;
unsigned long	arg3;
gregset_t     *	regsetp;
{
	return (regsetp->_i386._eflags & WPMASK) == WPMASK ?
			uwaitpid (arg1, arg2, arg3) : uwait (NULL);
}

