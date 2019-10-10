#ifndef	__SYS_UTIME_H__
#define	__SYS_UTIME_H__

/*
 * This header is defined in the POSIX.1 standard ISO/IEC 9945-1:1990, and as
 * such client programs which include this header should not use any symbols
 * which end in "_t".
 */

/*
 * The contents of this header are also defined by the provisions of binary
 * compatibility standards such as the iBCS2 specification and the System V
 * ABI. The visibility of certain symbols defined by those standards may be
 * controlled by feature-test macros such as _SYSV3 for iBCS2, and _SYSV4 for
 * System V, Release 4.
 */

#include <sys/ccompat.h>
#include <sys/__time.h>

/*
 * Note that this header imports <sys/__time.h> rather than <sys/_time.h>,
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

#endif	/* ! defined (__SYS_UTIME_H__) */
