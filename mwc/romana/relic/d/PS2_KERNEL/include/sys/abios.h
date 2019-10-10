/* (-lgl
 * 	COHERENT Version 3.2.1
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef ABIOS_H
#define ABIOS_H	ABIOS_H

/* These point to the proper offset in the function transfer tables */
#define START_P 	0
#define	INTERRUPT_P	4
#define TIME_OUT_P	8

/* 
 * Initialization structures.
 */

typedef struct a_sys_parm_struct {
	long common_start_fptr;    /* Far ptr to Start Routine Entry    */
	long common_interrupt_fptr;/* Far ptr to Interrupt Routine Entry*/
	long common_timeout_fptr;  /* Far ptr to TIme Out Routine Entry */
	int  stack;		   /* Amount of stack required by ABIOS */
	long reserved[4];
	int  num_entries;	   /* Number of ABIOS extensions        */
} a_sys_parm;

typedef struct a_init_table_struct {
	int	deviceid;	/* This will always be 02h for the HD */
	int	num_logicalids;	/* The number of HDs with the same id */
	int	db_length;	/* Length of dev block for this dev   */
	long	initdev_fptr;	/* Far ptr to init dev routine entry  */
	int	rb_length;	/* Length of request blk for this dev */
	int	ftt_length;	/* Length of function transfer table  */
	int	dp_length;	/* Required Length of the data ptr    */
 				/* fields in the Common Data Area     */
	char	secdevice_id;	/* Level of hw that ABIOS supports    */
	char	revision;	/* DD revsion that ABIOS supports     */
	int	reserved[3];
} a_init_table;


/* 
 * Floppy Disk Structures
 */

typedef struct f3vals1_struct {	/* Read device Parameters */
	int	sectors_track;
	int	sector_size;
	int	device_control;
	int	drive_type;
	long	reserved;	/* Init to 0 */
	long	motor_off;	/* Time to delay before turning motor off */
	long	motor_on;
	int	reserved1;
	int	cylinders;
	int	reserved2;
	char	heads;
	char	retries;
	char	fill_byte;	/* Fill byte for formats */
	long	head_settle;
	char	gap_rwv;	/* Gap length for read/write/verify */
	char	gap_format;	/* Gap length for format */
	char	dlength;	/* Data length */
	char	reserved3[6];
	char	status;		/* Disk controller status byte */
} f3vals1;

typedef struct f5vals1_struct {	/* Reset/Initialize Floppy Disk */
	int	reserved;	/* Init to 0 */
} f5vals1;

typedef struct f8vals1_struct {	/* Read/write floppy disk */
	int	reserved;	/* Init to 0 */
	long	dummy;
	long	reserved1;	/* Init to 0 */
	long	dptr;		/* 32 bit data pointer */
	int	reserved2;	/* Init to 0 */
	long	wait_time;	/* Time to wait */
	int	sectors_read;	/* Before the call - Number of sectors
				 * to read. After the call completes -
				 * number of sectors read. */
	int	cylinder;	/* Cylinder number 0 based */
	int	dummy1;
	char	head;		/* Head number */
	char	dummy2[6];
	int	sector;		/* Sector number */
} f8vals1;

typedef struct fbvals1_struct {	/* Verify floppy disk */
	char	dummy[6];
	long	reserved1;	/* Init to 0 */
	int	reserved2;	/* Init to 0 */
	long	rb_address;	/* Relative block address */
	long	reserved3;	/* Init to 0 */
	long	wait_time;	/* Time to wait */
	int	blocks_verify;	/* Before the call - Number of blocks 
				 * to verify.After the call completes 
				 * - number of blocks verified. */
	char	caching;	/* 0-caching is ok, 1-do not cache */
	int	soft_error;
} fbvals1;

typedef struct ffvals1_struct {	/* Turn disk motor off */
	int	reserved;	/* Init to 0 */
} ffvals1;

typedef struct request_block_struct_fl {
	int	length;		/* Request block length */
	int	logical_id;	/* Logical Id of the device */
	int	unit;		/* Which Hard disk: 0 - 1st etc */
	int	function;	/* Function number to execute */
	long	reserved;	/* Init to 0 */
	int	ret_code;	/* Value returned by the function */
	int	time_out;
	union	
	{
		f3vals1	f3;
		f5vals1	f5;
		f8vals1	f8;
		fbvals1	fb;
		ffvals1	ff;
		char	padding[0x41];	/* To make the Requset Block 
						at least 0x51 bytes long */
	} vars;
} request_block_fl;


/* 
 * Hard Disk Structures
 */

typedef struct f3vals2_struct {	/* Read device Parameters */
	int	sectors_track;
	int	sector_size;
	int	device_control;
	int	drive_type;
	long	cylinders;
	char	heads;
	char	retries;
	long	block_addresses;
	long	reserved;	/* Init to 0 */
	long	reserved1;
	int	max_blocks;
} f3vals2;

typedef struct f5vals2_struct {	/* Reset/Initialize Fixed Disk */
	int	reserved;	/* Init to 0 */
	char	dummy[22];
	long	wait_time;
} f5vals2;

typedef struct f8vals2_struct {	/* Read/Write fixed disk */
	int	reserved;	/* Init to 0 */
	long	dummy;
	long	reserved1;	/* Init to 0 */
	long	dptr;		/* 32 bit data pointer */
	int	reserved2;	/* Init to 0 */
	long	rb_address;	/* Relative block address */
	long	reserved3;	/* Init to 0 */
	long	wait_time;	/* Time to wait */
	int	blocks_read;	/* Before the call - Number of blocks 
				 * to read. After the call completes 
				 * - number of blocks read. */
	char	caching;	/* 0-caching is ok, 1-do not cache */
	int	soft_error;
} f8vals2;

typedef struct fbvals2_struct {	/* Verify fixed disk */
	char	dummy[6];
	long	reserved1;	/* Init to 0 */
	int	reserved2;	/* Init to 0 */
	long	rb_address;	/* Relative block address */
	long	reserved3;	/* Init to 0 */
	long	wait_time;	/* Time to wait */
	int	blocks_verify;	/* Before the call - Number of blocks 
				 * to verify.After the call completes 
				 * - number of blocks verified. */
	char	caching;	/* 0-caching is ok, 1-do not cache */
	int	soft_error;
} fbvals2;

typedef struct request_block_struct_hd {
	int	length;		/* Request block length */
	int	logical_id;	/* Logical Id of the device */
	int	unit;		/* Which Hard disk: 0 - 1st etc */
	int	function;	/* Function number to execute */
	long	reserved;	/* Init to 0 */
	int	ret_code;	/* Value returned by the function */
	int	time_out;
	union	
	{
		f3vals2	f3;
		f5vals2	f5;
		f8vals2	f8;
		fbvals2	fb;
		char	padding[0x72];	/* To make the Requset Block 
						0x80 bytes long */
	} vars;
} request_block_hd;

#endif
