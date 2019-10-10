#include "ps.h"

/*
 * Print out process status.
 */
void
main(argc, argv)
	int argc;
	char *argv[];
{
	int i;		/* Every program needs a loop counter :-).  */
	int num_procs;	/* How many processes are there? */
	/*
	 * The command arguments in argv are digested into 'args'.
	 */
	PROC_ARGS args;

	/*
	 * proc_table is the process table as read from /dev/proc.
	 */
	PROC *proc_table;

	/*
	 * proc_data is the table of all information for each process.
	 * It includes pointers into proc_table, and lots of other
	 * goodies.
	 */
	PROC_DATA *proc_data;

	/*
	 * Digest the command line.
	 *
	 * This routine will not return if there are errors in
	 * the command line.
	 */
	cvt_args( &args, argc, argv );

	/*
	 * Read in all that handy cached information.
	 */
	/*
	 * There is no caching yet.
	 */

	/*
	 * Read in the process table.  Note that memory for
	 * proc_table will be allocated.
	 *
	 * The number of processes (i.e. the size of proc_table)
	 * will be returned in num_procs.
	 */
	proc_table = create_proc_table( &num_procs, &args );

	/*
	 * Pull together all the data for each process into
	 * a single table of PROC_DATA structures.
	 */
	proc_data = create_proc_data( proc_table, num_procs, &args );
	
	/*
	 * Print any headers we might need.
	 */
	print_headers( &args );

	/*
	 * Walk through proc_data[] printing each appropriate entry.
	 */
	for ( i = 0; i < num_procs; ++i ) {
		if ( wanted_process( &proc_data[i], &args ) ) {
			print_process( &proc_data[i], &args );
		}
	}

} /* main() */

/*
 * Digest the command line.
 */
void
cvt_args( args, argc, argv )
	PROC_ARGS *args;
	int argc;
	char *argv[];
{
} /* cvt_args() */

/*
 * Is this process one we want to print?
 */
boolean
wanted_process( process, args )
	PROC_DATA *process;
	PROC_ARGS *args;
{
	return( TRUE );	/* STUB */
} /* wanted_process() */
