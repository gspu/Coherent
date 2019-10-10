/*
 * Character list.
 */
#ifndef	 CLIST_H
#define	 CLIST_H
#include <sys/types.h>
#ifdef _I386
#include <sys/reg.h>
#else
#include <sys/machine.h>
#endif

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
	int	cq_cc;			/* Character count */
	cmap_t	cq_ip;			/* Input pointer */
	int	cq_ix;			/* Input index */
	cmap_t	cq_op;			/* Output pointer */
	int	cq_ox;			/* Output index */
} CQUEUE;

#ifdef KERNEL
extern	int	cltwant;		/* A wanted flag */
extern	cmap_t	cltfree;		/* Character free list */

#endif

#endif
