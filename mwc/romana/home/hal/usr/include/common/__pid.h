/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON___PID_H__
#define	__COMMON___PID_H__

/*
 * This internal header file defines the internal data type "__pid_t".
 * This type is equivalent to the POSIX.1 data type "pid_t" but given
 * an internal name so that header files can refer to it without exporting
 * the user-level type.
 */

typedef	long		__pid_t;

#endif	/* ! defined (__COMMON___PID_H__) */
