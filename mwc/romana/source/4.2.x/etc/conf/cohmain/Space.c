/* Generated from Space.spc on Wed Apr  6 09:44:29 1994 CDT */
/*
 * Configurable information for COHERENT kernel startup.
 */

#define __KERNEL__	 1

#include <sys/stat.h>
#include <sys/types.h>
#include "conf.h"

unsigned long	_entry = BAR_BAR;
unsigned long	_bar = BAR_ENTRY;

dev_t		rootdev = ROOTDEV_SPEC;
dev_t		pipedev = PIPEDEV_SPEC;

int		ronflag = RONFLAG_SPEC;

unsigned	NBUF = NBUF_SPEC;
unsigned	NHASH = NHASH_SPEC;
unsigned	NINODE = NINODE_SPEC;
unsigned	NCLIST = NCLIST_SPEC;
unsigned	ALLSIZE = ALLSIZE_SPEC;

int		PHYS_MEM  = PHYS_MEM_SPEC;

/* Number of IDE drives installed if PS1 system (CMOS won't show them). */
short		at_drive_ct = AT_DRIVE_CT;

/*
 * File system tunables.
 */

/* Flag to say whether we should try and keep the free-block list sorted. */
int	t_sortblocks = FS_SORT_BLOCKS;

/* Read-ahead count, or -1. */
int	t_readahead = FS_READ_AHEAD;

/* Number of record locks. */
unsigned int		RLOCKS = RLOCKS_SPEC;

/*
 * Miscellaneous.
 */

/* Set nonzero to allow signal() to catch SIGSEGV. */
unsigned char	CATCH_SEGV = MISC_CATCH_SEGV;

/* Set nonzero to produce trap information on the system console */
unsigned char	CONSOLE_TRAP_DUMP = MISC_CONSOLE_TRAP;

/* Set nonzero to produce reports about spurious NMIs on console */
unsigned char	NMI_DUMP = MISC_NMI_TRAP;

/* Set nonzero to user backtrace display with register dump on user traps. */
unsigned char	do_user_bt = USER_BACKTRACE;

/* Set nonzero if cache is to be enabled when CPU is Cyrix upgrade part. */
int		CYRIX_CACHE = CYRIX_CACHE_SPEC;

/*
 * Cyrix CPU type, of interest because internal cache should be enabled
 * using special instructions, but only on Cyrix upgrade parts, not on
 * OEM parts.
 *
 * 0		=> uninitialized - let COHERENT autosense.  The default.
 *
 * 4		=> Cx486_SRx
 * 5		=> Cx486_DRx
 * 6		=> Cx486_SRx2
 * 7		=> Cx486_DRx2
 * 0xFFFE	=> Cyrix OEM CPU
 * 0xFFFF	=> Non-Cyrix CPU
 */
int		CYRIX_CPU = CYRIX_CPU_SPEC;

/*
 * The following is nonzero when running one of the early releases (Xfree 1.2)
 * of MWC's port of the X11r5 window manager are to run.
 */
int		X00_fix = X00_FIX;
