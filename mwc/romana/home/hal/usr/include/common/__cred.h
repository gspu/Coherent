/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON___CRED_H__
#define	__COMMON___CRED_H__

/*
 * This internal header file defines the internal data type "__cred_t",
 * from which the System V DDI/DKI type "cred_t" is derived.
 */

#include <common/_uid.h>


/*
 * Credentials structure. It contains all the credentials-related
 * information about the user.
 */

typedef	struct __cred	__cred_t;

#endif	/* ! defined (__COMMON___CRED_H__) */
