/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * The user process area.
 */

#ifndef	 UPROC_H
#define	 UPROC_H	UPROC_H

#include <sys/param.h>
#include <sys/types.h>
#include <sys/machine.h>
#include <sys/dir.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <signal.h>

/*
 * Open segment structure.
 */
typedef struct sr {
	int	 sr_flag;		/* Flags for this reference */
	vaddr_t	 sr_base;		/* Virtual address base */
	vaddr_t	 sr_size;		/* Mapped in window size */
	struct	 seg *sr_segp;		/* Segment pointer */
} SR;

/*
 * Flags (sr_flag).
 */
#define SRFPMAP	01			/* Segment is mapped in process */
#define SRFDUMP	02			/* Dump segment */
#define	SRFDATA	04			/* Data segment */

/*
 * User process structure.
 */
typedef struct uproc {
	char	 u_error;		/* Error number (must be first) */
	char	 u_flag;		/* Flags (for accounting) */
	int	 u_uid;			/* User id */
	int	 u_gid;			/* Group id */
	int	 u_ruid;		/* Real user id */
	int	 u_rgid;		/* Real group id */
	unsigned u_umask;		/* Mask for file creation */
	struct	 inode *u_cdir;		/* Current working directory */
	struct	 inode *u_rdir;		/* Current root directory */
	struct	 fd *u_filep[NUFILE];	/* Open files */
	struct	 sr u_segl[NUSEG];	/* User segment descriptions */
	int	 (*u_sfunc[NSIG])();	/* Signal functions */

	/*
	 * System working area.
	 */
	struct	 seg *u_sege[NUSEG];	/* Exec segment descriptors */
	MPROTO	 u_sproto;		/* User prototype */
	MCON	 u_syscon;		/* System context save */
	MENV	 u_sigenv;		/* Signal return */
	MGEN	 u_sysgen;		/* General purpose area */
	int	 u_args[(MSASIZE*sizeof(char)+sizeof(int)-1)/sizeof(int)];
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
	char	 u_comm[10];		/* Command name */
	time_t	 u_btime;		/* Beginning time of process */
	int	 u_memuse;		/* Average memory usage */
	long	 u_block;		/* Count of disk blocks */

	/*
	 * Profiler fields.
	 */
	vaddr_t  u_ppc;			/* Profile pc from clock */
	vaddr_t	 u_pbase;		/* Profiler base */
	vaddr_t	 u_pbend;		/* Profiler base end */
	vaddr_t	 u_pofft;		/* Offset from base */
	vaddr_t	 u_pscale;		/* Scaling factor */

	/*
	 * Miscellaneous things.
	 */
	int	 u_argc;		/* Argument count (for ps) */
	unsigned u_argp;		/* Offset of argv[0] (for ps) */
	int	 u_signo;		/* Signal number (for debugger) */
} UPROC;

#ifdef KERNEL
/*
 * Current user area.
 */
extern	UPROC	u;

#endif

#endif
