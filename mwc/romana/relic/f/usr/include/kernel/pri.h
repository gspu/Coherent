#ifndef	__KERNEL_PRI_H__
#define	__KERNEL_PRI_H__

/*
 * The following abstract values are used in the DDI/DKI for specifying the
 * priority to be given to processes after awakening from kernel sleep.
 * Clients are permitted to specify a relative bias of up to +/- 3 from the
 * values specified below.
 *
 * The actual values chosen below are such that given the bias, abstract
 * priorities can be mapped via a table into whatever concrete form is
 * desired by the scheduling algorithm. In particular, it is not possible for
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

