/* (-lgl
 * 	COHERENT Version 3.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * malloc.h
 * Header file for malloc.c, notmem.c, realloc.c.
 */

#ifndef	MALLOC_H
#define	MALLOC_H

/* Manifest constants. */
/*
 * N.B. 2^ARENASIZE must be greater than sizeof(struct mblock)
 * so that malloc.c/newarena() does not cause trouble when it combines arenas.
 */
#define	ARENASIZE	11	/* round arena sizes up to 2^11==2048	*/
#define	BADSBRK   ((char *)-1)	/* sbrk() failure return value		*/
#define	BLOCKSIZE	1	/* round block sizes up to 2^1==2	*/
#define	DECRSIZE	256	/* arena size decrement at end		*/
#define	FREE		1	/* free block marker			*/
#define	LEASTFREE (2*sizeof(unsigned))	/* free nothing smaller		*/

/* Macros. */
#define	bumpp(p,n)	(((char *)(p))+(n))	/* bump pointer p by n bytes */
#define	isfree(x)	(((unsigned)(x))&FREE)
#define	mblockp(p)	((char *)(p)-sizeof(unsigned))	/* convert malloc ptr */
#define	realsize(x)	((x) & ~FREE)
#define	roundup(i,j)	(((i) + (1 << (j)) - 1) & ~((1 << (j)) - 1))

/* Memory block structure. */
typedef	struct	mblock	{
	unsigned blksize;
	union {
		char	 *next;
		unsigned usera[];
	} uval;
} MBLOCK;

/* Globals common to malloc/realloc/memok/notmem. */
extern	unsigned __a_count;
extern	MBLOCK	*__a_scanp, *__a_first;

#endif
