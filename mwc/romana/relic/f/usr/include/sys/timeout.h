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

#include <common/feature.h>
#include <sys/types.h>
#if	_I386
#include <sys/reg.h>
#else
#include <sys/machine.h>
#endif

#if	! __KERNEL__
# error	You must be compiling the kernel to use this header
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
#if	! _I386
	faddr_t	  t_ldrv;		/* Loadable driver function */
#endif
} TIM;

/*
 * Global variables.
 */
extern	TIM *	timq[256];		/* Timer queues */

#endif	/* ! defined (__SYS_TIMEOUT_H__) */
