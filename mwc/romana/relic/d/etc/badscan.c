/*
 * badscan.c
 * 2/7/91
 * Usage: badscan [-v] [-o proto] [-b boot] device count
 *	device	  device to be scanned (e.g. /dev/rat0a)
 *	count	  size of file system to be scanned
 *	sdev	  master boot record for device size info (e.g. /dev/at0x)
 * Options:
 *	-b boot	  use "boot" for boot in prototype file (default: /conf/boot)
 *	-o proto  output file name for prototype file  (default: stdout)
 *	-v	  verbose messages as to the percentage of file system scanned
 *
 * Due to Inetco's braindamaged notion of removing stdio from everything, this
 * does not #include <stdio.h> and uses sys write() calls to fd 1 and fd 2 in
 * lieu of stdout and stderr.  This should be fixed when someone gets the urge.
 *
 * $Log:	/newbits/etc/badscan.c,v $
 * Revision 1.6	91/02/22  12:14:25 	bin
 * updated version provided by stevesf for v311
 * 
 * Revision 1.7	91/02/07  12:36:00 	bin
 * steve 2/7/91
 * Following norm's suggestion, SCSI badscan now issues warning but
 * the continues with block-by-block scan of device.
 * 
 * Revision 1.6	91/02/07  09:58:21 	bin
 * steve 2/7/91
 * Added include of <sys/devices.h>, changed explicitly declared floppy
 * and hard disk major device manifest constants accordingly.
 * Added check for major==SCSI; if so, print "SCSI devices do not require
 * badscan" and exit successfully.
 * Added "badscan:" at start of fatal() messages.
 * Added comment moaning about Inetco avoiding stdio.
 * 
 * Revision 1.5	90/04/20  09:01:50 	bin
 * steve 4/20/90
 * Previous version failed on floppy disks because HDGETA ioctl only works
 * on hard disks.  Now it checks the major number, does HDGETA to find
 * tracksize on hard disk and uses tracksize=1 (sector by sector scan)
 * on floppies.  Future: should add FDGETA ioctl to floppy driver and
 * let badscan (and mkfs) find default floppy size automatically.
 * 
 * Revision 1.4	90/04/19  14:08:36 	bin
 * steve 4/19/90
 * Change -v messages slightly for use during install;
 * use '\r' instead of '\n', overwrite with "Done\n" when finished.
 * 
 * Revision 1.3	90/03/27  08:33:48 	bin
 * steve 03/27/90
 * Mark entire bad track as bad rather than reading each sector in track.
 * 
 * Revision 1.2	90/03/22  09:57:41 	root
 * Find sectors per track with ioctl rather than assuming 17.
 * Rearranged code, changed fatal error messages, changed screwy spacing.
 * 	steve 3/22/90
 * 
 * 88/03/23	Allan Cornish	/usr/src/cmd/etc/badscan.c
 * Reads are no longer attempted past logical end of partition,
 * when a logical track straddles the partition boundary.
 *
 * 88/01/27	Jim Belton	/usr/src/cmd/etc/badscan.c
 * Fixed bug for counts >= 32K by declaring atol().
 *
 * 86/11/03	Joe Iu
 * Added -o and -b options, where the protofile and the boot file name
 * can be specified by the user explicitly.
 *
 * 85/12/03	Allan Cornish
 * Multi-sector read algorithm adjusted to ensure track alignment,
 * even on track 0 where master boot block is not included in partition.
 *
 * 85/11/16	Allan Cornish
 * Added track reads, with block by block retry on error.
 *
 * 85/11/13	Allan Cornish
 * Added estimate of time remaining to each progress report.
 *
 * 85/11/06	Allan Cornish
 * Added -v flag to report progress every 10 percent of scan.
 *
 * 88/07/11     Jim Napier
 * Modified sput() to retry write if returns zero.  This was causing an
 * invalid prototype file to be created when using the Seagate ST-251
 * hard disk which contains bad blocks.
 */

#include <sys/stat.h>
#include <sys/devices.h>
#include <sys/fdisk.h>
#include <sys/hdioctl.h>

#define	BUFSIZ	512
#define	MAXUINT	((unsigned)65535L)
#define	NULL	((char *)0)
#define	TRUE	(0 == 0)

/*
 * External functions.
 */
extern	long	atol();
extern	char	*malloc();

/*
 * Forward referenced functions.
 */
void		bad();
void		fatal();
void		lput();
void		sput();
void		usage();

/*
 * Global variables.
 */
char		*bfile = "/conf/boot";
int		bflag;
int 		oflag;
struct stat	sb;
int		vflag;

/**
 * main 	- validate number and type of arguments.
 *		- scan device (arg1) for bad blocks until lim (arg2) reached.
 *		- generate a mkfs prototype file on standard output or the
 *		  designated protofile.
 */
main(argc, argv) register int argc; register char *argv[];
{
	register int n;
	long bno;
	long incr;
	long lim;
	long t0, t1;
	int percent;
	int nspt;
	char *buf;
	hdparm_t hdparms;
	unsigned int tracksize;
	unsigned long base;

	/*
	 * Read options declared.
	 */
	while((--argc > 0) && ((*++argv)[0] == '-')) {

		if((*argv)[2] != '\0')
			usage();

		switch((*argv)[1]) {

		case 'v':
			if(vflag)
				usage();
			vflag = TRUE;
			break;

		case 'o':
			if(oflag  ||  (--argc <= 0))
				usage();
			oflag = TRUE;

 			/*
			 * Open protofile to store badscan information.
			 */
			close(1);
			if(creat(*++argv, 0644) == -1)
				fatal("cannot create ", *argv);
			break;

		case 'b':
			if(bflag  ||  (--argc <= 0))
				usage();

			bflag = TRUE;
			/*
			 * Set boot file name.
			 */
			bfile = *++argv;
			break;

		default:
			usage();
			break;
		}
	}

 	/*
	 * Check to ensure that only arg1 and arg2 are available.
	 */
	if (argc != 2)
		usage();

	/*
	 * The first argument not associated with an option must be a
	 * character or block special file.
	 */
	if (stat(argv[0], &sb) < 0)
		fatal("cannot stat: ", argv[0]);
	sb.st_mode &= S_IFMT;
	if ((sb.st_mode != S_IFCHR) && (sb.st_mode != S_IFBLK))
		fatal("not a device special file: ", argv[0]);
	if ((n = major(sb.st_rdev)) == SCSI_MAJOR)
		sput(2, "badscan: warning: SCSI devices do not require badscan\n");
	else if (n != AT_MAJOR && n != FL_MAJOR)
		fatal("not a disk device: ", argv[0]);

	/*
	 * Open the special file (arg1).
	 * Find the size of a track.
	 */
	close(0);
	if (open(argv[0], 0) == -1)
		fatal("cannot open: ", argv[0]);
	if (n == AT_MAJOR) {
		/* Get hard disk track size with ioctl. */
		if (ioctl(0, HDGETA, (char *)&hdparms) == -1)
			fatal("cannot get disk device parameters: ", argv[0]);
		tracksize = hdparms.nspt;
	} else {
		/* Badscan a floppy disk or a SCSI disk. */
		/* Floppy ioctl FDGETA should be implemented but is not (yet). */
#if	0
		/* Kludge: this code knows floppy minor to tracksize mapping. */
		n = minor(sb.st_rdev) & 7;
		if (n < 3)		/* minor 0, 1, 2, 8, 9, 10, ... */
			tracksize = 8;
		else if (n < 6)		/* minor 3, 4, 5, 11, 12, 13, ... */
			tracksize = 9;
		else if (n == 6)	/* minor 6, 14, ... */
			tracksize = 15;
		else			/* minor 7, 15, ... */
			fatal("unsupported minor number", argv[0]);
#else
		/* Scan block by block for now. */
		tracksize = 1;
#endif
	}

	/*
	 * Allocate a track buffer.
	 */
	if (tracksize > MAXUINT/BUFSIZ || (buf = malloc(tracksize * BUFSIZ)) == NULL)
		fatal("cannot allocate track buffer", "");

	/*
	 * Validate and evaluate length (arg2)
	 */
	lim = atol(argv[1]);
	if (lim <= 0) {

		register struct hdisk_s *hp;
		int f2;

		if ((f2 = open(argv[1], 0)) < 0)
			fatal("bad size: ", argv[1]);
		if (read(f2, buf, 512) != 512)
			fatal("cannot read: ", argv[1]);
		close(f2);
		hp = buf;
		if (hp->hd_sig != HDSIG)
			fatal("bad partition table: ", argv[1]);
		lim  = hp->hd_partn[ sb.st_rdev & 3 ].p_size;
		base = hp->hd_partn[ sb.st_rdev & 3 ].p_base;
		nspt = tracksize - base % tracksize;
		if (lim <= 0)
			fatal("null partition: ", argv[1]);
	}

	/*
	 * Create header for mkfs prototype file.
	 */
	sput(1, bfile);
	sput(1, "\n");
	lput(1, lim);
	sput(1, " ");
	lput(1, (lim/6 + 7) & ~7L);	/* ensure ninode is multiple of 8 */
	percent = 10;
	incr = lim / 10;
	time(&t0);

	/*
	 * Scan for bad blocks.
	 * The first and last tracks may have less than tracksize sectors.
	 */
	for (bno=0; bno < lim; (bno += nspt), (nspt = tracksize)) {

		/*
		 * Try a track read first.
		 */
		lseek(0, bno * BUFSIZ, 0);

		/*
		 * Avoid reading past end of partition.
		 */
		if (bno + nspt > lim)
			nspt = lim - bno;

		/* Mark each block in a bad track as bad. */
		if (read(0, buf, (nspt * BUFSIZ)) != (nspt * BUFSIZ))
			for (n=0; n < nspt; ++n)
				bad(bno+n);

		/*
		 * Periodically generate reports
		 */
		if (vflag && (bno >= incr) && (bno < lim)) {

			/*
			 * Estimate seconds remaining to next 1/10 minute.
			 */
			time(&t1);
			t1 = (t1 - t0) * (100 - percent);
			t1 += percent - 1;
			t1 /= percent;
			t1 += 5;

			lput(2, (long) percent);
			sput(2, "% done: ");
			if (t1 <  6000)
				sput(2, " ");
			if (t1 <   600)
				sput(2, " ");
			lput(2, t1 / 60);
			sput(2, ".");
			lput(2, (t1 % 60) / 6);
			sput(2, " minutes remaining...\r");

			percent += 10;
			incr =  (lim * percent) / 100;
		}
	}
	sput(2, "Done.                               \n");
	sput(1, "\nd--755 0 0\n$\n");
	exit(0);
}

/**
 * void
 * bad(n)
 * long	n;
 *	Input:	n	- bad block location.
 *	Action:	Flag block n as being bad.
 *	Return:	None.
 *	Note:	None.
 */
void
bad(n) register long n;
{
	static int  nbad =  0;
	static long last = -1;

	if ((last+1) != n)
		nbad = 0;
	last = n;
	if ((nbad & 7) == 0)
		sput(1, "\n%b");
	sput(1, " ");
	lput(1, n);
	++nbad;
}

/**
 * void
 * fatal(s1, s2)
 * char	* s1, *s2;
 *	Input:	s1, s2	- pointer to error message strings.
 *	Action:	Print fatal message, terminate with extreme prejudice.
 *	Return:	Never return. Always exit.
 *	Note:	None.
 */
void
fatal(s1, s2) char * s1, * s2;
{
	sput(2, "badscan: ");
	sput(2, s1);
	sput(2, s2);
	sput(2, "\n");
	exit(1);
}

/**
 * void
 * lput(fd, num)
 * int	fd;
 * unsigned long num;
 *
 *	Input:	fd	- output file descriptor.
 *		num	-
 *	Action:	Convert long num to ascii string sent to file fd.
 *	Return:	None.
 *	Note:	None.
 */
void
lput(fd, num) int	fd; unsigned long num;
{
	register char * cp;
	static char buf[16];

	cp = &buf[15];

	/*
	 * Compute character equivalent value of long num.
	 */
	do {
		*--cp = (num % 10) + '0';
	} while (num /= 10);
	sput(fd, cp);
}

/**
 * void
 * sput(fd, s)
 * int	fd;
 * char	* s;
 *
 *	Input:	fd	- output file descriptor.
 *		s	- pointer to character string.
 *	Action:	Write string s to file descriptor fd.
 *	Return:	None.
 *	Note:	None.
 */
void
sput(fd, s) int	fd; register char * s;
{
	register char * cp;
	int i,j;

	/*
	 * Get location of end of string.
	 */
	for (cp = s; *cp != '\0'; ++cp)
		;
	
	/* WRITE WITH RETRY IF WRITE() RETURNS 0 */
	i = cp-s;
	while ((j = write(fd, s, i)) == 0) ;
	  
}

/**
 * void
 * usage()
 *	Input:	None.
 *	Action:	Display command format.
 *	return:	None.
 *	Note:	None.
 */
void
usage()
{
	fatal(	"Usage: badscan [-v] [-o proto] [-b boot] dev size\n",
		"       badscan [-v] [-o proto] [-b boot] dev sdev");
}

/* end of badscan.c */
