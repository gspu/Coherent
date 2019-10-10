/*
 * /usr/include/sys/hdioctl.h
 *
 * Ioctl support for hard disk devices.
 *
 * Revised: Sun Aug  1 17:24:39 1993 CDT
 */
#ifndef __SYS_HDIOCTL_H__
#define	__SYS_HDIOCTL_H__

#define	HDIOC	('H' << 8)
#define	HDGETA	(HDIOC|1)	/* get drive attributes */
#define	HDSETA	(HDIOC|2)	/* set drive attributes */

/*
 * Drive attributes
 * Note: all fields defined as bytes to prevent compiler arith probs.
 *	All multi-byte fields are stored low-byte first.
 * This struct is configured for binary compatibility with ROM data!
 */
typedef struct hdparm_s {
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

/* Macro for initializing drive parameter tables. */
#define _HDPARMS(cyl,hd,spt,ctl,pcomp)	{ \
	{ (cyl) & 0xFF, (cyl) >> 8 }, hd, { 0, 0 }, \
	{ (pcomp) & 0xFF, (pcomp) >> 8 }, 0, ctl, \
	{ 0, 0, 0 }, { 0 , 0 }, spt, 0 }

/* Convert from a 2-element unsigned char array to unsigned short. */
#define _CHAR2_TO_USHORT(c_array)	\
	((unsigned short) ((c_array)[1] << 8) | (c_array)[0])

/* Copy number into a 2-element unsigned char array. */
#define _NUM_TO_CHAR2(c_array, num) \
	(((c_array)[0] = num & 0xFF), ((c_array)[1] = num >> 8))

#if __KERNEL__
#define N_ATDRV	2			/* only two drives supported */
#endif

#endif /* ndef __SYS_HDIOCTL_H__ */
