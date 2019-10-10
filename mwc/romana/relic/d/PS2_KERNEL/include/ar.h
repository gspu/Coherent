/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * This is the format of the header at the start of every archive member.
 * This is not the same as V7.
 * To prevent confusion the magic number is different.
 */

#ifndef	AR_H
#define	AR_H

#ifndef DIRSIZ
#include <sys/dir.h>
#endif

#define ARMAG	0177535			/* Magic number */

struct	ar_hdr {
	char	ar_name[DIRSIZ];	/* Member name */
	time_t	ar_date;		/* Time inserted */
	short	ar_gid;			/* Group id */
	short	ar_uid;			/* User id */
	short	ar_mode;		/* Mode */
	fsize_t	ar_size;		/* File size */
};
/*
 * Name of header module for ranlib
 */
#ifndef	HDRNAME
#define	HDRNAME "__.SYMDEF"
#endif
/*
 * Header module is list of all global defined symbols
 * in all load modules
 */
#ifndef	L_OUT_H
#include <n.out.h>
#endif
typedef	struct	ar_sym {
	char	ar_id[NCPLN];		/* symbol name */
	fsize_t	ar_off;			/* offset of load module */
} ar_sym;				/* ...from end of header module */

#endif
