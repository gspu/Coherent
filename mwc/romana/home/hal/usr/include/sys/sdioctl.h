/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_SDIOCTL_H__
#define __SYS_SDIOCTL_H__

/*
 * ioctl constants for SCSI devices.
 */

#define	SDIOC		('S'<<8)
#define	SCSI_HA_CMD	(SDIOC|0)
#define	SCSI_CMD	(SDIOC|1)
#define	SCSI_CMD_IN	(SDIOC|2)
#define	SCSI_CMD_OUT	(SDIOC|3)

/*
 * Temporary additions for haiscsi from C. Hilton
 */
#define SDGTINQ     (SDIOC | 0)     /* Get inquiry info */

#endif	/* ! defined (__SYS_SDIOCTL_H__) */
