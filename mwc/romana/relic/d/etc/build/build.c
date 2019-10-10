/*
 * build.c
 * 07/09/92	COH 386 release
 *
 * Build (install) COHERENT on a system, part 1.
 * The second part of the install procedure is in install.c.
 * Uses common routines in build0.c,
 * serial number checking in numtab.c and serialno.c.
 * Requires floating point output: cc build.c build0.c numtab.c serialno.c -f
 * Usage: build [ -dv ]
 * Options:
 *	-d	Debug, echo commands without executing
 *	-v	Verbose
 *
 * In addition to the files necessary to run the single user system
 * (/coherent, /etc/init, /bin/sh, /dev/console, /dev/null, etc.),
 * the build disk from which this program runs must contain:
 *	In /bin:	chgrp, chown, cpdir, date, echo, ln, mkdir, mv, rm, touch
 *	In /conf:	boot, mboot, patch
 *	In /dev:	at[01][abcdx], rat[01][abcd]
 *	In /etc:	ATclock, badscan, fdisk, mkdev, mkfs, mount, umount
 * It must also contain directories /mnt and /tmp.
 * This program runs from a write-protected floppy-disk-based COHERENT
 * boot disk, so it writes only to directory /tmp (mounted on a RAM disk).
 */

#include <stdio.h>
#include <canon.h>
#include <string.h>
#include <time.h>
#include <sys/devices.h>
#include <sys/fdisk.h>
#include <sys/filsys.h>
#include <sys/types.h>
#include <access.h>
#include "build0.h"
#include "serialno.h"

/* Compilation switches. */
#define	DOSSHRINK	0		/* punt dosshrink for now	*/

/* Manifest constants. */
#define	VERSION		"3.7"
#define	USAGE		"Usage: /etc/build [ -dv ]\n"
#define	ATDEVS		(NPARTN+NPARTN)	/* number of AT disk devices	*/
#define	BSIZE		512		/* sector size			*/
#if _I386
#define	BAR_BAR		"__"		/* 1st copy of serial # */
#define	BAR_ENTRY	"_entry"	/* 2nd copy of serial # */
#define	MAXSIZE		500		/* suggested max size (MB)	*/
#define	MINSIZE		9		/* required root size (MB)	*/
#define	NEEDSIZE	10		/* suggested min root size (MB)	*/
#define	PIPEDEV		"pipedev"	/* kernel pipe F.S. device */
#define	RONFLAG		"ronflag"	/* kernel readonly root F.S. flag */
#define	ROOTDEV		"rootdev"	/* kernel root F.S. device */
#else
#define	BAR_BAR		"___"		/* 1st copy of serial # */
#define	BAR_ENTRY	"_entry_"	/* 2nd copy of serial # */
#define	MAXSIZE		75		/* suggested max size (MB)	*/
#define	MINSIZE		8		/* required root size (MB)	*/
#define	NEEDSIZE	10		/* suggested min root size (MB)	*/
#define	PIPEDEV		"pipedev_"	/* kernel pipe F.S. device */
#define	RONFLAG		"ronflag_"	/* kernel readonly root F.S. flag */
#define	ROOTDEV		"rootdev_"	/* kernel root F.S. device */
#endif
#define	NAMESIZE	6		/* max device name buffer size	*/
#define	NDEVICES	24		/* number of disk devices	*/

/* (unsigned long) sectors to (double) megabytes. */
#define	meg(sec)	((double)sec * BSIZE / 1000000.)

/* Device table structure. */
typedef	struct	device	{
	char		d_xname[NAMESIZE];	/* partition table name	*/
	char		d_dname[NAMESIZE];	/* device name		*/
	int		d_major;		/* major number		*/
	int		d_minor;		/* minor number		*/
	int		d_flags;		/* flags		*/
	unsigned long	d_size;			/* size in blocks	*/
}	DEVICE;

/* Flag bits. */
#define	F_COH	0x01				/* COHERENT partition	*/
#define	F_BOOT	0x02				/* Active		*/
#define	F_ROOT	0x04				/* Root			*/
#define	F_FS	0x08				/* File system exists	*/
#define	F_MOUNT	0x10				/* Mounted by /etc/rc	*/
#define	F_PROTO	0x20				/* Proto created	*/
#define	F_SCAN	0x40				/* Badscanned		*/
#define	F_ATDEV	0x80				/* AT device		*/
#define	isflag(i, f)	((devices[i].d_flags & (f)) != 0)
#define	notflag(i, f)	((devices[i].d_flags & (f)) == 0)
#define	clrflag(i, f)	devices[i].d_flags &= ~(f)
#define	setflag(i, f)	devices[i].d_flags |= (f)

/*
 * Device table.
 * add_devices() adds entries for devices created by /etc/mkdev.
 * fdisk() zaps the entries for which the xdevice open fails.
 */
DEVICE	devices[NDEVICES] = {
	{ "at0x", "at0a", AT_MAJOR, 0, F_ATDEV, 0L },
	{ "at0x", "at0b", AT_MAJOR, 1, F_ATDEV, 0L },
	{ "at0x", "at0c", AT_MAJOR, 2, F_ATDEV, 0L },
	{ "at0x", "at0d", AT_MAJOR, 3, F_ATDEV, 0L },
	{ "at1x", "at1a", AT_MAJOR, 4, F_ATDEV, 0L },
	{ "at1x", "at1b", AT_MAJOR, 5, F_ATDEV, 0L },
	{ "at1x", "at1c", AT_MAJOR, 6, F_ATDEV, 0L },
	{ "at1x", "at1d", AT_MAJOR, 7, F_ATDEV, 0L }
};

/* Externals. */
extern	long	atol();
extern	char	*fgets();
extern	long	lseek();
extern	time_t	time();

/* Forward. */
void	add_devices();
void	badscan();
void	copy();
char	*devname();
void	done();
void	fdisk();
void	get_timezone();
int	is_fs();
void	mkdev();
void	mkfs();
void	patches();
char	*protoname();
char	*rawname();
void	rootpatch();
void	set_date();
void	uucp();
void	user_devices();
void	welcome();
char	*xname();

/* Globals. */
int	active = -1;			/* active partition	*/
char	*activeos;			/* active partition OS	*/
char	buf2[NBUF];			/* extra buffer		*/
HDISK_S	hd;				/* hard disk boot block	*/
int	mboot;				/* mboot replaced	*/
int	ncohdev;			/* number of COHERENT devices */
int	ndevices = ATDEVS;		/* number of devices	*/
int	protoflag;			/* prototypes created	*/
int	root;				/* root partition	*/
char	tzone[NBUF];			/* timezone		*/
char	tzone5[NBUF];			/* timezone for Sys V	*/

main(argc, argv) int argc; char *argv[];
{
	register char *s;

	argv0 = argv[0];
	abortmsg = 1;
	usagemsg = USAGE;
	if (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s; ++s) {
			switch(*s) {
			case 'd':	++dflag;	break;
			case 'v':	++vflag;	break;
			case 'V':
				fprintf(stderr, "%s: V%s\n", argv0, VERSION);
				break;
			default:	usage();	break;
			}
		}
		--argc;
		++argv;
	}
	if (argc != 1)
		usage();

	welcome();
	set_date();
	mkdev();
	fdisk();
	rootpatch();
	badscan();
	mkfs();
	copy();
	user_devices();
	uucp();
	sys("/conf/ldker", S_FATAL);
	patches();
	sys("/bin/echo /etc/build: success >>/mnt/etc/install.log", S_NONFATAL);
	sprintf(cmd, "TIMEZONE=\"%s\" /bin/date >>/mnt/etc/install.log", tzone);
	sys(cmd, S_NONFATAL);
	sys("/bin/echo >>/mnt/etc/install.log", S_NONFATAL);
	done();
	sync();
	sys("/etc/reboot -p", S_IGNORE);
	/* NOTREACHED */
	exit(0);
}

/*
 * Append devices as specified by /etc/mkdev to devices table.
 */
void
add_devices()
{
	register FILE *fp;
	register int i, n;

	if ((fp = fopen("/tmp/devices", "r")) == NULL)
		return;
	for (i = ndevices; i < NDEVICES; i++) {
		n = fscanf(fp, "%s %s %d %d\n",
			devices[i].d_xname, devices[i].d_dname,
			&devices[i].d_major, &devices[i].d_minor);
		if (n == 0 || n == EOF)
			break;
		else if (n != 4)
			fatal("scanf failed on /tmp/devices, n=%d", n);
		++ndevices;
	}
	if (i == NDEVICES)
		nonfatal("too many devices, excess ignored");
	fclose(fp);
}

/*
 * Scan each COHERENT device for bad blocks.
 */
void
badscan()
{
	register int i;
	register char *name;

	cls(0);
	printf(
"The next step in installation is to scan each COHERENT partition\n"
"for bad blocks.  Be patient, this takes a few minutes.\n"
		);
	for (i = 0; i < ndevices; i++) {
		if (notflag(i, F_COH) || notflag(i, F_ATDEV))
			continue;	/* scan only AT device COH partitions */
		printf("\n");
		name = devname(i, 0);
		if (isflag(i, F_FS)) {
			printf(
"Partition %d (%s) already contains a COHERENT filesystem.\n"
"If you wish to continue to use the existing filesystem, you can skip\n"
"scanning it for bad blocks.  If you want to replace it with an empty\n"
"filesystem, you must scan it for bad blocks first.\n",
				i, name);
			if (yes_no("Do you want to scan %s for bad blocks",
				name) == 0)
				continue;
		}
		printf("Scanning partition %d:\n", i);
		setflag(i, F_SCAN);
		sprintf(cmd, "/etc/badscan -v -o %s %s %s",
			protoname(i), rawname(i, 1), xname(i, 1));
		if (sys(cmd, S_NONFATAL) == 0) {
			setflag(i, F_PROTO);
			++protoflag;
		}
	}
}

/*
 * Mount the root filesystem, copy files to it, patch /coherent.
 * Kludge around as required.
 */
void
copy()
{
	cls(0);
	printf(
"The next step is to copy some COHERENT files from the diskette to the\n"
"root filesystem of your hard disk.  This will take a few minutes...\n"
		);

	/* Mount the filesystem. */
	sprintf(cmd, "/etc/mount %s /mnt", devname(root, 1));
	sys(cmd, S_FATAL);

	/* Copy kernel patch and link scripts in case regen needed some day */
	/* Do it now - don't wait for the whole diskette to cpdir */
	sprintf(cmd, "/bin/cpdir -ad%s /conf /mnt/conf", (vflag) ? "v" : "");
	sys(cmd, S_FATAL);
	sprintf(cmd, "/bin/cpdir -ad%s /tmp /mnt/conf/gen", (vflag) ? "v" : "");
	sys(cmd, S_FATAL);
	printf( "..............\n" );

	/* Copy the boot floppy to it. */
	sprintf(cmd, "/bin/cpdir -ad%s -smnt -sbegin -stmp -s/conf / /mnt",
	  (vflag) ? "v" : "");
	sys(cmd, S_FATAL);
	if (!is_dir("/mnt/mnt"))
		sys("/bin/mkdir /mnt/mnt", S_FATAL);
	sys("/bin/chmog 0755 bin bin /mnt/mnt", S_NONFATAL);

	/* Write entry to /etc/install.log. */
	sys("/bin/echo /etc/build: >>/mnt/etc/install.log", S_NONFATAL);
	sprintf(cmd, "TIMEZONE=\"%s\" /bin/date >>/mnt/etc/install.log", tzone);
	sys(cmd, S_NONFATAL);

#if 0
	/* If /etc/fdisk created patched /tmp/coherent, replace /coherent. */
	if (exists("/mnt/tmp/coherent")) {
		sys("/bin/mv /mnt/tmp/coherent /mnt/coherent", S_FATAL);
		sys("/bin/chmod 0400 /mnt/coherent", S_NONFATAL);
		sys("/bin/chown sys /mnt/coherent", S_NONFATAL);
		sys("/bin/chgrp sys /mnt/coherent", S_NONFATAL);
	}
#endif

	/* If /etc/mkdev created devices in /tmp/dev, copy them to /dev. */
	/* Remove the copies in /tmp/dev on the hard disk. */
	if (exists("/tmp/dev"))
		sys("/bin/cpdir -d /tmp/dev /mnt/dev", S_FATAL);
#if !_I386
	/*
	 * As of COH 3.2, support for COM ports is not built into the system.
	 * Echo lines to /tmp/drvld.all to drvld com line support,
	 * then replace /mnt/etc/drvld.all and make sure permissions are right.
	 */
	sys("/bin/echo /etc/drvld -r /drv/al0 >>/tmp/drvld.all", S_NONFATAL);
	sys("/bin/echo /etc/drvld -r /drv/al1 >>/tmp/drvld.all", S_NONFATAL);
#endif
	if (exists("/tmp/drvld.all")) {
		sys("/bin/cp /tmp/drvld.all /mnt/etc/drvld.all", S_NONFATAL);
		sys("/bin/chmog 0744 root root /mnt/etc/drvld.all", S_NONFATAL);
	}

	sys("/bin/cat /tmp/ttys >>/mnt/etc/ttys", S_NONFATAL);

	/* Grow /lost+found to make room for files. */
	sys("cd /mnt/lost+found; "
	    "/bin/touch a b c d e f g h i j k l m n o p q r s t u v w x y z; "
	    "/bin/rm [a-z]", S_IGNORE);

	/* Create /autoboot. */
	sys("/bin/ln -f /mnt/coherent /mnt/autoboot", S_FATAL);

	/* Replace the build version of /etc/brc with the install version. */
	sys("/bin/rm /mnt/etc/brc", S_NONFATAL);
	sys("/bin/ln -f /mnt/etc/brc.install /mnt/etc/brc", S_FATAL);

	/* Link root device to /dev/root. */
	sprintf(cmd, "/bin/ln -f /mnt%s /mnt/dev/root", devname(root, 0));
	sys(cmd, S_FATAL);

	/* Write the timezone to /etc/timezone. */
	sprintf(cmd, "/bin/echo export TIMEZONE=\\\"%s\\\" >/mnt/etc/timezone", tzone);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/bin/echo export TZ=\\\"%s\\\" >>/mnt/etc/timezone", tzone5);
	sys(cmd, S_NONFATAL);

	/* Write the serial number to /etc/serialno. */
	sprintf(cmd, "/bin/echo %s >/mnt/etc/serialno", serialno);
	sys(cmd, S_NONFATAL);

	/* Save the prototypes from /tmp to /conf. */
	if (protoflag)
		sys("/bin/mv /tmp/*.proto /mnt/conf", S_NONFATAL);
}

/*
 * Generate a device name from a DEVICE entry name.
 * Return a pointer to the statically allocated name.
 * If flag and not one of the built-in AT device names,
 * the device is in /tmp/dev rather than /dev.
 * Sleazy hack: this always writes "/tmp/dev/..." in the buffer and
 * massages the return value accordingly so that subsequent calls
 * with same i but different flag will not clobber previous return values.
 */
char *
devname(i, flag) int i, flag;
{
	static char name[4+4+1+NAMESIZE];	/* e.g. "/tmp/dev/at0a" */

	sprintf(name, "/tmp/dev/%s", devices[i].d_dname);
	return (flag && notflag(i, F_ATDEV)) ? name : name+4;
}

/*
 * Done.
 * Print useful information.
 */
void
done()
{
	cls(1);
	printf(
"You have installed the COHERENT operating system onto your hard disk.\n"
"To install files from the remaining diskettes in the installation kit,\n"
"you must boot the COHERENT system from the hard disk.  It will prompt\n"
"you to install the remaining diskettes in the installation kit.\n"
"\n"
"After you finish reading this information, remove the floppy disk,\n"
"hit <Enter> and your system will automatically reboot.\n"
"\n"
		);
	if (mboot) {
		printf(
"If you type a partition number (0 to 7) while\n"
"the boot procedure is trying to read the floppy disk,\n"
"your system will boot the operating system on that partition.\n"
			);
		if (active != -1) {
			printf("If you type nothing, your system will boot ");
			if (active == root)
				printf("COHERENT (partition %d).\n", active);
			else {
				printf("active partition %d", active);
				if (activeos != NULL)
					printf(" (%s)", activeos);
				printf(".\n", active);
			}
		}
	} else
		printf(
"You must boot the new COHERENT root filesystem on partition %d.\n",
			root);
	printf("\nNow remove the floppy disk so your system does not boot from the floppy.\n");
	if (mboot && root != active)
		printf(
"You MUST type %d when the system tries to read the floppy disk during the boot\n"
"procedure to boot the partition containing the new COHERENT root filesystem.\n",
			root);
}

/*
 * Get partition table information.
 */
void
fdisk()
{
	register int fd, i, j, n, part, cohpart, flag;
	char *fname, *s;

	cls(1);
	printf(
"This installation procedure allows you to create one or more partitions\n"
"on your hard disk to contain the COHERENT system and its files.\n"
"Each disk drive may contain no more than four logical partitions.\n"
"If all four partitions on your disk are already in use, you will\n"
"have to overwrite at least one of them to install COHERENT.\n"
"If your disk uses fewer than four partitions and has enough unused space\n"
"for COHERENT (%d megabytes), you can install COHERENT into the unused space.\n"
"If you intend to install MS-DOS after installing COHERENT,\n"
"you must leave the first physical partition free for MS-DOS.\n"
"\n"
"The next part of the installation procedure will let you change the\n"
"partitions on your hard disk.  Data on unchanged hard disk partitions\n"
"will not be changed.  However, data already on your hard disk may be\n"
"destroyed if you change the base or the size of a logical partition,\n"
"or if you change the order of the partition table entries.\n"
"If you need to back up existing data from the hard disk,\n"
"type <Ctrl-C> now to interrupt COHERENT installation; then reboot your\n"
"system and back up your hard disk data onto diskettes.\n"
"\n"
		, NEEDSIZE);
	cls(1);
	printf(
"COHERENT initialization normally writes a new master bootstrap program onto\n"
"your hard disk.  The COHERENT master boot allows you to boot the operating\n"
"system on one selected disk partition (the active partition) automatically;\n"
"it also allows you to boot the operating system on any disk partition by\n"
"typing a key when you reboot.  Mark Williams strongly recommends that you\n"
"use the COHERENT master boot.  However, the COHERENT master boot may not\n"
"work with some operating systems (for example, Xenix) if you make the\n"
"COHERENT partition active; instead, leave the other partition (e.g. Xenix)\n"
"active and boot COHERENT by typing a key.  If you do not use the COHERENT\n"
"bootstrap, you must understand how to boot the COHERENT partition using your\n"
"existing bootstrap program.\n"
"\n"
		);
	if (yes_no("Do you want to use the COHERENT master boot"))
		++mboot;

retry:
	/* Construct an /etc/fdisk command with appropriate xdevice names. */
	strcpy(cmd, "/etc/fdisk -cB");
	if (mboot)
		strcat(cmd, "b /conf/mboot");
	for (i = 0; i < ndevices; i++) {
		if (i == 0
		 || strcmp(devices[i-1].d_xname, devices[i].d_xname) != 0) {
			if ((fd = open(xname(i, 1), 0)) < 0)
				continue;
			close(fd);
			strcat(cmd, " ");
			strcat(cmd, xname(i, 1));
		}
	}
	sys(cmd, S_FATAL);		/* do the fdisk command */

	/* Read the partition table and set device flags appropriately. */
	for (i = part = 0; i < ndevices; ++i) {
		if (i != 0
		 && strcmp(devices[i-1].d_xname, devices[i].d_xname) == 0)
			continue;		/* partition already done */
		fname = xname(i, 1);
		if ((fd = open(fname, 0)) < 0)
			continue;		/* cannot open xdevice */
		if (read(fd, &hd, sizeof hd) != sizeof hd)
			fatal("%s: read failed", fname);
		close(fd);
		if (hd.hd_sig != HDSIG) {
			nonfatal("%s: invalid partition table", fname);
			continue;
		}
		/* The partition table is valid, check its partitions. */
		for (j = 0; j < NPARTN && i + j < ndevices; j++) {
			n = i + j;		/* index in devices[] */
			if (part != n) {
				/*
				 * Copy over unopenable partitions.
				 * This allows subsequent code to use
				 * the devices[] index as the partition number.
				 */
				devices[part] = devices[n];
				n = part;
			}
			part++;			/* another valid partition */
			if (hd.hd_partn[j].p_boot != 0) {
				setflag(n, F_BOOT);
				if (active == -1)
					active = n;	/* first active partition */
				switch(hd.hd_partn[j].p_sys) {
				case SYS_COH:
					activeos = "COHERENT";
					break;
				case SYS_DOS_12:
				case SYS_DOS_16:
				case SYS_DOS_XP:
				case SYS_DOS_LARGE:
					activeos = "MS-DOS";
					break;
				case SYS_XENIX:
					activeos = "Xenix";
					break;
				default:
					activeos = NULL;
					break;
				}
			}
			if (hd.hd_partn[j].p_sys != SYS_COH)
				continue;

			/* Make sure the device can be accessed. */
			s = devname(n, 1);
			if (!exists(s)) {
				nonfatal("cannot open COHERENT partition %d (%s)",
					n, devname(n, 0));
				continue;
			} else if (hd.hd_partn[j].p_size == 0L) {
				nonfatal("COHERENT partition %d (%s) is empty",
					j, devname(n, 0));
				continue;
			}

			/* OK, set flags in the device table. */
			++ncohdev;
			setflag(n, F_COH);
			devices[n].d_size = hd.hd_partn[j].p_size;
			if (is_fs(s, devices[n].d_size))
				setflag(n, F_FS);

			/* Make sure the device is not mounted. */
			sprintf(cmd, "/etc/umount %s 2>/dev/null", s);
			sys(cmd, S_IGNORE);
		}
	}
	ndevices = part;
	if (ndevices == 0)
		fatal("cannot open partition tables");
	else if (ncohdev == 0)
		fatal("no COHERENT partition found");
	cls(0);
	printf("Your system includes %d COHERENT partition%s:\n",
		ncohdev, (ncohdev == 1) ? "" : "s");
	printf("Drive Partition\t  Device\tMegabytes\n");
	for (flag = i = 0; i < ndevices; i++)
		if (isflag(i, F_COH)) {
			cohpart = i;
			printf("%3d\t%3d\t%s\t%.2f\n",
				i/NPARTN,
				i,
				devname(i, 0),
				meg(devices[i].d_size));
			if (((int)meg(devices[i].d_size)) > MAXSIZE)
				flag = 1;
		}
	if (flag) {
		printf(
"\n"
"Your system includes a large COHERENT filesystem (larger than %d megabytes).\n"
#if !_I386
"The /etc/mkfs command which builds COHERENT 286 filesystems may run out of\n"
"memory and fail on large filesystems.\n"
#endif
"You should repartition the hard disk to define smaller COHERENT partitions.\n",
			MAXSIZE);
		if (yes_no("Do you want to repartition the hard disk"))
			goto retry;
		printf("\n");
	}
	if (ncohdev == 1) {
		root = cohpart;
		setflag(root, F_ROOT);
		return;
	}
	printf(
"You must specify one COHERENT partition as the root filesystem.\n"
"The root filesystem contains the files normally used by COHERENT.\n"
"The root filesystem should contain at least %d megabytes.\n",
		NEEDSIZE);
	if (ndevices > ATDEVS)
		printf(
"The COHERENT root filesystem must be on partition 0 through 7.\n"
			);
	if (active != -1 && isflag(active, F_COH)) {
		printf("COHERENT partition %d is marked as active in the partition table.\n",
			active);
		printf("If you choose it as the root, you can boot COHERENT automatically.\n");
	}
	printf("\n");
again:
	s = get_line("Which partition do you want to be the root filesystem?");
	root = *s - '0';
	if (*++s != '\0' || root < 0 || root >= ATDEVS || notflag(root, F_COH)) {
		printf("Enter a number between 0 and 7 which specifies a COHERENT partition.\n");
		goto again;
	}
	if (meg(devices[root].d_size) < (double)NEEDSIZE) {
		printf("Partition %d contains only %.2f megabytes.\n",
			root, meg(devices[root].d_size));
		if (meg(devices[root].d_size) < (double)MINSIZE) {
			printf("It is too small to contain the COHERENT root filesystem.\n");
			goto again;
		}
		if (!yes_no("Are you sure you want it to be the root partition"))
			goto again;
	}
	setflag(root, F_ROOT);
}

/*
 * Set up a nonstandard timezone.
 */
void
get_timezone(dstflag)
int dstflag;
{
	register char *s;
	int diff;
	char std_abbr[20], dst_abbr[20];
	int east_of_gr;

	/* tzone5 is like tzone except no colons and number is in hours */

	printf(
"You need to specify an abbreviation for your timezone,\n"
"whether you are east or west of Greenwich, England,\n"
"and the difference in minutes between your timezone\n"
"and Greenwich Time (called UT or GMT).  For example,\n"
"Germany is 60 minutes of time east of Greenwich.\n"
		);
	s = get_line("Abbreviation for your timezone:");
	strcpy(std_abbr, s);
	east_of_gr = yes_no("Is your timezone east of Greenwich");
	s = get_line("Difference in minutes from GMT:");
	diff = atoi(s);
	if (east_of_gr)
		diff = -diff;
	if (dstflag) {
		s = get_line("Abbreviation for your daylight savings timezone:");
		strcpy(dst_abbr, s);
		sprintf(tzone, "%s:%d:%s:1.1.4", std_abbr, diff, dst_abbr);
		sprintf(tzone5, "%s%d%s", std_abbr, diff/60, dst_abbr);
	} else {
		sprintf(tzone, "%s:%d:", std_abbr, diff);
		sprintf(tzone5, "%s%d", std_abbr, diff/60);
	}
}

/*
 * Check if a special file is a well-formed filesystem.
 * This routine is derived from code in "mount.c".
 * Here the check that "special" is a block special file is eliminated
 * and the size is checked against the partition size.
 */
int
is_fs(special, size) char *special; unsigned long size;
{
	static struct filsys f;
	register int fd;
	register struct filsys *fp;
	register daddr_t *dp;
	register ino_t *ip, maxinode;

	if ((fd = open(special, 0)) < 0			/* cannot open */
	 || lseek(fd, (long)SUPERI*BSIZE, 0) == -1L	/* seek failed */
	 || read(fd, &f, sizeof(f)) != sizeof(f))	/* read failed */
		return 0;
	close(fd);

	/* Canonical stuff. */
	fp = &f;
	canshort(fp->s_isize);
	candaddr(fp->s_fsize);
	canshort(fp->s_nfree);
	for (dp = &fp->s_free[0]; dp < &fp->s_free[NICFREE]; dp += 1)
		candaddr(*dp);
	canshort(fp->s_ninode);
	for (ip = &fp->s_inode[0]; ip < &fp->s_inode[NICINOD]; ip += 1)
		canino(*ip);
	candaddr(fp->s_tfree);
	canino(fp->s_tinode);

	/* Test for rationality. */
	maxinode = (fp->s_isize - INODEI) * INOPB + 1;
	if (fp->s_isize >= fp->s_fsize)
		return 0;
	if ((fp->s_tfree < fp->s_nfree)
	||  (fp->s_tfree >= fp->s_fsize - fp->s_isize + 1))
		return 0;
	if ((fp->s_tinode < fp->s_ninode) || (fp->s_tinode >= maxinode-1 ))
		return 0;
	for (dp = &fp->s_free[0]; dp < &fp->s_free[fp->s_nfree]; dp += 1)
		if ((*dp < fp->s_isize) || (*dp >= fp->s_fsize))
			return 0;
	for (ip = &fp->s_inode[0]; ip < &fp->s_inode[fp->s_ninode]; ip += 1)
		if ((*ip < 1) || (*ip > maxinode))
			return 0;
	if (fp->s_fsize > (daddr_t)size)
		return 0;
	if (fp->s_fsize > (daddr_t)size)
		nonfatal("warning: filesystem size=%ld but partition size=%ld",
			(long)fp->s_fsize, size);
	return 1;
}

/*
 * Make new devices with /etc/mkdev if appropriate.
 */
void
mkdev()
{
	int hdc;

	cls(0);
printf("Most PC compatible computer systems use MFM, RLL, IDE, or ESDI disk\n");
printf("controllers and disk drives.  A few percent use SCSI disk drives.\n");
printf("Please indicate the type(s) of disk drive(s) used in your computer system.\n");
printf("If you are uncertain of the type, please select choice 1.\n\n");
printf("Are you using:\n\n");
printf("1.  AT-compatible hard drive controller (IDE/RLL/MFM/ESDI).\n");
printf("2.  SCSI hard drive controller.\n");
printf("3.  Both.\n\n");
	hdc = get_int(1, 3, "Enter your choice:");

	sprintf(cmd, "/etc/mkdev -b%s%s %s %s",
		(dflag) ? "d" : "",
		(vflag) ? "v" : "",
		(hdc == 1 || hdc == 3) ? "at" : "",
		(hdc == 2 || hdc == 3) ? "scsi" : "");
		sys(cmd, S_NONFATAL);
	if (hdc == 2 || hdc == 3)
		add_devices();
}

/*
 * Make filesystems on COHERENT partitions.
 */
void
mkfs()
{
	register int i;
	char *name;

	cls(0);
	printf(
"You must create an empty COHERENT filesystem on each COHERENT partition\n"
"before you can use it.  Creating an empty filesystem will destroy all\n"
"previously existing data on the partition.\n"
		);
	for (i = 0; i < ndevices; i++) {
		if (notflag(i, F_COH) || (isflag(i, F_ATDEV) && notflag(i, F_SCAN)))
			continue;
		printf("\n");
		if (isflag(i, F_FS))
			printf("Partition %d (%s) already contains a COHERENT filesystem.\n",
				i, devname(i, 0));
		name = devname(i, 1);
again:
		if (yes_no("Do you want to create a new COHERENT filesystem on partition %d", i)) {
			if (notflag(i, F_ATDEV))
				sprintf(cmd, "/etc/mkfs %s %lu", name, devices[i].d_size);
			else if (notflag(i, F_PROTO)) {
				printf("The attempt to scan %s for bad blocks previously failed.",
					name);
				if (yes_no("Do you want to create a new filesystem on it without a bad block list"))
					sprintf(cmd, "/etc/mkfs %s %lu", name, devices[i].d_size);
				else
					continue;
			} else
				sprintf(cmd, "/etc/mkfs %s %s",
					name, protoname(i));
			clrflag(i, F_FS);
			if (sys(cmd, S_NONFATAL) == 0) {
				setflag(i, F_FS);
				if (notflag(i, F_PROTO)) {
					/* Stick a boot block on device. */
					/* The proto does it in the other case. */
					sprintf(cmd, "/bin/cp /conf/boot %s", name);
					sys(cmd, S_NONFATAL);
				}

				/*
				 * Mount the file system,
				 * create /lost+found,
				 * unmount it.
				 */
				sprintf(cmd, "/etc/mount %s /mnt", name);
				if (sys(cmd, S_NONFATAL))
					continue;
				sprintf(cmd, "/bin/mkdir /mnt/lost+found");
				if (sys(cmd, S_NONFATAL) == 0)
					sys(
"cd /mnt/lost+found; "
"/bin/touch a b c d e f g h i j k l m n o p q r s t u v w x y z; "
"/bin/rm [a-z]",
						S_IGNORE);
				sprintf(cmd, "/etc/umount %s", name);
				sys(cmd, S_NONFATAL);
			} else if (i == root)
				fatal("%s: root partition mkfs failed", name);
		} else if (i == root) {
			if (notflag(i, F_FS)) {
				printf("You must create a filesystem on the root partition.\n");
				goto again;
			} else {
				/* Stick a boot block on the root device. */
				sprintf(cmd, "/bin/cp /conf/boot %s", name);
				sys(cmd, S_NONFATAL);
			}
		}
	}
}

/*
 * Validate "name" to see if its an OK UUCP sitename or domain name.  If
 * anything suspicious is found, query the user and allow them to change
 * their answer.  The domain defaults to UUCP.  In order to avoid having
 * 10,000 machines called bbsuser, no default exists for the sitename.
 *
 * Return true if OK, false otherwise.
 */
int
ok_name(name, type)
unsigned char *name;		/* User's response to site/domain question */
int	type;			/* 'd' == domain, 'u' == uucpname/site */
{
	int warn = 0;
	char	save[NBUF];	/* save off name for caller */

	if (type == 'd' && name[0] == '.')
		strcpy(name, name+1);
	strcpy(save, name);
	if (name[0] == '\0') {			/* no input ? */
		if (type == 'd') {
			strcpy(name, "UUCP");	/* default to UUCP domain */
			return 1;
		} else {
			return 0;		/* no defaults for sitename */
		}
	}
	if (type == 'u' && strlen(name) > 7) {
		++warn;
		printf(
"The system name you chose is greater than seven characters in length.\n"
		);
	}
	if ((type == 'd' && strspn(name, "abcdefghijklmnopqrstuvwxyz"
			 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			 "0123456789"
			 ".-_") != strlen(name))
	  || (type == 'u' && strspn(name, "abcdefghijklmnopqrstuvwxyz"
			 "0123456789") != strlen(name))) {
		++warn;
		printf("The name you chose contains invalid characters.\n");
	}
	if (!warn)
		return 1;
	if (yes_no("Would you like to choose a different name"))
		return 0;
	strcpy(name, save);
	return 1;
}

/*
 * If PATCHFILE exists, execute it.
 */
void patches()
{
	if (access(PATCHFILE, AREAD) == 0) {
		sprintf(cmd, "/bin/sh %s", PATCHFILE);
		sys(cmd, S_NONFATAL);
	}
}

/*
 * Generate a prototype name from a DEVICE entry name.
 * Return a pointer to the statically allocated name.
 */
char *
protoname(i) int i;
{
	static char pname[5+NAMESIZE+6];	/* e.g. "/tmp/at0a.proto" */

	sprintf(pname, "/tmp/%s.proto", devices[i].d_dname);
	return pname;
}

/*
 * Generate a raw device name from a DEVICE entry name.
 * Return a pointer to the statically allocated name.
 * If flag and not one of the built-in AT device names,
 * the device is in /tmp/dev rather than /dev.
 */
char *
rawname(i, flag) int i, flag;
{
	static char rname[4+4+1+1+NAMESIZE];	/* e.g. "/tmp/dev/rat0a" */

	sprintf(rname, "/tmp/dev/r%s", devices[i].d_dname);
	return (flag && notflag(i,  F_ATDEV)) ? rname : rname+4;
}
/*
 * Patches to be done to the kernel - but can't do them yet because
 * kernel hasn't been linked.
 */
void
rootpatch()
{
	/*
	 * After fdisk() we know "root".  Write to PATCHFILE the patches
	 * that will be needed when a new kernel is linked at /mnt/coherent.
	 */
	sprintf(cmd,
	  "echo /conf/patch /mnt/coherent %s=0 %s=0x%lx:l %s=0x%lx:l >> %s\n",
	  RONFLAG, BAR_BAR, atol(serialno),
	  BAR_ENTRY, atol(serialno), PATCHFILE);
	sys(cmd, S_FATAL);
	sprintf(cmd,
	  "echo /conf/patch /mnt/coherent "
	  "\\\"%s\\=makedev\\(%d,%d\\)\\\" "
	  "\\\"%s\\=makedev\\(%d,%d\\)\\\" >> %s\n",
	  ROOTDEV, devices[root].d_major, devices[root].d_minor,
	  PIPEDEV, devices[root].d_major, devices[root].d_minor, PATCHFILE);
	sys(cmd, S_FATAL);
}

/*
 * Date and time.
 */
void
set_date()
{
	register char *s;
	int dst_conv;		/* 1 if DST conversion will be used */
	int dst_now;		/* 1 if DST in effect today */
	int n;
	char *tz;
	time_t now;
	struct tm *tmp;
	char *timestr;

again:
#if 1	/* new set_date */

	/*
	 * yyy:
	 *
	 * dst_conv = FALSE
	 * dst_now = FALSE
	 *
	 * if using DST conversion
	 *	dst_conv = TRUE
	 *	if DST in effect today
	 *		dst_now = TRUE
	 * get date from system clock
	 * if dst_conv and dst_now
	 *	add 1 hour to date fetched
	 * display date
	 * while date not correct
	 *	if proceed without setting clock
	 *		goto xxx
	 *	read date from kb
	 *	write date to CMOS clock and RAM clock
	 *	if dst_conv and dst_now
	 *		subtract 1 hour from date entered
	 *		write adjusted date to CMOS clock
	 * xxx:
	 * set TIMEZONE and TZ variables
	 * if date, TIMEZONE, and TZ not all correct
	 *	goto yyy
	 */
	cls(0);
	dst_conv = 0;
	dst_now = 0;
	printf(
"You can run COHERENT with or without conversion for daylight savings time\n"
"(summer time).  You should normally run with daylight savings time\n"
"conversion.  However, if you are going to use both COHERENT and MS-DOS\n"
"and you choose to run with daylight savings time conversion,\n"
"your time will be wrong (by one hour) during daylight savings time\n"
"while you are running under MS-DOS.\n"
"\n"
		);
	if (yes_no(
	  "Do you want COHERENT to use daylight savings time conversion")) {
		dst_conv = 1;
		printf(
"\n"
"By default, COHERENT assumes daylight savings time begins on the\n"
"first Sunday in April and ends on the last Sunday in October.\n"
"If you want to change the defaults, edit the file \"/etc/timezone\"\n"
"after you finish installing COHERENT.\n"
"\n"
		);
		if (yes_no("Is daylight savings time currently in effect"))
			dst_now = 1;
	}
	sys("/bin/date `/etc/ATclock` > /dev/null", S_NONFATAL);
	now = time(0);
	if (dst_conv && dst_now)
		now += 3600;
	timestr = ctime(&now);
	printf(
"\nAccording to your system clock, your local date and time are:\n"
	);
	printf("%s\n", timestr);
	if (!yes_no("Is this correct")) {
		n = 0;
		do {
			if (++n > 3) {
				printf(
"The command which sets the internal real-time clock of your system is\n"
"failing repeatedly.  Either you are entering the date and time incorrectly\n"
"or your clock hardware is not completely AT-compatible.  If your clock\n"
"hardware is incompatible, you can continue with the installation without\n"
"setting the clock correctly.  However, if you do so, subsequent clock\n"
"references (including file access and modification time information) will be\n"
"incorrect and some commands (such as \"date\") will not function correctly.\n"
					);
				if (yes_no("Do you want to proceed without setting the clock correctly"))
					break;
				n = 0;
			}
			s = get_line(
"\nEnter the correct date and time in the form YYMMDDHHMM.SS:"
				);
			sprintf(cmd, "/etc/ATclock %s >/dev/null", s);
		} while (sys(cmd, S_NONFATAL) != 0);
		sys("/bin/date `/etc/ATclock` >/dev/null", S_NONFATAL);

		if (dst_conv && dst_now) {
			/* Adjust for DST: set hardware clock back one hour. */
			now = time(0) - 3600;
			tmp = localtime(&now);
			sprintf(cmd,
			  "/etc/ATclock %02d%02d%02d%02d%02d.%02d >/dev/null",
			  tmp->tm_year, tmp->tm_mon + 1, tmp->tm_mday,
			  tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
			sys(cmd, S_NONFATAL);
		}

	}

	/* Timezone. */
	cls(0);
	printf(
"Please choose one of the following timezones:\n"
"\t0\tCentral European\n"
"\t1\tGreenwich\n"
"\t2\tNewfoundland\n"
"\t3\tAtlantic\n"
"\t4\tEastern\n"
"\t5\tCentral\n"
"\t6\tMountain\n"
"\t7\tPacific\n"
"\t8\tYukon\n"
"\t9\tAlaska\n"
"\t10\tBering\n"
"\t11\tHawaii\n"
"\t12\tOther\n"
		);
	do {
		s = get_line("Timezone code:");
	} while ((n = atoi(s)) < 0 || n > 12);
	switch (n) {
	/* N.B. entries truncated at tz[8] below if !dst_conv. */
	case 0:		tz = "EST:-60:EDT:1.1.4";	break;
	case 1:		tz = "GMT:000:GDT:1.1.4";	break;
	case 2:		tz = "NST:210:NDT:1.1.4";	break;
	case 3:		tz = "AST:240:ADT:1.1.4";	break;
	case 4:		tz = "EST:300:EDT:1.1.4";	break;
	case 5:		tz = "CST:360:CDT:1.1.4";	break;
	case 6:		tz = "MST:420:MDT:1.1.4";	break;
	case 7:		tz = "PST:480:PDT:1.1.4";	break;
	case 8:		tz = "YST:540:YDT:1.1.4";	break;
	case 9:		tz = "AST:600:ADT:1.1.4";	break;
	case 10:	tz = "BST:660:BDT:1.1.4";	break;
	case 11:	tz = "HST:600:HDT:1.1.4";	break;
	case 12:	tz = NULL;			break;
	}

	if (tz == NULL)
		get_timezone(dst_conv);
	else {
		strcpy(tzone, tz);
		if (dst_conv) {
			/* for TZ, AST:240:ADT becomes AST4ADT */
			sprintf(tzone5, "%.3s%d%cDT",
			  tz, atoi(tzone + 4)/60, tz[0]);
		} else {
			/* for TZ, AST:240 becomes AST4 */
			sprintf(tzone5, "%.3s%d", tz, atoi(tzone + 4)/60);
			tzone[8] = '\0';
		}
	}
	/* Done, print current time and retry if user botched it. */
	printf("\nYour current local date and time are now:\n");
	sprintf(cmd, "TIMEZONE='%s' /bin/date -s `/etc/ATclock`", tzone);
	sys(cmd, S_NONFATAL);

	/* Write the timezone to /tmp/timezone for debug */
	sprintf(cmd, "/bin/echo export TIMEZONE=\\\"%s\\\" >/tmp/timezone", tzone);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/bin/echo export TZ=\\\"%s\\\" >>/tmp/timezone", tzone5);
	sys(cmd, S_NONFATAL);
#else
	cls(0);
	/* Get correct local time, set system time accordingly. */
	printf(
"It is important for the COHERENT system to know the correct date and time.\n"
"You must provide information about your timezone and daylight savings time.\n"
"\n"
"According to your computer system clock, your current local date and time are:\n"
		);
	sys("/bin/date `/etc/ATclock`", S_NONFATAL);
	if (!yes_no("Is this correct")) {
		n = 0;
		do {
			if (++n > 3) {
				printf(
"The command which sets the internal real-time clock of your system is\n"
"failing repeatedly.  Either you are entering the date and time incorrectly\n"
"or your clock hardware is not completely AT-compatible.  If your clock\n"
"hardware is incompatible, you can continue with the installation without\n"
"setting the clock correctly.  However, if you do so, subsequent clock\n"
"references (including file access and modification time information) will be\n"
"incorrect and some commands (such as \"date\") will not function correctly.\n"
					);
				if (yes_no("Do you want to proceed without setting the clock correctly"))
					break;
				n = 0;
			}
			s = get_line(
"Enter the correct date and time in the form YYMMDDHHMM.SS:"
				);
			sprintf(cmd, "/etc/ATclock %s >/dev/null", s);
		} while (sys(cmd, S_NONFATAL) != 0);
		sys("/bin/date `/etc/ATclock` >/dev/null", S_NONFATAL);
	}

	/* DST. */
	cls(0);
	printf(
"You can run COHERENT with or without conversion for daylight savings time\n"
"(summer time).  You should normally run with daylight savings time\n"
"conversion.  However, if you are going to use both COHERENT and MS-DOS\n"
"and you choose to run with daylight savings time conversion,\n"
"your time will be wrong (by one hour) during daylight savings time\n"
"while you are running under MS-DOS.\n"
"\n"
		);
	dst_conv = yes_no("Do you want COHERENT to use daylight savings time conversion");
	if (dst_conv) {
		printf(
"\n"
"By default, COHERENT assumes daylight savings time begins on the\n"
"first Sunday in April and ends on the last Sunday in October.\n"
"If you want to change the defaults, edit the file \"/etc/timezone\"\n"
"after you finish installing COHERENT.\n"
"\n"
			);
		if (yes_no("Is daylight savings time currently in effect")) {
			/* Adjust for DST: set hardware clock back one hour. */
			now = time(NULL) - 60 * 60;
			tmp = localtime(&now);
			sprintf(cmd, "/etc/ATclock %02d%02d%02d%02d%02d.%02d >/dev/null",
				tmp->tm_year, tmp->tm_mon + 1, tmp->tm_mday,
				tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
			sys(cmd, S_NONFATAL);
		}
	}

	/* Timezone. */
	cls(0);
	printf(
"Please choose one of the following timezones:\n"
"\t0\tCentral European\n"
"\t1\tGreenwich\n"
"\t2\tNewfoundland\n"
"\t3\tAtlantic\n"
"\t4\tEastern\n"
"\t5\tCentral\n"
"\t6\tMountain\n"
"\t7\tPacific\n"
"\t8\tYukon\n"
"\t9\tAlaska\n"
"\t10\tBering\n"
"\t11\tHawaii\n"
"\t12\tOther\n"
		);
	do {
		s = get_line("Timezone code:");
	} while ((n = atoi(s)) < 0 || n > 12);
	switch (n) {
	/* N.B. entries truncated at tz[8] below if !dst_conv. */
	case 0:		tz = "EST:-60:EDT:1.1.4";	break;
	case 1:		tz = "GMT:000:GDT:1.1.4";	break;
	case 2:		tz = "NST:210:NDT:1.1.4";	break;
	case 3:		tz = "AST:240:ADT:1.1.4";	break;
	case 4:		tz = "EST:300:EDT:1.1.4";	break;
	case 5:		tz = "CST:360:CDT:1.1.4";	break;
	case 6:		tz = "MST:420:MDT:1.1.4";	break;
	case 7:		tz = "PST:480:PDT:1.1.4";	break;
	case 8:		tz = "YST:540:YDT:1.1.4";	break;
	case 9:		tz = "AST:600:ADT:1.1.4";	break;
	case 10:	tz = "BST:660:BDT:1.1.4";	break;
	case 11:	tz = "HST:600:HDT:1.1.4";	break;
	case 12:	tz = NULL;			break;
	}

	if (tz == NULL)
		get_timezone(dst_conv);
	else {
		strcpy(tzone, tz);
		if (!dst_conv)
			tzone[8] = '\0';
	}

	/* Done, print current time and retry if user botched it. */
	printf("\nYour current local date and time are now:\n");
	sprintf(cmd, "TIMEZONE='%s' /bin/date -s `/etc/ATclock`", tzone);
	sys(cmd, S_NONFATAL);
#endif	/* new set_date */
	if (!yes_no("Is this correct"))
		goto again;
}

/*
 * Configure user devices.
 * Assumes hard disk filesystem mounted on /mnt.
 * Write lines to /etc/mount.all, /etc/umount.all to [u]mount the user devices.
 */
void
user_devices()
{
	register int i, status;
	register char *s, *s2, *name, *rname;

	if (ncohdev == 1) {
		sys("/bin/echo /dev/root >>/mnt/etc/checklist", S_NONFATAL);
		return;
	}

	/* Create user device names. */
	cls(0);
	printf(
"Your system includes %d partition%s in addition to the root partition.\n"
"These partitions are usually mounted on directories in the COHERENT\n"
"root filesystem when the system goes into multiuser mode.\n"
"For example, one non-root partition might be mounted on\n"
"directory \"/u\", another on \"/v\", and so on.\n"
"You now may specify where you want each partition mounted.\n",
		ncohdev - 1, ncohdev == 2 ? "" : "s");
	for (i = 0; i < ndevices; i++) {
		if (notflag(i, F_COH) || notflag(i, F_FS) || isflag(i, F_ROOT))
			continue;
		name = devname(i, 0);
		rname = rawname(i, 0);
		printf("\nPartition %d (%s):\n", i, name);
		if (yes_no("Do you want %s mounted", name)) {
			setflag(i, F_MOUNT);
again:
			s = get_line("Where do you want to mount it?");
			if (*s != '/') {
				printf("Type a directory name beginning with '/', such as \"/u\".\n");
				goto again;
			} else if ((s2 = strchr(s, ' ')) != NULL)
				*s2 = '\0';
			sprintf(cmd, "/mnt/%s", s);
			if ((status = is_dir(cmd)) == -1) {
				printf("%s exists but is not a directory.\n", s);
				goto again;
			} else if (status == 1) {
				strcpy(buf2, s);
				printf("Directory %s already exists.\n", s);
				if (!yes_no("Are you sure you want %s mounted on %s", name, s))
					goto again;
				s = buf;
				strcpy(s, buf2);
			} else {
				/* Make the target directory, uid=bin, gid=bin. */
				sprintf(cmd, "/bin/mkdir -r /mnt%s", s);
				if (sys(cmd, S_NONFATAL))
					goto again;
				sprintf(cmd, "/bin/chown bin /mnt%s", s);
				sys(cmd, S_NONFATAL);
				sprintf(cmd, "/bin/chgrp bin /mnt%s", s);
				sys(cmd, S_NONFATAL);
			}
			printf("%s will be mounted on %s when COHERENT goes multiuser.\n",
				name, s);

			/* Change e.g. /usr/src to usr_src. */
			strcpy(buf2, &s[1]);
			while ((s2 = strchr(buf2, '/')) != NULL)
				*s2 = '_';

			/* Make link to pseudo-device, e.g. "/dev/usr_src". */
			sprintf(cmd, "/mnt/dev/%s", buf2);
			if (exists(cmd))
				status = 1;		/* use normal name */
			else {
				sprintf(cmd, "/bin/ln -f /mnt%s /mnt/dev/%s",
					name, buf2);
				if ((status = sys(cmd, S_NONFATAL)) == 0)
					printf(
"/dev/%s is linked to %s to provide a mnemonic device name.\n",
						buf2, name);
			}

			/* Add lines to /etc/mount.all, /etc/umount.all. */
			if (status == 0)
				sprintf(cmd, "/bin/echo /etc/mount /dev/%s %s >>/mnt/etc/mount.all",
					buf2, s);
			else
				sprintf(cmd, "/bin/echo /etc/mount %s %s >>/mnt/etc/mount.all",
					name, s);
			sys(cmd, S_NONFATAL);
			if (status == 0)
				sprintf(cmd, "/bin/echo /etc/umount /dev/%s >>/mnt/etc/umount.all",
					buf2);
			else
				sprintf(cmd, "/bin/echo /etc/umount %s >>/mnt/etc/umount.all",
					name);
			sys(cmd, S_NONFATAL);

			/* And again, for the raw device. */
			sprintf(cmd, "/mnt/dev/r%s", buf2);
			if (exists(cmd))
				status = 1;
			else {
				sprintf(cmd, "/bin/ln -f /mnt%s /mnt/dev/r%s",
					rname, buf2);
				if ((status = sys(cmd, S_NONFATAL)) == 0)
					printf(
"/dev/r%s is linked to %s to provide a mnemonic device name.\n",
						buf2, rname);
			}

			/* Add raw device line to /etc/checklist. */
			if (status == 0)
				sprintf(cmd, "/bin/echo /dev/r%s >>/mnt/etc/checklist",
					buf2);
			else
				sprintf(cmd, "/bin/echo %s >>/mnt/etc/checklist",
					rname);
			sys(cmd, S_NONFATAL);
		} else {
			/* Not mounted, check using standard name. */
			sprintf(cmd, "/bin/echo %s >>/mnt/etc/checklist", rname);
			sys(cmd, S_NONFATAL);
		}
	}
	sys("/bin/echo /dev/root >>/mnt/etc/checklist", S_NONFATAL);

	/* Link /dev/dos if desired. */
	if (yes_no("Do you use both COHERENT and MS-DOS on your hard disk")) {
		i = get_int(0, ndevices-1, "Enter the partition number of your MS-DOS partition:");
		sprintf(cmd, "/bin/ln -f /mnt%s /mnt/dev/dos", devname(i, 0));
		if (sys(cmd, S_NONFATAL) == 0)
			printf(
"Device name /dev/dos is now linked to %s for use as a mnemonic\n"
"device name.  You may use the \"dos*\" family of commands to transfer files\n"
"to and from the MS-DOS partition on your hard disk as well as MS-DOS floppies.\n",
				devname(i, 0));
		printf("\n");
	}
}


/*
 * Set up site/machine specific info in files /etc/uucpname and /etc/domain
 */
void
uucp()
{
	unsigned char *cp;

	cls(1);
	printf(
"In order to use COHERENT's electronic mail facility and UUCP subsystem,\n"
"you must choose a \"site name\" for your computer system.  In general, a site\n"
"name consists of lower case letters or digits and should be at most seven\n"
"characters in length.  The name you choose should be unique if you intend\n"
"to access any other computer systems.  Some of the more well known site\n"
"names include \"mwc\", \"uunet\", \"clout\", \"decwrl\", \"hp\", \"kgbvax\", "
"\"prep\",\n\"seismo\", and \"ucbvax\".\n\n"
	);
	for (;;) {
		cp = get_line("Please enter the site name for this system: ");
		if (ok_name(cp, 'u'))
			break;
	}
	sprintf(cmd, "/bin/echo \"%s\" >/mnt/etc/uucpname", cp);
	sys(cmd, S_NONFATAL);

	printf(
"\nThe COHERENT mail subsystem supports \"domain addressing\" in addition to\n"
"traditional \"bang paths\".  Until your system becomes part of a registered\n"
"domain, you may use the UUCP pseudo-domain.  Domain names consist of groups\n"
"of letters and digits separated by periods (dots).  Some of the more well\n"
"known domains include \"com\", \"edu\", \"gov\", \"org\", \"net\", as well as domains\n"
"covering a geographical area, such as the Chicago area \"chi.il.us\" domain.\n"
"If you are not registered in a domain, or if you are uncertain about this\n"
"question, simply press the <Enter> key to default to the UUCP pseudo-domain.\n\n"
	);
	for (;;) {
		cp = get_line("Please enter the domain name for this system: ");
		if (ok_name(cp, 'd'))
			break;
	}
	sprintf(cmd, "/bin/echo \"%s\" >/mnt/etc/domain", cp);
	sys(cmd, S_NONFATAL);
}

/*
 * Hi there.
 */
void
welcome()
{
	register char *s;
	int i;

	cls(0);
	printf(
"\n\n\n\n\n\n\n\n"
"                              The COHERENT System\n\n"
"                    (c) 1982, 1992 by Mark Williams Company\n\n"
"                     60 Revere Drive, Northbrook, IL  60062\n\n"
"                        708-291-6700, 708-291-6750 (FAX)\n"
"\n\n\n\n\n\n"
		);

	cls(1);
	printf(
"Welcome to the COHERENT operating system!\n\n"
"Your computer is now running COHERENT from the floppy disk.\n"
"This program will install COHERENT onto your hard disk.\n"
"\n"
"You can interrupt installation at any time by typing <Ctrl-C>;\n"
"then reboot and start the installation procedure again.\n"
"Please be patient and read the instructions on the screen carefully.\n"
"\n"
		);
#if 0
	cls(1);
	printf(
"If you do not know the BIOS parameters for your hard disk drive,\n"
"please reset your computer NOW and enter \"dpb\" at the boot prompt.\n"
"Copy the displayed parameter values for later reference, then reset\n"
"again and restart installation by entering \"begin\" at the boot prompt.\n"
		);
#endif
	cls(1);
	sys("/etc/kbdinstall -b", S_NONFATAL);

	cls(1);
	printf(
"A card included with your distribution gives the serial number\n"
"of your copy of COHERENT.\n"
		);
	for (i = 1; i <= 3; i++) {
		s = get_line("Type in the serial number from the card:");
		if (isserial(s))
			return;
		printf("Invalid serial number, please try again.\n");
	}
	fatal("invalid serial number");
}

/*
 * Generate a partition table device name from a DEVICE entry name.
 * Return a pointer to the statically allocated name.
 * If flag and not one of the built-in AT device names,
 * the device is in /tmp/dev rather than /dev.
 */
char *
xname(i, flag) int i, flag;
{
	static char xname[4+4+1+NAMESIZE];	/* e.g. "/tmp/dev/at0x" */

	sprintf(xname, "/tmp/dev/%s", devices[i].d_xname);
	return (flag && notflag(i,  F_ATDEV)) ? xname : xname+4;
}

/* end of build.c */
