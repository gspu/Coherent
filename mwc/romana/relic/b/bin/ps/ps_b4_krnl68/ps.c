static char _version[]="ps version 2.10";
/*
 *	Modifications copyright INETCO Systems Ltd.
 *
 * Print out process statuses.
 *
 * $Log:	ps.c,v $
 * Revision 1.7  92/09/18  09:42:29  bin
 * version 210 fom piggy
 * 
 * Revision 1.3  91/07/17  14:22:55  bin
 * stevesf supplied as INETCO source because previous sources are of
 * questionable origin... this one works
 * 
 * Revision 1.2	89/06/12  15:15:22 	src
 * Bug:	A directory at the end of the '/dev' directory would cause 'ps'
 * 	to crash with the message "Cannot open <dir> in /dev".
 * Fix:	A stat was being done without the return code being checked. (ART)
 * 
 * Revision 1.1	89/06/12  14:42:24 	src
 * Initial revision
 * 
 * 88/02/15	Allan Cornish	/usr/src/cmd/cmd/ps.c
 * Kernel processes are now displayed regardless of -ax flags.
 *
 * 87/11/25	Allan Cornish	/usr/src/cmd/cmd/ps.c
 * Debug flag now -D.  Drivers now displayed by -d flag.
 *
 * 87/11/12	Allan Cornish	/usr/src/cmd/cmd/ps.c
 * Modified to support Coherent 9.0 [protected mode] segmentation.
 *
 * 87/10/20	Allan Cornish	/usr/src/cmd/cmd/ps.c
 * Now extracts cmd name from u_comm field in uproc struct for kernel procs.
 * Kernel processes only displayed if -d [debug] flag given.
 *
 * 87/09/28	Phil Selby	/usr/src/cmd/cmd/ps.c
 * Altered so that the terminal name rather than major and minor
 * device number is printed out
 *
 * 84/08/21	Rec
 * Added gflag for group identification.
 *
 * 84/07/16	Lauren Weinstein
 * Initial version.
 */

#include <sys/coherent.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <dirent.h>
#include <sys/seg.h>
#include <sys/stat.h>
#include <sys/uproc.h>
#include <sys/mmu.h>
#include <signal.h>
#include <access.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <coff.h>
#include <pwd.h>

/*
 * This is a kludge for the i8086 only and will be
 * made to disappear when the segmentation (jproto)
 * is thrown away.
#undef	SISTACK
#define	SISTACK	SIPDATA
 */

/*
 *	Terminal information stored in memory for use by ps in TTY field
 */

struct handle {
	dev_t	dnum;		/* device number */
	char dname[DIRSIZ];	/* device name */
	struct handle *nptr;	/* pointer to next */
} *fptr, *lptr;

/*
 * Maximum sizes.
 */
#define ARGSIZE	512

/*
 * Functions to see if a pointer is in alloc space and to map a
 * pointer from alloca space to this process.
 */
#define map(p)		(&(allp[(char *)(p) - (char *) callocp.sr_base]))

/*
 * For easy referencing.
 */
#define NUM_SYMS	7	/* Number of symbols to look up.  */
#define	aprocq		nl[0].n_value
#define	autime		nl[1].n_value
#define astime		nl[2].n_value
#define aallocp		nl[3].n_value
#define	aend		nl[4].n_value
#define	asysmem		nl[5].n_value
#define	au		nl[6].n_value

#define TRUE	(1==1)
#define FALSE	(1==2)
#define LESSER(a, b)	((a < b)?a:b)

/*
 * Defines for referencing page tables.
 */
#define BPCSHIFT	12	/* Shift this many to convert bytes to clicks.  */
#define ONE_CLICK	4096	/* Bytes in a click.  */
#define CLICK_OFFSET	0x00000fffL	/* Bits in virtual address indexing into page.  */
#define PT_CLICK_ADDR	0xfffff000L	/* Bits in pte pointing at the page.  */
#define PT_PRESENT(entry)  (0x01 == (entry & 0x01))	/* Is the page for
							 * this pte present? 
							 */
/*
 * This is the address of the start of the U area segment.  This
 * should probably be extracted somehow from an include file.
 * It can be derived with much pain from the s_vmem field in the seg
 * struct from the U area segment.
 */
#define USEG_BASE	0xfffff000

/*
 * Variables.
 */
int	aflag;				/* All processes */
int	dflag;				/* Driver flag */
int	dbflag;				/* Debug flag */
int	fflag;				/* Print out all fields */
int	gflag;				/* Print out process groups */
int	lflag;				/* Long format */
int	mflag;				/* Print scheduling values */
int	nflag;				/* No header */
int	rflag;				/* Print out real sizes */
int	tflag;				/* Print times */
int	wflag;				/* Wide format */
int	xflag;				/* Get special processes */
int	Pflag;				/* UNDOCUMENTED: ignore present bit.  */
dev_t	ttdev;				/* Terminal device */

/*
 * Table for namelist.
 */
SYMENT nl[NUM_SYMS];

/*
 * Symbols.
 */
char	 *allp;				/* Pointer to alloc space */
char	 *kfile;			/* Kernel data memory file */
char	 *nfile;			/* Namelist file */
char	 *mfile;			/* Memory file */
char	 *dfile;			/* Swap file */
char	 argp[ARGSIZE];			/* Arguments */
int	 kfd;				/* Kernel memory file descriptor */
int	 mfd;				/* Memory file descriptor */
int	 dfd;				/* Swap file descriptor */
struct	 uproc u;	 		/* User process area */
unsigned cutime;			/* Unsigned time */
PROC	 cprocq;			/* Process queue header */
SR	 callocp;			/* Size of alloc area */
SYSMEM	 sysmem;			/* Useful system-memory info.
					 * This variable MUST be called
					 * "sysmem" for the MAPIO() macro.
					 */
char *malloc();
char *uname();
unsigned cval();
cseg_t	pt_index();
char *pick_nfile();

main(argc, argv)
	int argc;
	char *argv[];
{
	register int i;
	register char *cp;

#if 0
fprintf(stderr, "initialise()\n");	/* DEBUG */
#endif /* 0 */
	initialise();
#if 0
fprintf(stderr, "parse args()\n");	/* DEBUG */
#endif /* 0 */
	for (i=1; i<argc; i++) {
		for (cp=&argv[i][0]; *cp; cp++) {
			switch (*cp) {
			case '-':
				continue;
			case 'a':
				aflag++;
				continue;
			case 'c':
				if (++i >= argc)
					usage();
				nfile = argv[i];
				continue;
			case 'd':
				dflag++;
				continue;
			case 'D':
				dbflag++;
				continue;
			case 'f':
				fflag++;
				continue;
			case 'g':
				gflag++;
				continue;
			case 'k':
				if (++i >= argc)
					usage();
				mfile = argv[i];
				continue;
			case 'l':
				lflag++;
				continue;
			case 'm':
				mflag++;
				continue;
			case 'n':
				nflag++;
				continue;
			case 'P':
				Pflag++;
				continue;
			case 'r':
				rflag++;
				continue;
			case 't':
				tflag++;
				continue;
			case 'w':
				wflag++;
				continue;
			case 'x':
				xflag++;
				continue;
			default:
				usage();
			}
		}
	}

#if 0
fprintf(stderr, "execute()\n");	/* DEBUG */
#endif /* 0 */
	execute();

#if 0
fprintf(stderr, "exit(0)\n");	/* DEBUG */
#endif /* 0 */
	exit(0);
}

/*
 * Initialise.
 */
initialise()
{
	register char *cp;

	aflag = 0;
	gflag = 0;
	lflag = 0;
	mflag = 0;
	nflag = 0;
	Pflag = 0;
	xflag = 0;
	nfile = pick_nfile();
	kfile = "/dev/kmem";
	mfile = "/dev/mem";
	dfile = "/dev/swap";
	if ((cp=malloc(BUFSIZ)) != NULL)
		setbuf(stdout, cp);

	/* Initialise the request for coffnlist().  */
	strcpy(nl[0]._n._n_name, "procq");
	nl[0].n_type = -1;
	strcpy(nl[1]._n._n_name, "utimer");
	nl[1].n_type = -1;
	strcpy(nl[2]._n._n_name, "stimer");
	nl[2].n_type = -1;
	strcpy(nl[3]._n._n_name, "allocp");
	nl[3].n_type = -1;
	strcpy(nl[4]._n._n_name, "end");
	nl[4].n_type = -1;
	strcpy(nl[5]._n._n_name, "sysmem");
	nl[5].n_type = -1;
	strcpy(nl[6]._n._n_name, "u");
	nl[6].n_type = -1;

}

/*
 * Print out usage.
 */
usage()
{
	panic("Usage: ps [-][acdfgklmnrtwx]");
}


/*
 * Print out information about processes.
 */
execute()
{
	int fd;
	int c, l;
	int loop_count;		/* Keep track of the number of entries read.  */
	register PROC *pp1, *pp2;

#if 0
	printf("execute()\n");
	fflush(stdout);
#endif /* 0 */

	/*
	 * Check to see if the desired kernel exists and is accessable.
	 * NB: the access() system call uses the REAL uid to check
	 * permissions--it will not work here.
	 */
	if (-1 == (fd = open(nfile, O_RDONLY))) {
		panic("%s is not readable or does not exist.", nfile);
	}
	close(fd);

#if 0
fprintf(stderr, "Extract symbol information from the kernel %s.\n", nfile);	/* DEBUG */
#endif /* 0 */
	/*
	 * Extract symbol information from the kernel.
	 */
	if (0 == coffnlist(nfile, nl, "", NUM_SYMS) ) {
		panic("Can not use kernel image %s.", nfile);
	}

	if (nl[0].n_type == -1) {
		panic("Bad namelist file %s", nfile);
	}

#if 0
fprintf(stderr, "Open the physical memory device.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
	/*
	 * Open the physical memory device.
	 */
	if ((mfd=open(mfile, 0)) < 0) {
		panic("Cannot open %s", mfile);
	}

#if 0
fprintf(stderr, "Open the virtual memory device.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
	/*
	 * Open the virtual memory device.
	 */
	if ((kfd = open(kfile, 0)) < 0) {
		panic("Cannot open %s", kfile);
	}

	/*
	 * Open swap device if it exists
	 */
	dfd = open(dfile, 0);

#if 0
fprintf(stderr, "Fetch the head of the process queue.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
	/*
	 * Fetch the head of the process queue.
	 */
	kread((long)aprocq, &cprocq, sizeof (cprocq));

	/*
	 * Fetch information about system memory.
	 */
	kread((long)asysmem, &sysmem, sizeof (sysmem));

	

#if 0
fprintf(stderr, "Take a snapshot of kernel memory.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
	/*
	 * Take a snapshot of kernel memory.
	 */
	kread((unsigned long)aallocp, &callocp, sizeof (callocp));

#if 0
	fprintf(stderr, "callocp.sr_size: %x\n", callocp.sr_size);
	fflush(stderr);
#endif /* 0 */

	if ((allp=malloc(callocp.sr_size)) == NULL) {
		panic( "Out of core or invalid kernel specified" );
	}

#if 0
fprintf(stderr, "kread(%x, %x, %x)\n",	/* DEBUG */
		(unsigned long)callocp.sr_base, allp, callocp.sr_size);
fflush(stderr);
#endif /* 0 */

	kread((unsigned long)callocp.sr_base, allp, callocp.sr_size);


#if 0
fprintf(stderr, "Fetch the current tick time.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
	/*
	 * Fetch the current tick time.
	 */
	kread((long)autime, &cutime, sizeof (cutime));

#if 0
fprintf(stderr, "Find out what our controlling terminal is.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
	/*
	 * Find out what our controlling terminal is.
	 */
	settdev();

	fttys( "/dev");	/* load all the devices in dev */

#if 0
fprintf(stderr, "Calculate the length of the output line.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
	/*
	 * Calculate the length of the output line.
	 */
	l = strlen("TTY       PID");
	if (dbflag)
		l += strlen("0xffffffff ");
	if (gflag)
		l += strlen(" GROUP");
	if (lflag)
		l += strlen("  PPID      UID    K    F S     EVENT");
	if (mflag)
		l += strlen("      CVAL      SVAL       IVAL       RVAL");
	if (tflag)
		l += strlen(" UTIME STIME");
	if (nflag == 0) {
		if (dbflag)
			printf("           ");
		printf("TTY       PID");
		if (gflag)
			printf(" GROUP");
		if (lflag)
			printf("  PPID      UID    K    F S      EVENT");
		if (mflag)
			printf("      CVAL      SVAL       IVAL       RVAL");
		if (tflag)
			printf(" UTIME STIME");
		putchar('\n');
		fflush(stdout);
	}

#if 0
fprintf(stderr, "Walk through the process queue printing out each entry.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
	/*
	 * Walk through the process queue printing out each entry.
	 */
	loop_count = 0;	/* How many PROC entries have we seen?  */
	pp1 = &cprocq;
	while ((pp2=pp1->p_nback) != (PROC *) aprocq) {
		loop_count++;

#if 0
fprintf(stderr, "count: %d\n", loop_count);	/* DEBUG */
fflush(stderr);
#endif /* 0 */

		if (range((char *)pp2) == 0)
			panic("Fragmented list");
		pp1 = map(pp2);

		/*
		 * Kernel process - display only if '-d' argument given.
		 */
		if ( pp1->p_flags & PFKERN ) {
#if 0
fprintf(stderr, "PFKERN\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
			if ( dflag == 0 ) {
				continue;
			}
		}

		/*
		 * Unattached process - display only if '-x' argument given.
		 */
		else if ( pp1->p_ttdev == NODEV ) {
#if 0
fprintf(stderr, "NODEV\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
			if ( xflag == 0 ) {
				continue;
			}
		}

		/*
		 * Attached to other terminal - display only if '-a' arg given.
		 */
		else if ( pp1->p_ttdev != ttdev ) {
#if 0
fprintf(stderr, "Other tty\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
			if ( aflag == 0 )
				continue;
		}

#if 0
fprintf(stderr, "ASSERTION: pp1 is a proc entry we want to print.\n");	/* DEBUG */
fflush(stderr);
#endif /* 0 */
		/*
		 * ASSERTION: pp1 is a proc entry we want to print.
		 */

		if (dbflag)
			printf("0x%8x ", pp2);
		ptty(pp1);
		printf(" %5d", pp1->p_pid);
		if (gflag)
			printf(" %5d", pp1->p_group);
		if (lflag) {
			printf(" %5d", pp1->p_ppid);
			fflush(stdout);
			printf(" %8.8s", uname(pp1));
			fflush(stdout);
			psize(pp1);
			fflush(stdout);
			printf(" %4o", pp1->p_flags);
			fflush(stdout);
			printf(" %c", c=state(pp1, pp2));
			fflush(stdout);
			if (c == 'S') {
				print_event(pp1);
			} else {
				if (fflag)
					printf("          -");
				else
					printf("           ");
			}
			fflush(stdout);
		}
		if (mflag) {
			printf(" %9u", cval(pp1, pp2));
			printf(" %9u", pp1->p_sval);
			printf(" %10d", pp1->p_ival);
			printf(" %10d", pp1->p_rval);
		}
		if (tflag) {
			ptime(pp1->p_utime);
			ptime(pp1->p_stime);
		}
		printf("  ");
		printl(pp1, (wflag?132:80)-l-1);

		putchar('\n');
		fflush(stdout);
	}
	rttys();	/* release all alloced space */
}

/*
 * Set our terminal device.
 */
settdev()
{
	register PROC *pp1, *pp2;
	register int p;
	int loop_count;

	p = getpid();
	pp1 = &cprocq;

	loop_count = 0;	/* How many PROC entries have we seen?  */
	while ((pp2=pp1->p_nforw) != (PROC *) aprocq) {
		loop_count++;
#if 0
fprintf(stderr, "settdev() count: %d\n", loop_count);	/* DEBUG */
fflush(stderr);
#endif /* 0 */
		
#if 0
fprintf(stderr, "range():sr_base: %x < %x < sr_base + sr_size: %x\n",
		callocp.sr_base, pp2, callocp.sr_base + callocp.sr_size);	/* DEBUG */
fflush(stderr);
#endif /* 0 */
		if (range((char *)pp2) == 0)
			break;

#if 0
fprintf(stderr, "About to map %x\n", pp2);	/* DEBUG */
fflush(stderr);

fprintf(stderr, "offset = (%x - sr_base) = %x \n",
		pp2, ((char *)pp2) - callocp.sr_base );	/* DEBUG */
fflush(stderr);
fprintf(stderr, "(allp:%x + offset): \n",
		allp, allp + ( ((char *) pp2) - ( (char *) callocp.sr_base) ) );	/* DEBUG */
fflush(stderr);
#endif /* 0 */

		pp1 = map(pp2);

#if 0
fprintf(stderr, "mapped %x to %x\n", pp2, pp1);	/* DEBUG */
fflush(stderr);
#endif /* 0 */
		if (pp1->p_pid == p) {
			ttdev = pp1->p_ttdev;
			return;
		}
	}
	ttdev = NODEV;
}

/*
 *	Finds all special files in the specified directory (e.g. /dev)
 */

fttys( dirname)

char *dirname;

{
	int filein;		/* directory file descriptor */
	struct stat sbuf;	/* stat structure */
	struct direct dbuf;	/* directory buffer structure */
	
	/* move to the appropriate directory and open file for reading */
	
	chdir( dirname );

	if( ( filein = open( ".", 0) ) < 0 ) {
		fprintf( stderr, "Cannot open '%s' in /dev\n", dirname );
		exit( 1 );
	}

	read( filein, &dbuf, sizeof(dbuf) );	/* read . */
	read( filein, &dbuf, sizeof(dbuf) );	/* read .. */

	while( read( filein, &dbuf, sizeof(dbuf) ) ) {
		if( stat( dbuf.d_name, &sbuf ) < 0 )
			continue;

		if( sbuf.st_mode & S_IFCHR )
			addname( dbuf.d_name, sbuf.st_rdev );

		else if( sbuf.st_mode & S_IFDIR )
			fttys( dbuf.d_name );
	}

	close( filein );
	chdir( ".." );
}

/*
 *	Adds a name and device to the master list of ttys.
 */

addname( devname, devnum )
	char *devname;
	dev_t devnum;
{
	extern struct handle *fptr;
	extern struct handle *lptr;
	
	if( fptr == (struct handle *) NULL ) {
		if( (fptr=(struct handle *) malloc( sizeof(struct handle))) ==
				(struct handle *) NULL ) {
			fprintf( stderr, "Out of memory\n");
			exit( 1 );
		}

		fptr->nptr = (struct handle *) NULL;
		strcpy( fptr->dname, devname);
		fptr->dnum = devnum;
	}

	else {
		lptr = fptr;

		while( ( lptr->nptr != (struct handle *) NULL ) &&
			( lptr->dnum != devnum ) ) lptr = lptr->nptr;

		if( lptr->dnum == devnum )
			return;

		if( (lptr->nptr=(struct handle *)malloc(sizeof(struct handle)))
				== (struct handle *) NULL ) {
			fprintf( stderr, "Out of memory\n");
			exit( 1 );
		}

		lptr = lptr->nptr;
		lptr->nptr = (struct handle *) NULL;
		strcpy( lptr->dname, devname);
		lptr->dnum = devnum;
	}
}

/*
 *	Release allocated space on exit (default action anyway)
 */

rttys()
{
	/* release allocated space */
	
	lptr = fptr;
	while( lptr->nptr != (struct handle *) NULL ) {
		fptr = lptr;
		lptr = fptr->nptr;
		free( fptr);
	}
	free( lptr);
}

/*
 *	Print the controlling terminal name.  If not found it prints the
 *	major and minor device numbers.  If no controlling terminal on the
 *	process then '--------' is printed.
 */

ptty( pp )
	register PROC *pp;
{
	register dev_t d;

	/* when supplied with a device number, look for the name
	   of the special file from the dev directory */
	   
	if( ((dev_t)( d = pp->p_ttdev )) == NODEV ) {
		printf( "-------");
		return;
	}

	lptr = fptr;
	while( lptr->nptr != (struct handle *) NULL ) {
		if( lptr->dnum == d ) {
			printf( "%-7.7s", lptr->dname);
			return;
		}
		lptr = lptr->nptr;
	}
	if( lptr->dnum == d ) {
		printf( "%-7.7s", lptr->dname);
		return;
	}
	printf( "%3d %3d", major( d ), minor( d ) );
	return;
}

/*
 * Given a process, return it's user name.
 */
char *
uname(pp)
	register PROC *pp;
{
	static char name[8];
	register struct passwd *pwp;

	if ((pwp=getpwuid(pp->p_ruid)) != NULL)
		return (pwp->pw_name);
	sprintf(name, "%d", pp->p_ruid);
	return (name);
}

/*
 * Return the core value for a process.
 */
unsigned
cval(pp1, pp2)
	register PROC *pp1;
	PROC *pp2;
{
	unsigned u;
	register PROC *pp3, *pp4;

	if (pp1->p_state == PSSLEEP) {
		u = (cutime-pp1->p_lctim) * 1;
		if (pp1->p_cval+u > pp1->p_cval)
			return (pp1->p_cval+u);
		return (-1);
	}
	u = 0;
	pp3 = &cprocq;
	while ((pp4=pp3->p_lforw) != (PROC *) aprocq) {
		if (range((char *)pp4) == 0)
			break;
		pp3 = map(pp4);
		u -= pp3->p_cval;
		if (pp2 == pp4)
			return (u);
	}
	if (pp1->p_pid == getpid())
		return (pp1->p_cval);
	return (0);
} /* cval() */

/*
 * Return the size in K of the given process.
 */
psize(pp)
	register PROC *pp;
{
	long len;
	register SEG *sp;
	register int n;

	len = 0;
	for (n=0; n<NUSEG+1; n++) {
		if (rflag == 0)
			if (n==SIUSERP || n==SIAUXIL)
				continue;
		if ((sp=pp->p_segp[n]) == NULL)
			continue;
		if (range((char *)sp) == 0) {
 			printf("   ?K");
			return;
		}
		sp = map(sp);
		len += sp->s_size;
	}
	if (len != 0)
		printf("%4ldK", len/1024);
	else {
		if (fflag)
			printf("    -");
		else
			printf("     ");
	}
}

/*
 * Get the state of the process.
 */
state(pp1, pp2)
	register PROC *pp1;
	char *pp2;
{
	register unsigned s;

	s = pp1->p_state;
	if (s == PSSLEEP) {
		if (pp1->p_event == pp2)
			return ('W');
		if ((pp1->p_flags&PFSTOP) != 0)
			return ('T');
		return ('S');
	}
	if (s == PSRUN)
		return ('R');
	if (s == PSDEAD)
		return ('Z');
	return ('?');
}

/*
 * Given a time in HZ, print it out.
 */
ptime(l)
	long l;
{
	register unsigned m;

	if ((l=(l+HZ/2)/HZ) == 0) {
		if (fflag)
			printf("     -");
		else
			printf("      ");
		return;
	}
	if ((m=l/60) >= 100) {
		printf("%6d", m);
		return;
	}
	printf(" %2d:%02d", m, (unsigned)l%60);
}

/*
 * Print out the reason for a sleep.
 */
print_event(pp)
	register PROC *pp;
{
	/* Only print the u.u_sleep field if it is non-empty.  */

	if (	(u_init(pp->p_segp[SIUSERP], &u) != 0) &&
		('\0' != u.u_sleep[0]) ) {
			printf(" %10.10s", u.u_sleep );
	} else {
		/* Otherwise, print the address we are sleeping on.  */
		printf(" 0x%08X", pp->p_event);
	}

	fflush(stdout);
} /* print_event() */

/*
 * Print out the command line of a process.
 */
printl(pp, m)
	register PROC *pp;
{
	register char *cp;
	register int c;
	register int argc;
	register int n;
	static SR *srp;

	if (pp->p_state == PSDEAD) {
		printf("<zombie>");
		return;
	}
	if (pp->p_pid == 0) {
		printf("<idle>");
		return;
	}

	if (u_init(pp->p_segp[SIUSERP], &u) == 0) {
		printf("<ghost>");
		return;
	}

	printf(" %.10s", u.u_comm);
	return;

	/*
	 * Handle kernel processes.
	 */
	if ( pp->p_flags & PFKERN ) {
		printf("<%.*s>",sizeof(u.u_comm), u.u_comm[0] ? u.u_comm : "");
		return;
	}

	if ((argc=u.u_argc) <= 0)
		return;

	srp = &u.u_segl[SISTACK];
	printf("segread 2 in printl\n");
	printf("u.u_argp: %x, srp->sr_base: %x\n",
		u.u_argp, srp->sr_base);
	printf("u.u_argc: %x srp->sr_size: %x\n", u.u_argc, srp->sr_size);

	n = segread(&u.u_segl[SISTACK], u.u_argp, argp, 64);

	if (n == 0) {
		fprintf(stderr, "Bad segread()\n");	/* DEBUG */
		return;
	}

	m -= 2;
	cp = argp;

	while (argc--) {
		while ((c=*cp++) != '\0') {
			if (!isascii(c) || !isprint(c)) {
#if 0 /* Blocked out for test purposes.  */
				return;
#else
				putchar('.');
				continue;
#endif /* 0 */
			}
			if (m-- == 0)
				return;
			putchar(c);
		}
		fflush(stdout);
#if 0 /* Blocked out for test purposes.  */
		if (m-- == 0)
			return;
#endif /* 0 */
		if (argc != 0)
			putchar(' ');
	}
}


/*
 * Given a segment pointer `sp', read a u area into buffer `bp'.
 */
u_init(sp, bp)
	SEG *sp;
	char *bp;
{
#ifdef UPROC_VERSION
	/* Have we verrified the uproc version number?  */
	static version_ok = FALSE;
#endif /* UPROC_VERSION */

	register SEG *sp1;
	long offset;

#if 0
	printf("u_init(sp:%x, bp:%x)\n", sp, bp);
	fflush(stdout);
#endif /* 0 */

	if (range((char *)sp) == 0) {
		return (0);
	}

	sp1 = map(sp);

	/*
	 * Figure out how far into the U segment the U area is.
	 * We do this by subtracting the starting address of the
	 * U area segment from the address of u.
	 */
	offset = au - USEG_BASE;

	/*
	 * If the process is not swapped out, read directly from
	 * main memory.  Otherwise, read from the swap device.
	 */
	if ((sp1->s_flags&SFCORE) != 0) {
		if (0 == pt_mread( sp1->s_vmem, offset, bp, sizeof(UPROC) )) {
			return (0);
		}
	} else if (
	    dread((long)(sp1->s_daddr*BSIZE)+offset, bp, sizeof(UPROC)) < 0
	){
			return (0);
	}

#ifdef UPROC_VERSION
	/*
	 * Check the version number on this U area.
	 * I.e. does this ps match this kernel?
	 */
	if ( ((UPROC *) bp)->u_version != UPROC_VERSION ) {
		/*
		 * Only print the warning if we have not yet seen
		 * a valid version number, and then only once.
		 *
		 * If we have seen at least one valid version number,
		 * it probably means that this process was dying.
		 */
		if (!version_ok) {
			static int printed_once = FALSE;
			if (!printed_once) {
				fprintf( stderr,
				"\nps WARNING: u area version is %x, not %x.\n",
				((UPROC *) bp)->u_version, UPROC_VERSION );
				printed_once = TRUE;
			}
		}
		return (0);
	}
	version_ok = TRUE;	/* We've now seen one valid version number.  */
#endif /* UPROC_VERSION */
		
	return (1);
}


/*
 * Given an open segment pointer `sr' and an offset into the segment,
 * `s', read `n' bytes from the segment into the buffer `bp'.
 */
segread(sr, s, bp, n)
	SR *sr;
	vaddr_t s;
	char *bp;
	int n;
{
	register SEG *sp1;
	vaddr_t offset;

#if 0
	printf("segread(sr:%x, s:%x, bp:%x, n:%x)\n", sr, s, bp, n);
	fflush(stdout);
#endif /* 0 */

	if (range((char *)sr->sr_segp) == 0) {
		return (0);
	}

	sp1 = map(sr->sr_segp);

	/* If segment grows up... */
	if (0 == (SFDOWN & (sp1->s_flags)) ) {
		/* then sr_base is the bottom of the segment, */
		offset = s - sr->sr_base;
	} else {
		/* otherwise sr_base is the top of the segment.  */
		offset = s - (sr->sr_base - sr->sr_size);
	}

	/*
	 * If the process is not swapped out, read directly from
	 * main memory.  Otherwise, read from the swap device.
	 */
	if ((sp1->s_flags&SFCORE) != 0) {
		if (0 == pt_mread( sp1->s_vmem, offset, bp, n )) {
			return (0);
		}
	} else if (dread( (long)sp1->s_daddr*BSIZE + s, bp, n ) < 0 ) {
			return( 0 );
	}
	return (1);
} /* segread() */

/*
 * Read `n' bytes into the buffer `bp' from kernel memory
 * starting at seek position `s'.
 */
kread(s, bp, n)
	long s;
	char *bp;
	int n;
{
	lseek(kfd, (long)s, 0);
	if (read(kfd, bp, n) != n)
		panic("Kernel memory read error");
}

/*
 * Read `n' bytes into the buffer `bp' from absolute memory
 * starting at seek position `s'.
 */
mread(s, bp, n)
	long s;
	char *bp;
	int n;
{
	lseek(mfd, (long)s, 0);
	if (read(mfd, bp, n) != n)
		panic("Memory read error");
}

/*
 * Read `n' bytes into the buffer `bp' from the swap file
 * starting at seek position `s'.
 */
dread(s, bp, n)
	long s;
	char *bp;
	int n;
{
	/*
	 * If swap device exists go look at it
	 */
	if( dfd > 0 ) {
		lseek(dfd, (long)s, 0);

		if (read(dfd, bp, n) != n)
			panic("Swap read error");

		return( 1 );
	}

	return( 0 );
}

/*
 * Print out an error message and exit.
 */
panic(a1)
	char *a1;
{
	fflush(stdout);
	sleep(2);

	fprintf(stderr, "%r", &a1);
	fprintf(stderr, "\n");

	fflush(stderr);
	exit(1);
}

/*
 * Functions to see if a pointer is in alloc space and to map a
 * pointer from alloc space to this process.
 */
int
range(p)
	char *p;
{
	return (p>=(char *)(callocp.sr_base) &&
			 p<(char*)(callocp.sr_base + callocp.sr_size));
} /* range() */

/*
 * Read `n' bytes into the buffer `bp' from physical memory
 * starting at seek position `s', relative to the page table 'table'.
 *
 * 'table' is a fraction of a 386 page table.  The upper 20 bits of the
 * adjusted virtual address 's' form an index into the table.  The address
 * 's' must be adjusted so that it is relative to the start of the fractional
 * table 'table'.  'table' is fractional because COH386 does not store whole
 * page tables for non-running processes.
 *
 * The entries in the table are 32 bits long (called 'pte').  The lowest bit
 * is the present bit.  If this is 0 for ANY of the pages we are asked to
 * read, we return 0, indicating that we have read nothing.  The upper
 * 20 bits of this entry point in physical memory to a click.  The lower
 * 12 bits of the virtual address 's' are used as an index into this click
 * to find the desired data.
 *
 * All other bits in the page table entry are ignored by this routine.
 *
 * Returns 0 on failure, 1 on success.
 */

int
pt_mread(table, s, bp, n)
	cseg_t *table;	/* Page table.  */
	vaddr_t s;	/* Where to start reading.  */
	char *bp;	/* Buffer to copy into.  */
	int n;		/* Number of bytes to read.  */
{
	int to_read;		/* Number of bytes to read next.  */
	vaddr_t page_offset;	/* How far into page to start reading.  */
	cseg_t pt_entry;	/* Current entry from Page Table.  */


#if 0
	printf("pt_mread(table: %x, s: %x, bp: %x, n: %x)\n", table, s, bp, n);
	fflush(stdout);
	sleep(1);
#endif /* 0 */

	pt_entry = pt_index(table, s>>BPCSHIFT);

	if (!Pflag && !PT_PRESENT(pt_entry)) {
#if 0	/* If the page is not present, the proess probably died already.  */
		static printed_once = FALSE;
		if (!printed_once) {
			printf("\npage not present: %x\n", pt_entry);
			printed_once = TRUE;
		}
#endif /* 0 */
		return(0);
	}
	pt_entry &= PT_CLICK_ADDR;	/* Extract Address of click.  */
	page_offset = s & CLICK_OFFSET;	/* Extract offset into click.  */
	to_read = LESSER(n, ONE_CLICK - page_offset); /* How far to end of click?  */
	
	while (n > 0) {

#if 0
		printf("pt_mread(): mread(from: %x, to: %x, for: %x))\n",
		       pt_entry+page_offset, bp, to_read);
		fflush(stdout);
		sleep(1);
#endif /* 0 */

		mread(pt_entry+page_offset, bp, to_read);

		n -= to_read;	/* How many left?  */
		s += to_read;	/* From where?  */
		bp += to_read;	/* To where?  */

		pt_entry = pt_index(table, s>>BPCSHIFT);
		if (!Pflag && !PT_PRESENT(pt_entry)) {
			printf("partition not present: %x.\n", pt_entry);
			return(0);
		}
		pt_entry &= PT_CLICK_ADDR;	/* Extract Address of click.  */
		page_offset = s & CLICK_OFFSET;	/* Extract offset into click. */
		to_read = LESSER(n, ONE_CLICK);
	} /* while (n > 0) */

	return(1);
} /* pt_mread() */

cseg_t
pt_index(table, index)
	cseg_t *table;
	vaddr_t index;
{
	cseg_t pte;	/* The page table entry we are looking for.  */

#if 0
	printf("pt_index(0x%x, 0x%x)\n", table, index);
	fflush(stdout);
	sleep(1);
#endif /* 0 */

	kread(table+index, &pte, sizeof(cseg_t));

	return(pte);
} /* pt_index() */
