/*
 * /usr/include/sys/utsname.h
 *
 * Revised: Wed May 19 10:29:48 1993 CDT
 */
#ifndef	__SYS_UTSNAME_H__
#define	__SYS_UTSNAME_H__

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

#include <common/feature.h>
#include <common/ccompat.h>


#if	_SYSV4 && ! _SYSV3
# define	__SYS_NMLN	257
#else
# define	__SYS_NMLN	9
#endif


#if	!defined(_POSIX_SOURCE)
# define	SYS_NMLN	__SYS_NMLN
#endif

struct utsname {
	char		sysname [__SYS_NMLN];
	char		nodename [__SYS_NMLN];
	char		release [__SYS_NMLN];
	char		version [__SYS_NMLN];
	char		machine [__SYS_NMLN];
};

__EXTERN_C_BEGIN__

int		uname		__PROTO ((struct utsname * _name));

__EXTERN_C_END__

#endif	/* ! defined (__SYS_UTSNAME_H__) */
