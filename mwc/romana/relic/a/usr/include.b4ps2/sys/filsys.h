/* (-lgl
 * 	COHERENT Version 3.2.1
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Super block.
 */

#ifndef	 FILSYS_H
#define  FILSYS_H	FILSYS_H

#include <sys/types.h>
#include <sys/fblk.h>

/*
 * Size definitions.
 */
#define NICINOD	100			/* Number of free in core inodes */
#define BSIZE	512			/* Block size */
#define INOPB	8			/* Number of inodes per block */
#define BOOTBI	0			/* Boot block index */
#define SUPERI	1			/* Super block index */
#define INODEI	2			/* Inode block index */
#define BADFIN	1			/* Bad block inode number */
#define ROOTIN	2			/* Root inode number */

/*
 * Super block.
 */
struct filsys {
	unsigned short s_isize;		/* Firt block not in inode list */
#pragma align 2
	daddr_t	 s_fsize;		/* Size of entire volume */
#pragma align
	short	 s_nfree;		/* Number of addresses in s_free */
	daddr_t	 s_free[NICFREE];	/* Free block list */
	short	 s_ninode;		/* Number of inodes in s_inode */
	ino_t	 s_inode[NICINOD];	/* Free inode list */
	char	 s_flock;		/* Not used */
	char	 s_ilock;		/* Not used */
	char	 s_fmod;		/* Super block modified flag */
	char	 s_ronly;		/* Mounted read only flag */
#pragma align 2
	time_t	 s_time;		/* Last super block update */
	daddr_t	 s_tfree;		/* Total free blocks */
#pragma align
	ino_t	 s_tinode;		/* Total free inodes */
	short	 s_m;			/* Interleave factor */
	short	 s_n;			/* Interleave factor */
	char	 s_fname[6];		/* File system name */
	char	 s_fpack[6];		/* File system pack name */
	long	 s_unique;		/* Unique number */
};

/*
 * Functions.
 */
#define iblockn(ino)	(INODEI+(ino-1)/INOPB)
#define iblocko(ino)	((ino-1)%INOPB)

#endif
