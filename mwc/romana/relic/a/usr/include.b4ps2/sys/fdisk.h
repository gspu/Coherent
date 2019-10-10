/* (-lgl
 * 	COHERENT Version 3.0
 *	Copyright 1989 by Inetco Systems, Ltd.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * fdisk.h
 * Fixed disk configuration.
 * The first sector (boot block) of each hard disk
 * should contain a fixed disk boot record (HDISK_S).
 */

#ifndef	FDISK_H
#define	FDISK_H

#define	NPARTN	4			/* Partitions per drive	 */
#define	HDSIG	0xAA55			/* Signature word	 */
#define	SDEV	0x80			/* Special minor device	 */

/*
 * System indicators.
 * There is disagreement about some of these.
 * Is there an official list?
 */
#define	SYS_EMPTY	0	/* Empty			*/
#define	SYS_DOS_12	1	/* MS-DOS, 12-bit FAT		*/
#define	SYS_XENIX 	2	/* Xenix			*/
#define	SYS_DOS_16	4	/* MS-DOS, 16-bit FAT		*/
#define SYS_DOS_XP	5	/* MS-DOS, extended partition	*/
#define	SYS_DOS_LARGE	6	/* MS-DOS, large filesystem	*/
#define	SYS_COH		9	/* Coherent			*/
#define	SYS_SWAP	10	/* Coherent swap partition	*/

/*
 * Be careful when using the FDISK_S or HDISK_S structure:
 * the two high order bits of p_bsec and p_esec on the boot block
 * provide the two high order bits of p_bcyl and p_ecyl, respectively.
 */
#define SECMASK 0x3F	/* Mask for sector number bits of sector fields.     */
#define CYLMASK 0xC0	/* Mask for cylinder number bits of sector fields.   */
#define	bcyl(p)	((((p)->p_bsec & CYLMASK) << 2) | ((p)->p_bcyl))
#define	bhd(p)	((p)->p_bhd)
#define	bsec(p)	((p)->p_bsec & SECMASK)	
#define	ecyl(p)	((((p)->p_esec & CYLMASK) << 2) | ((p)->p_ecyl))
#define	ehd(p)	((p)->p_ehd)
#define	esec(p)	((p)->p_esec & SECMASK)

/* Per partition information. */
#pragma align 1
typedef	struct	fdisk_s {
	unsigned char	p_boot;		/* Boot indicator	*/
	unsigned char	p_bhd;		/* Beginning head	*/
	unsigned char	p_bsec;		/* Beginning sector	*/
	unsigned char	p_bcyl;		/* Beginning cylinder	*/
	unsigned char	p_sys;		/* System indicator	*/
	unsigned char	p_ehd;		/* Ending head		*/
	unsigned char	p_esec;		/* Ending sector	*/
	unsigned char	p_ecyl;		/* Ending cylinder	*/
	unsigned long		p_base;		/* Base block number	*/
	unsigned long		p_size;		/* Size in blocks	*/
} FDISK_S;
/* Hard disk master boot block. */
typedef	struct	hdisk_s {
	unsigned char	hd_boot[446];		/* Bootstrap		*/
#pragma align 2
	struct fdisk_s	hd_partn[NPARTN];	/* Partition info*/
#pragma align
	unsigned short	hd_sig;			/* Validating signature	*/
} HDISK_S;
#pragma align

#ifdef INKERNEL
/*
 * fdisk(dev_t sdev, FDISK_S pp[])
 * Read partition info from first block of a special device.
 * If valid, update partition array and return 1.
 */
extern	int	fdisk();
#endif

#endif

/* end of fdisk.h */
