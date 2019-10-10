/**
 *
 * fdisk(dev, fp)	--	Fixed Disk Configuration
 * dev_t dev;
 * struct fdisk_s *fp;
 *
 *	Input:	dev = special device to read partition information from
 * 		fp  = pointer to memory-resident partition info (to update)
 *
 *	Action:	Open special device for reading.
 *		Read first block from the device.
 *		If valid signature present on block,
 *			copy partition information to memory
 *
 *	Return:	1 = partition information successfully updated
 *		0 = failure (could not read block, or bad signature)
 */

#include <sys/coherent.h>
#include <sys/errno.h>
#include <sys/fdisk.h>
#include <sys/buf.h>
#include <sys/con.h>
#include <sys/file.h>

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

		if (bp = bread(dev, (daddr_t) 0, BUF_SYNC)) {	/* data read */

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
