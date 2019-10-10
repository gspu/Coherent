/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Timeout queue header.
 */
#ifndef	 __SYS_TIMEOUT_H__
#define	 __SYS_TIMEOUT_H__

#include <sys/types.h>
#ifdef _I386
#include <sys/reg.h>
#else
#include <sys/machine.h>
#endif

/*
 * Timer queue.
 */
typedef struct tim {
	struct	 tim *t_next;		/* Pointer to next */
	struct   tim *t_last;		/* Pointer to last */
	long	 t_lbolt;		/* Clock tick at which timeout occurs */
	int	 (*t_func)();		/* Function to be called */
	char	 *t_farg;		/* Argument */
#ifndef _I386
	faddr_t	  t_ldrv;		/* Loadable driver function */
#endif
} TIM;

#ifdef	 KERNEL
/*
 * Global variables.
 */
extern	TIM *	timq[256];		/* Timer queues */

#endif

#endif
