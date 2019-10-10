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
 * Revised: Fri Jul 16 12:23:39 1993 CDT
 */

#include <common/_limits.h>
#include <common/_tricks.h>
#include <common/_gregset.h>
#include <sys/debug.h>

#include <sys/coherent.h>
#include <sys/acct.h>
#include <sys/buf.h>
#include <canon.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <l.out.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/oldstat.h>
#include <sys/timeb.h>
#include <sys/fd.h>

#include <kernel/systab.h>


/*
 * emulate a 16 bit system call
 * called from trap.c
 */

static char cvtsig [] = 
{
	0,
	SIGHUP, SIGINT, SIGQUIT, SIGALRM, SIGTERM, SIGPWR, 
	SIGSYS, SIGPIPE, SIGKILL, SIGTRAP, SIGSEGV,
	SIGEMT, /* SIGDIVE */
	SIGEMT, /* SIGOVFL */
	SIGUSR1,
	SIGUSR2,
	SIGUSR2
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
int	obrk();
long	oalarm2 ();
long	otick ();


/*
 * Duplicate a file descriptor number.  This has the same calling
 * sequence as the dup2 system call and even uses the silly DUP2 bit.
 */

coh286dup(ofd, nfd)
register unsigned ofd;
register unsigned nfd;
{
	register FD *fdp;

	if ((fdp = fdget (ofd & ~ DUP2)) == NULL)
		return -1;
	if ((ofd & DUP2) != 0) {
		if (nfd >= NOFILE) {
			SET_U_ERROR (EBADF, "coh286dup ()");
			return -1;
		}
		ofd &= ~ DUP2;
		if (ofd == nfd)
			return nfd;
		if (u.u_filep [nfd] != NULL) {
			fdclose (nfd);
			if (u.u_error)
				return -1;
		}
	} else
		nfd = 0;

	return fddup (ofd, nfd);
}


int
ostime (timep)
time_t	      *	timep;
{
	return ustime (getuwd (timep));
}


int
opipe (pipep)
short	      *	pipep;
{
	short		res;

	res = upipe ();

	putusd (pipep, res);
	putusd (pipep + 1, u.u_rval2);
	return 0;
}


int
osetpgrp ()
{
	upgrp (1);
}

int
ogetpgrp ()
{
	upgrp (0);
}


int
ogeteuid ()
{
	(void) ugetuid ();
	return u.u_rval2;
}

int
ogetegid ()
{
	(void) ugetgid ();
	return u.u_rval2;
}


int
ounique ()
{
	return usysi86 (SYI86UNEEK);
}


int
okill (pid, signal)
short		pid;
unsigned	signal;
{
	if (signal >= __ARRAY_LENGTH (cvtsig)) {
		SET_U_ERROR (EINVAL, "286 kill ()");
		return -1;
	}

	return ukill (pid, cvtsig [signal]);
}


__sigfunc_t
osignal (signal, func, regsetp)
unsigned	signal;
__sigfunc_t	func;
gregset_t     *	regsetp;
{
	if (signal >= __ARRAY_LENGTH (cvtsig)) {
		SET_U_ERROR (EINVAL, "286 signal ()");
		return -1;
	}

	return usigsys (cvtsig [signal], func, regsetp);
}


#if	__SHRT_BIT != 16
# error	This code expects 16-bit shorts
#endif

long
olseek (fd, seeklo, seekhi, whence)
unsigned	fd;
unsigned short	seeklo;
unsigned short	seekhi;
unsigned	whence;
{
	return ulseek (fd, seeklo + (seekhi << 16), whence);
}


/* msgsys, shmsys, and semsys are not emulated */
/* poll is not emulated;NOTE:the code calls putuwd */

int
oldsys (regsetp)
gregset_t     *	regsetp;
{
	register struct	systab	*stp;
	unsigned int	callnum;
	int		i;
	int		res;
	int		args [MSACOUNT];

	u.u_error = 0;
	callnum = getusd (NBPS + regsetp->_i286._ip - sizeof (short));

	/*
	 * Check that we are on an INT instruction, and that the fetch did
	 * not cause a memory fault. Note that the magic NBPS number above,
	 * which presumably means "Number of Bytes Per Segment", is how to
	 * get to 286 code.
	 */

	if (u.u_error || (callnum & 0xFF) != 0xCD) 
		return SIGSYS;
	callnum = (callnum >> 8) & 0x7F;

	/* Print out this 286 call number only if tracing is on.  */
	T_PIGGY (0x2, printf ("[%d]", callnum));

	if (callnum >= __ARRAY_LENGTH (sys286tab))
		return SIGSYS;
	stp = sys286tab + callnum;

	/*
	 * This is crass bullshit which allows fucked code to get away with
	 * not fully intializing a structure which shouldn't even be in the
	 * U area at all.
	 */

#if	0
	u.u_io.io_seg = IOUSR;
#endif

	if (envsave (& u.u_sigenv)) {
		u.u_error = EINTR;
		goto done;
	}

	i = stp->s_nargs + 1;
	while (-- i > 0) {
		args [i - 1] = getusd (regsetp->_i286._usp +
				       i * sizeof (short));
	}

	if (u.u_error)
		return SIGSYS;

	/*
	 * Perform the system call and collect the return value in "res".
	 */

	res = __DOSYSCALL (stp->s_nargs, stp->s_func, args, regsetp);

	if (stp->s_type == __SYSCALL_LONG)
		regsetp->_i286._dx = res >> 16;
	else
		regsetp->_i286._dx = u.u_rval2;
	regsetp->_i286._ax = res;

done:
	if (u.u_error) {
		regsetp->_i286._ax = regsetp->_i286._dx = -1;
		putubd (MUERR, u.u_error);
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

	if ((fdp = fdget (fd)) == NULL)
		return -1;
	ip = fdp->f_ip;
	oistat (ip, & stat);
	kucopy (& stat, stp, sizeof (stat));
	return 0;
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
	IO		io;
	struct direct	dir;

	if (ftoi (np, 'r', & io, & dir) != 0)
		return -1;

	ip = u.u_cdiri;
	oistat (ip, & stat);

	if (kucopy (& stat, stp, sizeof (stat)) != sizeof (stat))
		SET_U_ERROR (EFAULT, "286 stat ()");

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

	switch (ip->i_mode & IFMT) {
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

	if (kucopy (& timeb, tbp, sizeof (timeb)) != sizeof (timeb))
		SET_U_ERROR (EFAULT, "286 ftime ()");
}


/*
 * Send a SIGALARM signal in `n' clock ticks.
 */

long
oalarm2(n)
long n;
{
	register PROC * pp = SELF;
	long s;
	extern sigalrm ();

	/*
	 * Calculate time left before current alarm timeout.
	 */
	s = 0;
	if (pp->p_alrmtim.t_last != NULL)
		s = pp->p_alrmtim.t_lbolt - lbolt;

	/*
	 * Cancel previous alarm [if any], start new alarm [if n != 0].
	 */

	timeout2 (& pp->p_alrmtim, (long) n, sigalrm, pp);

	/*
	 * Return time left before previous alarm timeout.
	 */
	return s;
}

/*
 * Return elapsed ticks since system startup.
 */

long
otick()
{
	return lbolt;
}

/*
 * Cause a signal routine to be executed.
 * Called from [coh/sig.c]
 */
oldsigstart (n, func, regsetp)
__sigfunc_t	func;
gregset_t     *	regsetp;
{
	int		i;
	struct {
		ushort_t	sf_signo;
		ushort_t	sf_prev_ip;
		ushort_t	sf_flags;
	} signal_frame;

	/*
	 *                 -1
	 * calculate cvtsig  [n]
	 *
 	 */

	signal_frame.sf_signo = n;
	for (i = 0 ; i < __ARRAY_LENGTH (cvtsig) ; i ++)
		if (cvtsig [i] == n) {
			signal_frame.sf_signo = i;
			break;
		}

	signal_frame.sf_prev_ip = regsetp->_i286._ip;
	signal_frame.sf_flags = regsetp->_i286._flags;

	/*
	 * Turn off single-stepping in signal handler.
	 */

	regsetp->_i286._flags &= ~ MFTTB;
	regsetp->_i286._ip = (ushort_t) func;
	regsetp->_i286._usp -= sizeof (signal_frame);

	i = kucopy (& signal_frame, regsetp->_i286._usp,
		    sizeof (signal_frame));
	ASSERT (i == sizeof (signal_frame));
}

/*
 * obrk()
 *
 * Argument is the new linear space value for the end of the PDATA segment.
 * As was done in COH286, arg of zero asks for the old upper limit.
 */
obrk (cp)
unsigned	cp;
{
	register int res;

	/*
	 * If cp nonzero
	 *	resize user data segment
	 * else
	 *	just give info - current brk address
	 */

	if (cp)
		res = ubrk (cp);
	else
		res = u.u_segl [SIPDATA].sr_base +
			SELF->p_segp [SIPDATA]->s_size;

	return res; 
}
