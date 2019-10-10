/*
 * Configurable information for COHERENT kernel startup.
 */

#define __KERNEL__	 1

#include <sys/stat.h>
#include <sys/types.h>
#include "conf.h"

/* Use cohtune for these. */

unsigned long	_entry = 0;
unsigned long	_bar = 0;

dev_t		rootdev = ROOTDEV_SPEC;
dev_t		pipedev = PIPEDEV_SPEC;

int		ronflag = RONFLAG_SPEC;

/* Use idtune for these. */

int		NBUF = NBUF_SPEC;
int		NHASH = NHASH_SPEC;
int		NINODE = NINODE_SPEC;
int		NCLIST = NCLIST_SPEC;
int		ALLSIZE = ALLSIZE_SPEC;

int		PHYS_MEM  = PHYS_MEM_SPEC;

/* Number of IDE drives installed if PS1 system (CMOS won't show them). */
short		at_drive_ct = AT_DRIVE_CT;

/* Maximum # of shared memory segments, systemwide. */
int		SHMMNI = SHMMNI_SPEC;

/* Max size in bytes of shared memory segment. */
int		SHMMAX = SHMMAX_SPEC;
