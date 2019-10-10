/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_PRI_H__
#define	__KERNEL_PRI_H__

/*
 * The following abstract values are used in the DDI/DKI to specify the
 * priority to be given to a process after it awakens from kernel sleep.
 * Clients are permitted to specify a relative bias of up to +/- 3 from the
 * values specified below.
 *
 * The values chosen below are such that given the bias, abstract
 * priorities can be mapped via a table into whatever concrete form is
 * desired by the scheduling algorithm.  In particular, it is not possible for
 * clients to infer anything about the relative priorities of different levels
 * given the information below.
 */

enum {
	prilo	= 3,			/* low priority */
	pritape	= 10,			/* appropriate for tape driver */
	primed	= 17,			/* medium priority */
	pritty	= 24,			/* appropriate for terminal driver */
	pridisk = 31,			/* appropriate for disk driver */
	prinet	= 38,			/* appropriate for network driver */
	prihi	= 45			/* high priority */
};

#endif	/* ! defined (__KERNEL_PRI_H__) */

