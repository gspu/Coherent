/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

/*
 * "/etc/mount" and "/etc/umount" maintain the mount table "/etc/mnttab",
 * which is an array of these structures.
 */

#ifndef __MNTTAB_H__
#define	__MNTTAB_H__

#include <common/__time.h>

#define	MNTNSIZ	32		/* Size of a mount filename	*/

struct	mnttab {
	char		mt_dev [MNTNSIZ];	/* Directory		*/
	char		mt_filsys [MNTNSIZ];	/* Special file		*/
	int		mt_ro_flg;		/* Read only flag	*/
	__time_t	mt_time;		/* Time mounted		*/
};

#endif	/* ! defined (__MNTTAB_H__) */
