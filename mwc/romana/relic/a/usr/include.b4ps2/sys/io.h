/* (-lgl
 * 	COHERENT Version 4.0
 *	Copyright 1984, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * I/O template.
 */

#ifndef	 IO_H
#define	 IO_H	IO_H

#include <sys/types.h>

/*
 * Structure used to store parameters for I/O.
 */
typedef struct io {
	int	 io_seg;		/* Space */
	unsigned io_ioc;		/* Count */
#ifdef _I386
	off_t	 io_seek;		/* Seek posiion */
	union {
		vaddr_t	vbase;		/* Virtual base */
		paddr_t	pbase;		/* Physical base */
	} io;
#else
	fsize_t	 io_seek;		/* Seek posiion */
	char	 *io_base;		/* Virtual base */
	paddr_t	 io_phys;		/* Physical base */
#endif
	short	 io_flag;		/* Flags: 0, IONDLY */
} IO;

/*
 * Types of space I/O operation is being performed from.
 */
#define IOSYS	0			/* System */
#define IOUSR	1			/* User */
#define IOPHY	2			/* Physical */

/*
 * No delay if results are not immediately available.
 */
#define	IONDLY	8

#endif
