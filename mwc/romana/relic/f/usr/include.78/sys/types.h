/*
 * /usr/include/sys/types.h
 *
 * Revised: Wed May 19 09:31:05 1993 CDT
 */

/*
 * This header is defined in the POSIX.1 standard ISO/IEC 9945-1:1990, and as
 * such client programs which include this program may not use any symbols
 * which end in "_t".
 */

/*
 * The contents of this header are also defined by the provisions of binary
 * compatibility standards such as the iBCS2 specification and the System V
 * ABI. The visibility of certain symbols defined by those standards may be
 * controlled by feature-test macros such as _SYSV3 for iBCS2, and _SYSV4 for
 * System V, Release 4.
 *
 * System headers which depend on this file for definitions should take care
 * when using symbols that have different definitions under the _SVR3 and
 * _SVR4 environments. This header defines both versions under internal names
 * beginning with o_ for SVR3 and n_ for SVR4, and then uses one of those
 * definitions as the base for the external definition.
 */

/*
 * Other symbols may be defined in this header according to the provisions of
 * the System V, Release 4 Multiprocessor DDI/DKI, which defines additional
 * symbols. The visibility of symbols unique to the DDI/DKI is controlled by
 * the _DDI_DKI feature-test macro. Headers in the DDI/DKI are part of the
 * kernel environment, and should not depend on the _SVR3 or _SVR4 feature-
 * tests.
 *
 * The rationale for all of this is to ease the transition between providing
 * an iBCS2 service (which is not, and indeed cannot be POSIX-compliant) and
 * an SVR4 service (which provides a POSIX-compliant service using interfaces
 * that are not binary compatible). Kernel code must use a single consistent
 * set of data declarations even when attempting to provide a service which
 * uses conflicting definitions.
 */
#ifndef	__SYS_TYPES_H__
#define	__SYS_TYPES_H__

typedef	unsigned short	o_dev_t;
typedef	unsigned long	n_dev_t;

typedef	short		o_nlink_t;
typedef	unsigned long	n_nlink_t;

typedef	unsigned short	o_ino_t;
typedef	unsigned long	n_ino_t;


/*
 * "ssize_t" and "size_t" are defined by several other headers. We appeal to
 * the common definition.  Here we also pull in internal definitions that
 * are kept in special system headers.
 */

#include <sys/_ssize.h>
#include <sys/_size.h>
#include <sys/__pid.h>


typedef	long		uid_t;
typedef	uid_t		gid_t;
typedef	__pid_t		pid_t;
typedef	unsigned long	mode_t;
typedef	long		off_t;


#if	!defined(_POSIX_SOURCE)

typedef	unsigned char	uchar_t;
typedef	unsigned short	ushort_t;
typedef	unsigned int	uint_t;
typedef	unsigned long	ulong_t;

typedef	char	      *	caddr_t;
typedef	long		daddr_t;	/* disk address */
typedef	__pid_t		id_t;
typedef	int		key_t;		/* for System V IPC */

/*
 * System V defines "ushort", "ulong", "u_char", "u_short", "u_int", and
 * "u_long" to satisfy its own portability needs. In addition, System V
 * defines "time_t" and "clock_t" here.  Coherent needs but a few of these.
 */
#include <sys/_time.h>

/*
 * This definition is peculiar to Coherent; it is the type used to represent
 * file sizes, which is an "off_t" in POSIX.1 usage. It is debatable whether
 * this needs to be a separate type from "off_t".
 */

typedef	off_t		fsize_t;

#endif


#if	_SYSV4 || _DDI_DKI

typedef	n_dev_t		dev_t;
typedef	n_nlink_t	nlink_t;
typedef	n_ino_t		ino_t;

#else

typedef	o_dev_t		dev_t;
typedef	o_nlink_t	nlink_t;
typedef	o_ino_t		ino_t;

#endif


#if	_DDI_DKI

/*
 * We keep the DDI/DKI definitions in a separate header, since they are
 * really unrelated to any user-level issues.
 */

#include <sys/v_types.h>

#endif


#if	!defined(_POSIX_SOURCE) && (defined (__KERNEL__) || _DDI_DKI)

/*
 * Old Coherent kernel code defined several types here that conflict with
 * DDI/DKI usage. Most have been changed to more portable equivalents, but
 * since this does not have a corresponding DDI/DKI definition it has been
 * left.
 *
 * Developers are advised to read the cautionary notice in <sys/__paddr.h>.
 * The nature of physical memory management is likely to undergo substantial
 * revision soon.
 */

#include <sys/__paddr.h>
typedef __paddr_t	paddr_t;

#endif

#if	!defined(_POSIX_SOURCE) && (defined (__KERNEL__) || _SYSV4)
typedef	unsigned char	unchar;
typedef	unsigned short	ushort;
typedef	unsigned int	uint;
typedef	unsigned long	ulong;
#endif

#endif	/* ! defined (__SYS_TYPES_H__) */
