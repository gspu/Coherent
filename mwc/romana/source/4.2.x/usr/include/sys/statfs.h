/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_STATFS_H__
#define __SYS_STATFS_H__

/*
 * Structure for "generic superblock".
 * This structure is used by statfs and fstatfs system calls.
 */

struct statfs {
	short	f_fstyp;	/* File system type. 1 for 512 bytes blocks */
	long	f_bsize;	/* Block size */
	long	f_frsize;	/* Fragment size */
	long	f_blocks;	/* Total number of blocks */
	long	f_bfree;	/* Count of free blocks */
	long	f_files;	/* Total number of file nodes */
	long	f_ffree;	/* Count of free file nodes */
	char	f_fname[6];	/* Volume name */
	char	f_fpack[6];	/* Pack name */
};

#endif	/* ! defined (__SYS_STATFS_H__) */
