/* (-lgl
 * 	COHERENT Version 3.0
 *	Copyright 1989 by Inetco Systems, Ltd.
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
	fsize_t	 io_seek;		/* Seek posiion */
	char	 *io_base;		/* Virtual base */
	paddr_t	 io_phys;		/* Physical base */
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
