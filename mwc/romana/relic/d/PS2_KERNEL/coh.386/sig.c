/* $Header: /kernel/kersrc/coh.386/RCS/sig.c,v 1.2 92/08/04 12:34:36 bin Exp Locker: bin $ */
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
 * Signal handling.
 *
 * $Log:	sig.c,v $
 * Revision 1.2  92/08/04  12:34:36  bin
 * changed for ker 59
 * 
 * Revision 1.2  92/01/06  12:00:24  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:14:24	src
 * Initial revision
 * 
 * 87/11/05	Allan Cornish		/usr/src/sys/coh/sig.c
 * New seg struct now used to allow extended addressing.
 *
 * 86/11/19	Allan Cornish		/usr/src/sys/coh/sig.c
 * sigdump() initializes the (new) (IO).io_flag field to 0.
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

/*
 * Set up the action to be taken on a signal.
 */
usigsys(signal, func)
int	signal;
register void (*func)();
{
	register PROC *pp;
	register sig_t s;
	register int (*old_sig)();
	int	sigtype;

	sigtype = signal & ~0xFF;
	signal ^= sigtype;

	pp = SELF;
	if (signal<=0 || signal>NSIG || signal==SIGKILL) {
		u.u_error = EINVAL;
		return;
	}

	/*
	 * In order to avoid runaway, don't allow user to set
	 * handler for SIGSEGV to anything but SIG_DFL.
	 *
	 * We should do something more sophisticated, like detecting
	 * two SEGV's in a row and then killing the process.
	 */
	if (signal == SIGSEGV && func != SIG_DFL) {
		u.u_error = EINVAL;
		return;
	}

	if (sigtype==SIGDEFER || sigtype==0) {
		if (func==SIG_IGN)
			sigtype = SIGIGNORE;
		if (func==SIG_HOLD)
			sigtype = SIGHOLD;
	}

	s = (sig_t)1 << --signal;
	if (pp->p_isig&s)
		old_sig = SIG_IGN;
	else if (pp->p_hsig&s)
		old_sig = SIG_HOLD;
	else
		old_sig = u.u_sfunc[signal];

	switch (sigtype) {
	case SIGHOLD:
		pp->p_hsig |= s;
		break;
	case SIGRELSE:
		pp->p_hsig &= ~s;
		if (nondsig()) {
			T_PIGGY( 0x100, printf("a(s)"); );
			actvsig();
		}
	case SIGIGNORE:
		pp->p_dfsig &= ~s;	/* No longer defaulted.  */
		pp->p_isig |= s;	/* Mark signal as ignored.  */
		pp->p_ssig &= ~s;	/* Turn off any pending signal.  */
		break;
	case 0:				/* old system entry */
	case SIGDEFER:			/* new system entry */
		u.u_sigreturn = (void (*)())u.u_regl[EDX];
		u.u_sfunc[signal] = func;
		/*
		 * Be sure to mark the signal as defaulted or not.
		 */
		if (SIG_DFL == func) {
			pp->p_dfsig |= s;
		} else {
			pp->p_dfsig &= ~s;
		}
		/*
		 * The signal is no longer ignored or held, and
		 * any pending signal is lost.
		 */
		pp->p_isig &= ~s;
		pp->p_hsig &= ~s;
		pp->p_ssig &= ~s;
		if (sigtype==SIGDEFER)
			pp->p_dsig |= s;
		else
			pp->p_dsig &= ~s;
		break;
	/* SIGPAUSE not done yet */
	default:
		u.u_error = SIGSYS;
		break;
	}
	return old_sig;
}


/*
 * Send a signal to the process `pp'.
 */
sendsig(sig, pp)
register unsigned sig;
register PROC *pp;
{
	register sig_t f;
	register int s;


	T_PIGGY( 0x40000000,
	    printf("<send sig: %d, id: %d, state: %x, flags: %x, event: %x>",
		   sig, pp->p_pid, pp->p_state, pp->p_flags, pp->p_event);
	); /* T_PIGGY() */

	/*
	 * Convert the signal to a bit position.
	 */
	f = ((sig_t)1) << (sig-1);

	/*
	 * If the signal is ignored, do nothing.
	 */
	if (pp->p_isig&f) {
		return;
	}

	/*
	 * This bit causes SIGCHLD to be ignored here in sendsig().
	 */
	T_PIGGY( 0x10000000, {
		if (SIGCHLD == sig) {
			printf("SIGCHLD ignored, ");
			return;
		}
	}
	);

	/*
	 * I do not understand delayed or held signals.
	 */
	if ((pp->p_ssig & f) && (pp->p_hsig|pp->p_dsig) & f)
		return;
	
	/*
	 * Acutally send the signal by flagging the needed bit.
	 */
	pp->p_ssig |= f;

	/*
	 * If the process is sleeping, wake it up so that
	 * it can process this signal.
	 */
	if (pp->p_state == PSSLEEP) {
		s = sphi();
		pp->p_lback->p_lforw = pp->p_lforw;
		pp->p_lforw->p_lback = pp->p_lback;
		addu(pp->p_cval, (utimer-pp->p_lctim)*CVCLOCK);
		setrun(pp);
		spl(s);
	}
}

/*
 * Return signal number if we have a non ignored or delayed signal, else zero.
 */
nondsig()
{
	register PROC *pp;
	register sig_t mask;
	register int signo;

	pp = SELF;
	signo = 0;
	/*
	 * Turn off all ignored signals.
	 */
	pp->p_ssig &= ~pp->p_isig;
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
	return (signo);
}

/*
 * If we have a signal that isn't ignored, activate it.
 */
actvsig()
{
	register int n;
	register PROC *pp;
	register int (*func)();
	sig_t s;


	/*
	 * Fetch an unprocessed signal.
	 * Return if there are none.
	 */
	if ((n = nondsig()) == 0)
		return;

	T_PIGGY( 0x40000, {
		if (SIGCHLD == n) {
			printf("-");
			return;
		}
	} );

	pp = SELF;

	/*
	 * Reset the signal to indicate that it has been processed.
	 */
	--n;
	pp->p_ssig &= ~((sig_t)1<<n);

	/*
	 * Fetch the user function that goes with this signal.
	 */
	func = u.u_sfunc[n];

	/*
	 * Store the signal number in the u area.  This is how
	 * a core dump records the death signal.
	 */
	u.u_signo = ++n;

	/*
	 * If the signal is not defaulted, go run the requested
	 * function.
	 */
	if (func != SIG_DFL) {
		if (XMODE_286)
			oldsigstart(n, func);
		else
			msigstart(n, func);
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
	 * Do something special for traced processes.  (?)
	 */
	if (pp->p_flags&PFTRAC) {
		pp->p_flags |= PFWAIT;
		n = ptret();
		pp->p_flags &= ~(PFWAIT|PFSTOP);
		if (n == 0)
			return;
	}

	/*
	 * Some signals cause a core file to be written.
	 */
	switch(n) {
	case SIGQUIT:
	case SIGILL:
	case SIGTRAP:
	case SIGABRT:
	case SIGFPE:
	case SIGSEGV:
	case SIGSYS:
		if (sigdump())
			n |= 0200;
		break;
	}
	pexit(n);
}

/*
 * Create a dump of ourselves onto the file `core'.
 */
sigdump()
{
	register INODE *ip;
	register SR *srp;
	register SEG * sp;
	register int n;
	register paddr_t ssize;
	extern	int	DUMP_LIM;

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
	for (srp=u.u_segl; u.u_error==0 && srp<&u.u_segl[NUSEG]; srp++) {
		if ((sp = srp->sr_segp)==NULL || (srp->sr_flag&SRFDUMP)==0)
			continue;
		u.u_io.io_seg = IOPHY;
		u.u_io.io.pbase = MAPIO(sp->s_vmem, 0);
		u.u_io.io_flag = 0;
		ssize = sp->s_size;
		if (ssize > DUMP_LIM) {
			printf("seg %d truncated from %d to %d bytes\n",
			  srp-u.u_segl, ssize, DUMP_LIM);
			ssize = DUMP_LIM;
		}
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
 */
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
		v_sleep((char *)&pts.pt_busy, CVPTSET, IVPTSET, SVPTSET, "ptrace");
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
 */
ptret()
{
	register PROC *pp;
	register PROC *pp1;
	register int sign;
	unsigned off;

	pp = SELF;
next:
	u.u_error = 0;
	if (pp->p_ppid == 1)
		return (SIGKILL);
	sign = -1;
	lock(pnxgate);
	pp1 = &procq;
	for (;;) {
		if ((pp1=pp1->p_nforw) == &procq) {
			sign = SIGKILL;
			break;
		}
		if (pp1->p_pid != pp->p_ppid)
			continue;
		if (pp1->p_state == PSSLEEP)
			wakeup((char *)pp1);
		break;
	}
	unlock(pnxgate);
	while (sign < 0) {
		if (pts.pt_busy==0 || pp->p_pid!=pts.pt_pid) {
			v_sleep((char *)&pts.pt_req, CVPTRET, IVPTRET, SVPTRET, "ptret");
			/* Something about signals to a traced child.  */
			goto next;
		}
		switch (pts.pt_req) {
		case 1:
			if (XMODE_286) {
				pts.pt_rval = getuwd(NBPS+pts.pt_addr);
				break;
			}
		case 2:
			pts.pt_rval = getuwd(pts.pt_addr);
			break;
		case 3:
			off = (unsigned)pts.pt_addr;
			if (off < UPASIZE)
				pts.pt_rval = *(int *)((char *)&u+off);
			else
				u.u_error = EINVAL;
			break;
		case 4:
			if (XMODE_286) {
				putuwd(NBPS+pts.pt_addr, pts.pt_data);
				break;
			}
		case 5:
			putuwd(pts.pt_addr, pts.pt_data);
			break;
		case 6:
			if (msetuof(pts.pt_addr, pts.pt_data) == 0)
				u.u_error = EINVAL;
			break;
		case 7:
			goto sig;
		case 8:
			sign = SIGKILL;
			break;
		case 9:
			msigsin();
		sig:
			if (pts.pt_data<0 || pts.pt_data>NSIG) {
				u.u_error = EINVAL;
				break;
			}
			sign = pts.pt_data;
			if (pts.pt_addr != SIG_IGN)
				msetppc((vaddr_t)pts.pt_addr);
			break;
		default:
			u.u_error = EINVAL;
		}
		if ((pts.pt_errs=u.u_error) == EFAULT)
			pts.pt_errs = EINVAL;
		pts.pt_busy = 0;
		wakeup((char *)&pts.pt_busy);
	}
	return (sign);
}
