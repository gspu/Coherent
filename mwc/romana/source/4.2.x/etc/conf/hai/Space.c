/* Generated from Space.spc on Wed Mar  2 08:51:15 1994 CST */
/*
 * Configurable information for "hai" (Adaptec, tape support) device driver.
 */

#define __KERNEL__	 1

#include <sys/haiscsi.h>
#include "conf.h"

/*
 * Patchable bit maps
 *   Bit n is 1 in HAI_DISK if there is a hard disk at SCSI ID n.
 *   Bit n is 1 in HAI_TAPE if there is a tape at SCSI ID n.
 */
int HAI_DISK = HAI_DISK_SPEC;
int HAI_TAPE = HAI_TAPE_SPEC;
int HAI_CDROM = HAI_CDROM_SPEC;

/*
 * Adaptec 154x configuration (factory defaults shown).
 *
 * HAI_AHABASE = 0x330		Port Base.
 * HAI_AHAINTR = 11		Host Adapter Interrupt vector.
 * HAI_AHADMA  = 5		DMA Channel.
 */

unsigned short	HAI_AHABASE = HAI_AHABASE_SPEC;
unsigned short	HAI_AHAINTR = HAI_AHAINTR_SPEC;
unsigned short	HAI_AHADMA  = HAI_AHADMA_SPEC;

/*
 * Number of heads and sectors per track are translation mode, and fixed
 * in the BIOS, so do not differ among attached drives.  Defaults shown.
 *
 * HAI_SD_HDS = 64;  number of heads on a drive (translation mode).
 *   If initialized zero, use BIOS values or default to 64.
 * HAI_SD_SPT = 32;  number of sectors per track (translation mode).
 *   If initialized zero, use BIOS values or default to 32.
 */

int	HAI_SD_HDS = HAI_SD_HDS_SPEC;
int	HAI_SD_SPT = HAI_SD_SPT_SPEC;

/*
 * hai_xfer_speed = hai_xfer_5_0;  MB/second AT bus transfer speed.
 * hai_bus_off_time = 4 [2..15]; Microseconds of bus off time per transfer.
 * hai_bus_on_time = 11 [1..64]; Microseconds of bus on time per transfer.
 */
unsigned char	hai_xfer_speed =	HAI_AHAXFERSPEED;
unsigned char	hai_bus_off_time =	HAI_AHABUSOFFTIME;
unsigned char	hai_bus_on_time =	HAI_AHABUSONTIME;
