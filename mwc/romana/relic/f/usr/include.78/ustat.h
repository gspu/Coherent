/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * System status structure
 */
#ifndef __USTAT_H__
#define __USTAT_H__

struct ustat {
	daddr_t	f_tfree;	/* Total free bloks */
	ino_t	f_tinode;	/* Number of free inodes */
	char	f_fname[6];	/* File system name (label) */
	char	f_fpack[6];	/* File system pack name */
};

#endif
