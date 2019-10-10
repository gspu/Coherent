/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __KERNEL_TIMEOUT_H__
#define	 __KERNEL_TIMEOUT_H__

#include <common/feature.h>
#include <sys/types.h>

/*
 * Timer queue.
 */

typedef struct tim {
	struct	 tim *t_next;		/* Pointer to next */
	struct   tim *t_last;		/* Pointer to last */
	long	 t_lbolt;		/* Clock tick at which timeout occurs */
	int	 (*t_func)();		/* Function to be called */
	char	 *t_farg;		/* Argument */
} TIM;

/*
 * Global variables.
 */
extern	TIM *	timq [32];		/* Timer queues */

#endif	/* ! defined (__KERNEL_TIMEOUT_H__) */
