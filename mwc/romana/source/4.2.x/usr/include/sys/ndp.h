/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_NDP_H__
#define __SYS_NDP_H__

#if	_KERNEL

/* bit positions in u.u_ndpFlags */
#define NF_NDP_USER	1	/* this process has used the ndp */
#define NF_NDP_SAVED	2	/* ndp status is saved in u area */
#define NF_EM_TRAPPED	4	/* no ndp, em trap has occurred */

/* supported coprocessor types - will autosense if initially unpatched */
#define NDP_TYPE_AUTO		0
#define NDP_TYPE_NONE		1
#define NDP_TYPE_287		2
#define NDP_TYPE_387		3
#define NDP_TYPE_486		4

#define NDP_IRQ		13	/* 387 uses Irq for unmasked exceptions */
#define NDP_PORT	0xF0	/* 387 uses this port to clear exception */

#endif /* _KERNEL */

#endif /* ! defined (__SYS_NDP_H__) */
