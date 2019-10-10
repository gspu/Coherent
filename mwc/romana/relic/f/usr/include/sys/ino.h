/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Disk inode.
 */

#ifndef	 __SYS_INO_H__
#define	 __SYS_INO_H__

#include <sys/types.h>
#include <common/_time.h>


/*
 * Structure of inode as it appears on disk.
 */
struct dinode {
	unsigned short di_mode;		/* Mode */
	short	 di_nlink;		/* Link count */
	short	 di_uid;		/* User id of owner */
	short	 di_gid;		/* Group id of owner */
	fsize_t	 di_size;		/* Size of file in bytes */

	union dia_u {
		 char	 di_addb[40];	/* Disk block addresses */
		 dev_t	 di_rdev;	/* Device */

		 struct dip_s {		/* Pipes */
			 char	 dp_addp[30];
			 short	 dp_pnc;
			 short	 dp_prx;
			 short	 dp_pwx;
		} di_p;
	} di_a;

	time_t	 di_atime;		/* Last access time */
	time_t	 di_mtime;		/* Last modify time */
	time_t	 di_ctime;		/* Last creation time */
};

/*
 * Compatibility.
 */
#define	di_addr	di_a.di_addb
#define di_addp	di_a.di_p.dp_addp
#define di_pnc	di_a.di_p.dp_pnc
#define di_prx	di_a.di_p.dp_prx
#define di_pwx	di_a.di_p.dp_pwx

/*
 * Miscellaneous manifests.
 */
#define	NBN	128			/* Number of indirects per block */
#define L2NBN	7			/* Log2(NBN) */
#define ND	10			/* Number of direct blocks */
#define NI	3			/* Number of indirect blocks */
#define NADDR	13			/* Total number of addresses */

/*
 * Modes.
 */
#define	IFMT	0170000			/* Type */
#define IFDIR	0040000			/* Directory */
#define IFCHR	0020000			/* Character special file */
#define	IFBLK	0060000			/* Block special file */
#define IFREG	0100000			/* Regular file */
#define IFMPC	0030000			/* Multiplexed character special */
#define IFMPB	0070000			/* Multiplexed block special */
#define IFPIPE	0010000			/* Pipe */
#define ISUID	0004000			/* Set user id on execution */
#define ISGID	0002000			/* Set group id on execution */
#define ISVTXT	0001000			/* Save swapped text even after use */

#endif
