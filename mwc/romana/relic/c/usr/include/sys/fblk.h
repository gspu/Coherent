/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Disk free block.
 */

#ifndef	 FBLK_H
#define	 FBLK_H	FBLK_H

#include <sys/types.h>

/*
 * Number of free blocks in free list.
 */
#define NICFREE	64

/*
 * Free list block structure.
 */
struct fblk {
	short	 df_nfree;		/* Number of free blocks */
	daddr_t	 df_free[NICFREE];	/* Free blocks */
};

#endif
