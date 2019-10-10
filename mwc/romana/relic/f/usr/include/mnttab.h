/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/*
 * /usr/include/mnttab.h
 * "/etc/mount" and "/etc/umount" maintain the mount table "/etc/mnttab",
 * which is an array of these structures.
 */

#ifndef __MNTTAB_H__
#define	__MNTTAB_H__

#include <sys/types.h>

#define	MNTNSIZ	32		/* Size of a mount filename	*/

struct	mnttab {
	char	mt_dev[MNTNSIZ];	/* Directory		*/
	char	mt_filsys[MNTNSIZ];	/* Special file		*/
	int	mt_ro_flg;		/* Read only flag	*/
	time_t	mt_time;		/* Time mounted		*/
};

#endif

/* end of /usr/include/mnttab.h */
