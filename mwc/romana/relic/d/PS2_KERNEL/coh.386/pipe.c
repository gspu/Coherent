/* $Header: /kernel/kersrc/coh.386/RCS/pipe.c,v 1.2 92/08/04 12:33:59 bin Exp Locker: bin $ */
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
 * Pipes.
 *
 * $Log:	pipe.c,v $
 * Revision 1.2  92/08/04  12:33:59  bin
 * changed for ker 59
 * 
 * Revision 1.2  92/01/06  11:59:52  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:14:07	src
 * Initial revision
 * 
 * 86/11/19	Allan Cornish		/usr/src/sys/coh/pipe.c
 * Added check for non-blocking read and write if (io_flag & IPNDLY) set.
 * Eliminated use of i_a inode field since now included in inode macros.
 */
#include <sys/coherent.h>
#include <errno.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <signal.h>

/*
 * Create and return a locked pipe inode.  This is called from the
 * pipe system call.
 */
INODE *
pmake(mode)
{
	register INODE *ip;

	if ((ip=ialloc(pipedev, IFPIPE|mode)) != NULL) {
		iclear(ip);
		ip->i_pnc = 0;
		ip->i_prx = 0;
		ip->i_pwx = 0;
	}
	return (ip);
}

/*
 * Open a pipe given the inode pointer.
 */
popen(ip, mode)
{
	T_HAL(0x10, printf("popen(%x,%x) ", ip, mode));
}

/*
 * Close a pipe inode.
 */
pclose(ip)
register INODE *ip;
{
	if (ip->i_refc == 2) {
		pevent(ip);
		ip->i_flag |= IFEOF;
	}
}

/*
 * Only one end of the pipe is going to be left.
 */
pevent(ip)
register INODE *ip;
{
	if ((ip->i_flag&IFWFR) != 0) {
		ip->i_flag &= ~IFWFR;
		wakeup((char *)&ip->i_pwx);
	}
	if ((ip->i_flag&IFWFW) != 0) {
		ip->i_flag &= ~IFWFW;
		wakeup((char *)&ip->i_prx);
	}
}

/*
 * Read from a pipe.  The given inode is locked.
 */
pread(ip, iop)
register INODE *ip;
register IO *iop;
{
	register unsigned n;
	register unsigned ioc;
#ifdef TRACER
	int old_ioc = iop->io_ioc;
#endif /* TRACER */

	while (ip->i_pnc == 0) {

		/*
		 * Logical End of File.
		 */
		if ((ip->i_flag&IFEOF) != 0) {
			ip->i_flag &= ~IFEOF;
			break;
		}

		/*
		 * Nobody left to write.
		 */
		if (ip->i_nlink==0 && ip->i_refc<2)
			break;

		/*
		 * Non-blocking read.
		 */
		if ( iop->io_flag & IONDLY ) {
			u.u_error = EAGAIN;
			goto pread_done;
		}

		/*
		 * Wait for pipe data.
		 */
		ip->i_flag |= IFWFW;
		iunlock(ip);
		v_sleep((char *)&ip->i_prx, CVPIPE, IVPIPE, SVPIPE, "pipe data");
		/* Wait for pipe data.  */
		ilock(ip);
	}

	/*
	 * Clear EOF flag.
	 */
	if ((ip->i_flag&IFEOF)!=0 && ip->i_pnc==0)
		ip->i_flag &= ~IFEOF;

	ioc = iop->io_ioc;
	while (u.u_error==0 && ioc>0 && ip->i_pnc>0) {

		/*
		 * Calculate length of data to be read.
		 */
		if ((n=PIPSIZE-ip->i_prx) > ioc)
			n = ioc;
		if (n > ip->i_pnc)
			n = ip->i_pnc;

		/*
		 * Read data.
		 */
		iop->io_ioc = n;
		iop->io_seek = ip->i_prx;
		fread(ip, iop);
		n -= iop->io_ioc;
		if ((ip->i_prx+=n) == PIPSIZE)
			ip->i_prx = 0;
		ip->i_pnc -= n;
		ioc -= n;
	}
	iop->io_ioc = ioc;

	/*
	 * Wake processes waiting to write.
	 */
	if ((ip->i_flag&IFWFR)!=0 && ip->i_pnc<PIPSIZE) {
		ip->i_flag &= ~IFWFR;
		wakeup((char *)&ip->i_pwx);
	}

pread_done:
	T_HAL(0x10, printf("pread:%d ", old_ioc - iop->io_ioc));
	return;
}

/*
 * Write to a pipe.  The given inode is locked.
 */
pwrite(ip, iop)
register INODE *ip;
register IO *iop;
{
	register unsigned n;
	register unsigned ioc;
#ifdef TRACER
	int old_ioc = iop->io_ioc;
#endif /* TRACER */

	ioc = iop->io_ioc;
	while (u.u_error==0 && ioc>0) {

		/*
		 * Nobody left to read.
		 */
		if ( (ip->i_refc < 2) && (ip->i_nlink == 0) ) {
			u.u_error = EPIPE;
			sendsig(SIGPIPE, SELF);
			goto pwrite_done;
		}

		/*
		 * Calculate free space in pipe.
		 */
		if ( (n=PIPSIZE-ip->i_pwx) > ioc )
			n = ioc;
		if (n > PIPSIZE-ip->i_pnc)
			n = PIPSIZE - ip->i_pnc;

		/*
		 * Non-blocking write.
		 */
		if ( iop->io_flag & IONDLY ) {
			if ( (n != ioc) || (ip->i_flag & IFEOF) ) {
				u.u_error = EAGAIN;
				goto pwrite_done;
			}
		}

		/*
		 * Insufficent space or EOF still pending.
		 */
		if (n==0 || (ip->i_flag&IFEOF)!=0) {
			ip->i_flag |= IFWFR;
			iunlock(ip);
			v_sleep((char *)&ip->i_pwx, CVPIPE, IVPIPE, SVPIPE, "pwrite");
			/* Insufficent space or EOF still pending.  */
			ilock(ip);
			continue;
		}
		iop->io_ioc = n;
		iop->io_seek = ip->i_pwx;
		fwrite(ip, iop);
		n -= iop->io_ioc;
		if ((ip->i_pwx+=n) == PIPSIZE)
			ip->i_pwx = 0;
		ip->i_pnc += n;
		ioc -= n;

		/*
		 * Wait processes waiting to read.
		 */
		if ((ip->i_flag&IFWFW) && ip->i_pnc>0) {
			ip->i_flag &= ~IFWFW;
			wakeup((char *)&ip->i_prx);
		}
	}
	iop->io_ioc = ioc;

pwrite_done:
	T_HAL(0x10, printf("pwrite:%d ", old_ioc - iop->io_ioc));
	return;
}
