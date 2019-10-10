/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__PID_H__
#define	__COMMON__PID_H__

/*
 * This internal header defines the POSIX.1 data type "pid_t".  We simply
 * rely on another internal definition of a private name, which exists so
 * that headers can declare a "pid_t" datum without exporting the "pid_t"
 * type into the user's namespace.
 */

#include <common/__pid.h>

typedef	__pid_t		pid_t;

#endif	/* ! defined (__COMMON__PID_H__) */
