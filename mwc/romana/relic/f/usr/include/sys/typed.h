/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/* typed.h -- Data structures for an implimentation of internally
 * typed data structures.
 *
 * This file has 5 major sections:
 * TYPED_SPACE DECLARATIONS	-- Declarations for typed_space in general.
 * DATA TYPES			-- Values for typed_space.ts_type.
 *	Make at least one entry here when you add new data types.
 * STRUCTURE DECLARATIONS	-- Declarations for specific typed spaces.
 *	Declare structures that describe the typed_space.ts_data element.
 * FUNCTION DECLARATIONS	-- Function names for specific typed spaces.
 *	Declare all the functions need to manipulate your data type.
 * USEFUL SYMBOLS		-- Symbols needed by specific typed spaces.
 *	Anything else you need to define should go here.
 */

#ifndef __SYS_TYPED_H__
#define __SYS_TYPED_H__

#include <common/feature.h>

/* This file ought to be rewritten to adjust itself based on the contents
 * of the ANSI file limits.h.
 */

typedef signed char int8;
#define MAXINT8		((int8) 127)
typedef unsigned char uint8;
#define MAXUINT8	((uint8) 255)
typedef short int16;
#define MAXINT16	((int16) 32767)
typedef unsigned short uint16;
#define MAXUINT16	((uint16) 65535)
typedef long int32;
#define MAXINT32	((int32) 2^31 - 1)
typedef unsigned long uint32;
#define MAXUINT32	((uint32) (((uint32) 2^32) - 1))

/* TYPED_SPACE DECLARATIONS.  */
typedef int16 space_type;		/* The type of a space.  */

/* This is the fundamental unit of data manipulated by these routines.  */
typedef struct typed_space_struct {
	int32	ts_size;
	space_type	ts_type;
	char	ts_data[0];	/* WARNING: non-ANSI code (works with gcc) */
} typed_space;

/* Macro for declaring a typed_space of length len.  */
#define TYPED_SPACE(name, len, type) struct {		\
	int32	ts_size;				\
	space_type	ts_type;			\
	char	ts_data[len - (sizeof(typed_space))];	\
} name = { (int32) len, type }

/* Macros for overtly altering a typed space.  */
	/* Change the size of a space.  */
#define RESIZE(space, size)	(space)->ts_size = ((int32) size)
	/* Change the type of a space.  */
#define RETYPE(space, type)	(space)->ts_type = ((space_type) type)

/*
 * DATA TYPES:  (values for ts_type in a typed_space)
 * All data types start with "T_".
 * Similar data types share a common prefix which is defined like this:
 * #define T_COMMON	(space_type)0xhh00
 * Specific types are then assigned sequentially from that:
 * #define T_COMMON_SPECIFIC	T_COMMON+n
 */

#define T_UNKNOWN	((space_type)0)

/* FIFO's */
#define T_FIFO		0x100
#define T_FIFO_SIC	T_FIFO+1  /* Static In-core Fifo.  */
#define T_FIFO_DIC	T_FIFO+2  /* Dynamic In-core Fifo (can grow).  */
#define T_FIFO_SP	T_FIFO+3  /* Static Permanent Fifo (fixed size file). */
#define T_FIFO_DP	T_FIFO+4  /* Dynamic Permanent Fifo (ordinary file).  */

/* IBM PC BIOS derived structures */
#define T_BIOS		0x200
#define T_BIOS_DISK	T_BIOS+1  /* Disk geometry information.  */
#define T_BIOS_ROOTDEV	T_BIOS+2  /* Information on boot disk.  */

/* String based structures */
#define	T_STR		0x300
#define T_STR_ARGV	T_STR+1	  /* Argument vector ala main(argc, argv).  */
#define T_STR_STR	T_STR+2   /* NUL terminated string.  */
#define T_STR_ARGF	T_STR+3	  /* Argument FIFO.  */


/* 
 * STRUCTURE DECLARATIONS.
 * These define the typed_space.ts_data elements that go with the above
 * data types.  They should be typedef'd to a name derivable from the
 * type name by removing the leading "T_" (see above.)
 *
 * E.g. For the type T_COMMON create this typedef:
 * typedef common {
 * ...
 * } COMMON;
 *
 * If the specific types need their own structs do them like this:
 * typedef common_specific {
 * ...
 * } COMMON_SPECIFIC;
 *
 */

/* This is the control structure for an open, non-destructive, typed fifo.
 * The elements of this non-destructive, typed fifo are all typed_space's.
 */
typedef struct fifo {
	typed_space *f_space;	/* Where the data really are stored.	*/
	typed_space *f_offset;	/* Current pointer into f_space.	*/
	int16	f_flags;	/* Open for reading or writing, etc...	*/
} FIFO;

/* This is the structure describing the geometry of a disk drive.  */
typedef struct bios_disk {
	uint16 dp_drive;		/* Drive number.		*/
	uint16 dp_cylinders;		/* Number of cylinders.		*/
	uint16 dp_heads;		/* Number of heads.		*/
	uint16 dp_sectors;		/* Sectors per track.		*/
} BIOS_DISK;

/* Describe the partition we are booting off of.  */
typedef struct bios_rootdev {
	uint8 rd_partition;	/* Partition number.  */
} BIOS_ROOTDEV;

/* FUNCTION DECLARATIONS (these should one day be prototypes.)  */

/* FIFO routines */
FIFO *fifo_open();		/* Open a typed space as a fifo.	*/
typed_space *fifo_read();	/* Read a typed space from a fifo.	*/
typed_space *fifo_write();	/* Write a typed space into an open fifo.  */
typed_space *fifo_write_untyped();  /* Write data into an open fifo.  */
int fifo_rewind();		/* Go back to the start of the fifo.	*/
int fifo_close();		/* Finish with using a typed space as a fifo.  */

/* USEFUL SYMBOLS.
 * If appropriate, be sure to define a form of NULL for your new data
 * type.  E.g. for the type COMMON
 * #define COMMON_NULL ((COMMON *) 0)
 *
 * If you need any other symbols define them here.
 */

/* Useful symbols for FIFOS.  */
#define F_NULL	((FIFO *) 0)	/* The void FIFO.  */
#define NFIFOS	20		/* Maximum of 20 fifos at a time... */

/* Values for FIFO.f_flags.  */
#define F_READ	0x0001
#define F_WRITE	0x0002

#if	__KERNEL__
#define BG_LEN 512	/* Length of boot_gift.  */
#endif	/* __KERNEL__ */

#endif	/* ! defined (__SYS_TYPED_H__) */
