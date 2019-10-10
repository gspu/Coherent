/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure for the mount table maintained by
 * `/etc/mount' and `/etc/umount'.
 * The file `/etc/mtab' is an array of these structures.
 */

#ifndef	MTAB_H
#define	MTAB_H	MTAB_H

#define	MNAMSIZ	32		/* Size of a mount filename */

struct	mtab {
	char	mt_name[MNAMSIZ];
	char	mt_special[MNAMSIZ];
	int	mt_flags;
};

#endif
