/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 5.0
 *	Copyright (c) 1982, 1993.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * File:	coh.386/sig.c
 *
 * Purpose:	signal handling
 *
 * Revised: Tue May  4 11:59:15 1993 CDT
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <sys/coherent.h>
#include <errno.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <sys/ptrace.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/core.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
typedef void (*VFPTR)();	/* pointer to void function */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int	actvsig();
int	nondsig();
int	ptret();
int	ptset();
void	sendsig();
void	sigDefault();
void	sigHold();
void	sigIgnore();
void	sigPause();
void	sigRelease();
int	sigSet();
int	sigdump();
int	usigsys();

static struct _fpstate * empack();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
/*
 * Patchable variables.
 *
 * Patch DUMP_TEXT nonzero to make text segment show up in core files.
 * Patch DUMP_LIM set the upper limit in bytes of how much of a
 * segment is written to a core file.
 *
 * Patch CATCH_SEGV nonzero if you are trying to run software that was
 * written in blatant defiance of the SVID 2 caution that handling SIGSEGV
 * is nonportable and should not be assumed valid on all systems.
 */
int	DUMP_TEXT = 0;
int	DUMP_LIM=512*1024;
int	CATCH_SEGV = 0;

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * Given 1-based signal number, ask whether a signal handler was
 * attached to the current process using sigset().  This allows
 * the kernel to process sigset() differently from signal(), as
 * required.
 *
 * return 1 if sigset(), 0 if not.
 */
int
sigSet(signal)
int signal;
{
	return (SELF->p_dsig & SIG_BIT(signal)) ? 1 : 0;
}

/*
 * Given 1-based signal number, ignore that signal in the current process.
 */
void
sigIgnore(signal)
int signal;
{
	int sigbit = SIG_BIT(signal);

	SELF->p_dfsig &= ~sigbit;	/* No longer defaulted.  */
	SELF->p_isig |= sigbit;		/* Mark signal as ignored.  */
	u.u_sfunc[signal - 1] = SIG_IGN;
}

/*
 * Given 1-based signal number, restore default handling for the current
 * process.
 *
 * There is some duplication of work done in sigAttach(), but sigDefault()
 * is also called from msig.c
 */
void
sigDefault(signal)
int signal;
{
	int sigbit = SIG_BIT(signal);

	SELF->p_dfsig |= sigbit;
	SELF->p_isig &= ~sigbit;
	u.u_sfunc[signal - 1] = SIG_DFL;
}

/*
 * Given 1-based signal number, hold that signal for the current process.
 */
void
sigHold(signal)
int signal;
{
	SELF->p_hsig |= SIG_BIT(signal);
}

/*
 * Given 1-based signal number, pause for that signal for the current process.
 */
void
sigPause(signal)
int signal;
{
	SELF->p_hsig &= ~SIG_BIT(signal);

	/*
	 * Like upause(), do a sleep on an event which never gets a wakeup.
	 * The sleep returns immediately if a signal was already holding.
	 */
	x_sleep((char *)&u, prilo, slpriSigCatch, "sigpause");
	actvsig();
}

/*
 * Given 1-based signal number, release that signal for the current process.
 */
void
sigRelease(signal)
int signal;
{
	SELF->p_hsig &= ~SIG_BIT(signal);
	if (nondsig()) {
		actvsig();
	}
}

/*
 * Given 1-based signal number, a pointer to a signal-handling function,
 * and a flag, attach the signal handler to the current process.
 *
 * Function pointer "func" may take on special values SIG_DFL, SIG_IGN,
 * and, if "how" is SIGSET, SIG_HOLD.
 *
 * The flag "how" is 0 if attachment is via signal(), SIGSET if attachment
 * is via sigset().
 *
 * Return the previously attached signal handler, or SIG_HOLD if signals
 * were previously held.
 */
VFPTR
sigAttach(signal, func, how)
int signal;
VFPTR func;
int how;
{
	VFPTR retval;
	int sigbit = SIG_BIT(signal);

	/*
	 * Set up the return value, which says what was previously
	 * done with the given signal.
	 */
	if (SELF->p_isig & sigbit)
		retval = (VFPTR)SIG_IGN;
	else if (SELF->p_hsig & sigbit)
		retval = (VFPTR)SIG_HOLD;
	else
		retval = u.u_sfunc[signal - 1];

	u.u_sigreturn = (VFPTR)(u.u_regl[EDX]);
	u.u_sfunc[signal - 1] = func;

	/*
	 * Remove previous default, ignore, or hold status.
	 */
	SELF->p_dfsig &= ~sigbit;
	SELF->p_isig &= ~sigbit;
	SELF->p_hsig &= ~sigbit;

	/*
	 * Any pending signal is lost.
	 */
	SELF->p_ssig &= ~sigbit;

	/*
	 * Special cases for handler.
	 */
	switch ((int)func) {
	case (int)SIG_DFL:
		sigDefault(signal);
		break;
	case (int)SIG_IGN:
		sigIgnore(signal);
		break;
	case (int)SIG_HOLD:
		sigHold(signal);
		break;
	}

	/*
	 * Remember whether handler was attached with sigset() vs signal().
	 */
	if (how == SIGSET)
		SELF->p_dsig |= sigbit;
	else
		SELF->p_dsig &= ~sigbit;

	return retval;
}

/*
 * Set up the action to be taken on a signal.
 */
int
usigsys(signal, func)
int	signal;
VFPTR func;
{
	int	sigtype;
	int	retval = 0;

	sigtype = signal & ~0xFF;
	signal &= 0xFF;

	T_HAL(8, if (signal == SIGINT)
	  printf("[%d]sigint(%x, %x) ", SELF->p_pid, sigtype, func));

	/* Range check on 1-based signal number. */
	if (signal <= 0 || signal > NSIG) {
		u.u_error = EINVAL;
		return;
	}

	/*
	 * Don't allow setting/holding/releasing some signals.
	 *
	 * NOTICE:  Ignoring SIGSEGV causes runaway user faults.
	 * SVID Issue 2 says *don't* do signal(SIGSEGV,...)!!!
	 */
	if (signal == SIGKILL) {
		u.u_error = EINVAL;
		return;
	}

	if (signal == SIGSEGV && CATCH_SEGV == 0) {
		u.u_error = EINVAL;
		return;
	}
	 
	switch (sigtype) {
	case SIGHOLD:
		sigHold(signal);
		break;
	case SIGRELSE:
		sigRelease(signal);
		break;
	case SIGIGNORE:
		sigIgnore(signal);
		break;
	case 0:				/* old system entry */
		retval = (int)sigAttach(signal, func, 0);
		break;
	case SIGSET:			/* new system entry */
		retval = (int)sigAttach(signal, func, SIGSET);
		break;
	case SIGPAUSE:
		sigPause(signal);
		break;
	default:
		u.u_error = SIGSYS;
		break;
	}
	return retval;
}

/*
 * Send a signal to the process `pp'.
 * Return 1 if signal was sent.
 * Return 0 if signal was ignored.
 * The return value is of use to the trap handler.
 */
void
sendsig(sig, pp)
register unsigned sig;
register PROC *pp;
{
	register sig_t f;
	register int s;

	T_HAL(8, if (sig == SIGINT) printf("[%d]gets int ", pp->p_pid));

	/*
	 * Convert the signal to a bit position.
	 */
	f = SIG_BIT(sig);

	/*
	 * If the signal is ignored, and is not SIGCLD, do nothing.
	 */
	if ((pp->p_isig & f) && sig != SIGCLD) {
		goto sendSigDone;
	}

	/*
	 * No further processing for delayed or held signals.
	 */
	if ((pp->p_ssig & f) && (pp->p_hsig|pp->p_dsig) & f)
		goto sendSigDone;
	
	/*
	 * Actually send the signal by flagging the needed bit.
	 */
	pp->p_ssig |= f;

	/*
	 * If the process is sleeping, wake it up so that
	 * it can process this signal.
	 */
	if (pp->p_state == PSSLSIG) {
		s = sphi();
		pp->p_lback->p_lforw = pp->p_lforw;
		pp->p_lforw->p_lback = pp->p_lback;
#ifndef _I386
		addu(pp->p_cval, (utimer-pp->p_lctim)*CVCLOCK);
#endif
		setrun(pp);
		spl(s);
	}
sendSigDone:
	return;
}

/*
 * Return signal number if we have a non ignored or delayed signal, else zero.
 */
int
nondsig()
{
	register PROC *pp;
	register sig_t mask;
	register int signo;

	pp = SELF;
	signo = 0;

	/*
	 * Turn off all ignored signals except SIGCLD.
	 */
	pp->p_ssig &= ~(pp->p_isig & ~SIG_BIT(SIGCLD));

	/*
	 * If any signals have arrived, but which are not held,
	 * figure out what they are.
	 */
	if (pp->p_ssig&~pp->p_hsig) {
		/*
		 * There is at least one signal.  Extract its number
		 * from the signal bits.
		 */
		mask = (sig_t) 1;
		signo += 1;
		while (((pp->p_ssig&~pp->p_hsig) & mask) == 0) {
			mask <<= 1;
			signo += 1;
		}
	}
	return signo;
}

/*
 * If we have a signal that isn't ignored, activate it.
 */
int
actvsig()
{
	register int signum;
	register PROC *pp;
	register int (*func)();
	int ptval;

	/*
	 * Fetch an unprocessed signal.
	 * Return if there are none.
	 * The while() structure is only for traced processes.
	 */
	while (signum = nondsig()) {

		pp = SELF;

		/*
		 * Reset the signal to indicate that it has been processed.
		 * Bit table p_ssig uses 0-based signals, while signal.h
		 * lists 1-based signals.
		 */
		pp->p_ssig &= ~SIG_BIT(signum);

		/*
		 * Fetch the user function that goes with this signal.
		 * Function table u_sfunc uses 0-based signals, while signal.h
		 * lists 1-based signals.
		 */
		func = u.u_sfunc[signum-1];

		/*
		 * SIGCLD causes no work here if defaulted or ignored.
		 */
		if (signum == SIGCLD && (func == SIG_DFL || func == SIG_IGN))
			return;

		/*
		 * Store the (1-based) signal number in the u area.
		 * This is how a core dump records the death signal.
		 */
		u.u_signo = signum;

		/*
		 * If the signal is not defaulted, go run the requested
		 * function.
		 */
		if (func != SIG_DFL) {
			if (XMODE_286)
				oldsigstart(signum, func);
			else {
				msigstart(signum, func);
			}
			return;
		}

		/*
		 * ASSERTION:  the signal being processed is SIG_DFL'd.
		 */

		/*
		 * msysgen() is a nop for COHERENT 4.0.  The comment in the
		 * assembly code is "Nothing useful to save".
		 */
		msysgen(u.u_sysgen);

		/*
		 * When a traced process is signaled, it may need to exchange
		 * data with its parent (via ptret).
		 */
		if (pp->p_flags&PFTRAC) {
			pp->p_flags |= PFWAIT;
			ptval = ptret();
			T_HAL(0x10000, printf("ptret()=%x ", ptval));
			pp->p_flags &= ~(PFWAIT|PFSTOP);
			if (ptval == 0)
				/* see if another signal came in */
				continue;
			else
				signum = ptval;
		}

		/*
		 * Some signals cause a core file to be written.
		 */
		switch(signum) {
		case SIGQUIT:
		case SIGILL:
		case SIGTRAP:
		case SIGABRT:
		case SIGFPE:
		case SIGSEGV:
		case SIGSYS:
			if (sigdump())
				signum |= 0x80;
			break;
		}
		pexit(signum);
	}
}

/*
 * Create a dump of ourselves onto the file `core'.
 */
int
sigdump()
{
	register INODE *ip;
	register SR *srp;
	register SEG * sp;
	register int n;
	register paddr_t ssize;
	extern	int	DUMP_LIM;
	struct ch_info chInfo;

	if (SELF->p_flags&PFNDMP)
		return (0);
	u.u_io.io_seg  = IOSYS;
	u.u_io.io_flag = 0;
	/* Make the core with the real owners */
	schizo();
	if (ftoi("core", 'c')) {
		schizo();
		return (0);
	}
	if ((ip=u.u_cdiri) == NULL) {
		if ((ip=imake(IFREG|0644, 0)) == NULL) {
			schizo();
			return (0);
		}
	} else {
		if ((ip->i_mode&IFMT)!=IFREG
		 || iaccess(ip, IPW)==0
		 || getment(ip->i_dev, 1)==NULL) {
			idetach(ip);
			schizo();
			return (0);
		}
		iclear(ip);
	}
	schizo();
	u.u_error = 0;
	u.u_io.io_seek = 0;

	/* Write core file header */
	chInfo.ch_magic = CORE_MAGIC;
	chInfo.ch_info_len = sizeof(chInfo);
	chInfo.ch_uproc_offset = U_OFFSET;

	u.u_io.io_seg = IOSYS;
	u.u_io.io.vbase = &chInfo;
	u.u_io.io_ioc = sizeof(chInfo);
	u.u_io.io_flag = 0;

	sp->s_lrefc++;
	iwrite(ip, &u.u_io);
	sp->s_lrefc--;

	/*
	 * Added to aid in kernel debugging - if DUMP_TEXT is nonzero,
	 * dump the text segment (to see if it was corrupted) and set
	 * the dump flag so that postmortem utilities will know that
	 * text is present in the core file.
	 */
	if (DUMP_TEXT)
		u.u_segl[SISTEXT].sr_flag |= SRFDUMP;

	for (srp=u.u_segl; u.u_error==0 && srp<&u.u_segl[NUSEG]; srp++) {

		if ((srp->sr_flag & SRFDUMP)==0)
			continue;

		/* Don't try to dump empty segments. */
		if ((sp = srp->sr_segp)==NULL) {
			srp->sr_flag &= ~SRFDUMP;
			continue;
		}

		/* Don't dump segments too big to dump. */
		if (sp->s_size > DUMP_LIM)
			srp->sr_flag &= ~SRFDUMP;
	}

	for (srp=u.u_segl; u.u_error==0 && srp<&u.u_segl[NUSEG]; srp++) {

		/* Only dump segments flagged for dumping. */
		if ((srp->sr_flag & SRFDUMP)==0)
			continue;

		ssize = sp->s_size;
		u.u_io.io_seg = IOPHY;
		u.u_io.io.pbase = MAPIO(sp->s_vmem, 0);
		u.u_io.io_flag = 0;
		sp->s_lrefc++;
		while (u.u_error == 0 && ssize != 0) {
			n = ssize > SCHUNK ? SCHUNK : ssize;
			u.u_io.io_ioc = n;
			iwrite(ip, &u.u_io);
			u.u_io.io.pbase += n;
			ssize -= (paddr_t)n;
		}
		sp->s_lrefc--;
	}
	idetach(ip);
	return (u.u_error==0);
}

/*
 * Send a ptrace command to the child.
 *
 * "pid" is child pid.
 */
int
ptset(req, pid, addr, data)
unsigned req;
int *addr;
{
	register PROC *pp;

	lock(pnxgate);
	for (pp=procq.p_nforw; pp!=&procq; pp=pp->p_nforw)
		if (pp->p_pid == pid)
			break;
	unlock(pnxgate);
	if (pp==&procq || (pp->p_flags&PFSTOP)==0 || pp->p_ppid!=SELF->p_pid){
		u.u_error = ESRCH;
		return;
	}
	lock(pts.pt_gate);
	pts.pt_req = req;
	pts.pt_pid = pid;
	pts.pt_addr = addr;
	pts.pt_data = data;
	pts.pt_errs = 0;
	pts.pt_rval = 0;
	pts.pt_busy = 1;
	wakeup((char *)&pts.pt_req);
	while (pts.pt_busy) {
		x_sleep((char *)&pts.pt_busy, primed, slpriSigCatch, "ptrace");
		/* Send a ptrace command to the child.  */
	}
	u.u_error = pts.pt_errs;
	unlock(pts.pt_gate);
	return (pts.pt_rval);
}

/*
 * This routine is called when a child that is being traced receives a signal
 * that is not caught or ignored.  It follows up on any requests by the parent
 * and returns when done.
 *
 * After ptrace handling done in this routine, a real or simulated signal
 * may need to be sent to the traced process.
 * Return a signal number to be sent to the child process, or 0 if none.
 */
int
ptret()
{
	extern void (*ndpKfrstor)();
	register PROC *pp;
	register PROC *pp1;
	register int sign;
	unsigned off;
	int doEmUnpack = 0;

	struct _fpstate * fstp = empack();

	pp = SELF;
next:
	u.u_error = 0;
	if (pp->p_ppid == 1)
		return (SIGKILL);
	sign = -1;

	/* wake up parent if it is sleeping */
	lock(pnxgate);
	pp1 = &procq;
	for (;;) {
		if ((pp1=pp1->p_nforw) == &procq) {
			sign = SIGKILL;
			break;
		}
		if (pp1->p_pid != pp->p_ppid)
			continue;
		if (ASLEEP(pp1))
			wakeup((char *)pp1);
		break;
	}
	unlock(pnxgate);

	while (sign < 0) {
		/* If no pending ptrace transaction for this process, sleep. */
		if (pts.pt_busy==0 || pp->p_pid!=pts.pt_pid) {
			/* If a signal bit is set now, just exit - let
			 * actvsig() handle it next time through.
			 * Doing sleep and goto next will stick us in a loop */
			if (nondsig())
				return 0;
			x_sleep((char *)&pts.pt_req,
			  primed, slpriSigCatch, "ptret");
			goto next;
		}
		switch (pts.pt_req) {
		case PTRACE_RD_TXT:
			if (XMODE_286) {
				pts.pt_rval = getuwd(NBPS+pts.pt_addr);
				break;
			}
			/* Fall through for 386 mode processes. */
		case PTRACE_RD_DAT:
			pts.pt_rval = getuwd(pts.pt_addr);
			break;
		case PTRACE_RD_USR:
			/* See ptrace.h for valid offsets. */
			off = (unsigned)pts.pt_addr;
			if (off & 3)
				u.u_error = EINVAL;
			else if (off < PTRACE_FP_CW) {
				/* Reading CPU general register state */
				if (off == PTRACE_SIG)
					pts.pt_rval = u.u_signo;
				else
					pts.pt_rval = u.u_regl[off>>2];
			} else if (off < PTRACE_DR0) {
				/*
				 * Reading NDP state.
				 * If NDP state not already saved, save it.
				 * Fetch desired info.
				 * Restore NDP state in case we will resume.
				 */
				if (rdNdpUser()) {
					/* if using coprocessor */
					if (!rdNdpSaved()) {
						ndpSave(&u.u_ndpCon);
						wrNdpSaved(1);
					}
pts.pt_rval = ((int *)&u.u_ndpCon)[(off - PTRACE_FP_CW)>>2];
					ndpRestore(&u.u_ndpCon);
					wrNdpSaved(0);
				} else if (fstp) {
pts.pt_rval = getuwd(((int *)fstp) + ((off - PTRACE_FP_CW)>>2));
					/* if emulating */
				} else /* no ndp state to display */
					pts.pt_rval = 0;
			} else
				u.u_error = EINVAL;
			break;
		case PTRACE_WR_TXT:
			if (XMODE_286) {
				putuwd(NBPS+pts.pt_addr, pts.pt_data);
				break;
			}
			/* Fall through for 386 mode processes. */
		case PTRACE_WR_DAT:
			putuwd(pts.pt_addr, pts.pt_data);
			break;
		case PTRACE_WR_USR:
			/* See ptrace.h for valid offsets. */
			off = (unsigned)pts.pt_addr;

			if (off & 3)
				u.u_error = EINVAL;
			else if (off < PTRACE_FP_CW) {
				/* Writing CPU general register state */
				if (off == PTRACE_SIG)
					u.u_error = EINVAL;
				else
					u.u_regl[off>>2] = pts.pt_data;
			} else if (off < PTRACE_DR0) {
				if (rdNdpUser()) {
					/*
					 * Writing NDP state.
					 * If NDP state not already saved, save it.
					 * Store desired info.
					 * Restore NDP state in case we will resume.
					 */
					if (!rdNdpSaved()) {
						ndpSave(&u.u_ndpCon);
						wrNdpSaved(1);
					}
((int *)&u.u_ndpCon)[(off - PTRACE_FP_CW)>>2] = pts.pt_data;
					ndpRestore(&u.u_ndpCon);
					wrNdpSaved(0);
				} else if (fstp && ndpKfrstor) {
putuwd(((int *)fstp) + ((off - PTRACE_FP_CW)>>2), pts.pt_data);
					doEmUnpack = 1;
				}
			} else
				u.u_error = EINVAL;
			break;
		case PTRACE_RESUME:
			u.u_regl[EFL] &= ~MFTTB;
			goto sig;
		case PTRACE_TERM:
			sign = SIGKILL;
			break;
		case PTRACE_SSTEP:
			u.u_regl[EFL] |= MFTTB;
		sig:
			if (pts.pt_data<0 || pts.pt_data>NSIG) {
				u.u_error = EINVAL;
				break;
			}
			sign = pts.pt_data;
			if (pts.pt_addr != SIG_IGN) {
				u.u_regl[EIP] = (int)pts.pt_addr;
			}
			break;
		default:
			u.u_error = EINVAL;
		}
		if ((pts.pt_errs=u.u_error) == EFAULT)
			pts.pt_errs = EINVAL;
		pts.pt_busy = 0;
		wakeup((char *)&pts.pt_busy);
	}
	if (doEmUnpack)
		(*ndpKfrstor)(fstp, &u.u_ndpCon);
	return (sign);
}

/*
 * If using floating point emulator, make room on user stack and save
 * floating point context there.  Code elsewhere takes care of floating
 * point context if there is a coprocessor.
 *
 * Return the virtual address in user space of the context area, or
 * return NULL if not using FP emulation.
 */
static struct _fpstate *
empack()
{
	int uesp;
	int sphi, splo;
	SEG * segp;
	cseg_t * pp;
	struct _fpstate * ret = NULL;
	extern void (*ndpKfsave)();
	unsigned long sw_old;

	/* If not emulating, do nothing */
	if (rdNdpUser() || !rdEmTrapped() || !ndpKfsave)
		return NULL;

	/*
	 * Will copy at least u_sigreturn, _fpstackframe, and ndpFlags.
	 * If using ndp, need room for an _fpstate.
	 * If emulating, need room for an _fpemstate.
	 */
	uesp = u.u_regl[UESP] - sizeof(struct _fpstate);

	/* Add to user stack if necessary. */
	segp = u.u_segl[SISTACK].sr_segp;
	sphi = (XMODE_286) ? ISP_286 : ISP_386;
	splo = sphi - segp->s_size;

	if (splo > uesp) {
		pp = c_extend(segp->s_vmem, btoc(segp->s_size));
		if (pp==0) {
			printf("Empack failed.  cmd=%s  c_extend(%x,%x)=0 ",
			  u.u_comm, segp->s_vmem, btoc(segp->s_size));
			return NULL;
		}

		segp->s_vmem = pp;
		segp->s_size += NBPC;
		if (sproto(0)==0) {
			printf("Empack failed.  cmd=%s  sproto(0)=0 ",
			  u.u_comm);
			return NULL;
		}

		segload();
	}

	ret = (struct _fpstate *)uesp;
	(*ndpKfsave)(&u.u_ndpCon, uesp);
	sw_old = getuwd(&ret->sw);
	putuwd(&ret->status, sw_old);
	putuwd(&ret->sw, sw_old & 0x7f00);

	return ret;
}
