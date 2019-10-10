/* Generated from Space.spc on Wed Aug 10 15:43:55 1994 CDT */
/*
 * Configurable information for "ss" (Seagate/Future Domain SCSI) device driver.
 */

#define __KERNEL__	 1

#include <sys/scsiwork.h>
#include <sys/types.h>
#include "conf.h"

#define _TAG(tag)

/*
 * In the low byte of NSDRIVE, bit n is 1 if SCSI ID n is an installed target.
 * The high byte indicates which type of host adapter:
 *   00 - ST01/ST02
 *   80 - TMC-845/850/860/875/885
 *   40 - TMC-840/841/880/881
 *
 * So, 0x0001 would be ST01 or ST02 with a single hard drive at SCSI ID 0.
 * Default for the interrupt number, SS_INT, is 5.
 * Default for the base segment address, SS_BASE, is 0xCA00.
 */
unsigned int	NSDRIVE = (SS_DISK_SPEC | (SS_HBATYPE << 8));
unsigned int	SS_INT = SS_INT_SPEC;
unsigned int	SS_BASE = SS_BASE_SPEC;

/*
 * Hard drive paramters: cylinders, heads, sectors per track.
 */
_drv_parm_t drv_parm[MAX_SCSI_ID] = {
_TAG(SD0)	{ 0, 0, 0},
_TAG(SD1)	{ 0, 0, 0},
_TAG(SD2)	{ 0, 0, 0},
_TAG(SD3)	{ 0, 0, 0},
_TAG(SD4)	{ 0, 0, 0},
_TAG(SD5)	{ 0, 0, 0},
_TAG(SD6)	{ 0, 0, 0},
_TAG(SD7)	{ 0, 0, 0}
};

