/*
 * gift.c -- Prepare a gift of information for the program currently loaded.
 *
 * To pass a new data structure into the kernel:
 *
 * 1. Define your new data structure in typed.h.  You will probably want
 *    to define some supporting routines for your data structure.  These
 *    should go in a file by themselves.  Be sure to add the file to the
 *    tboot Makefile.
 * 
 * 2. Write a routine that takes at least an ffp, which will generate your
 *    data structure and write it into the ffp.  The routine should return 0
 *    if it ran out of space in the FIFO.  Other return values are permissible,
 *    but ignored.  Add arguments to prepare_gift() as needed.  It is called
 *    only from the end of execute() in execute.c.
 *
 * 3. Add a call to your routine to prepare_gift() in the section marked
 *    FILL THE BOX.  This is an if statement with || seperated calls.  The
 *    most important data structures should be called first, because later
 *    calls will be skipped if the FIFO fills.
 *
 * 4. In the kernel (probably in a driver) you will want to add a loop to
 *    look through the gift for your data structure:
 *
 *    FIFO *ffp;
 *    typed_space *tp;
 *
 *    ffp = fifo_open(&boot_gift, 0);	-- Open gift for reading. 
 * 
 *    if (F_NULL == ffp) {
 *	  indicate_error("Could not open boot_gift.");
 *    } else {
 *        while (T_NULL != (tp = fifo_read(ffp))) {	-- While not EOFIFO.
 *	      if (T_MYTYPE == tp->ts_type) {	-- Is this my type?
 *		  my_handler(tp->ts_data);	-- Process the data.
 *	      }
 *        }
 *    }
 *
 *    Be sure to include fifo.c and typed.h into your kernel.
 *	  
 */
#include <string.h>
#include <sys/fdisk.h>
#include <sys/typed.h>
#include "tboot.h"


#define GIFTBOX	4096 /* Size of the gift box (maximum size of gift.)  */


/* We have to build the gift in the local segment and then copy it in
 * place.  In a better world, the gift could be built in place.
 */
TYPED_SPACE(local_gift, GIFTBOX, T_FIFO_SIC); /* Static In-Core Fifo.  */


/* Prepare a gift of information for the program currently loaded.
 *
 * The gift is a Static In-Core FIFO whose objects are typed spaces.
 *
 * argv is the command vector needed by gift_argf().
 *
 * It should be placed in memory at data_seg:offset.
 */
void
prepare_gift(data_seg, offset, argv)
	uint16 data_seg;
	uint16 offset;
	char *argv[];
{
	FIFO *ffp;		/* Fifo pointer for a handle.  */
	typed_space gift_header;	/* Header for destination box.  */
	char buff[sizeof("65536")];	/* For outputting base 10 integers.  */

	/* Value of ds register, initialized in Startup.s.  */
	extern uint16 myds;

	/* Find out how big a gift we may offer.  */
	ffcopy(&gift_header, myds, offset, data_seg, sizeof(typed_space));
	if (gift_header.ts_size > local_gift.ts_size) {
		puts("WARNING: argument structures larger than ");
		itobase((int) local_gift.ts_size, buff, 10);
		puts(buff);
		puts(" bytes are not currently supported.\r\n");
		puts("Truncating down from ");
		itobase((int) gift_header.ts_size, buff, 10);
		puts(buff);
		puts(".\r\n");
		puts("This is probably harmless.\r\n");
	} else {
		/* Requested gift is smaller than what we can offer.  */
		local_gift.ts_size = gift_header.ts_size;
	}

	/* ASSERTION: local_gift is now no larger than boot_gift.  */

	/* Open the local gift box.  */
	if (F_NULL == (ffp = fifo_open(&local_gift, 1))) {
		puts("prepare_gift():  Can't open local_gift for writing.\r\n");
		puts("prepare_gift():  No information will be passed.\r\n");
		return;
	}

	/* FILL THE BOX.  */
	/* Stop filling in if we run out of space.  */
	if ((0 == gift_drive_params(ffp)) ||
	    (0 == gift_rootdev(ffp)) ||
	    (0 == gift_argf(ffp, argv))
	   ) {
	   	puts("prepare_gift(): WARNING: not enough room for all arguments.\r\n");
		puts("Only ");
		itobase((int) ((char *)ffp->f_offset - (char *)ffp->f_space), buff, 10);
		puts(buff);
		puts(" bytes submitted.\r\n");
	}

	fifo_close(ffp);

	/* Copy the gift into the loaded program.  */
	ffcopy(offset, data_seg, &local_gift, myds,
		(uint16) local_gift.ts_size);
} /* prepare_gift() */


/* Load the BIOS parameters loaded up by the startup code.  */
int
gift_drive_params(ffp)
	FIFO *ffp;
{
	int i;
	int num_of_drives;
	struct reg r;
	BIOS_DISK diskparms;
	
	
	num_of_drives = get_num_of_drives();

	if (verbose_flag) {
		puts("Found 0x");
		print16(num_of_drives);
		puts(" drives.\r\n");
	}

	for (i = 0; i < num_of_drives; ++i) {
	 
		r.r_ax = DISK_PARAMS;
		r.r_dx = HARD_DRIVE + i;
	
		intcall(&r, &r, DISKINT);	/* Ask the BIOS.  */

		diskparms.dp_drive = i;

		/* ch is the lower 8 bits of number of cylinders.
		 * The high two bits of cl are the top two bits of
		 * 10 bit number of cylinders.
		 *
		 * The cylinder count is actually 1 short.
		 */
		diskparms.dp_cylinders = (uint16) (
			  ((LOW(r.r_cx) >> 6) * 256) + /* Top two bits...  */
			  (HIGH(r.r_cx) + 1)
			);

	
		/* The head count is actually 1 short.  */

		diskparms.dp_heads = ((uint16) HIGH(r.r_dx)) + 1;
	
		/* Only the lower 6 bits of cl are the sectors per track.  */
		
		diskparms.dp_sectors = (uint16) (SIXBITS & LOW(r.r_cx));

		/* Store these parameters for delivery.  */
		if (T_NULL == fifo_write_untyped(ffp,
						 &diskparms,
						 (int32) sizeof(BIOS_DISK),
						 T_BIOS_DISK)
		   ) {
			puts("gift_drive_params() ran out of space.");
			return(0);
		}

	} /* for i = 0 to num_of_drives - 1 */

	return(1);	/* We didn't run out of space.  */

} /* gift_drive_params() */

/*
 * We'd really rather have a dynamic in-core fifo, but they are not
 * yet implimented.  We'll have to settle for a fixed length argument list.
 */
TYPED_SPACE(argf, BLOCK, T_FIFO_SIC);

/*
 * To read this item from bootgift, use the procedure outlined above in
 * point 4 to find the entry marked T_STR_ARGF.  You must then explicitly
 * recast it with RETYPE(tp->ts_data, T_FIFO_SIC).  Then you can open it
 * as a FIFO, with code modeled on point 4 above.  This scheme seemed
 * the simplest for uniquely identifying the argument FIFO.
 * Each element of the FIFO is a T_STR_STR, so ts_data for these is
 * just a NUL terminated string.
 */

/*
 * Write an argument fifo into ffp from the argument vector argv[].
 * Returns 0 if it runs out of space, 1 on success, 2 if it could not
 * open the local gift box for some reason.
 */
int
gift_argf(ffp, argv)
	FIFO *ffp;
	char *argv[];
{
	FIFO *argfp;	/* For the argument fifo we're going to build.  */
	char *current_token;
	int i;

	/* Open the local gift box.  */
	if (F_NULL == (argfp = fifo_open(&argf, 1))) {
		puts("gift_argf():  Can't open local argument fifo for writing.\r\n");
		puts("gift_argf():  No command line information will be passed.\r\n");
		return(2);
	}

	current_token = argv[0];
	/*
	 * Build the command fifo.
	 */
	for (i = 0; NULL != current_token; current_token = argv[++i]) {
		if (T_NULL == 
			fifo_write_untyped(argfp,
				   current_token,
				   (int32) (strlen(current_token) + 1),
				   T_STR_STR)
		) {
			puts("gift_argf():  WARNING:  Command line too long.\r\n");
			puts("Truncating.\r\n");
			break;
		}
	}


	/*
	 * Truncate argf to the minimum size needed.
	 */
	RESIZE(&argf, fifo_len(argfp));

	/*
	 * Now that we've filled the FIFO, let's mark
	 * it as an argument fifo.
	 */
	RETYPE(&argf, T_STR_ARGF);

	close(argfp);

	/* Write it into ffp.  */
	if (T_NULL == fifo_write(ffp, &argf)) {
		puts("gift_argf() ran out of space.");
		return(0);
	}

	return(1);
} /* gift_argf() */

/*
 * Write a structure describing the boot partition into a fifo.
 * Returns 1 on success, 0 if it runs out of space, or 2 if it
 * can't read the boot block.
 */
int
gift_rootdev(ffp)
	FIFO *ffp;
{
#define	NOPARTITION	255	/* Convenient illegal partition number.  */
#define PART_PER_DRIVE	4	/* Number of partitions per drive.  */

	BIOS_ROOTDEV myrootdev;	/* Build the data to be passed here.  */
	FDISK_S fp[NPARTN];	/* Room for a partition table from disk.  */
	int8 i;

	extern uint8 drive;	/* Drive number from secondary boot.  */
	extern uint32 first;	/* Block number of start of partition.  */
	
	/* Fetch the partition table from disk.  */
	if (0 == fdisk(fp)) {
		puts("gift_rootdev() WARNING: invalid boot block.\r\n");
		return(2);
	}

	/* Mark an invalid partition.  */
	myrootdev.rd_partition = NOPARTITION;

	/* Look for the current partition in the table.  */
	for (i=0; i < NPARTN; ++i) {
		if (first == fp[i].p_base) {
			myrootdev.rd_partition = i;
			break;
		}
	}

	if (NOPARTITION == myrootdev.rd_partition) {
		puts("gift_rootdev() WARNING: Can't find my partition.\r\n");
		return(2);
	}

	/* Adjust the partition for the drive number.  */
	myrootdev.rd_partition += (PART_PER_DRIVE * drive);

	if (T_NULL == fifo_write_untyped(ffp,
					 &myrootdev,
					 (int32) sizeof(BIOS_ROOTDEV),
					 T_BIOS_ROOTDEV)
	) {
		puts("Ran out of space in gift_rootdev().\r\n");
		return(0);
	} else {
		return(1);
	}
} /* gift_rootdev() */

/* Dump the contents of boot_gift.  */
void
dump_gift()
{
	extern typed_space boot_gift;

	puts("Dumping boot_gift.\r\n");
	dump_fifo(&boot_gift);
}

/* Dump the contents of a fifo.  */
void
dump_fifo(fifo)
	typed_space *fifo;
{
    FIFO *ffp;
    typed_space *tp;

    puts("Dumping a fifo.\r\n");

    if (F_NULL == (ffp = fifo_open(fifo, 0))) {	/* Open gift for reading.  */
	puts("Can't open fifo.\r\n");
	return;
    }

    while (T_NULL != (tp = fifo_read(ffp))) {	/* While not EOFIFO.  */
	switch(tp->ts_type) {
	case T_BIOS_DISK:
		dump_bios_disk(tp->ts_data);
		break;
	case T_BIOS_ROOTDEV:
		dump_rootdev(tp->ts_data);
		break;
	case T_FIFO_SIC:
		dump_fifo(tp);
		break;
	case T_STR_STR:
		puts("String: ");
		puts(tp->ts_data);
		break;
	case T_STR_ARGF:
		RETYPE(tp, T_FIFO_SIC);
		dump_fifo(tp);
		break;
	default:
		puts("Unknown type: 0x");
		print16(tp->ts_type);
		break;
	}
	puts("\r\n");
    }
    puts("Nothing more to dump in this fifo.\r\n");

} /* dump_gift() */

/* Dump a T_BIOS_DISK typed_space.  */
void
dump_bios_disk(a_disk)
	BIOS_DISK *a_disk;
{
	char buffer[BLOCK];

	puts("Dumping a BIOS_DISK struct.\r\n");

	puts("Drive ");
	itobase((uint16) a_disk->dp_drive, buffer, 10);
	puts(buffer);

	puts(":  Cylinders=");
	itobase((uint16) a_disk->dp_cylinders, buffer, 10);
	puts(buffer);

	puts("  Heads=");
	itobase((uint16) a_disk->dp_heads, buffer, 10);
	puts(buffer);

	puts("  Sectors per track=");
	itobase((uint16) a_disk->dp_sectors, buffer, 10);
	puts(buffer);

	puts("\r\n");
} /* dump_bios_disk() */


/* Dump a T_BIOS_ROOTDEV typed_space.  */
void
dump_rootdev(a_rootdev)
	BIOS_ROOTDEV *a_rootdev;
{
	puts("Dumping a BIOS_ROOTDEV struct.\r\n");

	puts("partition: 0x");
	print8(a_rootdev->rd_partition);
	puts("\r\n");
} /* dump_rootdev() */
