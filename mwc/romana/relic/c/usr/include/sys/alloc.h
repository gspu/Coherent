/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Allocator.
 */
#ifndef	 ALLOC_H
#define	 ALLOC_H

/*
 * Structure for allocator.
 */
typedef struct all {
	union all_u {
		char	*au_link;
		char	au_free[2];
	}	a_union;
	char	a_data[];
} ALL;

/*
 * Macros to transparently access allocator union.
 */
#define	a_link	a_union.au_link
#define	a_free	a_union.au_free

#if 0
/*
 * Portable defines for the allocator.
 */
#define align(p)	((ALL *)NULL + ((p) - (ALL *)NULL))
#define link(p)		(align((p)->a_link))
#define	tstfree(p)	((p)->a_link == (char *) link(p))
#define setfree(p)	((p)->a_link = (char *) link(p))
#define setused(p)	((p)->a_link = (char *) link(p) + 1)

#endif

#ifdef	KERNEL
/*
 * Functions and externals.
 */
extern	char	*alloc();
extern	ALL	*setarena();

#endif

#endif
