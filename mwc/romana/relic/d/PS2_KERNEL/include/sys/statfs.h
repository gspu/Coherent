/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/*
 * Structure for "generic superblock".
 * This structure is used by statfs and fstatfs system calls.
 */

#ifndef	STATFS_H
#define STATFS_H

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

#endif
