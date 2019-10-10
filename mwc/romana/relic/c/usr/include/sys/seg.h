/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Segments.
 */

#ifndef	 SEG_H
#define	 SEG_H	SEG_H

#include <sys/types.h>

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
	faddr_t	 s_faddr;		/* Memory access selector */
	fsize_t	 s_size;		/* Size in bytes */
	paddr_t  s_paddr;		/* Physical base address */
	daddr_t	 s_daddr;		/* Disk base address */
} SEG;

/*
 * Flags (s_flags).
 */
#define SFCORE	0000001			/* Memory resident */
#define	SFDOWN	0000002			/* Segment grows downward */
#define SFSHRX	0000004			/* Shared segment */
#define SFTEXT	0000010			/* Text segment */
#define SFHIGH	0000020			/* Allocate segment from high end */
#define	SFSYST	0000040			/* System segment */

/*
 * Pseudo flags.  (passed to salloc).
 */
#define	SFNSWP	0040000			/* Don't swap */
#define SFNCLR	0100000			/* Don't clear segment */

#ifdef KERNEL
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

#endif

#ifdef KERNEL
/*
 * Global variables.
 */
extern	int	sexflag;		/* Swapper existant */
extern	GATE	seglink;		/* Gate for s_forw and s_back */
extern	SEG	segswap;		/* Segments reserved for the swapper */
extern	SEG	segmq;			/* Memory segment queue */
extern	SEG	segdq;			/* Segment disk queue */
extern	SEG	segiom;			/* I/O memory segment */

#endif

#endif
