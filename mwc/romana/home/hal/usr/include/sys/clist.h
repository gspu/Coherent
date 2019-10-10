/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __SYS_CLIST_H__
#define	 __SYS_CLIST_H__

#include <common/feature.h>
#include <kernel/reg.h>

#if	! _KERNEL
# error	You must be compiling the kernel to use this header
#endif

typedef	unsigned int	cmap_t;

#define	NCPCL		124		/* Number of characters in clist */

/*
 * Character list structure.
 */

typedef struct clist {
	cmap_t	cl_fp;			/* Pointer to next */
	char	cl_ch[NCPCL];		/* Characters */
} CLIST;


/*
 * Character queue structure.
 */

typedef struct cqueue {
	int		cq_cc;		/* Character count */
	cmap_t		cq_ip;		/* Input pointer */
	int		cq_ix;		/* Input index */
	cmap_t		cq_op;		/* Output pointer */
	int		cq_ox;		/* Output index */
} CQUEUE;

#endif	/* ! defined (__SYS_CLIST_H__) */
