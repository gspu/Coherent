/* $Header: /src386/kernel/coh.386/RCS/pipe.c,v 1.9 93/04/16 06:49:10 bin Exp Locker: bin $ */
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
 * Revision 1.9  93/04/16  06:49:10  bin
 * Hal: kernel 76 update
 * 
 * Revision 1.7  93/04/14  10:06:40  root
 * r75
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
 *  These are nothing more than random different values at this point!
 *  Historically, these were bit's or'ed into ip->i_flag, no more!
 */

#define	IFWFR	1			/* Sleeping Waiting for a Reader */
#define	IFWFW	2			/* Sleeping Waiting for a Writer */


/*
 *  pmake(mode)  --  called from the upipe() system call in sys3.c
 *
 *  Creates and returns a locked pipe inode with the given mode on
 *  the pipedev.
 */

INODE *
pmake(mode)
{
	register INODE *ip;

	if ((ip=ialloc(pipedev, IFPIPE|mode)) != NULL)
		pclear(ip);
	pdump("M", ip, mode);
	return(ip);
}

pclear(ip)
register INODE *ip;
{
	ip->i_pnc =
	ip->i_prx =
	ip->i_pwx =
	ip->i_par =
	ip->i_paw =
	ip->i_psr =
	ip->i_psw = 0;
	ip->i_iev.e_pnext =
	ip->i_iev.e_dnext =
	ip->i_iev.e_dlast =
	ip->i_iev.e_procp =
	ip->i_oev.e_pnext =
	ip->i_oev.e_dnext =
	ip->i_oev.e_dlast =
	ip->i_oev.e_procp = NULL;
}

/*
 *  popen(ip, mode)  --  Opens a pipe inode, with the given mode.
 *			 Note:  The inode is locked upon entry.
 *
 *  This routine follows the requirements concerning opening pipes.
 *  Specifically, if opening readonly without O_NDELAY, then block
 *  until we have a writer.  If opening readonly with O_NDELAY, then
 *  return opened, no blocking.  If opening writeonly without O_NDELAY,
 *  then block until we have a reader.  If opening writeonly with
 *  O_NDELAY, then return an error, and set u.u_errno to ENXIO.
 *  Beware of subtle race conditions!  Also notice, I followed hal's
 *  style of no internal returns in a function.
 *
 *  Note: these pipe routines maintain the pipe counter variables:
 *	  ip->i_par:  Number of Awake readers
 *	  ip->i_paw:  Number of Awake writers
 *	  ip->i_psr:  Number of Sleeping readers
 *	  ip->i_psw:  Number of Sleeping writers
 */

popen(ip, mode)
register INODE *ip;
{
	pdump("OA", ip, mode);
	switch ( mode&(IPR|IPW) ) {
	case IPR:
		++ip->i_par;
		while ( !ip->i_paw && !ip->i_psw ) {
			if ( mode & IPNDLY )
				break;
			else {
				if ( psleep(ip, IFWFW) < 0 ) {
					--ip->i_par;
					goto popen_done;
				}
				if ( ip->i_pnc != 0 )
					break;
			}
		}
		pwake(ip, IFWFR);
		break;
	case IPW:
		++ip->i_paw;
		if ( !ip->i_par && !ip->i_psr ) {
			if ( mode & IPNDLY ) {
				u.u_error = ENXIO;
				--ip->i_paw;
				goto popen_done;
			} else {
				if ( psleep(ip, IFWFR) < 0 ) {
					--ip->i_paw;
					goto popen_done;
				}
			}
		}
		pwake(ip, IFWFW);
		break;
	case IPR|IPW:
		++ip->i_par;
		++ip->i_paw;
		pwake(ip, IFWFW);
		pwake(ip, IFWFR);
		break;
	}

popen_done:
	pdump("OZ", ip, mode);
	return;
}


/*
 *  pclose(ip, mode)  --  Opens a pipe inode, with the given mode.
 *			  Note:  The inode is locked upon entry.
 *
 *  This routine closes the given INODE with the given mode.  We
 *  must have the mode correct to maintain counters properly.
 *  Good thing that mode cannot be changed by fcntl()!
 */

pclose(ip, mode)
register INODE *ip;
{
	pdump("CA", ip, mode);
	pwake(ip, IFWFR);
	pwake(ip, IFWFW);
	if ( mode & IPR )
		if ( --ip->i_par < 0 )
			panic("Out of sync IPR in pclose");
	if ( mode & IPW )
		if ( --ip->i_paw < 0 )
			panic("Out of sync IPW in pclose");

	if ( !ip->i_paw && !ip->i_psw && !ip->i_par && !ip->i_psr )
		pclear(ip);
	pdump("CZ", ip, mode);
}


/*
 *  pread(ip, iop)  --  Reads from a pipe inode, accoring to the IO info.
 *			Note:  The inode is locked upon entry.
 *
 *  This routine follows the requirements concerning reading from pipes.
 *  Specifically, if there is no data in the pipe, then the read will
 *  block waiting for data, unless you have IONDLY set in which case
 *  it will simply return zero.  Notice, the traditional value returned
 *  from uread() is the number of characters actually read.  This is
 *  nothing more that iop->io_ioc on entry minus iop->io_ioc on exit.
 *  This routine also works with the ring buffer in the inode maintained
 *  by the variables ip->i_pnc:  Number of Characters in pipe.
 *		     ip->i_prx:  Offset in pipe to begin reading.
 *		     ip->i_pwx:  Offset in pipe to begin writing.
 *  Notice: we do not unlock the inode when we call fread(), this is to
 *  guarantee that we read all that is available even if we go to sleep.
 *  Subtle race condition?  I don't think so, since if we go to sleep
 *  in fread(), it's wrt a resource unrelated to this particular INODE.
 */

pread(ip, iop)
register INODE *ip;
register IO *iop;
{
	register unsigned n;
	register unsigned ioc;

	pdump("R", ip, 0);
	while (ip->i_pnc == 0) {
		if ( iop->io_flag & IONDLY )
			goto pread_done;
		if ( !ip->i_paw && !ip->i_psw )
			goto pread_done;
		if ( psleep(ip, IFWFW) < 0 )
			goto pread_done;
	}

	ioc = iop->io_ioc;
	while ( !u.u_error && (ioc > 0) && (ip->i_pnc > 0) ) {
		if ( (n = (PIPSIZE-ip->i_prx)) > ioc )
			n = ioc;
		if ( n > ip->i_pnc )
			n = ip->i_pnc;
		iop->io_ioc = n;
		iop->io_seek = ip->i_prx;
		fread(ip, iop);
		n -= iop->io_ioc;
		if ( (ip->i_prx+=n) == PIPSIZE )
			ip->i_prx = 0;
		if ( (ip->i_pnc-=n) == 0 ) {
			ip->i_prx =
			ip->i_pwx = 0;
		}
		ioc -= n;
	}
	iop->io_ioc = ioc;

	if ( ip->i_pnc < PIPSIZE )
		pwake(ip, IFWFR);

pread_done:
	return;
}


/*
 *  pwrite(ip, iop)  --  Writes to a pipe inode, accoring to the IO info.
 *			 Note:  The inode is locked upon entry.
 *
 *  This routine follows the requirements concerning writing to pipes.
 *  Specifically, if the pipe is full, then the write will block waiting
 *  for data to be consumed, unless you have IONDLY set in which case
 *  it will simply return zero.  Notice, the traditional value returned
 *  from uwrite() is the number of characters actually written.  This is
 *  nothing more that iop->io_ioc on entry minus iop->io_ioc on exit.
 *  In other words, iop->io_ioc had better be zero on exit.  The possibility
 *  does exist if the number of characters to be written is larger than
 *  PIPSIZE, and thus we do not guarantee atomic writes, that while the
 *  process is sleeping waiting for a reader to consume data, that the
 *  process will be woken from sleeping by a SIGNAL, thus causing a partial
 *  write.  The return value will be the actual number of character written.
 *  This routine also works with the ring buffer in the inode maintained
 *  by the variables ip->i_pnc:  Number of Characters in pipe.
 *		     ip->i_prx:  Offset in pipe to begin reading.
 *		     ip->i_pwx:  Offset in pipe to begin writing.
 *  Notice: we do not unlock the inode when we call fwrite(), this is to
 *  guarantee that we have an atomic write for all writes of size less
 *  than PIPSIZE, even if we go to sleep in the fwrite().  Subtle race
 *  condition?  I don't think so, since if we go to sleep in fwrite(),
 *  it's wrt a resource unrelated to this particular INODE.
 */

pwrite(ip, iop)
register INODE *ip;
register IO *iop;
{
	register unsigned n;
	register unsigned ioc;

	pdump("W", ip, 0);
	ioc = iop->io_ioc;
	while ( !u.u_error && (ioc > 0) ) {
		if ( !ip->i_par && !ip->i_psr ) {
			u.u_error = EPIPE;
			sendsig(SIGPIPE, SELF);
			goto pwrite_done;
		}
		if ( (n = (PIPSIZE-ip->i_pwx)) > ioc )
			n = ioc;
		if ( n > (PIPSIZE-ip->i_pnc) )
			n = PIPSIZE - ip->i_pnc;
		if ( (n == 0) || ((ioc <= PIPSIZE) && (n != ioc)) ) {
			if ( iop->io_flag & IONDLY )
				goto pwrite_done;
			if ( psleep(ip, IFWFR) < 0 )
				goto pwrite_done;
			continue;
		}
		iop->io_ioc = n;
		iop->io_seek = ip->i_pwx;
		fwrite(ip, iop);
		n -= iop->io_ioc;
		if ( (ip->i_pwx+=n) == PIPSIZE )
			ip->i_pwx = 0;
		ip->i_pnc += n;
		ioc -= n;

		if ( ip->i_pnc > 0 )
			pwake(ip, IFWFW);
	}
pwrite_done:
	iop->io_ioc = ioc;
}


/*
 *  psleep(ip, who)  --  go to sleep either waiting for a reader if (who==IFWFR)
 *		         or waiting for a writer if (who==IFWFW).
 *  Returns:  0  if woke up ok
 *	     -1  if woke up by signal (e.g. SIGALRM, SIGKILL, etc.)
 */

psleep(ip, who)
register INODE *ip;
{
	pdump("SA", ip, 0);
	iunlock(ip);
	switch ( who ) {
	case IFWFW:
		--ip->i_par;  ++ip->i_psr;
		x_sleep((char *)&ip->i_psw, primed, slpriSigCatch, "pipe wx");
		++ip->i_par;  --ip->i_psr;
		break;
	case IFWFR:
		--ip->i_paw;  ++ip->i_psw;
		x_sleep((char *)&ip->i_psr, primed, slpriSigCatch, "pipe rx");
		++ip->i_paw;  --ip->i_psw;
		break;
	default:
		panic("psleep() internal error");
	}
	ilock(ip);
	pdump("SZ", ip, 0);
	if ( SELF->p_ssig && nondsig() ) {
		u.u_error = EINTR;
		return -1;
	}
	return(0);
}


/*
 *  pwake(ip, who)  --  wake up processes which are waiting for a reader if
 *		        (who==IFWFR) or waiting for a writer if (who==IFWFW).
 */

pwake(ip, who)
register INODE *ip;
{
	pdump("KA", ip, 0);
	switch ( who ) {
	case IFWFW:
		if ( ip->i_psr )
			wakeup((char *)&ip->i_psw);
		if ( ip->i_pnc > 0 )
			pollwake(&ip->i_iev);
		break;
	case IFWFR:
		if ( ip->i_psw )
			wakeup((char *)&ip->i_psr);
		if ( (ip->i_pnc<PIPSIZE) && (ip->i_par || ip->i_psr) )
			pollwake(&ip->i_oev);
		break;
	default:
		panic("pwake() internal error");
	}
	pdump("KZ", ip, 0);
}


/*
 *  ppoll(ip, ev)  --  Poll the given pipe inode.
 *  INODE *ip  --  The inode in question.
 *  int ev     --  The event bit field.
 *  int msec   --  Number of msecs to wait.
 *  Returns or'ed bits according to the following rules:
 *  POLLIN:  indicates input is available for reading, notice it is possible
 *	     to read even if there are no more writers anywhere!
 *  POLLOUT: indicates room in pipe for new output, notice it is not possible
 *	     to write unless there is a reader attached!
 *
 *  No priority polls are supported.
 */

ppoll(ip, ev, msec)
register INODE *ip;
int ev, msec;
{
	register int rval = 0;

	if ( ev & POLLIN ) {
		if ( ip->i_pnc > 0 )
			rval |= POLLIN;
		else if ( msec != 0 )
			pollopen(&ip->i_iev);
	}
	if ( ev & POLLOUT ) {
		if ( (ip->i_pnc<PIPSIZE) && (ip->i_par || ip->i_psr) )
			rval |= POLLOUT;
		else if ( msec != 0 )
			pollopen(&ip->i_oev);
	}
	return( rval );
}

/*
 *  pdump(loc, ip, mode)  --  A kernel debugging output line.
 *  char *loc  --  prefix of line (two characters indicating where we are)
 *  INODE *ip  --  The inode information to dump
 *  int mode   --  The mode of the IO call, i.e. IPW, IPR, IPNDLY, ...
 */

#if 1
pdump()
{}
#else
pdump(loc, ip, mode)
char *loc;
register INODE *ip;
int mode;
{
	printf("%s ip=%x mde=%x nlk=%x rf=%x nc=%x rx=%x wx=%x",
		loc, ip, mode, ip->i_nlink, ip->i_refc,
		ip->i_pnc, ip->i_prx, ip->i_pwx);

	printf(" ar=%x aw=%x sr=%x sw=%x f=%x\n",
		ip->i_par, ip->i_paw, ip->i_psr, ip->i_psw, ip->i_flag);
}
#endif
