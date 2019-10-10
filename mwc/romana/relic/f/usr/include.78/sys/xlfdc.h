/*
 * /usr/include/sys/xlfd.h
 *
 * FDC support for Archive floppy tape.
 * This header should disappear soon!
 *
 * Revised: Fri May 21 13:32:25 1993 CDT
 */
#ifndef __SYS_XLFD_H__
#define __SYS_XLFD_H__

#define	FD0	0	/* major device number for floppy disk */

/*
 * the floppy disk minor device number is interpreted as follows:
 *     bits:
 *	 7  4 3 21 0
 * 	+----+-+--+-+
 * 	|fmt |s|pt|u|
 * 	+----+-+--+-+
 *     codes:
 *	u   - unit no. (0 or 1)
 *	pt  - partition no. (0 - 3)
 *	s   - single/double sided (1 = single)
 *	fmt - format code, no. of bytes per sector/ no. of sectors per trk.
 */

#define	PARTITION(x)	((getminor(x) & 0x06) >> 1)
#define UNIT(x)		(getminor(x) & 0x01)
#define	FRMT(x)		((getminor(x) & 0xf0) >> 4)
#define	SIDES(x)	((getminor(x) & 0x08) ? 1 : 2)

/*
 * Defines for controller access.
 */
#define	FDSTAT	0x3f4	/* I/O port addr of floppy controller status port */
#define	FDDATA	0x3f5	/* I/O port addr of floppy controller data port */

#define	FCBUSY	0x10	/* controller busy bit */
#define	IODIR	0x40	/* data reg I/O direction, 1 = read, 0 = write */
#define	IORDY	0x80	/* data register ready to xfer bit */

#define	FCRETRY	4000	/* this many ten microseconds equals 40ms. */

#define	CTIMOUT	0x02	/* Timed out waiting for IORDY in fdcmd */
#define	RTIMOUT	0x03	/* Timed out waiting for IORDY in fdresult*/
#define	NECERR	0x04	/* Controller wont go idle error flag */

#define ONESIDE 0x08    /* indicates in minor dev that we want single sided */

#define NUMDRV  2           /* maximum number of drives supported */
#define NHDS    2           /* default number of heads */
#define NSECS   9           /* default number of sectors per track */
#define NCYLS   40          /* default number of cylinders */
#define SECSIZE 512         /* default sector size */
#define SECSHFT 9
#define SECMASK (SECSIZE-1)

#define FD          ('R'<<8)
#define FDSPARAM    (FD | 00)   /* set the drive paramaters */
#define FDGPARAM    (FD | 01)   /* get the drive paramaters */
#define FDFMTTRK    (FD | 02)   /* format a single track */

#define OPEN_EMAX   1       /* max number of retries during open processing */
#define FORM_EMAX   3       /* max number of retries during format */
#define TRYRESET    5       /* try a reset after this many errrors */
#define NORM_EMAX   10      /* normal max number of retries */

#define RUNTIM  3
#define WAITTIM 4
#define LOADTIM 10
#define MTIME   100	/* no. of clock ticks in one second */
#define ETIMOUT 50	/* no. of clock ticks in 1/2 second */
#define T25MS   3	/* no. of clock ticks in 25 milliseconds */
#define T500MS  50	/* no. of clock ticks in 500 milliseconds */
#define T50MS   5	/* no. of clock ticks in 50 milliseconds */
#define T750MS  75	/* no. of clock ticks in 750 milliseconds */

#define OPENED  0x01
#define OPENING 0x02
#define RECAL   0x04
#define CLOSING 0x08
#define EXCLUSV 0x10
#define RSTDOPN 0x20

#define WINTR   0x01
#define	WRESET	0x02

#define D_NTRK  40      /* 40 tracks - double density */
#define Q_NTRK  80      /* 80 tracks - quad density */


#define FDCSR1      0x03F7
#define DOOROPEN    0x80
#define FDCTRL      0x03F2
#define NORESET     0x04
#define ENABINT     0x08
#define ENABMOTOR   0x10

#define FD0BSY      0x01    /* drive is seeking */
#define FD1BSY      0x02
#define FD2BSY      0x04
#define FD3BSY      0x08
#define FCBSY       0x10    /* controller is busy */
#define NODMA       0x20    /* controller in non-DMA mode */

/*
 * Floppy controller commands
 */
#define RDCMD       0x26
#define SEEK        0x0F
#define FORMAT      0x0D
#define READID      0x0A
#define SENSE_INT   0x08
#define REZERO      0x07
#define WRCMD       0x05
#define SENSE_DRV   0x04        /* read status register 3 */
#define SPECIFY     0x03

#define READDEL     0x0C
#define WRITEDEL    0x09
#define READTRACK   0x02
#define RAWCMD      0x7F	/* command is thru raw I/O ioctl */

#define INVALID     0x80        /* status register 0 */
#define ABNRMTERM   0x40
#define SEEKEND     0x20
#define EQCHK       0x10
#define NOTRDY      0x08

#define EOCYL       0x80        /* status register 1 */
#define CRCERR      0x20
#define OVRRUN      0x10
#define NODATA      0x04
#define MADR        0x01

#define FAULT       0x80        /* status register 3 */
#define WPROT       0x40
#define RDY         0x20
#define TWOSIDE     0x08

/* NEW_HARDWARE CMOS drive descriptions */
#define DRV_NONE    0x00
#define DRV_DBL     0x01
#define DRV_QUAD    0x02

	    /* encodings for the 'fdf_den' field in structure 'fdparam' */
#define DEN_MFM 0x40        /* double density disks */
#define DEN_FM  0x00        /* single density disks */

	    /* encodings for the 'fdf_bps' field in structure 'fdparam' */
#define BPS128  0           /* 128 bytes per sector */
#define BPS256  1           /* 256 bytes per sector */
#define BPS512  2           /* 512 bytes per sector */
#define BPS1024 3           /* 1024 bytes per sector */
#define MAXBPS  3           /* maximum value of the 'fdf_bps' field */

#define	FDNPART	3	/* number of partitions supported */
#define	FDMEMSIZE	(36*512)
/*
 * partition table for floppy disks
 * we support, 3 different partitions:
 * 	0 - the whole disk;
 *	1 - the first cylinder on the disk
 *	2 - the whole disk minus the first cylinder
 */
struct fdpartab {
	int	startcyl;	/* cylinder no. where partition starts */
	int	numcyls;	/* number of cylinders in partition */
};

#define	FDNSECT	8	/* no. of sector size/count types supported */
/*
 * sector table for floppy disks,
 * specifies number of bytes per sector and
 * number of sectors per track.
 */
struct fdsectab {
	unsigned short	fd_ssize;	/* number of bytes in a sector */
	unsigned short	fd_sshift;	/* shift to convert bytes to sectors */
	unsigned char	fd_nsect;	/* number of sectors per track */
	unsigned char	fd_drvs;	/* bitmask of drive types supporting */
	unsigned char	fd_gpln;	/* normal gap length */
	unsigned char	fd_gplf;	/* format gap length */
};

struct fdstate {
	char     fd_status;
	char     fd_maxerr;
	unsigned char  fd_drvtype;
	unsigned char  fd_trnsfr;
	unsigned char  fd_hst;
	unsigned char  fd_mst;
	unsigned char  fd_dstep;
	unsigned short fd_cylskp;
	unsigned short fd_ncyls;
	unsigned short fd_secsiz;
	unsigned short fd_secmsk;
	unsigned char  fd_secsft;
	unsigned char  fd_nsides;
	unsigned char  fd_nsects;
	unsigned char  fd_cylsiz;
	unsigned char  fd_curcyl;
	unsigned char  fd_lsterr;
	unsigned short fd_n512b;
	dev_t	       fd_device;
	struct   proc *fd_proc;
	unsigned char  fd_fmt;		/* auto format detection */
	unsigned char  fd_dskchg;	/* if == 0, format is up to date */
};

struct fdparam {        /* used by FDSPARAM and FDGPARAM commands */
	char    fdf_bps;    /* number of bytes per sector - encoded */
	char    fdf_spt;    /* number of sectors per track */
	char    fdf_gpln;   /* gap length for normal R/W operations */
	char    fdf_gplf;   /* gap length for format operations */
	char    fdf_dtl;    /* length of sector if 'fdf_bps' = 0 */
	char    fdf_fil;    /* fill byte to use while formatting */
	char    fdf_den;    /* FM or MFM encoding */
	char    fdf_nhd;    /* number of heads */
	short   fdf_ncyl;   /* number of cylinders */
};

struct fdformid {       /* used by FDFMTTRK command */
	char    fdf_track;
	char    fdf_head;
	char    fdf_sec;
	char    fdf_secsiz;
};

#define FIOC		('F'<<8)
#define	F_DTYP		(FIOC|60)	/* returns fd_drvtype */
#define F_FCR		(FIOC|61)	/* output to Floppy Control Register */
#define F_DOR		(FIOC|62)	/* output to Digital Output Register */
#define F_RAW		(FIOC|63)	/* general raw controller interface */

struct fdraw {			/* used by F_RAW command */
	char	fr_cmd[10];	/* user-supplied command bytes */
	short	fr_cnum;	/* number of command bytes */
	char	fr_result[10];	/* controller-supplied result bytes */
	short	fr_nbytes;	/* number to transfer if read/write command */
	char   *fr_addr;	/* where to transfer if read/write command */
};

struct  fdbufstruct {
	int         fbs_flags;
	caddr_t     fbs_addr;
	unsigned    int     fbs_size;
};

#endif	/* __SYS_XLFD_H__ */
