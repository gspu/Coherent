/*
 * Define useful symbols for use in the ps program.
 */
#ifndef PS_H	/* Rest of file.  */
#define PS_H

#include <sys/proc.h>
#include <sys/uproc.h>

/*
 * These flags, ar_proc_flags, indicate which processes should be printed.
 */
#define	ALL_PROC	0x0001		/* All processes */
#define	DRIVER		0x0002		/* Print drivers */
#define	SPECIAL		0x0004		/* Get special processes */
#define	USER		0x0008		/* All processes for a given user  */
#define ATTY		0x0010		/* All processes for a given tty */

/*
 * These flags indicate which fields should be printed.  There are
 * two kinds of these--flags that indicate a format and flags that
 * indicate specific fields.  They are the ar_field_flags.
 */
#define	ALL_FIELDS	0x0001		/* Print out all fields */
#define	LONG_FMT	0x0002		/* Long format */
#define	WIDE_FMT	0x0004		/* Wide format */
#define PID_ONLY	0x0008		/* Print only PIDs */

#define	GROUPS		0x0100		/* Print out process groups */
#define	SCHED		0x0200		/* Print scheduling values */
#define	TIMES		0x0400		/* Print times */

/*
 * These are miscellaneous flags, ar_msc_flags.
 */
#define	REAL_SIZES	0x0001		/* Print out real sizes */
#define	NO_HEAD		0x0002		/* No header */
#define	DEBUG		0x0004		/* Debug flag */
#define	IGN_PRESENT	0x0008		/* UNDOCUMENTED: ignore present bit.  */


/*
 * Here we define some assorted handy values.
 */
typedef int boolean;
#define TRUE	(1==1)
#define FALSE	(1==2)
#ifndef NULL
#define NULL	((char *) 0)
#endif /* NULL */

/*
 * This structure contains pointers to all structures which might
 * describe part of a process.
 *
 * We keep these critters in an array which we build on demand.
 */
typedef struct {
	PROC	*pr_proc;	/* Insert the process table entry here.	*/
	UPROC	*pr_u;		/* Insert the u area here.		*/
	char	*pr_argv[];	/* Insert the argument vector here.	*/
	char	pr_argc;	/* Insert the number of args here.	*/
} PROC_DATA;

/*
 * This structure contains a predigested version of the arguments to ps.
 */
typedef struct {
	unsigned ar_proc_flags;	  /* Which processes should be printed?	*/
	unsigned ar_field_flags;  /* Which fields should be printed?	*/
	unsigned ar_msc_flags;	  /* This is other useful info.		*/

} PROC_ARGS;

void main();
void cvt_args();		/* Digest the command line.  */
PROC *create_proc_table();	/* Read in the process table.  */
PROC_DATA *create_proc_data();	/* Fill in data about each process.  */
void print_headers();		/* Print column headings.  */
boolean wanted_process();	/* Do we want to print this process?  */
void print_process();		/* Print information about this process.  */
void print_tty();		/* Print a text name for a tty.  */
char *user_name();		/* Convert uid to a user name.  */
char *kumap();			/* Map some kernel memory into user space.  */
char state();			/* Is the process asleep, running, or zombie? */
void print_time();		/* Print a HZ time.  */
void print_command();		/* Print the command for this process.  */
void print_event();		/* Print out the reason for a sleep.  */
#endif /* PS_H */
