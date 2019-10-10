/*
 * Configurable information for "hai" (Adaptec, tape support) device driver.
 */

#define __KERNEL__	 1

#include "conf.h"

/*
 * Patchable bit maps
 *   Bit n is 1 in HAI_DISK if there is a hard disk at SCSI ID n.
 *   Bit n is 1 in HAI_TAPE if there is a tape at SCSI ID n.
 */
int HAI_DISK = 0;
int HAI_TAPE = 0;

/*
 * Adaptec 154x configuration (factory defaults shown).
 *
 * AHABASE =	0x330;	Port Base.
 * AHAINTR =	11;	Host Adapter Interrupt vector.
 * AHADMACHAN =	0x05;	DMA Channel.
 */

unsigned short	AHABASE =	0x330;
unsigned short	AHAINTR =	11;
unsigned short	AHADMACHAN =	0x05;

/*
 * Number of heads and sectors per track are translation mode, and fixed
 * in the BIOS, so do not differ among attached drives.  Defaults shown.
 *
 * HAI_SD_HDS = 64;  number of heads on a drive (translation mode).
 *   If initialized zero, use BIOS values or default to 64.
 * HAI_SD_SPT = 32;  number of sectors per track (translation mode).
 *   If initialized zero, use BIOS values or default to 32.
 */

int	HAI_SD_HDS = 64;
int	HAI_SD_SPT = 32;
