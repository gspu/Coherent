/* (-lgl
 * 	COHERENT Version 3.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef HDIOCTL_H
#define	HDIOCTL_H
#define	HDIOC	('H' << 8)
#define	HDGETA	(HDIOC|1)	/* get drive attributes */
#define	HDSETA	(HDIOC|2)	/* set drive attributes */

/*
 * Drive attributes
 * Note: all fields defined as bytes to prevent compiler arith probs.
 *	All multi-byte fields are stored low-byte first.
 *	ie: number of cylinders - (hd_ncyl [1] << 8) + hc_ncyl[0]
 */
typedef
struct hdparm_s {
	unsigned char	ncyl[2];	/* number of cylinders */
	unsigned char	nhead;		/* number heads */
	unsigned char	rwccp[2];	/* reduced write curr cyl */
	unsigned char	wpcc[2];	/* write pre-compensation cyl */
	unsigned char	eccl;		/* max ecc data length */
	unsigned char	ctrl;		/* control byte */
	unsigned char	fill2[3];
	unsigned char	landc[2];	/* landing zone cylinder */
	unsigned char	nspt;		/* number of sectors per track */
	unsigned char	hdfill3;
} hdparm_t;
#endif
