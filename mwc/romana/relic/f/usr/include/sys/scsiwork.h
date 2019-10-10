/*
 * /usr/include/sys/scsiwork.h
 *
 * Common SCSI portions of Adaptec and Seagate device drivers.
 *
 * Revised: Sun Aug  1 17:17:42 1993 CDT
 */
#ifndef __SYS_SCSIWORK_H__
#define __SYS_SCSIWORK_H__

#include <sys/buf.h>

#define	MAX_SCSI_ID	8
#define	MAX_LUN		4		/* limited by minor device number */ 

/*
 * drive_info contains the "per drive" flags
 */
extern	char	drive_info[MAX_SCSI_ID * MAX_LUN];

#define	D_DISK		0x01		/* disk-type device (random) */
#define	D_TAPE		0x02		/* tape-type device (sequential) */
#define	D_PRINTER	0x04		/* printer-type device */
#define	D_PROCESSOR	0x08		/* processor-type device */
#define	D_REMOVEABLE	0x10		/* media can be changed */
#define	D_WORM		0x20		/* WORM-type characteristics */

/*
 * Per disk controller data.
 * Only one host adapter; no more, no less.
 */

struct	scsi_work	{
	struct scsi_work *sw_actf;	/* Link to first */
	struct scsi_work *sw_actl;	/* Link to last */
	BUF		*sw_bp;		/* block request */
	long		sw_bno;
	char		sw_drv;		/* 000sssll s=SCSI_ID l=LUN */
	char		sw_type;
	char		sw_retry;
};

typedef	struct	scsi_work	scsi_work_t;

#define	ScmdTESTREADY		0x00
#define	ScmdREZERO		0x01
#define	ScmdREQUESTSENSE	0x03
#define	ScmdINQUIRY		0x12
#define	ScmdMODESENSE		0x1A
#define	ScmdREADCAPACITY	0x25
#define	ScmdREADEXTENDED	0x28
#define	ScmdWRITEXTENDED	0x2A

struct	scsi_cmd	{
	long	block, buffer;
	short	blklen, buflen;
	char	unit;			/* 000sssll s=SCSI_ID l=LUN */
	char	cmd;
};

typedef	struct	scsi_cmd	scsi_cmd_t;

#define G0CMDLEN	6	/* Group 0 commands are 6 bytes long  */
#define G1CMDLEN	10	/* Group 1 commands are 10 bytes long */
#define SENSELEN	22	/* number of bytes returned w/ req sense */
#define INQUIRYLEN	54	/* number of bytes returned w/ inquiry */
#define MODESENSELEN	92	/* number of bytes returned w/ mode sense */
#define READCAPLEN	8	/* number of bytes returned w/read capacity */

				/* Message types */
#define MSG_CMD_CMPLT	0x00	/* Command Complete */
#define MSG_SAVE_DPTR	0x02	/* Save SCSI data pointer */
#define MSG_RSTOR_DPTR	0x03	/* Restore SCSI pointers */
#define MSG_DISCONNECT	0x04	/* Target is about to disconnect */
#define MSG_ABORT	0x06	/* End the current SCSI bus cycle */
#define MSG_NOP		0x08	/* no-op */
#define MSG_DEV_RESET	0x0C	/* Bus Device Reset */
#define MSG_IDENTIFY	0x80	/* Identify, with no Disconnect */
#define MSG_IDENT_DC	0xC0	/* Identify, with Disconnect allowed */

#define CS_GOOD		0x00	/* Command Status from the drive */
#define CS_CHECK	0x02
#define CS_BUSY		0x08
#define CS_RESERVED	0x18

/*
 * Information Transfer Phase masks -
 * setting of RS_MESSAGE, RS_I_O, and RS_CTRL_DATA determines which of six
 * possible info transfer phases is occurring.
 */
#define XP_MSG_IN	(RS_MESSAGE | RS_I_O | RS_CTRL_DATA)
#define XP_MSG_OUT	(RS_MESSAGE          | RS_CTRL_DATA)
#define XP_STAT_IN	(             RS_I_O | RS_CTRL_DATA)
#define XP_CMD_OUT	(                      RS_CTRL_DATA)
#define XP_DATA_IN	(             RS_I_O               )
#define XP_DATA_OUT	(                                 0)

#define VTOP2(a1, a2)	vtop(a1,a2)

typedef struct {
	unsigned int		ncyl;
	unsigned char		nhead;
	unsigned char		nspt;
}	_drv_parm_t;

#endif
