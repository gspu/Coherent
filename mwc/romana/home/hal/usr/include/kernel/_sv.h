/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL__SV_H__
#define	__KERNEL__SV_H__

/*
 * This internal header file defines the internal data type "__dv_t".
 * It is equivalent to the synchronization variable type "sv_t", but given
 * an internal name so that header files can refer to it without exporting
 * its name into the user's namespace.
 */

typedef struct synch_var	__sv_t;

#endif	/* ! defined (__KERNEL__SV_H__) */

