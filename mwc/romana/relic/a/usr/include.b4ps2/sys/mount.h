/* (-lgl
 * 	COHERENT Version 3.2.1
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Mount table.
 */

#ifndef	 MOUNT_H
#define	 MOUNT_H

#include <sys/types.h>
#include <sys/filsys.h>

/*
 * Mount table structure.
 */
typedef struct mount {
	struct	 mount *m_next;		/* Pointer to next */
	struct	 inode *m_ip;		/* Associated inode */
	dev_t	 m_dev;			/* Device */
	int	 m_flag;		/* Flags */
	GATE	 m_ilock;		/* Inode lock */
	GATE	 m_flock;		/* Free list lock */
	struct	 filsys m_super;	/* Super block */
} MOUNT;

/*
 * Flags.
 */
#define	MFRON	001			/* Read only file system */

#ifdef KERNEL
/*
 * Functions.
 */
MOUNT	*fsmount();			/* fs2.c */
MOUNT	*getment();			/* fs2.c */

/*
 * Global variables.
 */
extern	MOUNT	*mountp;		/* Mount table */

#endif

#endif
