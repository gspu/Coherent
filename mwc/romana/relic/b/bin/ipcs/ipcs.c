/*
 * File:	ipcs.c
 *
 * Purpose:	main driver for ipcs utility.
 * Revision 1.1  92/10/08 bin
 * Initial revision
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <stdio.h>
#include <coff.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "ipcs.h"
/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int	iMemSeek();	/* Seek in the virtual memory */
/*
 * ----------------------------------------------------------------------
 *	Global data
 */

/* Option's flags. See man pages for more info */
short	qflag =	0,	/* message q */
	mflag =	0,	/* shared memory */
	sflag =	0,	/* semaphores */
	bflag =	0,	/* biggest */
	cflag =	0,	/* creator name */ 
	oflag = 0,	/* outstanding usage */
	pflag = 0,	/* process ID */
	tflag = 0,	/* time */
	aflag = 0,	/* include b, c, o, p, & t */
	Cflag = 0,	/* corefile */
	Nflag = 0;	/* namelist */

int	total_shmids = 0,	/* total shared memory segs found */
	total_sems = 0,		/* total semaphores found */
	usemsqs = 0;		/* is msq in use */

int	SHMMNI,			/* total # shared memory segments */
	SEMMNI,			/* total # semaphores */
	NMSQID;			/* total # message queues */

/*
 * ----------------------------------------------------------------------
 * Code.
 */
main(argc, argv)
int	argc;
char	*argv[];
{
	char		*opstring = "qmsbcoptaVC:N: ";
	extern char	*optarg;
	char		*namelist = NULL;
	char		*fname,
			*tmpfname;			/* kernel name */
	int		c;

	while ((c = getopt(argc, argv, opstring)) != EOF)
		switch (c) {
		case 'q':
			qflag = 1;
			break;
		case 'm':
			mflag = 1;
			break;
		case 's':
			sflag = 1;
			break;
		case 'b':
			bflag = 1;
			break;	
		case 'c':
			cflag = 1;
			break;
		case 'o':
			oflag = 1;
			break;
		case 'p':
			pflag = 1;
			break;
		case 't':
			tflag = 1;
			break;
		case 'a':
			aflag = 1;
			break;
		case 'C':
/*			Cflag = 1;
 *			corefile = optarg;
 *			break;
 */
			fprintf(stderr, 
				"ipcs: Corefile option NOT yet supported\n");
			exit(1);
		case 'N':
			Nflag = 1;
			namelist = optarg;
			break;
		case 'V':
			printf("ipcs version %s\n", VERSION);
			exit(0);
		default: 
			usage(c);
	}

	set_flags();

	/* Get kernel name. */
	tmpfname = (Nflag ? namelist : pick_nfile());
	if ((fname = malloc(strlen(tmpfname) + 1)) == NULL) {
		perror("ipcs: cannot malloc kernel name:");
		exit(1);
	}
	strcpy(fname, tmpfname);

	getmaxnum(fname);
	get_data(fname);

	/* Now we can print */
	if (qflag)
		print_q();
	if (mflag)
		print_m();
	if (sflag)
		print_s();

	exit(0);
}

/* 
 * set_flags does some additional processing for flags 
 */
set_flags() 
{
	/* Default is all ipc */
	if (!(qflag + mflag + sflag))
		qflag = mflag = sflag = 1;

	/* use all print options */
	if (aflag)
		bflag = cflag = oflag = pflag = tflag = 1;
}

/*
 * Get the following values from the corefile:
 *	SHMMNI:		max number of allowable shared memory segments
 *	SEMMNI:		max number of allowable semaphore sets
 *	NMSQID:		max number of allowable message queues
 */
getmaxnum(fname)
char	*fname;		/* Kernel file name */
{
	SYMENT 	sym[3];	/* The table of names to find */
	int	fd;	/* corefile file descriptor */
	int	val;	/* Read values buffer */
	int	i;	/* Loop index */

	/* Initialise SYMENT array */
	for (i = 0; i < 3; i++) {
		sym[i]._n._n_n._n_zeroes = 0;	/* stuff for coffnlist */
		sym[i].n_type = -1;
	}
	strcpy(sym[0].n_name, "SHMMNI");
	strcpy(sym[1].n_name, "SEMMNI");
	strcpy(sym[2].n_name, "NMSQID");

	/* do lookups. coffnlist returns 0 on error. */
	if (!coffnlist(fname, sym, NULL, 3)) {
		fprintf(stderr, "ipcs: error obtaining values from %s\n", 
									fname);
		exit(1);
	}

	/* Now we got addresses of the variables. So, we can go to memory
	 * and read proper values. sym[i].n_value contains addresses of
	 * variables.
	 */
	/* Get max number of allowable shared memory segments */
	if ((fd = iMemSeek(sym[0].n_value, 0)) < 0) 	/* Open and seek the */
		exit(1);				/* proper file */
	if (read(fd, &val, sizeof(int)) != sizeof(int)) {
		fprintf(stderr, "ipcs: read value of SHMMNI error\n");
		exit(1);
	}
	close(fd);
	SHMMNI = val;

	/* Get max number of allowable semaphores */
	if ((fd = iMemSeek(sym[1].n_value, 0)) < 0) 	/* Open and seek the */
		exit(1);				/* proper file */
	if (read(fd, &val, sizeof(int)) != sizeof(int)) {
		fprintf(stderr, "ipcs: read value of SEMMNI error\n");
		exit(1);
	}
	close(fd);
	SEMMNI = val;

	/* Get max number of allowable message queues */
	if ((fd = iMemSeek(sym[2].n_value, 0)) < 0) 	/* Open and seek the */
		exit(1);				/* proper file */
	if (read(fd, &val, sizeof(int)) != sizeof(int)) {
		fprintf(stderr, "ipcs: read value of NMSQID error\n");
		exit(1);
	}
	NMSQID = val;

	close(fd);
}

/*
 * iMemSeek opens file and seeks in the memory. Uses /dev/kmem for low memory
 * and /dev/kmemhi for the high memory.
 * Return descriptor to the proper memory device or -1 on error.
 */
int	iMemSeek(lWhere, iHow)
long	lWhere;	
int	iHow;
{
	int	fd;				/* File descriptor */
	char	*cpMemLow = "/dev/kmem";	/* Low memory device */
	char	*cpMemHigh = "/dev/kmemhi";	/* High memory device */
	char	*cpMem;				/* Memory to use */
	long	lMemBorder = 0x80000000;	/* Border between devices */
	long	lMemWhere;			/* Point to seek */
	char	cErrBuf[32];			/* Error buffer */

	if (lWhere & lMemBorder) { 
		cpMem = cpMemHigh;
		lMemWhere = lWhere ^ lMemBorder;
	} else {
		cpMem = cpMemLow;
		lMemWhere = lWhere;
	}
	/* Open proper memory device */
	if ((fd = open(cpMem, O_RDONLY)) < 0) {
		sprintf(cErrBuf, "ipcs: cannot open %s", cpMem);
		perror(cErrBuf);
		return -1;
	}
	/* Seek to the requested position */
	if (lseek(fd, lMemWhere, iHow) < 0) {
		perror("ipcs: seek failed:");
		close(fd);
		return -1;
	}
	return fd;
}

/* 
 * ipcs usage. Print message and die 
 */
usage(c) 
int	c;
{
	fprintf(stderr, "ipcs:  illegal option - %c\n", c);
	fprintf(stderr, "usage: "
		 "ipcs [-abcmopqstV] [-C corefile] [-N namelist]\n");
	exit(1);
}
