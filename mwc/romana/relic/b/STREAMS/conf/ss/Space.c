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
 */
unsigned int	NSDRIVE = 0x0001;
unsigned int	SS_INT = 5;
unsigned int	SS_BASE = 0xCA00;

/* ncyl, nhead, nspt */
_drv_parm_t drv_parm[MAX_SCSI_ID] = {
_TAG(SD0)	{ 0, 0, 0},
_TAG(SD1)	{ 0, 0, 0},
_TAG(SD2)	{ 0, 0, 0},
_TAG(SD3)	{ 0, 0, 0},
_TAG(SD4)	{ 0, 0, 0},
_TAG(SD5)	{ 0, 0, 0},
_TAG(SD6)	{ 0, 0, 0},
_TAG(SD7)	{ 0, 0, 0},
};

