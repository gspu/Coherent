/* fifo_rewind.c -- Rewind an internally typed fifo.  */
#include <sys/typed.h>

/* Go back to the start of the fifo.
 * Takes a FIFO.  For reading, go back to the first space; for writing
 * truncate the FIFO to empty.
 * Returns 1 on success, 0 otherwise.
 */
int
fifo_rewind(ffp)
	FIFO *ffp;
{
	/* Error checking of arguments.  */
	if (0 == ffp->f_flags) {
		return(0);	/* This ffp is not open.  */
	}

	/* Either reading or writing MUST be set.  */
	if ((F_READ != F_READ & ffp->f_flags) &&
	    (F_WRITE != F_WRITE & ffp->f_flags) ){
		return(0);	/* Illegal flags.  */
	}

	/* Rewind to the first space in this fifo (skip the header).  */
	ffp->f_offset = ((typed_space *) (ffp->f_space)) + 1;

	return(1);
} /* fifo_rewind() */
