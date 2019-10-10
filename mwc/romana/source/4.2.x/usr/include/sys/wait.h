/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_WAIT_H__
#define	__SYS_WAIT_H__

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

#include <common/ccompat.h>
#include <common/__pid.h>
#include <common/_wait.h>

#if	_SYSV4 && ! _SYSV3

# define	WEXITED		__WEXITED
# define	WTRAPPED	__WTRAPPED

# define	WSTOPFLG	__WSTOPFLG
# define	WCOREFLG	__WCOREFLG

# define	WCONTINUED	__WCONTINUED
# define	WNOWAIT		__WNOWAIT

# define	WIFCONTINUED(stat)	__WIFCONTINUED (stat)
# define	WCOREDUMP(stat)		__WCOREDUMP (stat)

#endif


#define	WUNTRACED	__WUNTRACED
#define	WNOHANG		__WNOHANG

#define	WIFEXITED(stat)		__WIFEXITED (stat)
#define	WEXITSTATUS(stat)	__WEXITSTATUS (stat)

#define	WIFSIGNALED(stat)	__WIFSIGNALED (stat)
#define	WTERMSIG(stat)		__WTERMSIG (stat)

#define	WIFSTOPPED(stat)	__WIFSTOPPED (stat)
#define	WSTOPSIG(stat)		__WSTOPSIG (stat)


__EXTERN_C_BEGIN__

__pid_t		wait		__PROTO ((int * _stat_loc));
__pid_t		waitpid		__PROTO ((__pid_t _pid, int * _stat_loc,
					  int _options));

__EXTERN_C_END__

#endif	/* ! defined (__SYS_WAIT_H__) */
