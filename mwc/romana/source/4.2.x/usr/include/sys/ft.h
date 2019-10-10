#ifndef __FT_H__
#define __FT_H__

#include <common/__caddr.h>
#include <common/feature.h>
#include <common/ccompat.h>
#include <sys/types.h>
#include <kernel/timeout.h>

/* Constants describing the data format.				*/

#define	FT_SEG_SIZE	32768		/* bytes per QIC-80/40 segment	*/
#define	FT_BLK_SIZE	1024		/* bytes per QIC-80/40 block	*/
#define	FT_BLK_PER_SEG	32		/* 32 blocks per segment	*/
#define FT_NUM_ECC_BLKS 3		/* 3 ECC blocks per segment	*/
#define FT_SEC_PER_PTRK	128		/* Sectors per disk pseudotrack.*/

/*
 * Minor numbering.  Highest two bits must be 01 (see sys/fdc765.h).
 *
 *	01fr yxuu
 *
 *	f  = 0=QIC-40, 1=QIC-80
 *	r  = 0=rewind on close, 1=no rewind on close
 *
 *	y  = 0=205' tape length, 1=307.5'
 *	x    unused
 *	uu = FDC unit number (0..3)
 *
 *	Tunable parameters for soft select, CMS vs. A/M/S.
 */

#define FT_Q80_FORMAT(dev)	(dev & 0x20)
#define FT_NORWDCL(dev)		(dev & 0x10)
#define FT_LONGTAPE(dev)	(dev & 0x08)
#define FT_UNIT(dev)		(dev & 0x03)

/************************************************************************/
/*	  QIC 117 commands						*/
/************************************************************************/

#define	QIC_CMD_RST	1	/* reset				*/
#define	QIC_CMD_RNB	2	/* report next bit			*/
#define	QIC_CMD_PAUS	3	/* pause				*/
#define	QIC_CMD_MPAUS	4	/* micro step pause			*/
#define	QIC_CMD_ALTTO	5	/* alternate command timeout		*/
#define	QIC_CMD_STS	6	/* status				*/
#define	QIC_CMD_ECD	7	/* report error code			*/
#define	QIC_CMD_DRVCN	8	/* report drive configuration		*/
#define	QIC_CMD_ROMVN	9	/* report ROM version			*/
#define	QIC_CMD_FWD	10	/* logical forward			*/
#define	QIC_CMD_BOT	11	/* rewind to begining of tape		*/
#define	QIC_CMD_EOT	12	/* forward to end of tape		*/
#define	QIC_CMD_SEEK	13	/* seek head to track			*/
#define	QIC_CMD_CAL	14	/* rewind, calibrate drive		*/
#define	QIC_CMD_FMD	15	/* format mode				*/
#define	QIC_CMD_WRF	16	/* write reference bursts		*/
#define	QIC_CMD_VMD	17	/* verify mode				*/
#define	QIC_CMD_STOP	18	/* stop tape motion			*/
#define	QIC_CMD_HDUP	21	/* micro step head up			*/
#define	QIC_CMD_HDDN	22	/* micro step head down			*/
#define	QIC_CMD_SKPB	25	/* skip n segments back			*/
#define	QIC_CMD_SKPF	26	/* skip n segments forward		*/
#define	QIC_CMD_RATE	27	/* select rate				*/
#define	QIC_CMD_NMD	30	/* normal (primary) mode		*/
#define	QIC_CMD_VNDID	32	/* report vendor id			*/
#define	QIC_CMD_TPSTAT	33	/* report tape status			*/
#define	QIC_CMD_SKPBX	34	/* skip n segments back extended	*/
#define	QIC_CMD_SKPFX	35	/* skip n segments forward extended	*/

/* These are Archive/Mountain/Summit variants, not QIC standard. */
#define QIC_CMD_SS2	20	/* A/M/S soft select enable 2		*/
#define QIC_CMD_SS1	23	/* A/M/S soft select enable 1		*/
#define QIC_CMD_SS_OFF	24	/* A/M/S soft select disable		*/

/* These are CMS variants, not QIC standard. */
#define QIC_CMD_NWTP	45	/* CMS new tape				*/
#define QIC_CMD_CSS_ON	46	/* CMS soft select enable		*/
#define QIC_CMD_CSS_OFF	47	/* CMS soft select disable		*/

/************************************************************************/
/*	  QIC 117 Report Drive Status bits				*/
/************************************************************************/
#define QIC_STAT_RDY	0x01	/* drive ready or idle			*/
#define QIC_STAT_ERR	0x02	/* error detected			*/
#define QIC_STAT_PRSNT	0x04	/* cartridge present			*/
#define QIC_STAT_WPROT	0x08	/* cartridge write protected		*/
#define QIC_STAT_NEWCT	0x10	/* new cartridge			*/
#define QIC_STAT_REFD	0x20	/* cartridge referenced			*/
#define QIC_STAT_BOT	0x40	/* at physical BOT			*/
#define QIC_STAT_EOT	0x80	/* at physical EOT			*/

/************************************************************************/
/*	  QIC 117 error codes						*/
/************************************************************************/

/* command received while drive not ready */
#define	QIC_ERR_NOT_READY	1

/* cartridge not present or removed */
#define	QIC_ERR_NO_CART		2

/* motor speed error (not within 1%) */
#define	QIC_ERR_MOTOR_SPEED	3

/* motor speed fault (jammed, or gross speed error) */
#define	QIC_ERR_MOTOR_FAULT	4

/* cartridge write protected */
#define	QIC_ERR_WPROT		5

/* undefined or reserved command code */
#define	QIC_ERR_BAD_CMD		6

/* illegal track address specified for seek */
#define	QIC_ERR_SEEK_TRK	7

/* illegal command in report subcontext */
#define	QIC_ERR_RPT_CMD		8

/* illegal entry into a diagnostic mode */
#define	QIC_ERR_DIAG_ENTRY	9

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


/*************************************************************************
 *	Constants for setting data rate at tape drive (QIC_CMD_RATE).
 *	Rates are in bits/second.
 ************************************************************************/

enum {
	FT_DATA_RATE_250K = 0,
	FT_DATA_RATE_500K = 2,
	FT_DATA_RATE_1MEG = 3,
	FT_DATA_RATE_2MEG = 1
};

#if _KERNEL

/* Command timeout values. */

enum {
	FT_CAL_SECS	= 120,	/* max # seconds for seek load point	*/
	FT_PAUSE_SECS	= 15,	/* max # seconds for pause		*/
	FT_XFER_SECS	= 10,	/* max # seconds for read		*/
	FT_RDY_SECS	= 120,	/* max # seconds for drive ready	*/
	FT_SEEK_SECS	= 15,	/* max # seconds for seek head to track	*/
	FT_SKIP_SECS	= 180,	/* max # seconds for skip fwd or back	*/
	FT_STOP_SECS	= 15,	/* max # seconds for tape stop		*/
	FT_WIND_SECS	= 180	/* max # seconds for wind or rewind	*/
};

/* Tape i/o retry limits. */

enum {
	FT_GET_HEADER_TRIES = 5,	/* # of segments we look at for hdr */
	FT_READ_ID_TRIES = 4,		/* # of tries reading FDC sector ID */
	FT_READ_BLK_RETRIES = 2,	/* # of tries reading FDC sector    */
	FT_SKIP_TRIES = 4,		/* # of tries skipping to a segment */
	FT_WRITE_BLK_RETRIES = 2	/* # of tries reading FDC sector    */
};

extern int	FT_BLAB;
extern int	FT_ACKBLAB;

extern int	FT_NBUF;
extern int	FT_CUSHION;

/* The following format enum's are bound to a table in ftCvt.c */

enum {
	FT_Q80_307 = 0,		/* QIC-80, 307.5'	*/
	FT_Q80_205 = 1,		/* QIC-80, 205'		*/
	FT_Q40_307 = 2,		/* QIC-40, 307.5'	*/
	FT_Q40_205 = 3		/* QIC-40, 205'		*/
};

/* Tape motion, as tracked by the driver. */

enum {
	FT_STOPPED,
	FT_MOVING,
	FT_WINDING		/* High speed mode.	*/
};

/* Physical tape coordinates used in QIC-117 commands. */

struct ftTapeLocn	{
	int	ftTLtrack;
	int	ftTLsegment;
	int	ftTLblock;
};

/* Tape segment i/o error tracking. */

#define FT_NUM_ERR_BLK 3	/* Allow 3 blocks with i/o err per segment. */

struct FtSegErr {
	unsigned char	se_errCount;
	unsigned char	se_errBlock [FT_NUM_ERR_BLK];
};

/* Diskette pseudo coordinates used in commands sent to FDC. */

struct ftDiskLocn	{
	unsigned char	ftDLcylinder;	/* "C" */
	unsigned char	ftDLhead;	/* "H" */
	unsigned char	ftDLsector;	/* "R" */
	unsigned char	ftDLsecsize;	/* For debugging - "N"	*/
};

/* Wait result status. */

enum FtWtRslt {
	FT_RESULT_OK		= 0,
	FT_RESULT_TIMED_OUT	= 1,
	FT_RESULT_SIGNALED	= 2
};

/* I/O direction for low-level block transfer routine. */
enum {
	FT_READ = 33,
	FT_WRITE = 34
};

/* Switch for using or ignoring Bad Block Table. */
enum {
	FT_USE_BBT,
	FT_DONT_USE_BBT
};

/* Driver state variables. */

struct FT {
	/* booleans */
	char	ft_wakeMeUp;		/* 1 = sleeping til next FDC IRQ */
	char	ft_ackNeeded;		/* 1 = awaiting ACK to rpt cmd	*/
	char	ft_ackMissed;		/* 1 = wanted ACK, didn't get	*/
	char	ft_ftruMissed;		/* 1 = missed FINAL TRUE BIT	*/
	char	ft_open;		/* 1 = device is open		*/
	char	ft_write;		/* 1 = open for write, 0=read	*/
	char	ft_hdrOk;		/* 1 = header segment read	*/
	char	ft_refOk;		/* 1 = reference burst found	*/
	char	ft_timedOut;		/* 1 = an FDC op timed out.	*/
	char	ft_seekAdj;		/* 1 = have previous seek adj.	*/
	char	ft_norwdcl;		/* 1 = NO rewind on close	*/

	/* very short ints */
	unsigned char	ft_pcn;		/* present cylinder #		*/
	unsigned char	ft_bitsNeeded;	/* # of Report Next Bit's to do	*/
	unsigned char	ft_bitsRcvd;	/* # of report bits received	*/
	unsigned char	ft_drv;		/* drive number, 0..3		*/
	unsigned char	ft_softsel;	/* soft select, 0,1,2		*/
	unsigned char	ft_qicstat;	/* result of QIC 117 status	*/

	/* short ints */
	unsigned short	ft_report;	/* where reported bits go	*/
	unsigned short	ft_errstat;	/* result of QIC 117 err status	*/

	unsigned short ft_firstDataSeg;	/* # of first data segment	*/
	unsigned short ft_lastDataSeg;	/* # of last data segment	*/

	int	ft_segment;		/* First segment in buffer.	*/
	int	ft_numSegs;		/* # of valid segments in buffer. */

	long	ft_lastseek;		/* just past last byte read/written */
	long	ft_segbase;		/* offset in segments for seeks */

	/* other structs */
	TIM	ft_tim;
};

extern struct FT ft;

/* Global ft driver functions. */

/* ft.c */
int		ftBBT		__PROTO ((int segment, unsigned int *bbtValp));
int		ftBufVtop	__PROTO ((__caddr_t vadd, __paddr_t * paddp));
int		ftCmd		__PROTO ((int cmd));
int		ftFwdTape	__PROTO ((void));
int		ftPauseTape	__PROTO ((void));
int		ftReadID	__PROTO ((struct ftDiskLocn * loc));
int		ftReadBlocks	__PROTO ((int logBlock, int numBlocks,
				  caddr_t readBuf));
int		ftReadyWait	__PROTO ((unsigned int seconds));
int		ftRewindTape	__PROTO ((void));
int		ftSeekTape	__PROTO ((int track));
void		ftSelect	__PROTO ((unsigned char drive,
				  unsigned char softsel));
int		ftSkipBack	__PROTO ((int segs));
int		ftSkipFwd	__PROTO ((int segs));
int		ftStopTape	__PROTO ((void));
int		ftStsWthErr	__PROTO ((void));
enum FtWtRslt	ftWaitWithTimeout __PROTO ((int ticks, char * reason));
int		ftWindTape	__PROTO ((void));

/* ftMotion.c */
int		ftMGetMtn	__PROTO ((void));
void		ftMNewTape	__PROTO ((void));
int		ftMPosTape	__PROTO ((int logSeg));
void		ftMSetMtn	__PROTO ((int motion));
void		ftMSetLoc	__PROTO ((struct ftDiskLocn * dl));
void		ftMSetLocB	__PROTO ((int blockno));

/* ftGRB.c */
int		ftGetRefBurst	__PROTO ((void));

/* ftCvt.c */
int		ftCBlocksLeft	__PROTO ((struct ftTapeLocn * tlp));
int		ftCBlksPerVol	__PROTO ((void));
int		ftCDLInc	__PROTO ((struct ftDiskLocn * dlp));
int		ftCDLtoSec	__PROTO ((struct ftDiskLocn * dlp,
		  int * sector));
int		ftCDLtoLS	__PROTO ((struct ftDiskLocn * dlp,
		  int * logicalSeg));
int		ftCLBtoDL	__PROTO ((int logicalBlk,
				  struct ftDiskLocn * dlp));
int		ftCLStoTL	__PROTO ((int logicalSegIn,
		  struct ftTapeLocn * tapeLocOut));
int		ftCSectoTL	__PROTO ((int sector,
		  struct ftTapeLocn * tapeLocOut));
int		ftCvtSetFmt	__PROTO ((int format));
void		ftCvtSetFtParms	__PROTO ((unsigned int spt, unsigned int tracks,
		  unsigned int heads, unsigned int cyls));
void		ftCvtUnsetFmt	__PROTO ((void));

/* ftx.c */

void		ftxIntr		__PROTO ((void));
int		ftXferFDC	__PROTO ((int logBlock, int numBlocks,
  caddr_t ioBuf, struct ftDiskLocn * dlp, int bbtSw, int ioDir, int retry));
void		ftxSetDrive	__PROTO ((unsigned char drive));

/* ecc */

void		initEcc		__PROTO ((void));
int		decodeEcc	__PROTO ((unsigned char * adr,
		  unsigned char nb, unsigned char errc, unsigned char err0,
		  unsigned char err1, unsigned char err2 ));
void		encodeEcc	__PROTO ((unsigned char * adr,
		  unsigned char nb));

#endif /* _KERNEL */

#endif /* __FT_H__ */
