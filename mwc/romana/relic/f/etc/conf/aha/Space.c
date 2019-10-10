/*
 * Configurable information for "aha" (Adaptec, no tape support) device driver.
 */

#define __KERNEL__	 1

#include "conf.h"

/*
 * Adaptec ROM translates at 64 heads, except the Tandy version, which
 * uses 16 heads.  Kernel variable SD_HDS is patchable for this reason.
 *
 * SDIRQ = interrupt channel.
 * SDBASE = base i/o port address.
 * SDDMA = dma channel.
 *
 * Number of heads and sectors per track are translation mode, and fixed
 * in the BIOS, so do not differ among attached drives.
 *
 * AHA_SD_HDS = number of heads on a drive (translation mode).
 *   If initialized zero, use BIOS values or default to 64.
 * AHA_SD_SPT = number of sectors per track (translation mode).
 *   If initialized zero, use BIOS values or default to 32.
 */
int	SDIRQ	= 11;
int	SDBASE	= 0x0330;
int	SDDMA	= 5;

int	AHA_SD_HDS = 0;
int	AHA_SD_SPT = 0;
