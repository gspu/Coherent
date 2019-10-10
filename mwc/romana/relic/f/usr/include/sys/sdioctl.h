/*
 * /usr/include/sys/sdioctl.h
 *
 * ioctl constants for SCSI devices.
 *
 * Revised: Mon Jun 21 05:52:59 1993 CDT
 */

#ifndef __SYS_SDIOCTL_H__
#define __SYS_SDIOCTL_H__

#define	SDIOC		('S'<<8)
#define	SCSI_HA_CMD	(SDIOC|0)
#define	SCSI_CMD	(SDIOC|1)
#define	SCSI_CMD_IN	(SDIOC|2)
#define	SCSI_CMD_OUT	(SDIOC|3)

/*
 * Temporary additions for haiscsi from C. Hilton
 */
#define SDGTINQ     (SDIOC | 0)     /* Get inquiry info */

#endif
