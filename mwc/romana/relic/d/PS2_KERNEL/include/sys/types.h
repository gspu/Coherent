/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Machine dependent types.
 */

#ifndef TYPES_H
#define TYPES_H	TYPES_H

/*
 * Mapping types.
 */
typedef	unsigned int	aold_t;		/* Auxiliary map save		*/
typedef	unsigned int	bmap_t;		/* Buffer map			*/
typedef	unsigned int	bold_t;		/* Buffer map save		*/
typedef	unsigned int	cmap_t;		/* Clist map			*/
typedef	unsigned int	cold_t;		/* Clist map save		*/
typedef	unsigned int	dmap_t;		/* Driver map			*/
typedef	unsigned int	dold_t;		/* Driver map save		*/

/*
 * System types.
 */
typedef	unsigned short	comp_t;		/* Accounting			*/
typedef	long		daddr_t;	/* Disk address			*/
typedef	unsigned short	dev_t;		/* Device			*/
typedef	long	 	fsize_t;	/* Lengths (same as off_t)	*/
typedef	unsigned short	ino_t;		/* Inode number			*/
typedef	long	 	off_t;		/* Lengths			*/
typedef	long	 	paddr_t;	/* Physical memory address	*/
typedef	long	 	sig_t;		/* Signal bits			*/
typedef	long	 	time_t;		/* Time				*/
typedef	unsigned int	vaddr_t;	/* Virtual memory address	*/
typedef	char	 	GATE[2];	/* Gate structure		*/
#if	_I386
typedef	long		cseg_t;		/* Page descriptor		*/
#else
typedef	long		faddr_t;	/* Far virtual memory address	*/
typedef	unsigned int	saddr_t;	/* Segmenation address		*/
#endif

#endif
