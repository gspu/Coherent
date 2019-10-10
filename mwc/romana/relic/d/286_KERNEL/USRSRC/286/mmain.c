/* $Header: /usr/local/src/sys/i8086/src/RCS/mmain.c,v 1.2 91/06/20 14:40:23 bin Exp Locker: piggy $ */
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
 * 8086/8088 Coherent.
 * All machines.
 * Machine dependent stuff.
 *
 * $Log:	mmain.c,v $
 * Revision 1.2  91/06/20  14:40:23  bin
 * update provided by hal
 * 
 * Revision 1.2	88/08/05  15:43:42	src
 * Bug:	Spawning large number of processes would cause system to crash.
 * Fix:	Kernel alloc space no longer overlaps loadable driver data.
 * 
 * Revision 1.1	88/03/24  17:39:46	src
 * Initial revision
 * 
 * 88/02/24	Allan Cornish		/usr/src/sys/i8086/src/mmain.c
 * corebot is now aligned on a 512 byte boundary.
 *
 * 87/11/30	Allan Cornish		/usr/src/sys/i8086/src/mmain.c
 * Check for kernel data space > 64 Kbytes now done AFTER rounding up.
 *
 * 87/11/21	Allan Cornish		/usr/src/sys/i8086/src/mmain.c
 * Use of bruc/ctob macros eliminated since no longer valid in protected mode.
 *
 * 87/11/14	Allan Cornish		/usr/src/sys/i8086/src/mmain.c
 * Init code+data now split into icodep/icodes and idatap/idatas.
 *
 * 87/11/12	Allan Cornish		/usr/src/sys/i8086/src/mmain.c
 * Corebot/coretop now paddr_t rather than saddr_t to support protected mode.
 *
 * 87/10/05	Allan Cornish		/usr/src/sys/i8086/src/mmain.c
 * Loadable driver data slot allocation added.
 *
 * 87/05/08	Allan Cornish		/usr/src/sys/i8086/src/mmain.c
 * Ctob(sds) is now cast as ctob((paddr_t)sds) to avoid address truncation.
 *
 * 86/07/23	Allan Cornish
 * Added check for kernel data space exceeding 64 Kbytes.
 */
#include <sys/coherent.h>
#include <sys/i8086.h>
#include <sys/clist.h>
#include <errno.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/uproc.h>
#include <sys/buf.h>
#include <sys/typed.h>

/* Arguments are passed into the kernel through boot_gift.
 * If you start getting "Not enough room for all arguments." messages
 * at boot time, just increase the BG_LEN  to whatever you need.
 * This structure is EXACTLY BG_LEN bytes long.
 */
#define BG_LEN 512
TYPED_SPACE(boot_gift, BG_LEN, T_FIFO_SIC);

saddr_t uasa;	/* Currently active uarea */

/*
 * General initialisation.
 */
i8086()
{
	register unsigned allocp;
	extern vaddr_t	aicodep;
	extern vaddr_t	aicodes;
	extern vaddr_t	aidatap;
	extern vaddr_t	aidatas;
	extern vaddr_t	etext;
	extern vaddr_t	end;
	auto faddr_t	fp;
	long datsize;
	unsigned bsize, csize, isize, ssize;

	/*
	 * Set up memory bases.
	 * Align the buffers modulo BSIZE (512) in the physical space,
	 * so that any machines that have only 16 bit DMA counters will
	 * work out.
	 */
	datsize = (long)&end;
	datsize += ALLSIZE;
	datsize += NBUF * sizeof(BUF);
	datsize += ssize = NSLOT*(sizeof(int) + slotsz);
	datsize += isize = NINODE*sizeof(INODE);
	datsize += csize = NCLIST*sizeof(CLIST);
	datsize += bsize = NBUF*BSIZE;
	datsize = (datsize + 511) & ~511;
	if ( datsize >= 0x10000L )
		panic("Kernel data exceeds 64 Kbytes");

	blockp  = datsize - bsize - ((sds&0x1F)<<4);
	clistp  = (unsigned)blockp - csize;
	inodep  = (unsigned)clistp - isize;
	slotp   = (unsigned)inodep - ssize;
	allocp = &end;
	blockp += (sds << 4L);
	if ((unsigned)allocp > (unsigned)slotp)
		panic("No alloc space");
	corebot = ((sds << 4L) + datsize + 511) & ~511;
	asize = (unsigned)slotp - allocp;
	msize = (coretop-holetop+holebot-corebot) / 1024;
	allkp = setarena(allocp, asize);
	icodep = (char *)&aicodep;
	icodes = (int)&aicodes;
	idatap = (char *)&aidatap;
	idatas = (int)&aidatas;
	fp = ptov( corebot, (fsize_t) UPASIZE );
	uasa   = FP_SEL(fp);
}

