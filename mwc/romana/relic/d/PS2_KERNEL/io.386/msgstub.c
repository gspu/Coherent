/* $Header: /kernel/kersrc/io.386/RCS/msgstub.c,v 1.2 92/08/04 12:54:08 bin Exp Locker: bin $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1985
 *	An unpublished work by INETCO Sys/

#include <sys/coherent.h>
#include <errno.h>
#include <sys/inode.h>
#include <sys/fdisk.h>
#include <sys/buf.h>
#include <sys/con.h>

fdisk(dev, fp)
dev_t dev;
register struct fdisk_s *fp;
{
	register struct hdisk_s *hp;
	BUF *bp;
	int s, i;
	int ret = 0;

	s = sphi();
	dopen(dev, IPR, DFBLK);

	if (u.u_error == 0) {		/* special device now open */

		if (bp = bread(dev, (daddr_t) 0, 1)) {	/* data read */

			/* buffer cache is in kernel data space */
#ifdef _I386
			hp = bp->b_vaddr;
#else
			hp = FP_OFF(bp->b_faddr);
#endif

			if(hp->hd_sig==HDSIG) {
				for (i=0; i < NPARTN; ++i)
					*fp++ = hp->hd_partn[i];
				ret   = 1;
			}
			brelease(bp);
		}
		dclose(dev);
	}
	spl(s);
	return ret;
}
                                                                                                                                                                                                                                                                                                                              io.386/fl.c       