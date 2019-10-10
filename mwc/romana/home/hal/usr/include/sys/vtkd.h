/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_VTKD_H__
#define __SYS_VTKD_H__

/*
 * Console ioctl's
 */

#define KIOC		('K' << 8)      /* ioctl base */
#define KDMAPDISP       (KIOC | 2)      /* map display into user space */
#define KDSKBMODE       (KIOC | 6)      /* turn scan code xlate on/off */
#define KDMEMDISP       (KIOC | 7)      /* dump byte of virt/phys mem  */
#define KDADDIO         (KIOC | 11)     /* enable io to a single port */
#define KDDELIO         (KIOC | 12)     /* disable io to a single port */
#define KDGKBSTATE	(KIOC | 19) 	/* Get kb shift state */
#define KDENABIO        (KIOC | 60)     /* enable IO */
#define KDDISABIO       (KIOC | 61)     /* disable IO */
#define KIOCINFO	(KIOC | 62) 	/* determine workstation of VT */
#define KIOCSOUND       (KIOC | 63)     /* start sound generation */ 
#define KDGETLED	(KIOC | 65) 	/* get led state */
#define KDSETLED        (KIOC | 66)     /* set led state */


/*
 * Masks for KDGKBSTATE
 */

#define M_KDGKBSTATE_SHIFT	0x01
#define M_KDGKBSTATE_CTRL	0x02
#define M_KDGKBSTATE_ALT	0x04


/*
 * Masks for KDGETLED / KDSETLED
 */

#define M_KDGETLED_SCRLOCK	0x01
#define M_KDGETLED_NUMLOCK	0x02
#define M_KDGETLED_CAPLOCK	0x04

#define M_KDSETLED_SCRLOCK	M_KDGETLED_SCRLOCK
#define M_KDSETLED_NUMLOCK	M_KDGETLED_NUMLOCK
#define M_KDSETLED_CAPLOCK	M_KDGETLED_CAPLOCK


struct kd_memloc {
	char	      *	vaddr;		/* virtual address */
	char	      *	physaddr;	/* physical address */
	long		length;		/* size in bytes */
	long		ioflg;		/* enable I/O addresses if non-zero */
};

#endif /* ! defined (__SYS_VTKD_H__) */

