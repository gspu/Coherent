/* (-lgl
 * 	COHERENT Version 4.0
 *	Copyright 1984, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/io.h
 *
 * I/O template.
 *
 * Revised: Wed Apr  7 15:09:53 1993 CDT
 */

#ifndef	 __SYS_IO_H__
#define	 __SYS_IO_H__

#include <common/feature.h>
#include <common/__paddr.h>
#include <sys/types.h>

/*
 * Structure used to store parameters for I/O.
 */
typedef struct io {
	int	 io_seg;		/* Space */
	unsigned io_ioc;		/* Count */
#if	_I386
	off_t	 io_seek;		/* Seek posiion */
	union {
		caddr_t	vbase;		/* Virtual base */
		__paddr_t pbase;	/* Physical base */
	} io;
#else
	fsize_t	 io_seek;		/* Seek posiion */
	char	 *io_base;		/* Virtual base */
	__paddr_t	io_phys;	/* Physical base */
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
 * No delay if results are not immediately available. IONDLY is the funky
 * internal name for O_NDELAY and IONONBLOCK is the version for O_NONBLOCK.
 */
#define	IONDLY		8
#define	IONONBLOCK	16

#endif	/* ! defined (__SYS_IO_H__) */
