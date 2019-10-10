#ifndef	__KERNEL__DEFER_H__
#define	__KERNEL__DEFER_H__

/*
 * This header file is intended as the sole point of definition for the
 * internal data type "deffuncp_t". This definition is imported by several
 * kernel headers which should otherwise remain disjoint.
 */

#include <common/ccompat.h>

typedef	void	     (*	__deffuncp_t)	__PROTO ((void));

#endif	/* ! defined (__KERNEL__DEFER_H__) */
