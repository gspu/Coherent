#include <sys/seg.h>
#include <sys/proc.h>
#include "ps.h"

/*
 * Determine the size of a process in K.
 */
unsigned long
proc_size( segp, real_flag )
	SEG *segp[];
	unsigned real_flag;	/* Determine real process size?  */
{
	long len;
	register SEG *sp;
	register int n;

	len = 0;
	for (n=0; n<NUSEG+1; n++) {
		if ( !real_flag ) {
			if (n==SIUSERP || n==SIAUXIL) {
				continue;
			}
		}
		if ( (sp = segp[n]) == NULL ) {
			continue;
		}
		/*
		 * If one segment can't be mapped, give up.
		 */
		if ( NULL == ( sp = kumap(sp, sizeof(SEG) ) ) ) {
			return 0;
		}

		/*
		 * Add to the running total of segment sizes.
		 */
		len += sp->s_size;

		/*
		 * We're done with this segment.
		 */
		free(sp);
	}
	return( len );
} /* proc_size() */

/*
 * What state is the process in; asleep, running, stopped, or zombie?
 * Returns a single character indicating which.
 */
char
state( pp )
	PROC *pp;
{
	register unsigned s;

	s = pp->p_state;
	if (s == PSSLEEP) {
		if ((pp->p_flags&PFSTOP) != 0)
			return ('T');
		return ('S');
	}
	if (s == PSRUN)
		return ('R');
	if (s == PSDEAD)
		return ('Z');
	return ('?');
} /* state() */

/*
 * Read in the process table.  Allocate memory for the whole process
 * table, which will be returned.  Returns NULL if something goes
 * wrong.  Also fills in a value for the number processes (num_procs).
 */
PROC *
create_proc_table( num_procs, pargs )
	int *num_procs;
	PROC_ARGS pargs;
{
} /* create_proc_table() */

/*
 * Fill in data about each process.  Allocate memory for
 * a table of PROC_DATA structs and fill it in to return.  Returns
 * NULL if anything goes wrong.
 */
PROC_DATA *
create_proc_data( proc_table, num_procs, pargs )
	PROC proc_table[];
	int num_procs;
	PROC_ARGS pargs;
{
} /* create_proc_data() */
