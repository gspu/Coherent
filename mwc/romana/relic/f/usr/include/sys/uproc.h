/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/uproc.h
 *
 * The user process area.
 *
 * Revised: Wed May 26 16:45:05 1993 CDT
 */

#ifndef	 __SYS_UPROC_H__
#define	 __SYS_UPROC_H__

#include <common/feature.h>
#include <common/_time.h>
#include <kernel/sigproc.h>
#if	_I386
#include <kernel/reg.h>
#include <kernel/param.h>
#include <ieeefp.h>
#else
#include <kernel/machine.h>
#include <kernel/const.h>
#endif
#include <sys/types.h>
#include <dirent.h>
#include <sys/io.h>
#include <sys/proc.h>

/*
 * NIGEL: This is a quick hack; we have removed the definition of the old
 * "struct direct" from anywhere that user code can see. We are still using
 * this for the kernel pending development of proper layering in the file-
 * system interface.
 */

#if	! DIRSIZ
# define	DIRSIZ		14
#endif

struct direct {
	ino_t		d_ino;
	char		d_name [DIRSIZ];
};


/*
 * User process structure.
 *
 * Remember to update UPROC_VERSION whenever you change this struct.
 *
 * U_OFFSET is the byte offset of uproc within segment SIUSERP.
 * See also the definition of "u" at start of as.s.
 */
#define U_OFFSET	0xB00
#define U_COMM_LEN	10
#define U_SLEEP_LEN	10

typedef struct uproc {
#if	_I386
	/* Magic number UPROC_VERSION identifies this uproc struct.  */
#define UPROC_VERSION 0x0107
	unsigned short u_version;	/* Version number for uproc struct */
#endif /* _I386 */
	int	 u_error;		/* Error number (must be first) */
	char	 u_flag;		/* Flags (for accounting) */
	int	 u_uid;			/* User id */
	int	 u_gid;			/* Group id */
	int	 u_ruid;		/* Real user id */
	int	 u_rgid;		/* Real group id */
	int	 u_euid;		/* Effective user id */
	int	 u_egid;		/* Effective group id */
	unsigned u_umask;		/* Mask for file creation */
	struct	 inode *u_cdir;		/* Current working directory */
	struct	 inode *u_rdir;		/* Current root directory */
	struct __tagged_fd
	       * u_filep[NOFILE];	/* Open files */
	struct	 sr u_segl[NUSEG];	/* User segment descriptions */

	__sigaction_t	u_sigact [NSIG];/* Signal action information */

	/*
	 * System working area.
	 */
	struct	 seg *u_sege[NUSEG];	/* Exec segment descriptors */
	MPROTO	 u_sproto;		/* User prototype */
	MCON	 u_syscon;		/* System context save */
	MENV	 u_sigenv;		/* Signal return */
	MGEN	 u_sysgen;		/* General purpose area */
#if	0
	int	u_args [MSACOUNT];
	struct	 io u_io;		/* User area I/O template */
	struct	 direct u_direct;	/* Directory name */
#endif
#if	! _I386
	int	 u_args [__DIVIDE_ROUNDUP_CONST (MSASIZE, sizeof (int)];
#endif

	/*
	 * Set by ftoi.
	 */

	ino_t	 u_cdirn;		/* Child inode number */
	struct	 inode *u_cdiri;	/* Child inode pointer */
	struct	 inode *u_pdiri;	/* Parent inode pointer */

	/*
	 * Accounting fields.
	 */
	char	 u_comm[U_COMM_LEN];	/* Command name */
#if	_I386
	char	 u_sleep[U_SLEEP_LEN];	/* Reason for sleeping */
#endif
	time_t	 u_btime;		/* Beginning time of process */
	int	 u_memuse;		/* Average memory usage */
	long	 u_block;		/* Count of disk blocks */

	/*
	 * Profiler fields.
	 */
	caddr_t  u_ppc;			/* Profile pc from clock */
	caddr_t	 u_pbase;		/* Profiler base */
	caddr_t	 u_pbend;		/* Profiler base end */
#if	_I386
	off_t	 u_pofft;		/* Offset from base */
	off_t	 u_pscale;		/* Scaling factor */
#else
	vaddr_t	 u_pofft;		/* Offset from base */
	vaddr_t	 u_pscale;		/* Scaling factor */
#endif

	/*
	 * Miscellaneous things.
	 */
	int	u_argc;			/* Argument count (for ps) */
	unsigned u_argp;		/* Offset of argv[0] (for ps) */
	int	u_signo;		/* Signal number (for debugger) */
#if	_I386
#if	0
	int	*u_regl;
#endif
	int	u_rval2;
	__sigfunc_t	u_sigreturn;
	union _fpcontext	u_ndpCon;	/* ndp state */
	int	u_ndpFlags;
	int	u_bpfmax;		/* max blocks per file */

	char	u_nigel [32];		/* for STREAMS */
#endif
} UPROC;

#endif	/* ! defined (__SYS_UPROC_H__) */
