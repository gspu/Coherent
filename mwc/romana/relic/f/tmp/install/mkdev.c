/*
 * mkdev.c
 * 06/28/93
 *
 * Allow the user to configure devices requiring loadable drivers.
 * Uses common routines in build0.c: cc -o mkdev mkdev.c build0.c
 * Usage: mkdev [ -bdv ] { scsi | at | floppytape } ...
 * Options:
 *	-b	Use special processing when invoked from /etc/build
 *	-d	Debug; echo commands without executing
 *	-v	Verbose
 *
 * Roughly, do the following for device "foo" ( at | aha154x | ss )
 *
 *	cp /drv/foo /tmp/drv/foo
 *	make necessary patches "xxx" to /tmp/drv/foo
 *	if not "at" device
 *		make nodes (mknod -f) for the device
 *	if build mode (bflag)
 *		if rootdev
 *			append to LDKERFILE:
 *				HD=foo.a
 *				HDUNDEF="-u foocon_"
 *				HDPATCH="drvl_+xx0=foocon_"
 *		/etc/drvld -r /tmp/drv/foo
 *		append to PATCHFILE:
 *			cp /tmp/drv/foo /mnt/drv/foo
 *		if rootdev, also append to PATCHFILE:
 *			/conf/patch /mnt/coherent xxx
 *	else - not build mode
 *		display message saying patched driver is at /tmp/drv/foo
 */

#define	__KERNEL__	1

#include <stdio.h>
#include <sys/devices.h>
#include <sys/con.h>
#include <string.h>

#include "build0.h"

#define	VERSION		"V4"		/* version number */
#define	USAGEMSG	"Usage:\t/etc/mkdev [ -bdv ] [ scsi | at | floppytape ] ...\n"
#define BUFLEN		50
#define AHA_HDS		64
#define AHA_DMA		5
#define AHA_IRQ		11
#define AHA_BASE	0x330
#define TANDY_HDS	16

/*
 * calculate the minor number for the specified floppy tape device:
 *	uu:	unit # (0-3)
 *	vv:	brand of drive: 0=Archive/Mountain/Summit, 1=CMS, 2&3=rsvd
 *	s:	select: 0=hard select, 1=soft select
 *	r:	rewind: 0=no rewind on close, 1=rewind on close
 */
#define	FL_TAPE_MINOR(uu,vv,s,r)   ((1<<6)|((uu)<<4)|((s)<<3)|((r)<<2)|(vv))
#define	FL_TAPE_HARD_SEL	0
#define	FL_TAPE_SOFT_SEL	1
#define	FL_TAPE_NOREW		0
#define	FL_TAPE_REW		1

/* Forward. */
void	scsi();
void	at();
void	floppy_tape();

/* Globals. */
int	bflag;				/* Invoked from /etc/build. */

main(argc, argv) int argc; char *argv[];
{
	register char *s;

	argv0 = argv[0];
	usagemsg = USAGEMSG;
	if (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s; ++s) {
			switch(*s) {
			case 'b':	++bflag;	break;
			case 'd':	++dflag;	break;
			case 'v':	++vflag;	break;
			case 'V':
				fprintf(stderr, "mkdev: %s\n", VERSION);
				break;
			default:	usage();	break;
			}
		}
		--argc;
		++argv;
	}

	if (argc == 1) {
		usage();
	} else {
		/* Do specified things. */
		while (--argc > 0) {
			if (strcmp (argv [1], "scsi") == 0)
				scsi ();
			else if (strcmp (argv [1], "at") == 0)
				at ();
			else if (strcmp (argv [1], "floppytape") == 0)
				floppy_tape ();
			else
				usage ();
			++argv;
		}
	}
	exit (0);
}

void
scsi()
{
	char *dev;
	int i, id, lun;
	int ss_dev = 0;
	int fut_dev = 0;
	unsigned nsdrive = 0;
	int ss_int = 5, new_int;
	unsigned int ss_base = 0xCA00, new_base;
	unsigned char ss_patch[200], buf[BUFLEN];
	FILE *fp;
	int aha_dev = 0, sd_hds = AHA_HDS, sd_dma = AHA_DMA;
	int hai_dev = 0;
	int sd_irq = AHA_IRQ, sd_base = AHA_BASE;
	int hai_tape = 0;
	int hai_disk = 0;
	int unit;
	int tape_dev;
	int choice;

	static int sd_irq_list[] = { 9, 10, 11, 12, 14, 15 };
	int sd_irq_ct = sizeof(sd_irq_list)/sizeof(sd_irq_list[0]);

	static int sd_base_list[] =
		  { 0x130, 0x134, 0x230, 0x234, 0x330, 0x334 };
	int sd_base_ct = sizeof(sd_base_list)/sizeof(sd_base_list[0]);

	static int sd_dma_list[] = { 0, 5, 6, 7 };
	int sd_dma_ct = sizeof(sd_dma_list)/sizeof(sd_dma_list[0]);

	cls(0);
	printf(
"COHERENT currently supports the following SCSI host adapters:\n"
"\n"
"(1) Adaptec AHA-154x series, no tape support\n"
"(2) Adaptec AHA-154x series, with tape support\n\n"
"(3) Seagate ST01 or ST02\n"
"(4) Future Domain TMC-845/850/860/875/885\n"
"(5) Future Domain TMC-840/841/880/881\n"
"\n"
		);
retry:
	switch(get_int(0, 5, "Enter a number from the above list or 0 to exit:")) {
	case 0:
		return;
	case 1:
		aha_dev = 1;
		break;
	case 2:
		hai_dev = 1;
		break;
	case 3:
		ss_dev = 1;
		break;
	case 4:
		ss_dev = 1;
		fut_dev = 1;
		nsdrive |= 0x8000;
		break;
	case 5:
		ss_dev = 1;
		fut_dev = 1;
		nsdrive |= 0x4000;
		break;
	default:
		goto retry;		/* should never happen */
	}

	/*
	 * If Adaptec, allow patching host adapter variables SD_HDS
	 * for Tandy variant of host BIOS.
	 */
	if (aha_dev) {
printf("\nMost versions of the Adaptec BIOS use 64-head translation mode.\n");
printf("A few, including some Tandy variants, use 16-head translation mode.\n\n");
		if (!yes_no("Do you want 64-head translation mode"))
			sd_hds = TANDY_HDS;
	}

	if (aha_dev || hai_dev) {
		sd_irq = get_allowed_int(sd_irq_list, sd_irq_ct, ga_dec,
		  sd_irq, ga_nonstrict,
		  "\nWhich IRQ does the host adapter use? ");
		sd_base = get_allowed_int(sd_base_list, sd_base_ct, ga_hex,
		  sd_base, ga_nonstrict,
		  "\nWhat is the hexadecimal host adapter base port address? ");
		sd_dma = get_allowed_int(sd_dma_list, sd_dma_ct, ga_hex,
		  sd_dma, ga_nonstrict,
		  "\nWhich DMA channel does the host adapter use? ");
	}

	/*
	 * If Seagate or Future Domain, allow patching host adapter
	 * variables SS_INT and SS_BASE.
	 */
	if (ss_dev) {
printf("\nPlease refer to the installation guide for your host adapter.\n");

		/* Get value to patch for SS_INT */
printf("\nWhich IRQ number does the host adapter use [%d]? ", ss_int);
		while (1) {
			new_int = ss_int;
			fgets(buf, BUFLEN, stdin);
			sscanf(buf, "%d", &new_int);
			if (new_int < 3 || new_int > 15)
printf("Type a number between 3 and 15 or just <Enter> for the default: ");
			else
				break;
		} /* endwhile */
		ss_int = new_int;

		/* Get value to patch for SS_BASE */
printf("Your host adapter is configured for a base segment address.  Possible\n");
printf("values are: C800, CA00, CC00, CE00, DC00, and DE00.\n");
printf("What is your 4-digit hexadecimal base address [%04lx]? ", ss_base);
		while (1) {
			new_base = ss_base;
			fgets(buf, BUFLEN, stdin);
			sscanf(buf, "%x", &new_base);
			if (new_base < 0xC800 || new_base > 0xDE00)
printf("Type a number between C800 and DE00 or just <Enter> for the default: ");
			else
				break;
		} /* endwhile */
		ss_base = new_base;
	}
	
	/* Make device nodes. */
	cls(0);
	if (hai_dev) {
		printf(
"You must specify a SCSI-ID (0 through 7) for each SCSI hard disk or tape\n"
"device.  Each SCSI hard disk device can contain up to four partitions.\n"
"Tape devices must be configured as logical unit number (LUN) 0.  All current\n"
"SCSI tape drives with embedded SCSI controllers default to LUN 0.\n\n"
			);
	} else {
		printf(
"You must specify a SCSI-ID (0 through 7) for each SCSI hard disk device.\n"
"Each SCSI hard disk device can contain up to four partitions.\n\n"
			);
	}

for (;;) {
	tape_dev = 0;

	if (hai_dev) {
		printf(
"1) Enter SCSI ID for SCSI hard disk (fixed or removable media)\n"
"2) Enter SCSI ID for SCSI tape drive\n\n"
"3) End entering SCSI ID values.\n\n"
		);
		choice = get_int(1, 3, "Your choice:");
	} else {
		printf(
"1) Enter SCSI ID for SCSI hard disk (fixed or removable media)\n"
"2) End entering SCSI ID values.\n\n"
		);
		choice = get_int(1, 2, "Your choice:");
	}

	if (hai_dev) {
		if (choice == 2)
			tape_dev = 1;
		if (choice == 3)
			break;
	} else {
		if (choice == 2)
			break;
	}

	if (tape_dev) {
		id = get_int(0, 7, "\nEnter the next tape device SCSI-ID:");
		hai_tape |= (1 << id);
	} else {
		id = get_int(0, 7, "\nEnter the next disk device SCSI-ID:");
		hai_disk |= (1 << id);
	}

	lun = 0;
	nsdrive |= (1 << id);

	/* Make /tmp/dev if bflag. */
	if (bflag) {
		if ((i = is_dir("/tmp/dev")) == 0)
			sys("/bin/mkdir /tmp/dev", S_FATAL);
		else if (i == -1)
			fatal("/tmp/dev is not a directory");
	}
	dev = (bflag) ? "/tmp/dev" : "/dev";

	/*
	 * If we are creating a SCSI tape device (Adaptec only),
	 * create special raw device nodes and generate links
	 * for /dev/tape and /dev/ntape to make it easy on the user.
	 */
#if !TEST
	if (tape_dev) {
		sprintf(cmd, "/etc/mknod -f %s/rst%d c %d %d",
			dev, id, SCSI_MAJOR, SCSI_minor(1, id, lun, 3));
		sys(cmd, S_NONFATAL);
		sprintf(cmd, "/etc/mknod -f %s/rst%dn c %d %d",
			dev, id, SCSI_MAJOR, SCSI_minor(1, id, lun, 1));
		sys(cmd, S_NONFATAL);
		sprintf(cmd, "/bin/ln -f %s/rst%d %s/tape",
			dev, id, dev);
		sys(cmd, S_NONFATAL);
		sprintf(cmd, "/bin/ln -f %s/rst%dn %s/ntape",
			dev, id, dev);
		sys(cmd, S_NONFATAL);

		/* set the device permissions. */
		sprintf(cmd, "/bin/chmog 600 sys sys %s/rst%d*", dev, id);
		sys(cmd, S_NONFATAL);
	} else {
		/*
		 * SCSI disk device:
		 * Make the cooked devices.
		 */
		for (i = 0; i < 4; i++) {
			sprintf(cmd, "/etc/mknod -f %s/sd%d%c b %d %d",
				dev, id, 'a'+i, SCSI_MAJOR, SCSI_minor(0, id, lun, i));
			sys(cmd, S_NONFATAL);
		}
		sprintf(cmd, "/etc/mknod -f %s/sd%dx b %d %d",
			dev, id, SCSI_MAJOR, SCSI_minor(1, id, lun, 0));
		sys(cmd, S_NONFATAL);
		
		/* make the raw devices. */
		for (i = 0; i < 4; i++) {
			sprintf(cmd, "/etc/mknod -f %s/rsd%d%c c %d %d",
				dev, id, 'a'+i, SCSI_MAJOR, SCSI_minor(0, id, lun, i));
			sys(cmd, S_NONFATAL);
		}
		sprintf(cmd, "/etc/mknod -f %s/rsd%dx c %d %d",
			dev, id, SCSI_MAJOR, SCSI_minor(1, id, lun, 0));
		sys(cmd, S_NONFATAL);

		/* set the device permissions. */
		sprintf(cmd, "/bin/chmog 600 sys sys %s/sd*[a-d] %s/rsd*[a-d]",dev,dev);
		sys(cmd, S_NONFATAL);
		sprintf(cmd, "/bin/chmog 600 root root %s/sd*x %s/rsd*x", dev, dev);
		sys(cmd, S_NONFATAL);
	} /* !(tape_dev) */
	
	/* append lines to /tmp/devices to pass device info to /etc/build. */
	if (bflag && !tape_dev) {
		for (i = 0; i < 4; i++) {
			sprintf(cmd, "/bin/echo sd%dx sd%d%c %d %d >>/tmp/devices",
				id, id, 'a'+i, SCSI_MAJOR, SCSI_minor(0, id, lun, i));
			sys(cmd, S_NONFATAL);
		}
	}
#endif
	cls(0);
	printf("%s device has been installed at id %d\n\n",
	  tape_dev ? "Tape" : "Disk", id);
}

	/* At this point all SCSI id's attached to the host are known. */

	/*
	 * Ugly patching stuff specific to "ss" driver.
	 */
	if (ss_dev) {

		/* "ss" device driver requires patching to work at all. */
		sprintf (ss_patch,
			 "NSDRIVE=0x%04x SS_INT=%d SS_BASE=0x%04x",
			 nsdrive, ss_int, ss_base);

		/* for target kernel */

		idenable_dev ("ss");

		sprintf (buf, "unsigned int\tNSDRIVE = 0x%04x;", nsdrive);
		cohtune_ent ("ss", "NSDRIVE", buf);

		sprintf (buf, "unsigned int\tSS_INT = %d;", ss_int);
		cohtune_ent ("ss", "SS_INT", buf);

		sprintf (buf, "unsigned int\tSS_BASE = %d;", ss_base);
		cohtune_ent ("ss", "SS_BASE", buf);

		/* for second boot kernel */
		fp = fopen(PATCHFILE,"a");

		fprintf (fp, "/conf/patch /mnt/coherent drvl+%d=sscon \n",
			 SCSI_MAJOR * sizeof (DRV));

		fprintf (fp, "/conf/patch /mnt/coherent %s\n", ss_patch);

		fclose (fp);

		/* for first boot kernel */

		sprintf (cmd, "/conf/kpatch -v %s", ss_patch);
		sys (cmd, S_FATAL);
		sprintf (cmd, "/conf/kpatch -v sscon:%d:in", SCSI_MAJOR);
		sys (cmd, S_FATAL);

		/*
		 * Allow patching of the loaded driver parameters.
		 */
		for (unit = 0; unit < 7; unit++) {
			if (nsdrive & (1 << unit)) {
				sprintf (cmd, "/etc/hdparms -%c%c %s/sd%dx",
					 'r', fut_dev ? 'f' : 's', dev, unit);
				sys (cmd, S_NONFATAL);
			}
		}
	} /* end of "ss" stuff */

	/*
	 * Ugly patching stuff specific to "aha154x" driver.
	 * At this point all SCSI id's attached to the host are known.
	 */
	if (aha_dev) {
		/*
		 * Tandy Adaptec BIOS spoofs different head count than
		 * Adaptec's Own Translation Mode.
		 */

		sprintf (ss_patch, "SD_HDS=%d SDDMA=%d SDIRQ=%d SDBASE=0x%x ",
			 sd_hds, sd_dma, sd_irq, sd_base);

		/* for target kernel */

		idenable_dev ("aha");

		sprintf (buf, "int\tSD_HDS = %d;", sd_hds);
		cohtune_ent ("aha", "SD_HDS", buf);

		sprintf (buf, "int\tSDDMA = %d;", sd_dma);
		cohtune_ent ("aha", "SDDMA", buf);

		sprintf (buf, "int\tSDIRQ = %d;", sd_irq);
		cohtune_ent ("aha", "SDIRQ", buf);

		sprintf (buf, "int\tSDBASE = 0x%x;", sd_base);
		cohtune_ent ("aha", "SDBASE", buf);

		/* for second boot kernel */
		fp = fopen (PATCHFILE,"a");

		fprintf (fp, "/conf/patch /mnt/coherent drvl+%d=sdcon\n",
			 SCSI_MAJOR * sizeof (DRV));

		fprintf (fp, "/conf/patch /mnt/coherent %s\n", ss_patch);

		fclose (fp);

		/* for first boot kernel */

		sprintf (cmd, "/conf/kpatch -v %s", ss_patch);
		sys (cmd, S_FATAL);
		sprintf (cmd, "/conf/kpatch -v sdcon:%d:in", SCSI_MAJOR);
		sys (cmd, S_FATAL);
	}

	/*
	 * Ugly patching stuff specific to "hai" driver.
	 * At this point all SCSI id's attached to the host are known.
	 */
	if (hai_dev) {
		/*
		 * Tandy Adaptec BIOS spoofs different head count than
		 * Adaptec's Own Translation Mode.
		 */
		sprintf (ss_patch, "AHADMACHAN=%d:s AHAINTR=%d:s "
				   "AHABASE=0x%x:s HAI_DISK=%d HAI_TAPE=%d ",
			 sd_dma, sd_irq, sd_base, hai_disk, hai_tape);

		/* for target kernel */

		idenable_dev ("hai");

		sprintf (buf, "int\tAHADMACHAN = %d;", sd_dma);
		cohtune_ent ("hai", "AHADMACHAN", buf);

		sprintf (buf, "int\tAHAINTR = %d;", sd_irq);
		cohtune_ent ("hai", "AHAINTR", buf);

		sprintf (buf, "int\tAHABASE = %d;", sd_base);
		cohtune_ent ("hai", "AHABASE", buf);

		sprintf (buf, "int\tHAI_DISK = %d;", hai_disk);
		cohtune_ent ("hai", "HAI_DISK", buf);

		sprintf (buf, "int\tHAI_TAPE = %d;", hai_tape);
		cohtune_ent ("hai", "HAI_TAPE", buf);

		/* for second boot kernel */
		fp = fopen (PATCHFILE,"a");

		fprintf (fp, "/conf/patch /mnt/coherent drvl+%d=scsicon \n",
			 SCSI_MAJOR * sizeof (DRV));

		fprintf (fp, "/conf/patch /mnt/coherent %s\n", ss_patch);

		fclose (fp);

		/* for first boot kernel */

		sprintf (cmd, "/conf/kpatch -v %s", ss_patch);
		sys (cmd, S_FATAL);
		sprintf (cmd, "/conf/kpatch -v scsicon:%d:in", SCSI_MAJOR);
		sys (cmd, S_FATAL);

	} /* end of "hai" stuff */
}

void
at()
{
	unsigned char at_patch[80];
	FILE *fp;
	int at_poll;
	char line[512];
	int normal_at_poll;
	int fl_dsk_ch_prob;
	int use_ps1;
	int at_drive_ct;

	/* Normal vs. alternate polling. */
	cls(0);
	printf(
"Most AT-compatible controllers work with NORMAL polling.\n\n"
"Perstor controllers and some IDE hard drives require ALTERNATE polling.\n\n"
"If you get \"<Watchdog Timeout>\" or \"at0:TO\" errors with normal polling,\n"
"use alternate polling.\n\n");

	if (normal_at_poll = yes_no ("Use NORMAL polling")) {
		strcpy (at_patch, "ATSREG=0x3F6 ");
		at_poll = 0x3F6;
	} else {
		strcpy (at_patch, "ATSREG=0x1F7 ");
		at_poll = 0x1F7;
	}
	cls(0);
	printf("%s polling will be used.\n\n",
	  normal_at_poll ? "NORMAL" : "ALTERNATE");

	/* Normal vs. PS1 hard disk counts. */
	if (use_ps1 = yes_no(
	  "Are you installing on an IBM PS1 or ValuePoint?")) {
		at_drive_ct = get_int(1, 2,
		  "Enter number of non-SCSI hard drives:");
		fl_dsk_ch_prob = 1;
	} else {
		fl_dsk_ch_prob = 0;
	}

	/* for target kernel */

	idenable_dev ("at");
	sprintf (line, "int\tATSREG = 0x%x;", at_poll);
	cohtune_ent ("at", "ATSREG", line);
	if (use_ps1)
		idtune_var("AT_DRIVE_CT", at_drive_ct);
	idtune_var("FL_DSK_CH_PROB", fl_dsk_ch_prob);

	/* for second boot kernel */
	fp = fopen (PATCHFILE,"a");
	fprintf (fp, "/conf/patch /mnt/coherent drvl+%d=atcon \n",
		 AT_MAJOR * sizeof (DRV));
	fprintf (fp, "/conf/patch /mnt/coherent %s\n", at_patch);
	fprintf (fp, "/conf/patch /mnt/coherent fl_dsk_ch_prob=%d\n",
	  fl_dsk_ch_prob);
	if (use_ps1)
		fprintf (fp, "/conf/patch /mnt/coherent at_drive_ct=%d\n",
		  at_drive_ct);
	fclose (fp);

	/* for first boot kernel */

	sprintf (cmd, "/conf/kpatch -v %s", at_patch);
	sys (cmd, S_FATAL);
	sprintf (cmd, "/conf/kpatch -v fl_dsk_ch_prob=%d", fl_dsk_ch_prob);
	sys (cmd, S_FATAL);
	if (use_ps1) {
		sprintf (cmd, "/conf/kpatch -v at_drive_ct=%d", at_drive_ct);
		sys (cmd, S_FATAL);
	}
	/* For PS1, this step must come AFTER at_drive_ct is fixed. */
	sprintf (cmd, "/conf/kpatch -v atcon:%d:in", AT_MAJOR);
	sys (cmd, S_FATAL);
}

/*
 * floppy_tape:	add support for floppy tape (a really disgusting device)
 *
 *	1) give user some info regarding COH support for floppy tape
 *	2) display a list of supported drives
 *	3) query the user for type of drive
 *	4) create device nodes for specified drive
 *	5) (optional) test for which unit number the drive is seen as
 */
void
floppy_tape()
{
	int	brand;		/* which brand of drive (from menu) */
	int	i;		/* loop counter */

	cls(0);
	printf(
"The COHERENT system supports several brands of \"floppy tape backup\" drives,\n"
"including QIC-40 and QIC-80 models from Archive, Colorado Memory Systems (CMS),\n"
"Mountain, and Summit.\n\n"
"Please specify the brand of floppy tape drive on this computer:\n\n"
"\t0) Archive, Mountain, or Summit\n"
"\t1) Colorado Memory Systems (CMS)\n"
"\t2) Other\n\n"
	);
	brand = get_int(0, 2, "Enter drive type:");
	if (brand == 2) {
		printf(
"\nYou have specified a brand of tape drive which is currently unsupported.\n"
		);
		return;
	}
	
	/*
	 * now create the following nodes for the specified drive brand:
	 *	 Rewind		No Rewind	 Unit #
	 *	--------	---------	---------
	 *	DEV/rct0	DEV/nrct0	(unit #0)
	 *	DEV/rct1	DEV/nrct1	(unit #1)
	 *	DEV/rct2	DEV/nrct2	(unit #2)
	 *	DEV/rct3	DEV/nrct3	(unit #3)
	 *	DEV/rctss	DEV/nrctss	(soft select)
	 *
	 * where DEV is /mnt/dev if called from /etc/build and /dev otherwise.
	 */
	for (i = 0; i <= 3; ++i) {
		sprintf(cmd, "/etc/mknod -f %s/rct%d c %d %d",
			(bflag) ? "/mnt/dev" : "/dev", i, FL_MAJOR,
			FL_TAPE_MINOR(i, brand, FL_TAPE_HARD_SEL, FL_TAPE_REW));
		sys(cmd, S_NONFATAL);
		sprintf(cmd, "/etc/mknod -f %s/nrct%d c %d %d",
			(bflag) ? "/mnt/dev" : "/dev", i, FL_MAJOR,
			FL_TAPE_MINOR(i, brand, FL_TAPE_HARD_SEL, FL_TAPE_NOREW));
		sys(cmd, S_NONFATAL);
	}
	sprintf(cmd, "/etc/mknod -f %s/rctss c %d %d",
		(bflag) ? "/mnt/dev" : "/dev", FL_MAJOR,
		FL_TAPE_MINOR(0, brand, FL_TAPE_SOFT_SEL, FL_TAPE_REW));
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/etc/mknod -f %s/nrctss c %d %d",
		(bflag) ? "/mnt/dev" : "/dev", FL_MAJOR,
		FL_TAPE_MINOR(0, brand, FL_TAPE_SOFT_SEL, FL_TAPE_NOREW));
	sys(cmd, S_NONFATAL);
}

/* end of mkdev.c */
