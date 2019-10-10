/* (-lgl
 *	Coherent 386 release 4.2-Beta
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/*
 * /usr/include/sys/tape.h
 *
 * Mon Nov  1 18:09:51 1993 CST
 */

#ifndef __SYS_TAPE_H__
#define __SYS_TAPE_H__

#define TAPE_IOC	('t' << 8)

#define T_RETENSION	(TAPE_IOC | 0x01) /* Retension tape */
#define T_RWD		(TAPE_IOC | 0x02) /* Rewind tape */
#define T_ERASE		(TAPE_IOC | 0x03) /* Erase tape */
#define T_WRFILEM	(TAPE_IOC | 0x04) /* Write file marks -- not used */
#define T_RST		(TAPE_IOC | 0x05) /* Reset */
#define T_RDSTAT	(TAPE_IOC | 0x06) /* Read status */
#define T_SFF		(TAPE_IOC | 0x07) /* Space Filemark Forward */
#define T_SBF		(TAPE_IOC | 0x08) /* Space Block Forward -- not used */
#define T_LOAD		(TAPE_IOC | 0x09) /* Load -- not used */
#define T_UNLOAD	(TAPE_IOC | 0x0A) /* Unload -- not used */
#define T_SFREC		(TAPE_IOC | 0x0B) /* Not used */
#define T_SBREC		(TAPE_IOC | 0x0C) /* Not used */
#define T_TINIT		(TAPE_IOC | 0x0D) /* Not used */
#define T_SFB		(TAPE_IOC | 0x28) /* Space Filemark Backward */
#define T_SBB		(TAPE_IOC | 0x29) /* Space Block Backward -- not used */

#define T_FORMAT	(TAPE_IOC | 0x0E) /* Format floppy tape */
#define T_SEEK		(TAPE_IOC | 0x20) /* Set byte offset, floppy tape */
#define T_TELL		(TAPE_IOC | 0x21) /* Get byte offset, floppy tape */

#define T_DEFAULT "/dev/tape"

/* Structures for reading tape status. */

/*
 * Floppy tape status - drive and cartridge - each field will be -1 if
 * not available on present equipment.
 */
struct FtStat {
	int	drvcn;		/* Drive Configuration. */
	int	romvn;		/* ROM Version. */
	int	vndid;		/* Vendor ID. */
	int	tpstat;		/* Tape Status. */
	int	qicstat;	/* Drive Status. */
	int	qicerrstat;	/* Drive Error Status. */
};

struct ScsiStat {
	int	unimplemented;
};

enum  TAPE_STAT_TYPE {
	TAPE_STAT_FT = 1,
	TAPE_STAT_SCSI
};

struct TapeStat {
	enum	TAPE_STAT_TYPE	tape_stat_type;
	union TapeStatUnion {
		struct FtStat	ftstat;
		struct ScsiStat	scsistat;
	} tsu;
};

#endif				/* __SYS_TAPE_H__ */
