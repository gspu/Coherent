/*
 * build.c
 * Wed Oct 27 05:53:16 1993 CST
 *
 * Build (install) COHERENT on a system, part 1.
 * The second part of the install procedure is in install.c.
 * Uses common routines in build0.c,
 * serial number checking in numtab.c and serialno.c.
 * Requires floating point output: cc build.c build0.c numtab.c serialno.c -f
 * Usage: build [ -dv ]
 * Options:
 *	-d	Debug, echo commands without executing
 *	-u	Update, rather than full installation
 *	-v	Verbose
 *
 * In addition to the files necessary to run the single user system
 * (/coherent, /etc/init, /bin/sh, /dev/console, /dev/null, etc.),
 * the build disk from which this program runs must contain:
 *	In /bin:	chgrp, chown, cpdir, date, echo, ln, mkdir, mv, rm, touch
 *	In /conf:	boot, mboot, patch, upd_suppress, upd_save
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
#include <unistd.h>
#include <sys/devices.h>
#include <sys/fdisk.h>
#include <sys/filsys.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "build0.h"
#include "serialno.h"

/* File names. */
#define DEVICE_FILE		"/tmp/devices"

/* Manifest constants. */
#define	VERSION		"5.0"
#define	USAGE		"Usage: /etc/build [ -dv ]\n"
#define	ATDEVS		(NPARTN+NPARTN)	/* number of AT disk devices	*/
#define	BSIZE		512		/* sector size			*/

#define	BAR_BAR		"_bar"		/* 1st copy of serial # */
#define	BAR_ENTRY	"_entry"	/* 2nd copy of serial # */
#define	MAXSIZE		500		/* suggested max size (MB)	*/
#define	MINSIZE		15		/* required root size (MB)	*/
#define	PIPEDEV		"pipedev"	/* kernel pipe F.S. device */
#define	ROOTDEV		"rootdev"	/* kernel root F.S. device */

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



DEVICE	devices[NDEVICES];


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
int	exists_and_nz();
void	fdisk();
int	is_fs();
void	mkdev();
void	mkfs();
void	new_boot();
void	patches();
char	*protoname();
char	*rawname();
void	rootpatch();
void	save_files();
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
int	ndevices = 0;			/* number of devices	*/
int	protoflag;			/* prototypes created	*/
int	root;				/* root partition	*/

int	update;				/* update, rather than install */

#define	NEEDSIZE_BEGIN	20	/* suggested new install min root size (MB) */
#define	NEEDSIZE_UPDATE	10	/* suggested update min root size (MB)	*/

int	needsize;

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
			case 'u':	++update;	break;
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
	if (update)
		needsize = NEEDSIZE_UPDATE;
	else
		needsize = NEEDSIZE_BEGIN;

	if (argc != 1)
		usage();

	welcome();		/* gets serial # for new installs */
	mkdev();

	fdisk();
	if (update) {
		new_boot();	/* write out new boot block for tboot */
		save_files();	/* gets serial # for updates */
	} else {
		badscan();
		mkfs();
	}

	/* Set serial number in tertiary kernel. */
	idtune_var("BAR_BAR", atol(serialno));
	idtune_var("BAR_ENTRY", atol(serialno));

	copy();

	if (!update)
		user_devices();

	rootpatch();
	patches();
	sprintf(cmd, "/etc/umount %s", devname(root, 1));
	sync();
	sys(cmd,S_NONFATAL);
	done();
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

	if ((fp = fopen(DEVICE_FILE, "r")) == NULL)
		return;

	for (i = ndevices; i < NDEVICES; i++) {
		n = fscanf(fp, "%s %s %d %d\n",
			devices[i].d_xname, devices[i].d_dname,
			&devices[i].d_major, &devices[i].d_minor);

		/* Flag AT drives only for badscannning. */
		if (devices[i].d_major == AT_MAJOR)
			setflag(i, F_ATDEV);

		if (n == 0 || n == EOF)
			break;
		else if (n != 4)
			fatal("scanf failed on %s, n=%d", DEVICE_FILE, n);
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
	int require_badscan_flag = 0;

	cls(1);

	/*
	 * Check to see if we need to badscan.  If not, we'll
	 * skip the whole section -- only need to badscan if
	 * there exists a Coherent partition on an AT drive.
	 */
	for(i = 0; i < ndevices; i++) 
		if(notflag(i, F_COH) || notflag(i, F_ATDEV))
			continue;
		else
			require_badscan_flag++;

	if(require_badscan_flag) {
		printf(
"The next step in installation is to scan each COHERENT partition on the\n"
"at-controller drives for bad blocks.  This will not write to the partition\n"
"being scanned.\n"
"Be patient.  This takes a few minutes.\n"
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
}

/*
 * Mount the root filesystem, copy files to it, patch /coherent.
 * Kludge around as required.
 *
 * THIS ENTIRE FUNCTION NEEDS TO BE A SHELL SCRIPT - WHEN THERE IS TIME!!!
 * - hal
 */
void
copy()
{
	printf(
"\nThe next step is to copy some COHERENT files from the diskette to the\n"
"root filesystem of your hard disk.  This will take a few minutes...\n"
		);

	/* Mount the filesystem. */
	sprintf(cmd, "/etc/mount %s /mnt", devname(root, 1));
	sys(cmd, S_FATAL);

	/* Since mkfs has been fixed, explicitly limit permissions on root. */
	sys("/bin/chmog 0755 root root /mnt", S_NONFATAL);

	if(update){
		printf(
"The existing /coherent kernel on the root file system will now be renamed to\n"
"coh.B4.420. After this the remainder of diskette #1 will be copied.\n\n"
		);

		/* move the existing kernel names /coherent to coh.B4.420 */
		sprintf(cmd,"/bin/cp /mnt/coherent /mnt/coh.B4.420 2>/dev/null");
		sys(cmd,S_NONFATAL);
	}

	/*
	 * Fix upd_suppress - delete names of files not present, when
	 * copying it to hard disk.
	 */
	if (update) {
		sprintf(cmd,
		  "cd /;> /mnt/conf/upd_suppress;"
		  "for F in $(cat /conf/upd_suppress);do "
		    "if [ -f /mnt/$F -o -d /mnt/$F ];then "
		       "echo $F >> /mnt/conf/upd_suppress;"
		    "fi;"
		  "done");
		sys(cmd, S_FATAL);
	}

	/* Copy the boot floppy. */
	sprintf(cmd, "/bin/cpdir -ad%s -smnt -sbegin -stmp %s / /mnt",
	(vflag) ? "v" : "",
	(update) ? "`/bin/sed -e \"s/^/-s/\" < /mnt/conf/upd_suppress`" : "");

	sys(cmd, S_FATAL);

	/* Copy idtune and idenable data files. */

	sprintf(cmd, "if test -f %s;then /bin/cp %s /mnt/conf;fi",
	IDCMDFILE, IDCMDFILE);
	sys(cmd, S_NONFATAL);

	sprintf(cmd, "if test -f %s;then /bin/cp %s /mnt/conf;fi",
	IDVARFILE, IDVARFILE);
	sys(cmd, S_NONFATAL);

	/* Copy patch file for debugging. */
	sprintf(cmd, "/bin/cp %s /mnt/conf", PATCHFILE);
	sys(cmd, S_FATAL);

	if (!is_dir("/mnt/mnt"))
		sys("/bin/mkdir /mnt/mnt", S_FATAL);
	sys("/bin/chmog 0755 bin bin /mnt/mnt", S_NONFATAL);

	if (!is_dir("/mnt/tmp"))
		sys("/bin/mkdir /mnt/tmp", S_FATAL);
	sys("/bin/chmog 0777 root root /mnt/tmp", S_NONFATAL);

	if (!is_dir("/mnt/tmp.pipe"))
		sys("/bin/mkdir /mnt/tmp.pipe", S_FATAL);
	sys("/bin/chmog 0777 root root /mnt/tmp.pipe", S_NONFATAL);

	/* If /etc/mkdev created devices in /tmp/dev, copy them to /dev. */
	/* Remove the copies in /tmp/dev on the hard disk. */
	if (exists("/tmp/dev"))
		sys("/bin/cpdir -d /tmp/dev /mnt/dev", S_FATAL);

	if (exists("/tmp/drvld.all")) {
		sys("/bin/cp /tmp/drvld.all /mnt/etc/drvld.all", S_NONFATAL);
		sys("/bin/chmog 0744 root root /mnt/etc/drvld.all", S_NONFATAL);
	}

	if (exists("/tmp/id-cmd")) {
		sys("/bin/cp /tmp/id-cmd /mnt/etc/conf/bin/id-cmd", S_NONFATAL);
		sys("/bin/chmog 544 root root /mnt/etc/conf/bin/id-cmd", S_NONFATAL);
	}

	/* Grow /lost+found to make room for files. */
	sys("cd /mnt/lost+found \n"
	    "/bin/touch `/bin/from 1 to 200` \n"
	    "/bin/rm *", S_IGNORE);

	/* Create /autoboot. */
	sys("/bin/ln -f /mnt/coherent /mnt/autoboot", S_FATAL);

	/* Replace the build version of /etc/brc with the install version. */
	sys("/bin/rm /mnt/etc/brc", S_NONFATAL);
	if (update)
		sys("/bin/ln -f /mnt/etc/brc.update /mnt/etc/brc", S_FATAL);
	else
		sys("/bin/ln -f /mnt/etc/brc.install /mnt/etc/brc", S_FATAL);

	/* Link root device to /dev/root. */
	sprintf(cmd, "/bin/ln -f /mnt%s /mnt/dev/root", devname(root, 0));
	sys(cmd, S_FATAL);

	if (!update) {
		/* Write the serial number to /etc/serialno. */
		sprintf(cmd, "/bin/echo %s >/mnt/etc/serialno", serialno);
		sys(cmd, S_NONFATAL);
	}

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

	sprintf(name, "/dev/%s", devices[i].d_dname);
	return (name);
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
"After you finish reading this information, remove the floppy disk.\n"
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
	} else {
		printf(
"You must boot the new COHERENT root filesystem on partition %d.\n",
		  root);
	}

	sync();
	printf(
"\nNow remove the floppy disk so your system does not boot from the floppy.\n");

	if (mboot && root != active)
		printf(
"You MUST type %d when the system tries to read the floppy disk during the boot\n"
"procedure to boot the partition containing the new COHERENT root filesystem.\n",
			root);
}

/*
 * See if the specified file exists and has non-zero length.
 */
exists_and_nz(fn)
char *fn;
{
	struct stat s;

	if (stat(fn, &s) == -1)
		return 0;			/* does not exist */
	return (s.st_size > 0);			/* exists and length > 0 */
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
	if (update) {
		printf("Reading existing partition tables...\n");
	} else {
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
"\n", needsize);
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
		if (yes_no("Do you want to install the COHERENT master boot"))
			++mboot;

retry:
	/* Construct an /etc/fdisk command with appropriate xdevice names. */
		strcpy(cmd, "/etc/fdisk -cB");
		if (mboot)
			strcat(cmd, "b /conf/mboot");


		for (i = 0; i < ndevices; i++) {
			if (i == 0
	 || strcmp(devices[i-1].d_xname, devices[i].d_xname) != 0) {
				if ((fd = open(xname(i, 1), 0)) < 0){
					continue;
				}
				close(fd);
				strcat(cmd, " ");
				strcat(cmd, xname(i, 1));
			}
		}
		sys(cmd, S_FATAL);		/* do the fdisk command */
	} /* !update */

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
	printf("\nYour system includes %d COHERENT partition%s:\n",
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
	if (!update && flag) {
		printf(
"\n"
"Your system includes a large COHERENT filesystem (larger than %d megabytes).\n"
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
		);
	if (!update)
		printf(
"The root filesystem contains the files normally used by COHERENT.\n"
"The root filesystem should contain at least %d megabytes.\n",
			needsize);
	if (ndevices > ATDEVS)
		printf(
"The COHERENT root filesystem must be on partition 0 through 7.\n"
			);
	if (active != -1 && isflag(active, F_COH)) {
		printf("COHERENT partition %d is marked as active in the partition table.\n",
			active);
		if (!update)
			printf(
"If you choose it as the root, you can boot COHERENT automatically.\n"
				);
	}
	printf("\n");
again:
	if (update)
		s = get_line("Which partition contains the COHERENT root filesystem?");
	else
		s = get_line("Which partition do you want to be the root filesystem?");
	root = *s - '0';
	if (*++s != '\0' || root < 0 || root >= ATDEVS || notflag(root, F_COH)) {
		printf("Enter a number between 0 and 7 which specifies a COHERENT partition.\n");
		goto again;
	}

	if (meg(devices[root].d_size) < (double)needsize) {
		if (update) {
			printf(
"Root partition has %d megabytes free - update needs about %d megabytes.\n",
	devices[root].d_size/2048, needsize);
			printf(
"\n"
"Your current COHERENT root filesystem may be too small to contain the\n"
"COHERENT 4.2.0 update along with the on-line manual pages and the on-line\n"
"dictionary.  If you wish to have the on-line dictionary installed, you\n"
"may need to do the following:\n\n"
"\t1) Exit from the update.\n"
"\t2) Boot from your existing COHERENT root partition.\n"
"\t3) Perform a full \"backup\" of ALL PARTITIONS on your hard disk\n"
"\t   include all of your existing programs, files, and any system\n"
"\t   files that you have modified using backup utilities \"cpio\",\n"
"\t   \"ustar\", or \"tar\".  If your hard disk includes partitions\n"
"\t   assigned to other operating systems, be sure to back these up also!\n"
"\t4) Perform a full installation of COHERENT 386 using the same disks\n"
"\t   supplied for the update.  Note that you will need to increase the\n"
"\t   size of the root partition, or you will need to select a different\n"
"\t   partition to contain the root filesystem.   Please refer to the\n"
"\t   chapter on installation found in the COHERENT 386 release notes.\n"
				);
			sync();
			if (yes_no("Do you wish to abort the update"))
				fatal("Insufficient disk space for update!");
		} else {
			printf("Partition %d contains only %.2f megabytes.\n",
				root, meg(devices[root].d_size));
			if (meg(devices[root].d_size) < (double)MINSIZE) {
				printf("It is too small to contain the COHERENT root filesystem.\n");
				goto again;
			}
			if (!yes_no("Are you sure you want it to be the root partition"))
				goto again;
		}
	}
	setflag(root, F_ROOT);
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
printf("controllers and disk drives.  Others use SCSI disk drives.\n");
printf("Please indicate the type(s) of disk drive(s) used in your computer system.\n");
printf("If you are uncertain of the type, please select choice 1.\n\n");
printf("Are you using:\n\n");
printf("1.  AT-compatible hard drive controller (IDE/RLL/MFM/ESDI).\n");
printf("2.  SCSI hard drive controller.\n");
printf("3.  Both.\n\n");
	hdc = get_int(1, 3, "Enter your choice:");

	/*
	 * Note: -b option to mkdev also OK for update
	 */
	sprintf(cmd, "/etc/mkdev -b%s%s %s %s",
		(dflag) ? "d" : "",
		(vflag) ? "v" : "",
		(hdc == 1 || hdc == 3) ? "at" : "",
		(hdc == 2 || hdc == 3) ? "scsi" : "");
	sys(cmd, S_NONFATAL);
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
		if (notflag(i, F_COH))
			continue;
		if(isflag(i, F_ATDEV) && notflag(i, F_SCAN))
			if(!yes_no("WARNING: AT partition not scanned - are you sure\n"
				   "you want to create a new filesystem on partition %d", i))
				continue;

		printf("\n");
		if (isflag(i, F_FS))
			printf("Partition %d (%s) already contains a COHERENT filesystem.\n",
				i, devname(i, 0));
		if (i == root)
			printf(
"\nWARNING!!!\n\n"
"The installation process expects a NEW file system in the root partition.\n"
"If you are trying to update an existing COHERENT partition, you must run\n"
"the COHERENT update.  If you are trying to install again after a partial\n"
"or failed installation, a new root file system must be created again now.\n\n"
			);
		name = devname(i, 1);
again:
		if (yes_no("Do you want to create a new COHERENT filesystem on partition %d", i)) {
			if(notflag(i, F_ATDEV) || isflag(i, F_ATDEV) && notflag(i, F_SCAN))
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
			printf("Making file system on %s - please wait...\n", name);
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
"cd /mnt/lost+found \n"
"/bin/touch `/bin/from 1 to 200` \n"
"/bin/rm *",
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
 * Place a bootstrap on the root filesystem.
 * This is only called from update.
 */
void
new_boot()
{
	sprintf(cmd, "/bin/cp /conf/boot %s", devname(root, 1));
	sys(cmd, S_FATAL);
}

/*
 * If PATCHFILE exists, execute it.
 */
void
patches()
{
	printf(
"\nThe kernel on your hard drive will now be patched to run on your system.\n");
	if (access(PATCHFILE, F_OK) == 0) {
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
	char line[120];

#if 0
	/*
	 * After fdisk() we know "root".  Write to PATCHFILE the patches
	 * that will be needed when a new kernel is linked at /mnt/coherent.
	 */
	sprintf(line, "dev_t\t\t%s = makedev(%d,%d);",
	  ROOTDEV, devices[root].d_major, devices[root].d_minor);
	cohtune_ent("cohmain", ROOTDEV, line);

	sprintf(line, "dev_t\t\t%s = makedev(%d,%d);",
	  PIPEDEV, devices[root].d_major, devices[root].d_minor);
	cohtune_ent("cohmain", PIPEDEV, line);
#endif

	/* For the second kernel. */
#if 1

	/* Use hard drive for pipedev on second kernel. */

	sprintf(cmd,
	  "echo /conf/patch -v /mnt/coherent ronflag=0 "
	  "\\\"%s\\=makedev\\(%d,%d\\)\\\" "
	  "\\\"%s\\=makedev\\(%d,%d\\)\\\" >> %s\n",
	  ROOTDEV, devices[root].d_major, devices[root].d_minor,
	  PIPEDEV, devices[root].d_major, devices[root].d_minor,
	  PATCHFILE);
#else

	/*
	 * Use ram1 for pipedev on second kernel as well as first.
	 * When using this code, /etc/brc.install and /etc/brc.update
	 * must mkfs and mount /dev/ram1 as a 384 block file sys.
	 */

	sprintf(cmd,
	  "echo /conf/patch -v /mnt/coherent ronflag=0 "
	  "\\\"%s\\=makedev\\(%d,%d\\)\\\" >> %s\n",
	  ROOTDEV, devices[root].d_major, devices[root].d_minor,
	  PATCHFILE);
#endif
	sys(cmd, S_FATAL);
}

/*
 * Need to mount root.
 * Need to set serialno.
 * Remove /usr/man/{ALL,COHERENT,MULTI}, /usr/man/man.tar*
 * Save off any special files before copying in the update
 * Need to umount root.
 */
void
save_files()
{
	char	buf[20];		/* more than enough for a serial # */
	int	fd;			/* file descriptor */
	int	i;
	char	*s;

	cls(1);
	printf("We will now mount your existing COHERENT root filesystem...\n");
	sprintf(cmd, "/etc/mount %s /mnt", devname(root, 1));
	sys(cmd, S_FATAL);

	if (!is_dir("/mnt/dev"))	/* sanity check! */
		fatal("Corrupt or incorrect root filesystem specified");
	/*
	 * Read in the old 286 serial number and save it off for later.
	 * If it doesn't exist on the disk, make the user re-enter
	 * the old serial number by hand.
	 */
	serialno[0] = '\0';
	buf[0] = '\0';
	if ((fd = open("/mnt/etc/serialno", 0)) >= 0) {
		read(fd, buf, sizeof(buf)-1);	/* ignore errors */
		close(fd);
		if (!isserial(buf))		/* sets serialno[] */
			serialno[0] = '\0';
	}
	if (serialno[0] == '\0') {
		printf(
"\nA card included with your original COHERENT distribution gives the\n"
"serial number of your copy of COHERENT.\n\n"
			);
		for (i = 1; i <= 3; i++) {
			s = get_line("Type in the serial number from the card:");
			if (isserial(s))
				break;
			if (i < 3)
				printf("Invalid serial number, please try again.\n");
			else
				fatal("invalid serial number");
		}
	}

	printf("\nRemoving old on-line COHERENT manual pages -- please wait...\n");
	sys("/bin/rm -rf /mnt/usr/man/ALL /mnt/usr/man/COHERENT "
	    "/mnt/usr/man/MULTI /mnt/usr/man/man.tar*", S_NONFATAL);
	sync();

	/*
	 * Save any special files just in case the user wants to
	 * get the old ones back...
	 *
	 * If old conf tree or id-* files exist, clean 'em up.
	 */
	sys("/etc/build.u /mnt", S_NONFATAL);

	sprintf(cmd, "rm -f /mnt/coherent.* /mnt/drv/*");  /* rm stale copies */
	sys(cmd, S_FATAL);

	sprintf(cmd, "/etc/umount %s", devname(root, 1));
	sys(cmd, S_FATAL);
}

/*
 * Configure user devices.
 * Assumes hard disk filesystem mounted on /mnt.
 * Write lines to /etc/mount.all to [u]mount the user devices.
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
			s = get_line("Where do you want to mount it?\n"
			"Type a directory name beginning with '/', "
			"such as \"/u\".\n");

			if (*s != '/') {
				printf("That name did not begin with '/'.\n");
				goto again;
			} else if (strcmp(s, "/usr") == 0) {
				printf(
"We do not recommend mounting over /usr.\n"
"If you want user home accounts in a separate partition, you can put\n"
"them all in /home, for example, and mount into that directory.\n");
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

			/* Add lines to /etc/mount.all. */
			if (status == 0)
				sprintf(cmd,
"/bin/echo /etc/mount /dev/%s %s >>/mnt/etc/mount.all", buf2, s);
			else
				sprintf(cmd,
"/bin/echo /etc/mount %s %s >>/mnt/etc/mount.all", name, s);
			sys(cmd, S_NONFATAL);

/* /etc/umount.all no longer depends on mount list. */
#if 0
			if (status == 0)
				sprintf(cmd, "/bin/echo /etc/umount /dev/%s >>/mnt/etc/umount.all",
					buf2);
			else
				sprintf(cmd, "/bin/echo /etc/umount %s >>/mnt/etc/umount.all",
					name);
			sys(cmd, S_NONFATAL);
#endif

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
		}
#if 0
/*
 * It is generally a nuisance to go checking file systems that the
 * customer doesn't even want mounted.  We will no longer do this.
 * hal - 93/11/02
 */
		else {
			/* Not mounted, check using standard name. */
			sprintf(cmd, "/bin/echo %s >>/mnt/etc/checklist", rname);
			sys(cmd, S_NONFATAL);
		}
#endif
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
 * Hi there.
 */
void
welcome()
{
	register char *s;
	int i;

	printf(
"\n\n\n\n\n\n\n\n\n\n\n\n"
"                              The COHERENT System\n\n"
"                Copyright (c) 1982, 1993 by Mark Williams Company\n\n"
"                     60 Revere Drive, Northbrook, IL  60062\n\n"
"                        708-291-6700, 708-291-6750 (FAX)\n"
"\n\n\n\n\n\n"
		);

	cls(1);
	printf(
"Welcome to the COHERENT operating system!\n\n"
"Your computer is now running COHERENT "
"386"
" from the floppy disk.\n");
	if (update)
		printf(
"This program will update your existing COHERENT 4.0.x system to COHERENT 4.2.0.\n"
"\n"
"Be sure to read the section on \"Updating\" in the COHERENT 386 Release\n"
"Notes prior to attempting this update!\n\n"
"Please be patient and read the instructions on the screen carefully.\n"
"\n"
			);
	else
		printf(
"This program will install COHERENT onto your hard disk.\n"
"\n"
"If you are already running COHERENT on your hard disk, you must perform an\n"
"update rather than a full installation.  To do so, please REBOOT NOW and\n"
"follow the detailed update instructions in the COHERENT release notes supplied\n"
"with this release.\n"
"\n"
"You can interrupt installation at any time by typing <Ctrl-C>;\n"
"then reboot and start the installation procedure again.\n"
"Please be patient and read the instructions on the screen carefully.\n"
"\n"
			);
	if (!update) {
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

	sprintf(xname, "/dev/%s", devices[i].d_xname);
	return (xname);
}

/* end of build.c */
