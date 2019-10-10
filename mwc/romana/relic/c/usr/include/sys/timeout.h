/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Timeout queue header.
 */
#ifndef	 TIMEOUT_H
#define	 TIMEOUT_H
#include <sys/types.h>
#include <sys/machine.h>

/*
 * Timer queue.
 */
typedef struct tim {
	struct	 tim *t_next;		/* Pointer to next */
	struct   tim *t_last;		/* Pointer to last */
	long	 t_lbolt;		/* Clock tick at which timeout occurs */
	int	 (*t_func)();		/* Function to be called */
	char	 *t_farg;		/* Argument */
	faddr_t	  t_ldrv;		/* Loadable driver function */
} TIM;

#ifdef	 KERNEL
/*
 * Global variables.
 */
extern	TIM *	timq[256];		/* Timer queues */

#endif

#endif
