/* fifo_close.c -- Close an internally typed fifo.  */
#include <sys/typed.h>

/* Finish with using a typed space as a fifo.
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

