/*
 * File:	fifo.c
 *
 * Purpose:	allow kernel to fetch data from real-mode bootstrap data area
 *
 * $Log:	fifo.c,v $
 * Revision 1.2  92/08/04  12:30:58  bin
 * changed for kernel 59
 * 
 * Revision 1.2  92/01/06  11:59:11  hal
 * Compile with cc.mwc.
 * 
 */

/*
 * Includes.
 */
#define KERNEL
#include <sys/typed.h>

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
FIFO * fifo_open();
int fifo_close();
typed_space * fifo_read();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 *
 * Arguments are passed into the kernel through boot_gift.
 * If you start getting "Not enough room for all arguments." messages
 * at boot time, just increase the BG_LEN  to whatever you need.
 * This structure is EXACTLY BG_LEN bytes long.
 */
TYPED_SPACE(boot_gift, BG_LEN, T_FIFO_SIC);

/*
 * fifo_open()
 *
 * Open a typed space as a fifo.
 *
 * Takes a typed_space that is already allocated, and a mode.    The type of
 * the typed space must be a FIFO.  Only T_FIFO_SIC has been implimented
 * (static, in-core fifo).
 *
 * The mode indicates whether to open for reading or writing.
 *  mode == 0 means read only.
 *  mode == 1 means write only.
 *  Other values are illegal.
 *
 * Returns a pointer to an initialized FIFO structure.  FIFO structures are
 * allocated from a pre-allocated array.  Returns F_NULL if it can't open
 * the fifo.
 */
FIFO *
fifo_open(fifo_space, mode)
	typed_space *fifo_space;
	int mode;
{
	/* ff_table is a table of FIFO structures which can be allocated on
	 * demand.  It is functionally similiar to the file descritor table
	 * in the kernel.
	 */
	static FIFO ff_table[NFIFOS];
	static int inited = (1==2);	/* Has ff_table been initialized?  */

	int i;		/* A handy counter.  */
	FIFO *the_fifo;	/* The fifo we are going to allocate.  */

	/* Initilize ff_table the first time we get called.  */
	if (!inited) {
		for (i = 0; i < NFIFOS; ++i) {
			ff_table[i].f_space = F_NULL;
			ff_table[i].f_flags = 0;
		}
		inited = 1;
	}

	/* Check the type of the space we were passed.  */
	switch (fifo_space->ts_type) {
	case T_FIFO:	/* Overly general type, assuming SIC.  */
		fifo_space->ts_type = T_FIFO_SIC;
		break;
	case T_FIFO_SIC:	/* Static In-core Fifo.  */
		break;
	case T_FIFO_DIC:	/* Dynamic In-core Fifo (can grow).  */
		return(F_NULL);	/* Unimplimented.  */
	case T_FIFO_SP:	/* Static Permanent Fifo (fixed size file). */
		return(F_NULL);	/* Unimplimented.  */
	case T_FIFO_DP:	/* Dynamic Permanent Fifo (ordinary file).  */
		return(F_NULL);	/* Unimplimented.  */
	default:
		return(F_NULL);	/* Illegal type encountered.  */
	}

	/* ASSERTION: fifo_space is a valid and implimented FIFO.  */

	/* Find the first free FIFO structure.  */

	/* This should be re-implimented using a malloc-based scheme.
	 * At the moment, the tertiary boot libraries do not include a
	 * malloc.
	 */
	for (i = 0; (i < NFIFOS) && (0 != ff_table[i].f_flags); ++i) {
		/* Do nothing else.  */
	}

	if (NFIFOS == i) {
		return(F_NULL);	/* No more free fifo structs.  */
	}

	the_fifo = &(ff_table[i]);

	/* ASSERTION: the_fifo points at a FIFO we can take.  */

	/* Initialize the FIFO struct.  */
	the_fifo->f_space = fifo_space;
	the_fifo->f_offset = fifo_space->ts_data;

	/* Initilize the flags.  */
	switch(mode) {
	case 0:	/* read */
		the_fifo->f_flags |= F_READ;
		break;
	case 1:	/* write */
		the_fifo->f_flags |= F_WRITE;
		break;
	default:
		return(F_NULL);	/* Illegal mode flag.  */
	}
		
	return(the_fifo);
} /* fifo_open() */

/*
 * fifo_close()
 *
 * Finish with using a typed space as a fifo.
 * Free up FIFO structure associated with a typed space.
 * Returns 0 if ffp was not open, 1 otherwise.
 */
int
fifo_close(ffp)
	FIFO *ffp;
{
	if (0 == ffp->f_flags) {
		return(0);	/* This ffp is not open.  */
	}
	ffp->f_space = F_NULL;
	ffp->f_offset = T_NULL;
	ffp->f_flags = 0;

	return(1);
} /* fifo_close() */

/*
 * fifo_read()
 *
 * Read a typed space from a fifo.
 * Return a pointer to the next typed space in the fifo ffp.  Returns
 * NULL on end of fifo.
 *
 * This read assumes that ffp->f_space has type T_FIFO_SIC.
 */
typed_space *
fifo_read(ffp)
	register FIFO *ffp;
{
	typed_space *retval;

	/* Read MUST be set.  */
	if (F_READ != F_READ & ffp->f_flags ) {
		return(T_NULL);  /* This ffp is not open for reading.  */
	}

	/* From here to the end of fifo_read is really fifo_read_sic().  */


	/* Space of size 0 marks EOFIFO.  */
	if ((long)0 == ffp->f_offset->ts_size) {
		retval = T_NULL;
	} else {
		/* Return the next space.  */
		retval = ffp->f_offset;
		/* Advance to the next space.  */
		(char *) ffp->f_offset += ffp->f_offset->ts_size;
	}

	return(retval);
} /* fifo_read() */

#ifdef TEST
#include <stdio.h>

/* This is the typed space we will use for our FIFO operations.  */
TYPED_SPACE(global_space, 128, T_FIFO_SIC);	/* Static In-Core Fifo.  */

int
main()
{
	FIFO *ffp;		/* Fifo pointer for a handle.  */
	char line[1024];	/* Place to put input lines.  */
	long size;		/* Length for line.  (Sizes are all long.)  */
	int i;

	typed_space *local_space;

	/* Open the fifo for writing.  */
	if (F_NULL == (ffp = fifo_open(&global_space, 1))) {
		fprintf(stderr, "Can't open global_space for writing.\n");
		exit(1);
	}

	do {
		printf("Feed me: ");
		gets(line);
		size = (long) (strlen(line) + 1);
	} while (T_NULL != fifo_write_untyped(ffp, line, size, T_STR_STR));

	if (0 == fifo_close(ffp)) {
		fprintf(stderr, "Failed to close global_space.\n");
		exit(1);
	}

	printf("OK, global_space is now full.\n");
	/* ASSERTION: We've filled global_space with strings.  */

	/* Open the fifo for reading.  */
	if (F_NULL == (ffp = fifo_open(&global_space, 0))) {
		fprintf(stderr, "Can't open global_space for reading.\n");
		exit(1);
	}

	/* Dump the contents of this FIFO.  */
	for (i = 1; T_NULL != (local_space = fifo_read(ffp)); ++i) {
		printf("%d: size: %ld: type: 0x%x\n", i,
			local_space->ts_size,
			local_space->ts_type);

		/* Assume everything is a NUL terminated string.  */
		printf("datum: %s\n\n", local_space->ts_data);
		printf("Hit <RETURN>");
		gets(line);
	}


	/* Rewind the file, and dump it out again.  */
	printf("Rewinding.\n");
	if (0 == fifo_rewind(ffp)) {
		fprintf("Can't rewind global_space.\n");
		exit(1);
	}

	for (i = 1; T_NULL != (local_space = fifo_read(ffp)); ++i) {
		printf("%d: size: %ld: type: 0x%x\n", i,
			local_space->ts_size,
			local_space->ts_type);

		/* Assume everything is a NUL terminated string.  */
		printf("datum: %s\n\n", local_space->ts_data);
	}

	if (0 == fifo_close(ffp)) {
		fprintf(stderr, "Failed to close global_space.\n");
		exit(1);
	}

	exit(0);
} /* main() */

#endif /* TEST */
