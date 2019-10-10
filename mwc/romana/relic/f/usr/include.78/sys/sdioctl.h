/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef __SYS_SDIOCTL_H__
#define __SYS_SDIOCTL_H__

#define	SDIOC		('S'<<8)
#define	SCSI_HA_CMD	(SDIOC|0)
#define	SCSI_CMD	(SDIOC|1)
#define	SCSI_CMD_IN	(SDIOC|2)
#define	SCSI_CMD_OUT	(SDIOC|3)

#endif
