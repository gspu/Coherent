/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__UTIME_H__
#define	__UTIME_H__

/*
 * This header is defined in the POSIX.1 standard ISO/IEC 9945-1:1990; and, as
 * such, client programs that include it should not use any symbols
 * which end in "_t".
 */

/*
 * The contents of this header are also defined by the provisions of binary
 * compatibility standards such as the iBCS2 specification and the System V
 * ABI. The visibility of certain symbols defined by those standards may be
 * controlled by feature-test macros such as _SYSV3 for iBCS2, and _SYSV4 for
 * System V, Release 4.
 */

#include <common/ccompat.h>
#include <common/__time.h>

/*
 * Note that this header imports <common/__time.h> rather than <common/_time.h>,
 * since POSIX.1 requires that the types of the "actime" and "modtime" members
 * be "time_t", yet does not specify "time_t" as being defined by this header.
 */

struct utimbuf {
	__time_t	actime;
	__time_t	modtime;
};

__EXTERN_C_BEGIN__

int		utime		__PROTO ((__CONST__ char * _path,
					  __CONST__ struct utimbuf * _times));

__EXTERN_C_END__

#endif	/* ! defined (__UTIME_H__) */
