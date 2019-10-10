/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __USTAT_H__
#define __USTAT_H__

#include <common/__daddr.h>
#include <common/_uid.h>

struct ustat {
	__daddr_t	f_tfree;	/* Total free blocks */
	__ino_t		f_tinode;	/* Number of free inodes */
	char		f_fname[6];	/* File system name (label) */
	char		f_fpack[6];	/* File system pack name */
};

#endif	/* ! defined (__USTAT_H__) */
