/*
 * /usr/include/sys/xl.h
 *
 * Archive floppy tape support.
 *
 * Revised: Wed Jun  2 12:16:16 1993 CDT
 */

#ifndef __SYS_XL_H__
#define __SYS_XL_H__

/************************************************************************/
/*	  QIC 117 commands						*/
/************************************************************************/
#define	QIC_CMD_RST	01	/* reset				*/
#define	QIC_CMD_RNB	02	/* report next bit			*/
#define	QIC_CMD_PAUS	03	/* pause				*/
#define	QIC_CMD_MPAUS	04	/* micro step pause			*/
#define	QIC_CMD_ALTTO	05	/* alternate command timeout		*/
#define	QIC_CMD_STS	06	/* status				*/
#define	QIC_CMD_ECD	07	/* report error code			*/
#define	QIC_CMD_DRVCN	08	/* report drive configuration		*/
#define	QIC_CMD_ROMVN	09	/* report ROM version			*/
#define	QIC_CMD_FWD	10	/* logical forward			*/
#define	QIC_CMD_BOT	11	/* rewind to begining of tape		*/
#define	QIC_CMD_EOT	12	/* forward to end of tape		*/
#define	QIC_CMD_SEEK	13	/* seek head to track			*/
#define	QIC_CMD_CAL	14	/* rewind, calibrate drive		*/
#define	QIC_CMD_FMD	15	/* format mode				*/
#define	QIC_CMD_WRF	16	/* write reference bursts		*/
#define	QIC_CMD_VMD	17	/* verify mode				*/
#define	QIC_CMD_STOP	18	/* stop tape motion			*/
#define	QIC_CMD_SKPB	25	/* skip n segments back			*/
#define	QIC_CMD_NMD	30	/* normal mode				*/
#define	QIC_CMD_VNDID	32	/* report vendor id			*/
#define	QIC_CMD_TPSTAT	33	/* report tape status			*/

/************************************************************************/
/*	  QIC 117 error codes						*/
/************************************************************************/

/* command received while drive not ready */
#define	QIC_ERR_NOT_READY	01

/* cartridge not present or removed */
#define	QIC_ERR_NO_CART		02

/* motor speed error (not within 1%) */
#define	QIC_ERR_MOTOR_SPEED	03

/* motor speed fault (jammed, or gross speed error) */
#define	QIC_ERR_MOTOR_FAULT	04

/* cartridge write protected */
#define	QIC_ERR_WPROT		05

/* undefined or reserved command code */
#define	QIC_ERR_BAD_CMD		06

/* illegal track address specified for seek */
#define	QIC_ERR_SEEK_TRK	07

/* illegal command in report subcontext */
#define	QIC_ERR_RPT_CMD		08

/* illegal entry into a diagnostic mode */
#define	QIC_ERR_DIAG_ENTRY	09

/* broken tape detected (based on hole sensor) */
#define	QIC_ERR_BROKEN_TAPE	10

/* warning - read gain setting error */
#define	QIC_ERR_READ_GAIN	11

/* command received while error status pending (obsolete) */
#define	QIC_ERR_ESTAT		12

/* command received while new cartridge pending */
#define	QIC_ERR_NEW_CART	13

/* command illegal or undefined in primary mode */
#define	QIC_ERR_PRI_MODE	14

/* command illegal or undefined in format mode */
#define	QIC_ERR_FMT_MODE	15

/* command illegal or undefined in verify mode */
#define	QIC_ERR_VFY_MODE	16

/* logical forward not at logical BOT in format mode */
#define	QIC_ERR_LOG_FWD		17

/* logical EOT before all segments generated */
#define	QIC_ERR_LOG_EOT		18

/* command illegal when cartridge not referenced */
#define	QIC_ERR_NOT_REF		19

/* self-diagnostic failed (cannot be cleared) */
#define	QIC_ERR_SELF_DX		20

/* warning EEPROM not initialized, defaults set */
#define	QIC_ERR_EE_DFLT		21

/* EEPROM corrupt or hardware failure */
#define	QIC_ERR_EE_BAD		22

/* motion timeout error */
#define	QIC_ERR_MOTION_TO	23

/* data segment too long - logical forward or pause */
#define	QIC_ERR_TOO_LONG	24

/* transmit overrun (obsolete) */
#define	QIC_ERR_XMIT_OVR	25

/* power on reset occurred */
#define	QIC_ERR_POWER_ON	26

/* software reset occurred */
#define	QIC_ERR_SOFT_RST	27

/* diagnostic mode 1 error */
#define	QIC_ERR_DX_1		28

/* diagnostic mode 2 error */
#define	QIC_ERR_DX_2		29

/* command received during noninterruptible process */
#define	QIC_ERR_NON_IRPT	30

/* rate selection error */
#define	QIC_ERR_RATE_SEL	31

/* illegal command while in high speed mode */
#define	QIC_ERR_HIGH_SPD	32

/* illegal seek segment value */
#define	QIC_ERR_SEEK_SEG	33 

/************************************************************************/
/*	request buffer structure					*/
/************************************************************************/
struct rb{
	struct	rb *nxt;	/* ptr to next packet	*/
	unchar	fun;		/* function		*/
	unchar	sts;		/* status		*/
	ushort	sgn;		/* segment #		*/
	paddr_t	adr;		/* addr of buffer	*/
	ulong	map;		/* bad sector map	*/
	unchar	hed;		/* fdc head		*/
	unchar	cyl;		/* fdc cylinder		*/
	unchar	sct;		/* base fdc sector	*/
	unchar	trk;		/* tape track		*/
	unchar	tps;		/* segment		*/
	unchar	idc;		/* read id cylinder	*/
	unchar	ids;		/* sct			*/
	unchar	nbk;		/* # blocks		*/
	unchar	erc;		/* error count		*/
	unchar	ers[ 3 ]; 	/* error sectors	*/
	ushort	tbl[ 33 ];	/* parsed segment params*/
};

struct rbq {			/* buf queue structure	*/
	struct	rb *top;
	struct	rb *bot;
};

/************************************************************************/
/* 	Volume table structure (128 bytes)				*/
/************************************************************************/

typedef struct {
	unsigned  char	ident[4]; 	/* Volume entry signature "VTBL"*/
	unsigned  short	data_seg_num;	/* Starting segment number	*/
	unsigned  short	last_seg_num;	/* Ending segment number  	*/
	unsigned  char	op_system[6];	/* OS type "unix"	  	*/
	unsigned  char	p1[43];		/*  Null (offset 14-56)	   	*/
	char		c_seq_num;	/* Multicartridge seq #    	*/
					/*  initial cartridge = 1  	*/
	unsigned  short	p3[34];		/*  Null (offset 58-126)   	*/
	unsigned  short	last_blk_size;	/* Reserved. For UNIX/Xenix 	*/
					/*  last block data size   	*/
} xlvtbl;

/************************************************************************/
/*	xl typedefs 						*/
/************************************************************************/

typedef	unsigned char * fpchr;
typedef	unsigned short * fpwrd;
typedef	unsigned long * fplng;

/************************************************************************/
/*	xldefines							*/
/************************************************************************/

#define	RBFRD	0x01		/* rb.fun values			*/
#define	RBFWT	0x02
#define	RBWFD	0x03

#define	XLSRDY	0x01		/* xl6sts equates			*/
#define	XLSEXC	0x02
#define	XLSCIN	0x04
#define	XLSWRP	0x08
#define	XLSCHG	0x10
#define	XLSREF	0x20
#define	XLSBOT	0x40
#define	XLSEOT	0x80
/*				  xlster equates			*/
#define	XLSNEC	0x80		/* nec error				*/
#define	XLSLSB	0x40		/* last status bit != 1			*/
#define	XLSSFT	0x20		/* soft error:, exc, chg, or no cin	*/
#define	XLSNTD	0x10		/* not tape drive			*/
#define	XLSNID	0x08		/* can't read id's			*/

#endif /* __SYS_XL_H__ */
