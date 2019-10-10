/* $Header: /y/coh.386/RCS/var.c,v 1.3 92/03/12 11:26:41 hal Exp $ */
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
 * Variables.
 *
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/con.h>
#include <sys/inode.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/ptrace.h>
#include <sys/seg.h>
#include <sys/timeout.h>
#include <sys/systab.h>	
#include <sys/mmu.h>	

int	debflag = 0;			/* coherent.h */

int	batflag;			/* coherent.h */
int	dev_loaded;			/* coherent.h */
int	DUMP_LIM=512*1024;		/* coherent.h */
int	outflag;			/* coherent.h */
int	ttyflag;			/* coherent.h */
unsigned utimer;			/* coherent.h */
long	 lbolt;				/* coherent.h */
TIM	stimer;				/* coherent.h */
unsigned asize;				/* coherent.h */
paddr_t	 clistp;			/* coherent.h */
MAKESR(blockp, _blockp);		/* coherent.h */
MAKESR(allocp, _allocp);		/* coherent.h */
struct	 all *allkp;			/* coherent.h */
#if USE_SLOT
int	NSLOT	= 64;			/* coherent.h */
int	slotsz	= 64;			/* coherent.h */
int	*	slotp;			/* coherent.h */
#endif
unsigned total_mem;			/* coherent.h */

unsigned bufseqn;			/* buf.h */
int	bufneed;			/* buf.h */
BUF	 swapbuf;			/* buf.h */
BUF	*bufl;				/* buf.h */

int	cltwant;			/* clist.h */
cmap_t	cltfree;			/* clist.h */

INODE	*inodep;			/* inode.h */
INODE	*acctip;			/* inode.h */

MOUNT	*mountp;			/* mount.h */

#ifdef TRACER				/* mwc_coherent.h */
unsigned t_errno = 0;
unsigned t_hal = 0;
unsigned t_piggy = 0;
unsigned t_vlad =0;
unsigned t_con =0;
#endif /* TRACER */

int	ISTSIZE	= 2048;			/* sys/param.h */
int	quantum;			/* proc.h */
int	disflag;			/* proc.h */
int	intflag;			/* proc.h */
int	cpid;				/* proc.h */
#if QWAKEUP
int	ntowake;			/* proc.h */
#endif
GATE	pnxgate;			/* proc.h */
PROC	procq;				/* proc.h */
PROC	*iprocp;			/* proc.h */
PROC	*eprocp;			/* proc.h */
PROC	*cprocp = &procq;		/* proc.h */
PLINK	linkq[NHPLINK];			/* proc.h */

struct	ptrace pts;			/* ptrace.h */

int	sexflag;			/* seg.h */
GATE	seglink;			/* seg.h */
#if	MONITOR
int	swmflag;			/* seg.h */
#endif
SEG	segswap;			/* seg.h */
SEG	segmq;				/* seg.h */
SEG	segdq;				/* seg.h */
SEG	segiom;				/* seg.h */
SYSMEM	sysmem;				/* mmu.h */

extern vaddr_t	aicodep;
extern vaddr_t	aicodes;

char	 *icodep = (char *)&aicodep;	/* coherent.h */
int	icodes = (int)&aicodes;	/* coherent.h */

TIM *	timq[256];			/* timeout.h */

int	vtactive;

/*
 * System call functions.
 */
int	unone();
int	unull();
int	uexit();
int	ufork();
int	uread();
int	uwrite();
int	uopen();
int	uclose();
int	uwait();
int	ucreat();
int	ulink();
int	uunlink();
int	uexece();
int	uchdir();
int	umknod();
int	uchmod();
int	uchown();
char	*ubrk();
int	ustat();
long	ulseek();
int	ugetpid();
int	umount();
int	uumount();
int	usetuid();
int	ugetuid();
int	ustime();
int	uptrace();
int	ualarm();
int	ufstat();
int	upause();
int	uutime();
int	ustty();
int	ugtty();
int	uaccess();
int	unice();
int	uftime();
int	usync();
int	ukill();
int	udup();
int	upipe();
int	utimes();
int	utime();
int	uprofil();
int	usetgid();
int	ugetgid();
int	(*usigsys())();
int	usload();
int	usuload();
int	uacct();
int	ulock();
int	uioctl();
int	ugetegid();
int	uumask();
int	uchroot();
int	ufcntl();
int	usetpgrp();
int	uulimit();
int	ufcntl();
int	upoll();
int	upgrp();
int	usysi86();
int	umsgsys();
int	ushmsys();
int	uutssys();
int	usemsys();
int	urmdir();
int	umkdir();
int	ugetdents();
int	ustatfs();
int	ufstatfs();
int	uadmin();
/*
 * Added by hal 91/10/10.
 * These are undocumented Xenix/V7 compatibility calls.
 */
int	ustty();
int	ugtty();

/*
 * System call table.
 */
int	ucohcall();
struct systab cohcall = {
	6,  INT,	ucohcall
};
struct systab sysitab[NMICALL] ={
	0,  INT,	unone,			/*  0 = ??? */
	1,  INT,	uexit,			/*  1 = exit */
	0,  INT,	ufork,			/*  2 = fork */
	3,  INT,	uread,			/*  3 = read */
	3,  INT,	uwrite,			/*  4 = write */
	3,  INT,	uopen,			/*  5 = open */
	1,  INT,	uclose,			/*  6 = close */
	0,  INT,	uwait,			/*  7 = wait */
	2,  INT,	ucreat,			/*  8 = creat */
	2,  INT,	ulink,			/*  9 = link */
	1,  INT,	uunlink,		/* 10 = unlink */
	0,  INT,	unone,			/* 11 = exec */
	1,  INT,	uchdir,			/* 12 = chdir */
	0,  INT,	utime,			/* 13 = utime */
	3,  INT,	umknod,			/* 14 = mknod */
	2,  INT,	uchmod,			/* 15 = chmod */
	3,  INT,	uchown,			/* 16 = chown */
	1,  INT,	ubrk,			/* 17 = break */
	2,  INT,	ustat,			/* 18 = stat */
	3,  LONG,	ulseek,			/* 19 = lseek */
	0,  INT,	ugetpid,		/* 20 = getpid */
	3,  INT,	umount,			/* 21 = mount */
	1,  INT,	uumount,		/* 22 = umount */
	1,  INT,	usetuid,		/* 23 = setuid */
	0,  INT,	ugetuid,		/* 24 = getuid */
	1,  INT,	ustime,			/* 25 = stime */
	4,  INT,	uptrace,		/* 26 = ptrace */
	1,  INT,	ualarm,			/* 27 = alarm */
	2,  INT,	ufstat,			/* 28 = fstat */
	0,  INT,	upause,			/* 29 = pause */
	2,  INT,	uutime,			/* 30 = utime */
	2,  INT,	ustty,			/* 31 = ustty */
	2,  INT,	ugtty,			/* 32 = ugtty */
	2,  INT,	uaccess,		/* 33 = access */
	1,  INT,	unice,			/* 34 = nice */
	4,  INT,	ustatfs,		/* 35 = statfs */
	0,  INT,	usync,			/* 36 = sync */
	2,  INT,	ukill,			/* 37 = kill */
	4,  INT,	ufstatfs,		/* 38 = ufstatfs */
	1,  INT,	upgrp,			/* 39 = pgrp */
	0,  LONG,	unone,			/* 40 = ??? */
	1,  INT,	udup,			/* 41 = dup */
	0,  INT,	upipe,			/* 42 = pipe */
	1,  INT,	utimes,			/* 43 = times */
	4,  INT,	uprofil,		/* 44 = profil */
	1,  INT,	ulock,			/* 45 = lock */
	1,  INT,	usetgid,		/* 46 = setgid */
	0,  INT,	ugetgid,		/* 47 = getgid */
	2,  INT,	usigsys,		/* 48 = signal */
	6,  LONG,	umsgsys,		/* 49 = msgsys */
	5,  LONG,	usysi86,		/* 50 = sysi86 */	
	1,  INT,	uacct,			/* 51 = acct */
	4,  INT,	ushmsys,		/* 52 = shmsys */
	5,  INT,	usemsys,		/* 53 = semsys */
	3,  INT,	uioctl,			/* 54 = ioctl */
	3,  INT,	uadmin,			/* 55 = uadmin */
	0,  INT,	unone,			/* 56 = ??? */
	3,  INT,	uutssys,		/* 57 = utssys */
	0,  INT,	unone,			/* 58 = ??? */
	3,  INT,	uexece,			/* 59 = exec */
	1,  INT,	uumask,			/* 60 = umask */
	1,  INT,	uchroot,		/* 61 = chroot */
	3,  INT,	ufcntl,			/* 62 = fcntl */
	2,  INT,	uulimit,		/* 63 = ulimit - n/i */
	0,  INT,	unone,			/* 64 = ??? (sload) */
	0,  INT,	unone,			/* 65 = ??? (suload */
	0,  INT,	unone,			/* 66 = ??? (fcntl) */
	0,  INT,        unone,			/* 67 = ??? (poll) */
	0,  INT,	unone,			/* 68 = ??? (msgctl) */
	0,  INT,	unone,			/* 69 = ??? (msgget) */
	0,  INT,	unone,			/* 70 = ??? (msgrcv) */
	0,  INT,	unone,			/* 71 = ??? (msgsnd) */
	0,  LONG,	unone,			/* 72 = ??? (alarm2) */
	0,  LONG,	unone,			/* 73 = tick  */
	0,  INT,	unone,			/* 74 = ??? */
	0,  INT,	unone,			/* 75 = ??? */
	0,  INT,	unone,			/* 76 = ??? */
	0,  INT,	unone,			/* 77 = ??? */
	0,  INT,	unone,			/* 78 = ??? */
	1,  INT,	urmdir,			/* 79 = rmdir */
	2,  INT,	umkdir,			/* 80 = mkdir */
	3,  INT,	ugetdents,		/* 81 = getdents */
	0,  INT,	unone,			/* 82 = ??? */
	0,  INT,	unone,			/* 83 = ??? */
	0,  INT,	unone,			/* 84 = ??? */
	0,  INT,	unone,			/* 85 = ??? */
	0,  INT,	unone,			/* 86 = ??? */
	3,  INT,        upoll,			/* 87 = poll */
};
extern	CON nlcon;
int	(*altclk)();		/* hook for polled devices */
