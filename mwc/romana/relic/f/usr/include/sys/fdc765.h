/*
 * /usr/include/sys/fdc765.h
 *
 * Support 765-style controller for diskette and floppy tape
 *
 * Revised: Thu Jun 17 13:01:39 1993 CDT
 */
#ifndef __SYS_FDC765_H__
#define __SYS_FDC765_H__

/*
 * ----------------------------------------------------------------------
 * Includes.
 */

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/* Port addresses */
#define FDCDOR	0x3F2			/* Digital output */
#define	FDCDAT	0x3F5			/* Data register */
#define	FDCMSR	0x3F4			/* Main status register */
#define	FDCRATE	0x3F7			/* Transfer rate (500,300,250 Kbps) */
					/* 0=500 1=300 2=250 3=1000 Kbps */
#define FDCCHGL 0x3F7			/* Port where we read the disk */
					/* changed line */

/* FDCDOR masks */
#define	DORDS	0x03			/* Drive select bits */
#define	DORNMR	0x04			/* Not master reset */
#define	DORIEN	0x08			/* Interrupt, DMA enable */
#define	DORMS	0xF0			/* Motor enables */

/* FDCDMSR masks */
#define	MSRDB	0x0F			/* Drive busy */
#define	MSRCB	0x10			/* Control busy */
#define	MSRNDMA	0x20			/* Not DMA */
#define	MSRDIO	0x40			/* Data direction */
#define	MSRRQM	0x80			/* Request for master */

/* FDCCHGL mask */
#define DSKCHGD 0x80			/* Diskette changed line bit. */

/* FDCRATE values (transfer rates) */
#define FDC_RATE_500K	0
#define FDC_RATE_300K	1
#define FDC_RATE_250K	2
#define FDC_RATE_1MEG	3
/*
 * Status Register 0 - Bit Definitions.
 */
#define	ST0_US0	0x01			/* Unit Select 0 */
#define	ST0_US1	0x02			/* Unit Select 1 */
#define	ST0_HD	0x04			/* Head Address */
#define	ST0_NR	0x08			/* Not Ready */
#define	ST0_EC	0x10			/* Equipment Check */
#define	ST0_SE	0x20			/* Seek End */
#define	ST0_IC	0xC0			/* Interrupt code */
#define	ST0_NT	0x00			/* Normal Termination */

/*
 * Status Register 1 - Bit Definitions.
 */
#define	ST1_MA	0x01			/* Missing Address Mark */
#define	ST1_NW	0x02			/* Not writeable */
#define	ST1_ND	0x04			/* No Data */
	/*	0x08 */			/* Not used - always 0 */
#define	ST1_OR	0x10			/* Overrun */
#define	ST1_DE	0x20			/* Data Error */
	/*	0x40 */			/* Not used - always 0 */
#define	ST1_EN	0x80			/* End of Cylinder */

/*
 * Status Register 2 - Bit Definitions.
 */
#define	ST2_MD	0x01			/* Missing Address Mark in Data Field */
#define	ST2_BC	0x02			/* Bad Cylinder */
#define	ST2_SN	0x04			/* Scan Not Satisfied */
#define	ST2_SH	0x08			/* Scan Equal Hit */
#define	ST2_WC	0x10			/* Wrong Cylinder */
#define	ST2_DD	0x20			/* Data Error in Data Field */
#define	ST2_CM	0x40			/* Control Mark */
	/*	0x80 */			/* Not used - always 0 */

/*
 * Status Register 3 - Bit Definitions.
 */
#define	ST3_US0	0x01			/* Unit Select 0 */
#define	ST3_US1	0x02			/* Unit Select 1 */
#define	ST3_HD	0x04			/* Head Address */
#define	ST3_TS	0x08			/* Two Sides */
#define	ST3_T0	0x10			/* Track 0 */
#define	ST3_RDY	0x20			/* Ready */
#define ST3_WP	0x40			/* Write Protected */
#define	ST3_FT	0x80			/* Fault */

/*
 * Controller Commands.
 */
#define	FDC_CMD_SPEC	0x03		/* Specify */
#define FDC_CMD_SDRV	0x04		/* Sense drive status */
#define	FDC_CMD_RCAL	0x07		/* Recal */
#define FDC_CMD_SINT	0x08		/* Sense interrupt status */
#define	FDC_CMD_SEEK	0x0F		/* Seek */
#define	FDC_CMD_WDAT	0x45		/* Write data */
#define FDC_CMD_RDID	0x4A		/* Read ID */
#define	FDC_CMD_FMT	0x4D		/* Format track */
#define	FDC_CMD_RDAT	0x66		/* Read data */

/* Look at minor # to see if target device is diskette or floppy tape. */
#define FDC_DISKETTE(dev)	(((dev) & 0xC0) == 0)
#define FDC_TAPE(dev)		(((dev) & 0xC0) == 0x40)

/*
 * See if Read/Write command is in progress.
 * If so, may not send another command yet.
 */
#define FDC_BUSY()	(inb(FDCMSR) & MSRCB)

/*
 * These definitions are out of the usual order because they are for
 * the FDC struct which follows.
 */
#define FDC_NUM_CMD_STAT	8
#define FDC_NUM_INT_STAT	4

enum {
	FDC_MOTOR_OFF = 0,
	FDC_MOTOR_ON = 1
};

enum {
	FDC_HEAD_0 = 0,
	FDC_HEAD_1 = 1
};

struct	FDC	{
	/* Command status buffer, and how many valid entries contained. */
	int		fdc_ncmdstat;
	unsigned char	fdc_cmdstat[FDC_NUM_CMD_STAT];

	/* Interrupt status buffer, and how many valid entries contained. */
	int		fdc_nintstat;
	unsigned char	fdc_intstat[FDC_NUM_INT_STAT];
};

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
extern struct FDC	fdc;

extern void		(*ftIntr)();
extern void		(*flIntr)();

#endif /*__SYS_FDC765_H__*/
