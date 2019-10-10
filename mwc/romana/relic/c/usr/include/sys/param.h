/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef PARAM_H
#define	PARAM_H
/*
 * All of the parameters
 * in this file may be edited,
 * but watch out that you do not
 * run out of virtual address
 * space in the system.
 * Some parameters are now adjustably set in con.c
 */
#define NDRV	32			/* Number of major device entries */
#define NUFILE	20			/* Number of user open files */
#define	NEXREAD	6			/* Read ahead */

#ifdef KERNEL
				/* These set in *con.c */
extern	int	NBUF;			/* Now an adjustable parameter */
extern	int	NINODE;			/* Now an adjustable parameter */
extern	int	NCLIST;			/* Now an adjustable parameter */
extern	int	ALLSIZE;		/* Now an adjustable paramenter */
extern	int	ISTSIZE;		/* Initial stack size */

#endif

#endif
