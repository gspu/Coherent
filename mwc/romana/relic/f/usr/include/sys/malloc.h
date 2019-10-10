/* (-lgl
 * 	COHERENT Version 4.0.2
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/malloc.h
 * Header file for malloc.c, notmem.c, realloc.c.
 */

#ifndef	__SYS_MALLOC_H__
#define	__SYS_MALLOC_H__

/* Manifest constants. */
/*
 * N.B. 1<<ARENASIZE must be greater than sizeof(struct mblock)
 * so that malloc.c/newarena() does not cause trouble when it combines arenas.
 */
#if	_I386
#define	ARENASIZE	12	/* round arena sizes up to 1<<12==4096	*/
#define	BLOCKSIZE	2	/* round block sizes up to 1<<2==4	*/
#else
#define	ARENASIZE	11	/* round arena sizes up to 1<<11==2048	*/
#define	BLOCKSIZE	1	/* round block sizes up to 1<<1==2	*/
#endif
#define	ALIGNMENT (1<<BLOCKSIZE) /* maximum alignment for any data type	*/
#define	BADSBRK   ((char *)-1)	/* sbrk() failure return value		*/
#define	DECRSIZE	4096	/* arena size decrement at end		*/
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
		unsigned usera [1];
	} uval;
} MBLOCK;

/* Globals common to malloc/realloc/memok/notmem. */
extern	unsigned __a_count;
extern	MBLOCK	*__a_scanp, *__a_first, *__a_top;

#endif	/* ! defined (__SYS_MALLOC_H__) */

