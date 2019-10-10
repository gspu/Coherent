/*
 * Configurable information for "at" (ST506-compatible) device driver.
 */

#define __KERNEL__	 1

#include <sys/hdioctl.h>
#include "conf.h"

#define _TAG(tag)

/*
 * ATSECS is number of seconds to wait for an expected interrupt.
 * ATSREG needs to be 3F6 for most new IDE drives;  needs to be
 *	1F7 for Perstor controllers and some old IDE drives.
 *	Either value works with most drives.
 */

int	ATSECS = 6;
int	ATSREG = 0x3F6;

struct hdparm_s	atparm[N_ATDRV] = {
_TAG(AT0)	_HDPARMS(0,0,0,0,0),
_TAG(AT1)	_HDPARMS(0,0,0,0,0),
};
