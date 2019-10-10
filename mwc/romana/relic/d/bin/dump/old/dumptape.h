/*
 * Dump tapes.
 * A dump tape begins with a header
 * record. This contains the attributes of the
 * tape.
 * The remainder of the tape is filled with
 * arrays of dumpdata records. The map comes first,
 * then all the directories, then all the files.
 */
#ifndef  TYPES_H
#include <types.h>
#endif
#ifndef  DIR_H
#include <dir.h>
#endif
#ifndef  INO_H
#include <ino.h>
#endif

/*
 * Dump tape header.
 * All entries are in cannonical
 * format on the tape.
 */
struct	dumpheader
{
	int	dh_magic;		/* Magic number */
	ino_t	dh_nino;		/* # of inodes on file system */
	time_t	dh_bdate;		/* Date at beginning of dump */
	time_t	dh_ddate;		/* Dump since date */
	int	dh_level;		/* Dump level */
	int	dh_reel;		/* Reel number in dump */
	fsize_t	dh_nbyte;		/* Length of disc */
	int	dh_blocking;		/* Blocking factor */
	char	dh_dev[DIRSIZ];		/* Name of dumped device */
	char	dh_fname[6];		/* File system name */
	char	dh_fpack[6];		/* File system pack name */
	int	dh_checksum;		/* Byte checksum */
};

#define	DH_MAG	0123456			/* Magic number */

/*
 * Dump data blocks.
 * These are all a big union.
 * They are packed into much larger
 * blocks on the tape.
 */
union	dumpdata
{
	struct	{
		int	dd_type;	/* Type = DD_EOT */
	};
	struct	{
		int	dd_type;	/* Type = DD_DATA */
		ino_t	dd_ino;		/* Inode number */
		daddr_t	dd_block;	/* Block number in file */
		int	dd_size;	/* Bytes used in this block */
		char	dd_data[BUFSIZ];/* Data */
	};
	struct	{
		int	dd_type;	/* Type = DD_INO */
		ino_t	dd_ino;		/* Inode number */
		struct	dinode dd_dinode; /* On disc inode */
	};
	struct	{
		int	dd_type;	/* Type = DD_MAP */
		ino_t	dd_ino;		/* Base inode of this map block */
		int	dd_nmap;	/* # of map entries */
		char	dd_map[BUFSIZ];	/* Some map */
	};
};

#define	DD_EOT	0			/* End of dump */
#define	DD_INO	1			/* Inode */
#define	DD_DATA	2			/* Data block */
#define	DD_MAP	3			/* Map */

#define	DD_BUSY	01			/* Inode in use */
#define	DD_DUMP	02			/* Inode dumped */
#define	DD_DIR	04			/* Inode is a directory */
#define	DD_DEJA	010			/* Already seen flag (dump) */
#define	DD_HERE	(DD_BUSY|DD_DUMP)	/* On the tape */

/*
 * This structure is used
 * to hold the dump dates in the
 * file `/etc/ddate'.
 */
struct	idates
{
	char	id_name[DIRSIZ];	/* Device name */
	int	id_incno;		/* Level */
	time_t	id_ddate;		/* The date of the dump */
};

#define	DDATE	"/etc/ddate"		/* Date file name */
#define	DTAPE	"/dev/dump"		/* Default dump tape */
#define	DFSYS	""			/* No default file system */
