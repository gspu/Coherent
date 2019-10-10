/* $Header: /kernel/kersrc/coh.286/RCS/var.c,v 1.1 92/07/17 15:18:55 bin Exp Locker: bin $ */
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
 * $Log:	var.c,v $
 * Revision 1.1  92/07/17  15:18:55  bin
 * Initial revision
 * 
 * Revision 1.2	89/08/01  13:57:35 	src
 * Bug:	#include <timeout.h> not accurate; timeout.h now in /usr/include/sys.
 * Fix:	#include <sys/timeout.h> now used. (ABC)
 * 
 * Revision 1.1	88/03/24  08:14:41 	src
 * Initial revision
 * 
 * 88/01/23	Allan Cornish		/usr/src/sys/coh/var.c
 * Default NSLOT increased from 10 to 64.
 *
 * 87/11/22	Allan Cornish		/usr/src/sys/coh/var.c
 * Holebot/holetop variables added to support extended memory.
 *
 * 87/11/14	Allan Cornish		/usr/src/sys/coh/var.c
 * Init code+data now split into icodep/icodes and idatap/idatas.
 *
 * 87/11/12	Allan Cornish		/usr/src/sys/coh/var.c
 * Corebot/coretop now paddr_t rather than saddr_t to support protected mode.
 *
 * 87/10/05	Allan Cornish		/usrs/rc/sys/coh/var.c
 * NSLOT, slotsz, and slotp variables added - loadable driver specific.
 *
 * 87/07/07	Allan Cornish		/usr/src/sys/coh/var.c
 * Lbolt variable added - clock ticks since startup - incremented by stand().
 * Timl variable replaced with timq variable.
 *
 * 87/02/01	Allan Cornish		/usr/src/sys/coh/var.c
 * ISTSIZE [stack size] changed from a define in /usr/include/sys/const.h to a
 * extern int in /usr/include/sys/param.h, with 4 Kbyte default set in var.c
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/con.h>
#include <sys/inode.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/ptrace.h>
#include <sys/seg.h>

int	 debflag = 0;			/* coherent.h */

int	 batflag;			/* coherent.h */
int	 outflag;			/* coherent.h */
int	 ttyflag;			/* coherent.h */
unsigned utimer;			/* coherent.h */
long	 lbolt;				/* coherent.h */
TIM	stimer;				/* coherent.h */
unsigned msize;				/* coherent.h */
unsigned asize;				/* coherent.h */
char	 *icodep;			/* coherent.h */
int	 icodes;			/* coherent.h */
char	 *idatap;			/* coherent.h */
int	 idatas;			/* coherent.h */
paddr_t	 corebot;			/* coherent.h */
paddr_t	 coretop;			/* coherent.h */
paddr_t	 holebot;			/* coherent.h */
paddr_t	 holetop;			/* coherent.h */
paddr_t	 blockp;			/* coherent.h */
paddr_t	 clistp;			/* coherent.h */
struct	 all *allkp;			/* coherent.h */
int	NSLOT	= 64;			/* coherent.h */
int	slotsz	= 64;			/* coherent.h */
int *	slotp;				/* coherent.h */

unsigned bufseqn;			/* buf.h */
int	 bufneed;			/* buf.h */
BUF	 swapbuf;			/* buf.h */
BUF	*bufl;				/* buf.h */

int	cltwant;			/* clist.h */
cmap_t	cltfree;			/* clist.h */

INODE	*inodep;			/* inode.h */
INODE	*acctip;			/* inode.h */

MOUNT	*mountp;			/* mount.h */

int	ISTSIZE	= 4096;			/* sys/param.h */

int	quantum;			/* proc.h */
int	disflag;			/* proc.h */
int	intflag;			/* proc.h */
int	cpid;				/* proc.h */
#ifdef QWAKEUP
int	ntowake;			/* proc.h */
#endif
GATE	pnxgate;			/* proc.h */
PROC	procq;				/* proc.h */
PROC	*iprocp;			/* proc.h */
PROC	*eprocp;			/* proc.h */
PROC	*cprocp;			/* proc.h */
PLINK	linkq[NHPLINK];			/* proc.h */

struct	ptrace pts;			/* ptrace.h */

int	sexflag;			/* seg.h */
GATE	seglink;			/* seg.h */
#ifndef NOMONITOR
int	swmflag;			/* seg.h */
#endif
SEG	segswap;			/* seg.h */
SEG	segmq;				/* seg.h */
SEG	segdq;				/* seg.h */
SEG	segiom;				/* seg.h */

TIM *	timq[256];			/* timeout.h */
