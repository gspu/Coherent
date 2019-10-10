/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL__TOID_H__
#define	__KERNEL__TOID_H__

/*
 * This internal header file defines the DDI/DKI data type 'toid_t'. This
 * type is used as a handle (or 'cookie') for timeout events so that they
 * can be cancelled after being scheduled.  The format of the handle data
 * is opaque.
 *
 * Handles are never explicitly released in the DDI/DKI, so using a pointer to
 * an event cell may not be a good idea (that's the theory; in practice, most
 * clients of the DDI/DKI timeout system do not use a timeout handle once
 * the timeout has run).
 *
 * Because clients need to pass these items as parameters and assign them to
 * variables, a pointer to an incomplete type would be a natural
 * representation.  However, since a pointer is inappropriate here, we use a
 * large scalar type.  Clients are cautioned not to perform any operation
 * other than assignment on such items, however.
 */

typedef	unsigned long	toid_t;


/*
 * The maximum-value defined below can't be used without <limits.h>, but we
 * define it here because it depends critically on the type of toid_t.
 */

#define	TOID_MAX	ULONG_MAX

#endif	/* ! defined (__KERNEL__TOID_H__) */

