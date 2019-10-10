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

#include <sys/param.h>
#include <sys/types.h>
#include <sys/_time.h>
#include <dirent.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <signal.h>

#ifdef _I386
#include <sys/reg.h>
#include <ieeefp.h>
#else
#include <sys/machine.h>
#endif

/*
 * User process structure.
 *
 * Remember to update UPROC_VERSION whenever you change this struct.
 *
 * U_OFFSET is the byte offset of uproc within segment SIUSERP.
 * See also the definition of "u" at start of as.s.
 */
#define U_OFFSET	0xC00
#define U_COMM_LEN	10
#define U_SLEEP_LEN	10

typedef struct uproc {
#ifdef _I386
	/* Magic number UPROC_VERSION identifies this uproc struct.  */
#define UPROC_VERSION 0x0105
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
	struct	 fd *u_filep[NOFILE];	/* Open files */
	struct	 sr u_segl[NUSEG];	/* User segment descriptions */
#ifdef _I386
	int	 (*u_sfunc[MAXSIG])();	/* Signal functions */
#else
	int	 (*u_sfunc[NSIG])();	/* Signal functions */
#endif

	/*
	 * System working area.
	 */
	struct	 seg *u_sege[NUSEG];	/* Exec segment descriptors */
	MPROTO	 u_sproto;		/* User prototype */
	MCON	 u_syscon;		/* System context save */
	MENV	 u_sigenv;		/* Signal return */
	MGEN	 u_sysgen;		/* General purpose area */
#ifdef _I386
	int	 u_args[MSACOUNT];
#else
	int	 u_args[(MSASIZE*sizeof(char)+sizeof(int)-1)/sizeof(int)];
#endif
	struct	 io u_io;		/* User area I/O template */

	/*
	 * Set by ftoi.
	 */
	ino_t	 u_cdirn;		/* Child inode number */
	struct	 inode *u_cdiri;	/* Child inode pointer */
	struct	 inode *u_pdiri;	/* Parent inode pointer */
	struct	 direct u_direct;	/* Directory name */

	/*
	 * Accounting fields.
	 */
	char	 u_comm[U_COMM_LEN];	/* Command name */
#ifdef _I386
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
#ifdef _I386
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
#ifdef _I386
	int	*u_regl;
	int	u_rval2;
	void	(*u_sigreturn)();
	union _fpcontext	u_ndpCon;	/* ndp state */
	int	u_ndpFlags;
	int	u_bpfmax;		/* max blocks per file */
#endif
} UPROC;

#endif
