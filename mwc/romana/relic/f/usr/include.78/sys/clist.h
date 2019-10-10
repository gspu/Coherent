/*
 * Character list.
 */
#ifndef	 __SYS_CLIST_H__
#define	 __SYS_CLIST_H__
#include <sys/types.h>
#ifdef _I386
#include <sys/reg.h>
#else
#include <sys/machine.h>
#endif

/*
 * NIGEL: Whatever a "cmap_t" is, the definition belongs here, not in
 * <sys/types.h>
 */

typedef	unsigned int	cmap_t;


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
