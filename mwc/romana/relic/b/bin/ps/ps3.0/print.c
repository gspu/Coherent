#include <stdio.h>
#include <sys/seg.h>
#include "ps.h"

/*
 * Print a process table entry along with any supporting information
 * to file pointer fp.
 */
void
print_process( fp, pdata, pargs )
	FILE *fp;
	PROC_DATA *pdata;
	PROC_ARGS *pargs;
{
	PROC *pp;
	unsigned field_flags;
	unsigned char proc_state;
	int len;

	/*
	 * If we have NULL arguments, just give up.
	 */
	if ( ( NULL == pdata ) || ( NULL == pargs ) ) {
		return;
	}

	/*
	 * Fetch off the PROC struct--we are going to use it a lot.
	 * If there is no proc struct, we can't print much of anything,
	 * so just give up.
	 */
	if ( NULL == (pp = pdata->pr_proc) ) {
		return;
	}

	/*
	 * We are going to use the ar_field_flags a lot.
	 */
	field_flags = pargs->ar_field_flags;
	 
	if ( PID_ONLY & field_flags ) {
		fprintf( fp, "%d", pdata->pr_proc->p_pid );
		return;
	}

	/*
	 * We keep a running total of the length of the output line.
	 */
	len = 0;

	/*
	 * Except when an exclusive flag is used (like PID_ONLY),
	 * always print the controlling tty and the process id.
	 */
	print_tty( fp, pp->p_ttdev );		len += 7;

	fprintf( fp, " %5d", pp->p_pid );	len += 6;

	/*
	 * Do we want the process group?
	 */
	if ( GROUPS & field_flags ) {
		fprintf( fp, " %5d", pp->p_group );	len += 6;
	}

	/*
	 * Has a LONG_FMT listing been requested?
	 */
	if ( LONG_FMT & field_flags ) {
		/*
		 * Who is our parent?
		 */
		fprintf( fp, " %5d", pp->p_ppid );	len += 6;
		/*
		 * What user really owns this process?
		 */
		fprintf( fp, " %8.8", user_name( pp->p_ruid ) ); len += 9;
		/*
		 * How large is this process?
		 * NB: We bleed memory by calling kumap this way.
		 */
		fprintf( fp, " %4ldK",
			 proc_size( kumap( pp->p_segp, sizeof(SEG) ),
			 	    (REAL_SIZES & pargs->ar_msc_flags)
			 )
		);		len += 6;
		/*
		 * How are the process flags set?
		 */
		fprintf( fp, " %4x", pp->p_flags );	len += 5;
		/*
		 * Is the process asleep, running, or zombie?
		 */
		proc_state = state( pp );
		fprintf( fp, " %c", proc_state );	len += 2;

		/*
		 * If the process is sleeping, find out what it is
		 * sleeping on.  If the process is not sleep, or there
		 * is no u area, just print a filler.
		 */
		if ( ('S' == proc_state) &&
		     ( NULL != pdata->pr_u ) ) {
			print_event( fp, pdata->pr_u->u_sleep, pp->p_event );
		} else {
			fprintf( fp, "          -" );
		}
		len += 11;
	}

	/*
	 * Have scheduling values been requested?
	 */
	if ( SCHED & field_flags ) {
		/*
		 * Print the CPU schedule value.
		 */
		fprintf( fp, " %9u", pp->p_cval );	len += 10;
		/*
		 * Print the swap schedule value.
		 */
		fprintf( fp, " %9u", pp->p_sval );	len += 10;
		/*
		 * Print the importance value.
		 */
		fprintf( fp, " %10d", pp->p_ival );	len += 11;
		/*
		 * Print the response value.
		 */
		fprintf( fp, " %10d", pp->p_rval );	len += 11;
	}

	/*
	 * Have times been requested?
	 */
	if ( TIMES & field_flags ) {
		print_time( fp, pp->p_utime );		len += 6;
		print_time( fp, pp->p_stime );		len += 6;
	}

	/*
	 * If we get this far, we always print some form of the command.
	 * It is this line we've been calculating the length for.
	 */
	fprintf(" ");					len += 1;
	print_command( fp, pdata, pargs, len );
	
	fprintf( fp, "\n" );
	fflush( fp );

} /* print_process() */

/*
 * Print out an appropriate header for a given set of process flags.
 */
void
print_headers( fp, pargs )
	FILE *fp;
	PROC_ARGS *pargs;
{
	unsigned field_flags;

	/*
	 * We are going to use the ar_field_flags a lot.
	 */
	field_flags = pargs->ar_field_flags;
	 
	/*
	 * There are no headers if they are explicitly surpressed, or
	 * if an exclusive flag like PID_ONLY is used.
	 */
	if ( ( PID_ONLY & field_flags ) ||
	     ( NO_HEAD & pargs->ar_msc_flags ) ) {
		return;
	}

	/*
	 * Except when an exclusive flag is used (like PID_ONLY),
	 * always print the controlling tty and the process id.
	 */
	fprintf( fp, "TTY       PID" );

	/*
	 * Do we want the process group?
	 */
	if ( GROUPS & field_flags ) {
		fprintf( fp, " GROUP" );
	}

	/*
	 * Has a LONG_FMT listing been requested?
	 */
	if ( LONG_FMT & field_flags ) {
		fprintf( fp, "  PPID      UID    K    F S      EVENT" );
	}

	/*
	 * Have scheduling values been requested?
	 */
	if ( SCHED & field_flags ) {
		fprintf( fp, "      CVAL      SVAL       IVAL       RVAL" );
	}

	/*
	 * Have times been requested?
	 */
	if ( TIMES & field_flags ) {
		fprintf( fp, " UTIME STIME" );
	}

} /* print_headers() */


/*
 * Print the name of a tty.
 */
void
print_tty( fp, ttdev )
	FILE *fp;
	dev_t ttdev;
{
} /* print_tty() */

/*
 * Print out the reason for a sleep.
 */
void
print_event( fp, sleep_str, event )
	FILE *fp;
	char *sleep_str;
	char *event;
{
	/*
	 * Only print the sleep field if it is non-empty.
	 */

	if ( '\0' != sleep_str[0] ) {
		fprintf( fp, " %10.10s", sleep_str );
	} else {
		/* Otherwise, print the address we are sleeping on.  */
		fprintf( fp, " 0x%08X", event);
	}

	fflush( fp );
} /* print_event() */

/*
 * Print out a time in HZ.
 */
void
print_time( atime )
	long atime;
{
	register unsigned min;

	if ((atime=(atime+HZ/2)/HZ) == 0) {
		printf("     -");
		return;
	}
	if ((min=atime/60) >= 100) {
		printf("%6d", min);
		return;
	}
	printf(" %2d:%02d", min, (unsigned)atime%60);
} /* print_time() */

/*
 * Print the command for this process.
 */
void
print_command( fp, pdata, pargs, line_len )
	FILE *fp;
	PROC_DATA *pdata;
	PROC_ARGS *pargs;
	int line_len;
{
	printf("STUB");	/* STUB */
} /* print_command() */
