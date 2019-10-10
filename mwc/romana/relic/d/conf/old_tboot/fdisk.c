/*
 *
 * fdisk( fp )	--	Fixed Disk Configuration
 * dev_t dev;
 * struct fdisk_s *fp;
 *
 *	Input:	fp  = pointer to memory-resident partition info (to update)
 *
 *	Action:	Read first block from the device.
 *		If valid signature present on block,
 *			copy partition information to memory
 *
 *	Return:	1 = partition information successfully updated
 *		0 = failure (could not read block, or bad signature)
 */

#include <sys/coherent.h>
#include <sys/uproc.h>
#include <errno.h>
#include <sys/inode.h>
#include <sys/fdisk.h>
#include <sys/buf.h>
#include <sys/con.h>

int
fdisk( fp )
register FDISK_S *fp;
{
	register HDISK_S *hp;
	BUF *bp, *xbread();
	int i;
	int ret = 0;

	/* xbread() reads relative to the start of the whole disk, rather
	 * than the start of the boot partition.  It also does not
	 * impliment sparse blocks.
	 */
	if ((BUF *) NULL != (bp = xbread((daddr_t) 0))) {  /* data read */
		hp = (HDISK_S *) bp->b_paddr;
		if ( hp->hd_sig == HDSIG ) {	/* valid data */
			for (i=0; i < NPARTN; ++i)
				*fp++ = hp->hd_partn[i];
			ret   = 1;
		}
		brelease( bp );
	}

	return ret;
}
