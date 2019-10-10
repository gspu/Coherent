/* (-lgl
 * 	COHERENT Version 3.2.2
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/rlock.h
 * Record locking.
 */

#ifndef _RLOCK_H
#define _RLOCK_H

/*
 * Internal record lock.
 * Active locks are attached to inode.
 */
typedef struct	rlock	{
	struct	rlock	*rl_next;	/* link, must be first		*/
	int		rl_type;	/* 0==read, 1==write		*/
	long		rl_start;
	long		rl_end;		/* -1 is EOF			*/
	PROC		*rl_proc;	/* owner's process		*/
} RLOCK;

/* Pending record lock. */
typedef struct	prlock	{
	struct	prlock	*prl_next;	/* link, must be first		*/
	RLOCK		*prl_rls;	/* active locks on desired inode */
	RLOCK		*prl_rl;	/* desired lock			*/
} PRLOCK;

extern	RLOCK	*freerl;		/* list of free record locks	*/
extern	PRLOCK	*pendrl;		/* list of all pending locks	*/
extern	GATE	rlgate;			/* only one proc changes locks at a time */

#endif

/* end of sys/rlock.h */
