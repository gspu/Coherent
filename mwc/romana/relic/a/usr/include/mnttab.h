/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure for the mount table maintained by
 * '/etc/mount' and '/etc/umount'.
 * The file '/etc/mtab' is an array of these structures.
 */

#ifndef MNTTAB_H
#define	MNTTAB_H	MNTTAB_H

#include <sys/types.h>

#define	MNTNSIZ	32		/* Size of a mount filename */

struct	mnttab {
	char	mt_dev[MNTNSIZ];	/* Directory */
	char	mt_filsys[MNTNSIZ];	/* Special file */
	int	mt_ro_flg;		/* Read only flag */
	time_t	mt_time;		/* Time mounted */
};

#endif
