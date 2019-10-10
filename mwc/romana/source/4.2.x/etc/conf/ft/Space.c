/* Generated from Space.spc on Wed Aug 10 07:31:03 1994 CDT */
/*
 * Configuration for floppy tape.
 * This module requires "fdc" to be enabled as well.
 */
#define	__KERNEL__	1

#include <sys/con.h>
#include <common/ccompat.h>

#include "conf.h"
#include <sys/ft.h>

unsigned int	ftNumBytes [FT_NBUF_SPEC];

struct FtSegErr	ftSegErr [FT_NBUF_SPEC];
unsigned char	ftNumBlks [FT_NBUF_SPEC];

/*
 * Add 1 to allocate memory for the tape header segment.
 * Add FT_BLK_SIZE to allow 1 Kbyte alignment.
 */

int		FT_NBUF = FT_NBUF_SPEC;

/* 0=hard select, 1=A/M/S, 2=CMS. */
int		FT_SOFT_SELECT = FT_SOFT_SELECT_SPEC;

/* Make the driver try to configure soft select or unit number. */
int		FT_AUTO_CONFIG = FT_AUTO_CONFIG_SPEC;

extern CON	ftcon;
CON		* ftCon = & ftcon;
