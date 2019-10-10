/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_DIR_H__
#define	__KERNEL_DIR_H__

#include <common/_uid.h>

#if	! DIRSIZ
# define	DIRSIZ		14
#endif

struct direct {
	o_ino_t		d_ino;
	char		d_name [DIRSIZ];
};

#endif	/* ! defined (__KERNEL_DIR_H__) */
