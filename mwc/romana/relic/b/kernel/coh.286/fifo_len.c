/* fifo_len.c -- Measure an internally typed fifo in bytes.  */
#include <sys/typed.h>


/* How long is an open fifo?  */
long
fifo_len(ffp)
	FIFO *ffp;
{
	/* ffp->f_offset points at the terminating NUL space.  */
	return(sizeof(typed_space) +
		((char *) ffp->f_offset) - ((char *)ffp->f_space));
} /* fifo_len() */
