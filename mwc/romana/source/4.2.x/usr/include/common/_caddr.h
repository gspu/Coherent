/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__CADDR_H__
#define	__COMMON__CADDR_H__

/*
 * This header defines the SVR4 data type "caddr_t".  This type represents
 * a general pointer type that is suitable for representing any legal address
 * within user space or kernel space.
 *
 * The sizeof () the type pointed at by this type must be 1.
 */

typedef	char	      *	caddr_t;

#endif	/* ! defined (__COMMON__CADDR_H__) */

