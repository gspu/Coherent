/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Stat.
 */

#ifndef	 OSTAT_H
#define	 OSTAT_H	OSTAT_H

#include <sys/types.h>

/*
 * Structure returned by stat and fstat system calls.
 */
#pragma align 2
struct oldstat {
	dev_t	 st_dev;		/* Device */
	ino_t	 st_ino;		/* Inode number */
	unsigned short st_mode;		/* Mode */
	short	 st_nlink;		/* Link count */
	short	 st_uid;		/* User id */
	short	 st_gid;		/* Group id */
	dev_t	 st_rdev;		/* Real device */
	long	 st_size;		/* Size */
	long	 st_atime;		/* Access time */
	long	 st_mtime;		/* Modify time */
	long	 st_ctime;		/* Change time */
};
#pragma align

/*
 * Modes.
 */
#define S_IFMT	0170000			/* Type */
#define S_IFDIR	0040000			/* Directory */
#define S_IFCHR	0020000			/* Character special */
#define S_IFBLK	0060000			/* Block special */
#define S_IFREG	0100000			/* Regular */
#define S_IFMPC	0030000			/* Multiplexed character special */
#define S_IFMPB	0070000			/* Multiplexed block special */
#define	S_IFPIP	0010000			/* Pipe */
#define	S_ISUID	0004000			/* Set user id on execution */
#define S_ISGID	0002000			/* Set group id on execution */
#define	S_ISVTX	0001000			/* Save swapped text even after use */
#define S_IREAD	0000400			/* Read permission, owner */
#define S_IWRITE 000200			/* Write permission, owner */
#define S_IEXEC	0000100			/* Execute/search permission, owner */

/*
 * Non existant device.
 */
#define NODEV	(-1)

/*
 * Functions.
 */
#define	major(dev)	((dev>>8)&0377)
#define minor(dev)	(dev&0377)
#define makedev(m1, m2)	((m1<<8)|m2)

#endif
