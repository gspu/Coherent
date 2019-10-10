/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Some constants.
 * Intel 8086/8088 machines.
 */

#ifndef	__SYS_CONST_H__
#define	__SYS_CONST_H__

#define	MSASIZE		14		/* Space reserved for sys args */
#define	ISTVIRT		0		/* Stack virtual base for sys exec */
#define	SOVSIZE		0		/* Stack overflow size */
#define	UPASIZE		1024		/* Size of user area */
#define MADSIZE		32767		/* Maximum addressable segment size */
#define SMICALL		0		/* Start of independent system calls */
#define NMICALL		74		/* Machine independent system calls */
#define SMDCALL		128		/* Start of dependent system calls */
#define NMDCALL		0		/* Machine dependent system calls */
#define	BSIZE		512		/* Buffer size */
#define SCHUNK		16384		/* I/O chunk size for swap and dump */
#define	NCPCL		126		/* Number of characters in clist */
#define	NPID		30000		/* Maximum process id */
#define	MAXU		((unsigned)0177777)

/*
 * There must be an even number of HZ in a second.
 */
#define	HZ		100		/* 8253 ticks per second */

#endif
