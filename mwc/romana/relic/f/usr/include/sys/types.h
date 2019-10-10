#ifndef	__SYS_TYPES_H__
#define	__SYS_TYPES_H__

/*
 * This header is defined in the POSIX.1 standard ISO/IEC 9945-1:1990, and as
 * such client programs which include this header may not use any symbols
 * which end in "_t".
 */

#include <common/feature.h>
#include <common/ccompat.h>

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

#include <common/_ssize.h>
#include <common/_size.h>
#include <common/_pid.h>
#include <common/_off.h>
#include <common/_uid.h>

typedef	__uid_t		uid_t;
typedef	__gid_t		gid_t;
typedef	__dev_t		dev_t;
typedef	__nlink_t	nlink_t;
typedef	__ino_t		ino_t;
typedef	__mode_t	mode_t;


#if	! _POSIX_SOURCE

/*
 * We build our definitions of 'uchar_t' and friends from a common base type.
 * System V defines "ushort", "ulong", "u_char", "u_short", "u_int", and
 * "u_long" to satisfy its own portability needs. In addition, System V
 * defines "time_t" and "clock_t" here. Be aware that POSIX.1 does not
 * make any of these types available through this header.
 */

#include <common/__types.h>
#include <common/_clock.h>
#include <common/_daddr.h>
#include <common/_time.h>
#include <common/_fsize.h>

typedef	__uchar_t	uchar_t;
typedef	__ushort_t	ushort_t;
typedef	__uint_t	uint_t;
typedef	__ulong_t	ulong_t;

typedef	__uchar_t	unchar;
typedef	__ushort_t	ushort;
typedef	__uint_t	uint;
typedef	__ulong_t	ulong;

typedef	char	      *	caddr_t;
typedef	__pid_t		id_t;
typedef	int		key_t;		/* for System V IPC */


/*
 * Old Coherent kernel code defined several types here that conflict with
 * DDI/DKI usage. Most have been changed to more portable equivalents, but
 * since these does not have a corresponding DDI/DKI definition it has been
 * left.
 *
 * Developers are advised to read the cautionary notice in <common/__paddr.h>.
 * The nature of physical memory management is likely to undergo substantial
 * revision soon.
 */

#include <common/__paddr.h>

typedef __paddr_t	paddr_t;
typedef	char	      *	vaddr_t;

typedef	__VOID__	_VOID;		/* for System V */

#endif


#if	_DDI_DKI

/*
 * We keep the DDI/DKI definitions in a separate header, since they are
 * really unrelated to any user-level issues.
 */

#include <kernel/v_types.h>

#endif	/* _DDI_DKI */

#endif	/* ! defined (__SYS_TYPES_H__) */
