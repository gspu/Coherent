/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __SYS_UPROC_H__
#define	 __SYS_UPROC_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/__caddr.h>
#include <common/__off.h>
#include <common/__time.h>
#include <common/_uid.h>

#include <kernel/sig_lib.h>
#include <kernel/param.h>
#include <kernel/dir.h>
#include <sys/io.h>
#include <sys/seg.h>
#include <ieeefp.h>
#include <dirent.h>

#if	! _KERNEL
# error	You must be compiling the kernel to use this header
#endif

/*
 * User process structure.
 *
 * U_OFFSET is the byte offset of uproc within segment SIUSERP.
 * See also the definition of "u" at start of as.s.
 *
 * The following version number /must/ be updated with every change to the
 * U area that is more major than adding a new member at the end.
 */

#define __UAREA_VERSION	0x010C

#define U_OFFSET	0xB00

typedef struct uproc {
	int		u_error;	/* Error number (must be first) */
	char	 	u_flag;		/* Flags (for accounting) */

	unsigned	u_umask;	/* Mask for file creation */
	struct inode  *	u_cdir;		/* Current working directory */
	struct inode  *	u_rdir;		/* Current root directory */
	struct __tagged_fd
		      *	u_filep [NOFILE];	/* Open files */

	__sigaction_t	u_sigact [_SIGNAL_MAX];/* Signal action information */

	/*
	 * System working area.
	 */

	struct __menv *	u_sigenvp;	/* Signal return */

	/*
	 * Set by ftoi.
	 */

	o_ino_t		u_cdirn;	/* Child inode number */
	struct inode  *	u_cdiri;	/* Child inode pointer */
	struct inode  *	u_pdiri;	/* Parent inode pointer */

	/*
	 * Accounting fields.
	 */

	__time_t 	u_btime;	/* Beginning time of process */


	/*
	 * Profiler fields.
	 */

	__caddr_t	u_ppc;			/* Profile pc from clock */
	__caddr_t	u_pbase;		/* Profiler base */
	__caddr_t	u_pbend;		/* Profiler base end */
	__off_t		u_pofft;		/* Offset from base */
	__off_t		u_pscale;		/* Scaling factor */

	/*
	 * This is a count of outstanding locks; this should always be 0 when
	 * returning to user mode, except in a few special cases.
	 */

	unsigned	u_lock_cnt;

	/*
	 * Miscellaneous things.
	 */

	int		u_argc;		/* Argument count (for ps) */
	__caddr_t	u_argp;		/* Offset of argv[0] (for ps) */

	int		u_rval2;
	__sighand_t   *	u_sigreturn;
	union _fpcontext u_ndpCon;	/* ndp state */
	int		u_ndpFlags;
	int		u_bpfmax;	/* max blocks per file */

	char		u_nigel [32];	/* for STREAMS */
} UPROC;

extern	UPROC	 u;			/* Current user area. */

/*
 * Declare some functions; some drivers try to take the address of these
 * functions to pass to functions like defer () and timeout ().
 */

void		wakeup ();


/*
 * To underscore the fact that you must only use this header for
 * kernel compiles, and that using this header ties you to only a single
 * release of COHERENT, we put the following into any object file that
 * includes it.
 */

#define	__UAREA_V_SYM(v) __CONCAT (__uarea_v_, v)

extern	char		__UAREA_V_SYM (__UAREA_VERSION) [];
static	char	      *	__use_uarea = __UAREA_V_SYM (__UAREA_VERSION);

#endif	/* ! defined (__SYS_UPROC_H__) */
