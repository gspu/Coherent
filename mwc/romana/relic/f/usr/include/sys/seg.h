/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/seg.h
 *
 * Segments.
 *
 * Revised: Mon May 24 15:50:18 1993 CDT
 */

#ifndef	 __SYS_SEG_H__
#define	 __SYS_SEG_H__

#include <common/feature.h>
#include <common/__paddr.h>
#include <sys/types.h>
#include <sys/ksynch.h>

#if	! __KERNEL__
# error	You must be compiling the kernel to use this header
#endif

/*
 * NIGEL: for some reason the type "cseg_t" was in <sys/types.h>. It belongs
 * here and in <sys/mmu.h> as much as it belongs anywhere.
 */

#ifndef	__CSEG_T
#define	__CSEG_T
typedef	long		cseg_t;
#endif

/*
 * Segment structure.
 */
typedef struct seg {
	struct	 seg * s_forw;		/* Forward pointer */
	struct	 seg * s_back;		/* Backward pointer */
	struct	 inode * s_ip;		/* Inode pointer for shared text */
	short	 s_flags;		/* Flags */
	short	 s_urefc;		/* Reference count of segment */
	short	 s_lrefc;		/* Lock reference count */
#if	_I386
	off_t	 s_size;		/* Size in bytes */
	cseg_t	 *s_vmem;		/* page table address */
#else
	faddr_t	 s_faddr;		/* Memory access selector */
	fsize_t	 s_size;		/* Size in bytes */
	__paddr_t  s_paddr;		/* Physical base address */
#endif
	daddr_t	 s_daddr;		/* Disk base address */
} SEG;

/*
 * Flags (s_flags).
 */
#define SFCORE	0x0001			/* Memory resident */
#define	SFDOWN	0x0002			/* Segment grows downward */
#define SFSHRX	0x0004			/* Shared segment */
#define SFTEXT	0x0008			/* Text segment */
#define SFHIGH	0x0010			/* Text segment */
#define	SFSYST	0x0020			/* System segment */

/*
 * Pseudo flags.  (passed to salloc).
 */
#define	SFNSWP	0x4000			/* Don't swap */
#define SFNCLR	0x8000			/* Don't clear segment */

/*
 * Functions.
 */
extern	SEG	*iomapvp();		/* bio.c */
extern	SEG	*segdupl();		/* seg.c */
extern	SEG	*ssalloc();		/* seg.c */
extern	SEG	*salloc();		/* seg.c */
extern	SEG	*segsext();		/* seg.c */
extern	SEG	*segdupd();		/* seg.c */
extern	SEG	*sdalloc();		/* seg.c */
extern	SEG	*smalloc();		/* seg.c */
extern	SEG	*shalloc();		/* seg.c */
extern	SEG	*exaread();		/* exec.c */
extern	SEG	*exsread();		/* exec.c */
extern	SEG	*exstack();		/* exec.c */

/*
 * Global variables.
 */
extern	int	sexflag;		/* Swapper existant */
extern	GATE	seglink;		/* Gate for s_forw and s_back */
extern	SEG	segswap;		/* Segments reserved for the swapper */
extern	SEG	segmq;			/* Memory segment queue */
extern	SEG	segdq;			/* Segment disk queue */
extern	SEG	segiom;			/* I/O memory segment */

/*
 * Open segment structure.
 */
typedef struct sr {
	int	 sr_flag;		/* Flags for this reference */
	caddr_t	 sr_base;		/* Virtual address base */
#if	_I386
	off_t	 sr_size;		/* Mapped in window size */
#else
	vaddr_t	 sr_size;		/* Mapped in window size */
#endif
	struct	 seg *sr_segp;		/* Segment pointer */
} SR;

/*
 * Flags (sr_flag).
 */
#define SRFPMAP	0x01			/* Segment is mapped in process */
#define SRFDUMP	0x02			/* Dump segment */
#define	SRFDATA	0x04			/* Data segment */
#define	SRFRODT	0x08			/* Data, read-only (used by shm) */
#define	SRFBERM	0x10			/* Segment to be removed (used by shm) */

#endif	/* ! defined (__SYS_SEG_H__) */
