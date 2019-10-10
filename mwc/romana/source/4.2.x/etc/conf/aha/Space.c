/* Generated from Space.spc on Wed Mar  2 11:20:14 1994 CST */
/*
 * Configurable information for "aha" (Adaptec, no tape support) device driver.
 */

#define __KERNEL__	 1

#include "conf.h"

/*
 * Adaptec ROM translates at 64 heads, except the Tandy version, which
 * uses 16 heads.  Kernel variable SD_HDS is patchable for this reason.
 *
 * AHA_SDIRQ = interrupt channel.
 * AHA_SDBASE = base i/o port address.
 * AHA_SDDMA = dma channel.
 *
 * Number of heads and sectors per track are translation mode, and fixed
 * in the BIOS, so do not differ among attached drives.
 *
 * AHA_SD_HDS = number of heads on a drive (translation mode).
 *   If initialized zero, use BIOS values or default to 64.
 * AHA_SD_SPT = number of sectors per track (translation mode).
 *   If initialized zero, use BIOS values or default to 32.
 */

int	AHA_SDIRQ	= HAI_AHAINTR_SPEC;
int	AHA_SDBASE	= HAI_AHABASE_SPEC;
int	AHA_SDDMA	= HAI_AHADMA_SPEC;

int	AHA_SD_HDS = AHA_SD_HDS_SPEC;
int	AHA_SD_SPT = AHA_SD_SPT_SPEC;
