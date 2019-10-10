/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/**
 *
 * fdisk( dev, fp )	--	Fixed Disk Configuration
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
#include <sys/uproc.h>
#include <errno.h>
#include <sys/inode.h>
#include <sys/fdisk.h>
#include <sys/buf.h>
#include <sys/con.h>

fdisk( dev, fp )
dev_t dev;
register struct fdisk_s *fp;
{
	register struct hdisk_s *hp;
	BUF *bp;
	int s, i;
	int ret = 0;

	s = sphi( );
	dopen( dev, IPR, DFBLK );

	if ( u.u_error == 0 ) {		/* special device now open */

		if (bp = bread(dev, (daddr_t) 0, 1)) {	/* data read */

			/* buffer cache is in kernel data space */
			hp = FP_OFF(bp->b_faddr);

			if ( hp->hd_sig == HDSIG ) {	/* valid data */

				for (i=0; i < NPARTN; ++i)
					*fp++ = hp->hd_partn[i];

				ret   = 1;
			}
			brelease( bp );
		}
		dclose( dev );
	}
	spl( s );
	return ret;
}
