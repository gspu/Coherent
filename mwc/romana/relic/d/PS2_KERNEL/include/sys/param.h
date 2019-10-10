/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef PARAM_H
#define	PARAM_H

/* WARNING!  Fix references to dev_loaded if ever NDRV > 8*sizeof(int) */
#define NDRV	32			/* Number of major device entries */
#define NOFILE	20			/* Number of user open files */
#define NUFILE	NOFILE			/* For historical reasons */
#define	NEXREAD	6			/* Read ahead */

#ifdef KERNEL			/* These set in *con.c */
extern	int	NBUF;			/* Now an adjustable parameter */
extern	int	NHASH;			/* Now an adjustable parameter */
extern	int	NINODE;			/* Now an adjustable parameter */
extern	int	NCLIST;			/* Now an adjustable parameter */
extern	int	ALLSIZE;		/* Now an adjustable paramenter */
extern	int	ISTSIZE;		/* Initial stack size (bytes) */
#endif

#ifdef _I386
#define	BPCSHIFT	12
#define	BPC1SHIFT	10
#define	BPSSHIFT	22
#define	NBPC		0x001000
#define	NBPS		0x400000

#define	ISP_286		0x10000
#define	ISP_386		stob(512)

#define	UPASIZE		4096	/* Size in bytes of user area */

#define	SYI86UNEEK	333

#define	MSACOUNT	6	/* Number of ints for system call args */
#define	ISTVIRT		0		/* Stack virtual base for sys exec */
#define	SOVSIZE		0		/* Stack overflow size */
#define MADSIZE		32767		/* Maximum addressable segment size */
#define SMICALL		0		/* Start of independent system calls */
#define NMICALL		88		/* Machine independent system calls */
#define COHCALL		500		/* Number of COH system call */
#define SMDCALL		128		/* Start of dependent system calls */
#define NMDCALL		0		/* Machine dependent system calls */
#define	BSIZE		512		/* Buffer size */
#define SCHUNK		16384		/* I/O chunk size for swap and dump */
#define	NCPCL		124		/* Number of characters in clist */
#define	NPID		30000		/* Maximum process id */
#define	MAXU		((unsigned)0177777)

#define COH_PRINTF	1
#define COH_DEVLOAD	2
#define COH_SETFPE	3

/*
 * There must be an even number of HZ in a second.
 */
#define	HZ		100		/* 8253 ticks per second */
#endif

#endif
