/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	Intel 386 port and extensions (16/32 bit compatibility)
 *	Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991
 -lgl)
 */

/*
 * i386/sys1632.c
 *
 * This file contains the code for those system calls whose implementation
 * must vary, according to system call arguments size (16 or 32 bits)
 *
 * exec: argv[], envp[] pointers (ingoing and outgoing)
 * istat:alignment of longs (called by ustat, ufstat in [sys?.c])
 * ftime:alignment of longs
 * lseek:argument is a long pointer
 * dup, dup2: old implementation
 * 
 * Revised: Fri Jun 11 06:36:06 1993 CDT
 */
#include <sys/coherent.h>
#include <sys/acct.h>
#include <sys/buf.h>
#include <canon.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <l.out.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/systab.h>
#include <sys/oldstat.h>
#include <sys/timeb.h>
#include <sys/fd.h>

/*
 * emulate a 16 bit system call
 * called from trap.c
 */

char cvtsig[] = 
{
	0,
	SIGHUP, SIGINT, SIGQUIT, SIGALRM, SIGTERM, SIGPWR, 
	SIGSYS, SIGPIPE, SIGKILL, SIGTRAP, SIGSEGV,
	SIGEMT, /* SIGDIVE */
	SIGEMT, /* SIGOVFL */
	SIGUSR1,
	SIGUSR2,
	SIGUSR2,
	-1
};

int	ostat();
int	ofstat();
int	oftime();
int	upgrp();
int	ugetuid();
int	ugetgid();
int	usysi86();
int	ulock();
int	ufcntl();
int	uexece();
int	fddup();
int	obrk();

static long ualarm2();
static long utick();

int
oldsys()
{
	register struct	systab	*stp;
	register int syscall, callnum, nargs;
	int	l;
	int	(*func)();
	int	swap, res;
	int	temp;

	u.u_error = 0;
	syscall = getuwd(NBPS+u.u_regl[EIP]-sizeof(short));
	if (u.u_error || (syscall&0xFF) != 0xCD) 
		return SIGSYS;
	callnum = (syscall>>8) & 0x7F;
	/* Print out this 286 call number only if tracing is on.  */
	T_PIGGY(0x2, printf("[%d]", callnum););

	stp = &sysitab[callnum];
	if (callnum >= NMICALL)
		return SIGSYS;
	u.u_io.io_seg = IOUSR;
	if (envsave(&u.u_sigenv)) {
		u.u_error = EINTR;
		goto done;
	}

	func = stp->s_func;
	swap = 0;
	nargs = stp->s_nargs;

	for (l=0; l<nargs; l++)
		u.u_args[l] = (unsigned short)
			getuwd(u.u_regl[UESP]+(l+1)*sizeof(short));

	switch (callnum) {
	case 7:
		nargs = 1;
		goto update;
	case 17:		/* brk(0)  was used in old Coherent */
		func = obrk;
		break;	
	case 18:		/* stat and fstat have 32 bit alignment now */
		func = ostat;
		break;
	case 25:		/* ustime() 386 takes a value, not a ptr */
		u.u_args[0] = getuwd(u.u_args[0]);
		break;
	case 28:
		func = ofstat;
		break;
	case 35:		/* ftime system call has gone away */
		func = oftime;
		nargs = 1;
		goto update;
	case 41:		/* kludge second argument for dup2() */
		nargs = 2;
		func = fddup;
		goto update;
	case 42:		/* pipe - store thru pointer */
		nargs = 1;
		goto update;
	case 72:		/* ualarm2 and utick have disappeared */
		func = ualarm2;
		nargs = 1;
		goto update;
	case 73:
		func = utick;
		nargs = 1;
		goto update;
	case 62:	/* setpgrp */
		u.u_args[0] = 1;
		nargs = 1;
		func = upgrp;
		break;
	case 63:	/* getpgrp */
		u.u_args[0] = 0;
		nargs = 1;
		func = upgrp;
		break;
	case 24:		/* getuid and geteuid are together now */
	case 57:
		swap = callnum==57;
		func = ugetuid;
		break;
	case 47:
	case 56:		/* getgid & getegid are together now */
		swap = callnum==56;
		func = ugetgid;
		break;
	case 45:		/* unique is a sys-86 call now */
		func = usysi86;
		u.u_args[0] = SYI86UNEEK;
		nargs = 1;
		break;
	case 37:		/* kill - signal#'s have changed */
		u.u_args[0] = (signed short) u.u_args[0]; /* Sign extend pid. */
		u.u_args[1] = cvtsig[u.u_args[1]];
		break;
	case 48:		/* signal - signal#'s have changed */
		u.u_args[0] = cvtsig[u.u_args[0]];
		break;	
	case 53:		/* ulock has moved */	
		func = ulock;
		nargs = 1;
		goto update;
	case 66:		/* fcntl has moved */	
		func = ufcntl;
		nargs = 3;
		goto update;
	case 11:		/* exec has only one entry point now */
		func = uexece;
		nargs = 3;
		goto update;
	case 19:		/* seek offset is 32 bits now ; shift */
		u.u_args[1] |= u.u_args[2]<<16;
		u.u_args[2] = (unsigned short)
			getuwd(u.u_regl[UESP]+4*sizeof(short));
		break;
	update:
		for (l=0; l<nargs; l++)
			u.u_args[l] = (unsigned short)
				getuwd(u.u_regl[UESP]+(l+1)*sizeof(short));
		break;
	}

	if (u.u_error)
		return SIGSYS;

	res = (*func)(u.u_args[0], u.u_args[1], u.u_args[2], u.u_args[3],
		u.u_args[4], u.u_args[5]);
	if (swap)
		res = u.u_rval2;

	switch (callnum) {
	case 7:			/* wait - must store u_rval2 thru pointer */
		if (u.u_args[0]) {
			putubd(u.u_args[0], u.u_rval2);
			putubd(u.u_args[0]+1, u.u_rval2>>8);
		}
		break;
	case 19:		/* lseek - upper 16 bits of result in dx */
		u.u_rval2 = res >> 16;
		break;
	case 42:		/* pipe - store thru pointer */
		putubd(u.u_args[0],   res);
		putubd(u.u_args[0]+1, res>>8);
		putubd(u.u_args[0]+2, u.u_rval2);
		putubd(u.u_args[0]+3, u.u_rval2>>8);
		res = 0;
		break;
	default:
			/* msgsys, shmsys, and semsys are not emulated */
			/* poll is not emulated;NOTE:the code calls putuwd */
		;
	}
	u.u_regl[EAX] = res;
	u.u_regl[EDX] = u.u_rval2;
done:
	if (u.u_error) {
		u.u_regl[EAX] = u.u_regl[EDX] = -1;
		putubd(MUERR, u.u_error);
		if (u.u_error == EFAULT)
			return SIGSYS;
	}
	return 0;
}

/*
 * Given a file descriptor, return a status structure.
 */
ofstat(fd, stp)
struct oldstat *stp;
{
	register INODE *ip;
	register FD *fdp;
	struct oldstat stat;

	if ((fdp=fdget(fd)) == NULL)
		return;
	ip = fdp->f_ip;
	oistat(ip, &stat);
	kucopy(&stat, stp, sizeof(stat));
	return (0);
}

/*
 * Return a status structure for the given file name.
 */
ostat(np, stp)
char *np;
struct oldstat *stp;
{
	register INODE *ip;
	struct oldstat stat;

	if (ftoi(np, 'r') != 0)
		return;
	ip = u.u_cdiri;
	oistat(ip, &stat);
	kucopy(&stat, stp, sizeof(stat));
	idetach(ip);
	return 0;
}

/*
 * Copy the appropriate information from the inode to the stat buffer.
 */
oistat(ip, sbp)
register INODE *ip;
register struct oldstat *sbp;
{
	sbp->st_dev = ip->i_dev;
	sbp->st_ino = ip->i_ino;
	sbp->st_mode = ip->i_mode;
	sbp->st_nlink = ip->i_nlink;
	sbp->st_uid = ip->i_uid;
	sbp->st_gid = ip->i_gid;
	sbp->st_rdev = NODEV;
	sbp->st_size = ip->i_size;
	sbp->st_atime = ip->i_atime;
	sbp->st_mtime = ip->i_mtime;
	sbp->st_ctime = ip->i_ctime;
	switch (ip->i_mode&IFMT) {
	case IFBLK:
	case IFCHR:
		sbp->st_rdev = ip->i_a.i_rdev;
		sbp->st_size = 0;
		break;
	case IFPIPE:
		sbp->st_size = ip->i_pnc;
		break;
	}
}

/*
 * Return date and time.
 */
oftime(tbp)
struct timeb *tbp;
{
	struct timeb timeb;

	timeb.time = timer.t_time;
	/* This should be a machine.h macro to avoid
	 * unnecessary long arithmetic and roundoff errors
	 */
	timeb.millitm = timer.t_tick*(1000/HZ);
	timeb.timezone = timer.t_zone;
	timeb.dstflag = timer.t_dstf;
	kucopy(&timeb, tbp, sizeof(timeb));
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
	extern sigalrm();

	/*
	 * Calculate time left before current alarm timeout.
	 */
	s = 0;
	if (pp->p_alrmtim.t_last != NULL)
		s = pp->p_alrmtim.t_lbolt - lbolt;

	/*
	 * Cancel previous alarm [if any], start new alarm [if n != 0].
	 */
	timeout2(&pp->p_alrmtim, (long) n, sigalrm, pp);

	/*
	 * Return time left before previous alarm timeout.
	 */
	return(s);
}

/*
 * Return elapsed ticks since system startup.
 */
long
utick()
{
	return(lbolt);
}

/*
 * Cause a signal routine to be executed.
 * Called from [coh/sig.c]
 */
oldsigstart(n, f)
{
	int i, n1;		
	register int	usp;

	usp = u.u_regl[UESP];

	/*
	 *                 -1
	 * calculate cvtsig  [n]
	 *
 	 */
	n1 = n; 
	for (i=0; cvtsig[i]>=0; i++)
		if (cvtsig[i]==n)
			n1 = i;
			
	putuwd(usp-3*sizeof(short), n1);
	putuwd(usp-2*sizeof(short), u.u_regl[EIP]);
	putubd(usp-2, u.u_regl[EFL]);
	putubd(usp-1, u.u_regl[EFL]>>8);
	u.u_regl[EFL] &= ~MFTTB;
	u.u_regl[EIP] = f;
	u.u_regl[UESP] -= 3*sizeof(short);
	if (n != SIGTRAP)
		u.u_sfunc[n-1] = SIG_DFL;
}

/*
 * Duplicate a file descriptor number.  This has the same calling
 * sequence as the dup2 system call and even uses the silly DUP2 bit.
 */
fddup(ofd, nfd)
register unsigned ofd;
register unsigned nfd;
{
	register FD *fdp;

	if ((fdp=fdget(ofd&~DUP2)) == NULL)
		return (-1);
	if ((ofd&DUP2) != 0) {
		if (nfd >= NOFILE) {
			u.u_error = EBADF;
			return (-1);
		}
		ofd &= ~DUP2;
		if (ofd == nfd)
			return (nfd);
		if (u.u_filep[nfd] != NULL) {
			fdclose(nfd);
			if (u.u_error)
				return (-1);
		}
	} else {
		for (nfd=0; nfd<NOFILE; nfd++)
			if (u.u_filep[nfd] == NULL)
				break;
		if (nfd == NOFILE) {
			u.u_error = EMFILE;
			return (-1);
		}
	}
	u.u_filep[nfd] = fdp;
	fdp->f_refc++;
	return (nfd);
}

/*
 * obrk()
 *
 * Argument is the new linear space value for the end of the PDATA segment.
 * As was done in COH286, arg of zero asks for the old upper limit.
 */
obrk(cp)
long cp;
{
	register int res;

	cp &= 0xffff;	/* Ward off sign extension problems with cp. */

	/*
	 * If cp nonzero
	 *	resize user data segment
	 * else
	 *	just give info - current brk address
	 */
	if (cp)
		res = ubrk(cp);
	else
		res = u.u_segl[SIPDATA].sr_base + SELF->p_segp[SIPDATA]->s_size;

	return res; 
}
