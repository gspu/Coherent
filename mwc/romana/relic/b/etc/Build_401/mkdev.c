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

#include <stdio.h>
#include <sys/devices.h>
#include "build0.h"

#define	VERSION		"V3.1"		/* version number */
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
			if (streq(argv[1], "scsi"))
				scsi();
			else if (streq(argv[1], "at"))
				at();
			else if (streq(argv[1], "floppytape"))
				floppy_tape();
			else
				usage();
			++argv;
		}
	}
	exit(0);
}

void
scsi()
{
	char *dev, *coh;
	int i, id, lun;
	int ss_dev = 0;
	int fut_dev = 0;
	unsigned nsdrive = 0;
	int ss_int = 5, new_int;
	unsigned int ss_base = 0xCA00, new_base;
	unsigned char ss_patch[80], buf[BUFLEN];
	FILE *fp;
	int aha_dev = 0, sd_hds = AHA_HDS, sd_dma = AHA_DMA;
	int sd_irq = AHA_IRQ, sd_base = AHA_BASE;
	int istape;		/* is a SCSI tape device */
#if !_I386
	rootflag = 0;
#endif

#if	0
	/* For future use, not much use with only one supported host adapter. */
again:
#endif
	cls(0);
	printf(
"COHERENT currently supports the following SCSI host adapters:\n"
"\n"
"(1) Adaptec AHA-154x series\n"
"(2) Seagate ST01 or ST02\n"
"(3) Future Domain TMC-845/850/860/875/885\n"
"(4) Future Domain TMC-840/841/880/881\n"
"\n"
		);
retry:
	switch(get_int(0, 4, "Enter a number from the above list or 0 to exit:")) {
	case 0:
		return;
	case 1:
		aha_dev = 1;
		coh = "aha";
		break;
	case 2:
		ss_dev = 1;
		coh = "ss";
		break;
	case 3:
		ss_dev = 1;
		fut_dev = 1;
		coh = "ss";
		nsdrive |= 0x8000;
		break;
	case 4:
		ss_dev = 1;
		fut_dev = 1;
		coh = "ss";
		nsdrive |= 0x4000;
		break;
	default:
		goto retry;		/* should never happen */
	}

	/* Make /tmp/drv if not already there. */
#if !_I386
	if ((i = is_dir("/tmp/drv")) == 0)
		sys("/bin/mkdir /tmp/drv", S_FATAL);
	else if (i == -1)
		fatal("/tmp/drv is not a directory");
#endif

	/*
	 * If Adaptec, allow patching host adapter variables SD_HDS
	 * for Tandy variant of host BIOS.
	 */
	if (aha_dev) {
printf("\nMost versions of the Adaptec BIOS use 64-head translation mode.\n");
printf("A few, including some Tandy variants, use 16-head translation mode.\n\n");
		if (!yes_no("Do you want 64-head translation mode"))
			sd_hds = TANDY_HDS;
printf("\nWhich IRQ does the host adapter use (9/10/11/12/14/15) [%d]? ",
	sd_irq);
		for (;;) {
			new_int = sd_irq;
			fgets(buf, BUFLEN, stdin);
			sscanf(buf, "%d", &new_int);
			switch(new_int) {
			case 9:
			case 10:
			case 11:
			case 12:
			case 14:
			case 15:
				goto ok_sdirq;
			}
printf("Type 9,10,11,12,14,15 or just <Enter> for the default: ");
		} /* endwhile */
ok_sdirq:
		sd_irq = new_int;

printf("\nWhat is the hexadecimal host adapter base port address \n");
printf("  (130/134/230/234/330/334) [%x]? ", sd_base);
		for (;;) {
			new_int = sd_base;
			fgets(buf, BUFLEN, stdin);
			sscanf(buf, "%x", &new_int);
			switch(new_int) {
			case 0x130:
			case 0x134:
			case 0x230:
			case 0x234:
			case 0x330:
			case 0x334:
				goto ok_sdbase;
			}
printf("Type 130,134,230,234,330,334 or just <Enter> for the default: ");
		} /* endfor */
ok_sdbase:
		sd_base = new_int;

printf("\nWhich DMA channel does the host adapter use (0/5/6/7) [%d]? ",
	sd_dma);
		for (;;) {
			new_int = sd_dma;
			fgets(buf, BUFLEN, stdin);
			sscanf(buf, "%d", &new_int);
			if (new_int != 0 && new_int != 5
			&& new_int != 6 && new_int != 7)
printf("Type 0,5,6,7 or just <Enter> for the default: ");
			else
				break;
		} /* endwhile */
		sd_dma = new_int;
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
	
	/*
	 * Set rootflag if root partition is on SCSI device (286).
	 * If using both AT and SCSI devices on COH 3.x, write to
	 * drvld.all for non-root device.
	 */
#if !_I386
	if (bflag) {
		cls(0);
		if (yes_no(
"Does your computer system include both a standard AT-type hard disk\n"
"and a SCSI hard disk" )) {
			if (yes_no(
"Will the COHERENT root partition be on this SCSI device")) {
				++rootflag;
sys("/bin/echo /etc/drvld -r /drv/at >> /tmp/drvld.all", S_FATAL);
			} else {	/* root is on "at" device */
sprintf(cmd, "/bin/echo /etc/drvld -r /drv/%s >> /tmp/drvld.all",
(aha_dev)?"aha154x":"ss");
				sys(cmd, S_FATAL);
			}
		} else {	/* SCSI-only system */
			++rootflag;
		}
	}
#endif

	/* Make device nodes. */
	cls(0);
	if (aha_dev) {
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

newdev:
	id = get_int(0, 7, "Enter the SCSI-ID:");

	istape = 0;
	if (aha_dev) {
		printf(
"\nPlease select the type of device from the following list:\n\n"
"\t0) SCSI hard disk (fixed or removable media)\n"
"\t1) SCSI tape drive\n"
"\t2) Other SCSI peripheral\n\n"
			);
		istape = get_int(0, 2, "Enter device type:");
		if (istape == 2) {
			printf(
"\nYou have specified a SCSI device which is currently unsupported.\n\n"
				);
			goto query_more_devices;
		}
	} /* aha_dev */
	lun = 0;
	nsdrive |= (1 << id);
#if 0
	lun = get_int(0, 3, "Enter the LUN:");
#endif

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
	if (aha_dev && istape) {
		sprintf(cmd, "/etc/mknod -f %s/rsd%d c %d %d",
			dev, id, SCSI_MAJOR, SCSI_minor(1, id, lun, 3));
		sys(cmd, S_NONFATAL);
		sprintf(cmd, "/etc/mknod -f %s/rsd%dn c %d %d",
			dev, id, SCSI_MAJOR, SCSI_minor(1, id, lun, 1));
		sys(cmd, S_NONFATAL);
		sprintf(cmd, "/bin/ln -f %s/rsd%d %s/tape",
			dev, id, dev);
		sys(cmd, S_NONFATAL);
		sprintf(cmd, "/bin/ln -f %s/rsd%dn %s/ntape",
			dev, id, dev);
		sys(cmd, S_NONFATAL);

		/* set the device permissions. */
		sprintf(cmd, "/bin/chmog 600 sys sys %s/rsd%d*", dev, id);
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
	} /* !(aha_dev && istape) */
	
	/* append lines to /tmp/devices to pass device info to /etc/build. */
	if (bflag && !istape) {
		for (i = 0; i < 4; i++) {
			sprintf(cmd, "/bin/echo sd%dx sd%d%c %d %d >>/tmp/devices",
				id, id, 'a'+i, SCSI_MAJOR, SCSI_minor(0, id, lun, i));
			sys(cmd, S_NONFATAL);
		}
	}

query_more_devices:
	if (yes_no((aha_dev) ?
"Do you have another SCSI disk or tape device on this host adapter" :
"Do you have another SCSI hard disk device on this host adapter"))
		goto newdev;

	/*
	 * Ugly patching stuff specific to "ss" driver.
	 * At this point all SCSI id's attached to the host are known.
	 */
	if (ss_dev) {
		int unit;

		/* "ss" device driver requires patching to work at all. */
#if _I386
		sprintf(ss_patch,
			"NSDRIVE=0x%04x SS_INT=%d SS_BASE=0x%04x",
			nsdrive, ss_int, ss_base);
		sprintf(cmd, "/conf/patch -K /begin %s", ss_patch);
#else
		sprintf(ss_patch,
			"NSDRIVE_=0x%04x SS_INT_=%d SS_BASE_=0x%04x",
			nsdrive, ss_int, ss_base);
		sys("/bin/cp /drv/ss /tmp/drv/ss", S_FATAL);
		sprintf(cmd, "/conf/patch /tmp/drv/ss %s", ss_patch);
#endif
		sys(cmd, S_FATAL);
		if (bflag) {
#if _I386
			fp = fopen(LDKERFILE, "a");
			fprintf(fp, "HD=\"${HD} ss.a\"\n");
			fprintf(fp, "HDUNDEF=\"${HDUNDEF} -u sscon\"\n");
			fprintf(fp, "HDPATCH=\"${HDPATCH} drvl+260=sscon \n");
			fclose(fp);
			fp = fopen(PATCHFILE,"a");
			fprintf(fp, "/conf/patch /mnt/coherent drvl+260=sscon \n");
			fclose(fp);
			sys("/conf/start_dev ss", S_FATAL);
#else /* 286 */
			if (rootflag) {
				fp = fopen(LDKERFILE, "a");
				fprintf(fp, "HD=ss.a\n");
				fprintf(fp, "HDUNDEF=\"-u sscon_\"\n");
				fprintf(fp, "HDPATCH=\"drvl_+130=sscon_\"\n");
				fclose(fp);
			}
			sys("/etc/drvld -r /tmp/drv/ss", S_FATAL);
#endif
			fp = fopen(PATCHFILE, "a");
#if !_I386
			fprintf(fp, "/conf/patch /mnt/drv/ss %s\n", ss_patch);
			if (rootflag)
#endif
				fprintf(fp,
				  "/conf/patch /mnt/coherent %s\n", ss_patch);
			fclose(fp);
			/*
			 * Allow patching of the loaded driver parameters.
			 */
			for (unit = 0; unit < 7; unit++)
				if (nsdrive & (1<<unit)) {
					sprintf(cmd,
					  "/etc/hdparms -%c%c %s/sd%dx",
#if _I386
					  'r',
#else
					  (rootflag)?'r':'b',
#endif
					  (fut_dev)?'f':'s', dev, unit);
					sys(cmd, S_NONFATAL);
				}
		} else
#if _I386
			printf("Patched in-core copy of ss driver\n");
#else
			printf("Patched driver at /tmp/drv/ss\n");
#endif

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
#if _I386
		sprintf(ss_patch,
		  "SD_HDS=%d SDDMA=%d SDIRQ=%d SDBASE=0x%x ",
		  sd_hds, sd_dma, sd_irq, sd_base);
		sprintf(cmd, "/conf/patch -K /begin %s", ss_patch);
#else */ 286 */
		sprintf(ss_patch,
		  "SD_HDS_=%d SDDMA_=%d SDIRQ_=%d SDBASE_=0x%x ",
		  sd_hds, sd_dma, sd_irq, sd_base);
		sys("/bin/cp /drv/aha154x /tmp/drv/aha154x", S_FATAL);
		sprintf(cmd, "/conf/patch /tmp/drv/aha154x %s", ss_patch);
		sys(cmd, S_FATAL);
#endif
		if (bflag) {
#if _I386
			fp = fopen(LDKERFILE, "a");
			fprintf(fp, "HD=\"${HD} aha154x.a\"\n");
			fprintf(fp, "HDUNDEF=\"${HDUNDEF} -u sdcon\"\n");
			fprintf(fp, "HDPATCH=\"${HDPATCH} drvl+260=sdcon\"\n");
			fclose(fp);
			fp = fopen(PATCHFILE,"a");
			fprintf(fp, "/conf/patch /mnt/coherent drvl+260=sdcon \n");
			fclose(fp);
			sys("/conf/start_dev aha", S_FATAL);
#else /* 286 */
			if (rootflag) {
				fp = fopen(LDKERFILE, "a");
				fprintf(fp, "HD=aha154x.a\n");
				fprintf(fp, "HDUNDEF=\"-u sdcon_\"\n");
				fprintf(fp, "HDPATCH=\"drvl_+130=sdcon_\"\n");
				fclose(fp);
			}
			sys("/etc/drvld -r /tmp/drv/aha154x", S_FATAL);
#endif
			fp = fopen(PATCHFILE, "a");
#if !_I386
			fprintf(fp, "/conf/patch /mnt/drv/aha154x %s\n",
			  ss_patch);
			if (rootflag)
#endif
				fprintf(fp,
				  "/conf/patch /mnt/coherent %s\n", ss_patch);
			fclose(fp);
		} else
#if _I386
			printf("Patched in-core aha154x driver\n");
#else
			printf("Patched driver at /tmp/drv/aha154x\n");
#endif
	} /* end of "aha154x" stuff */

#if	0
	if (yes_no("Is there another SCSI host adapter in your system"))
		goto again;
#endif
}

void
at()
{
	unsigned char at_patch[80];
	FILE *fp;
#if !_I386
	int i;
	int rootflag = 0;
#endif

	/* Make /tmp/drv if not there. COH 286 only */
#if !_I386
	if ((i = is_dir("/tmp/drv")) == 0)
		sys("/bin/mkdir /tmp/drv", S_FATAL);
	else if (i == -1)
		fatal("/tmp/drv is not a directory");
	sys("/bin/cp /drv/at /tmp/drv/at", S_FATAL);
#endif

	cls(0);
	printf(
"Most AT-compatible controllers work with NORMAL polling.\n\n"
"Perstor controllers and some IDE hard drives require ALTERNATE polling.\n\n"
"If you get \"<Watchdog Timeout>\" or \"at0:TO\" errors with normal polling,\n"
"use alternate polling.\n\n");

	if (yes_no("Use ALTERNATE polling"))
#if _I386
		strcpy(at_patch, "ATSREG=0x1F7 ");
	else
		strcpy(at_patch, "ATSREG=0x3F6 ");
	sprintf(cmd, "/conf/patch -K /begin %s", at_patch);
#else
		strcpy(at_patch, "ATSREG_=0x1F7 ");
	else
		strcpy(at_patch, "ATSREG_=0x3F6 ");
	sprintf(cmd, "/conf/patch /tmp/drv/at %s", at_patch);
#endif
	sys(cmd, S_FATAL);

	if (bflag){
#if !_I386
		if (yes_no(
"Will the COHERENT root partition be on an AT-type (non-SCSI) drive")) {
			++rootflag;
			fp = fopen(LDKERFILE, "a");
			fprintf(fp, "HD=at.a\n");
			fprintf(fp, "HDUNDEF=\"-u atcon_\"\n");
			fprintf(fp, "HDPATCH=\"drvl_+110=atcon_\"\n");
			fclose(fp);
		}
		sys("/etc/drvld -r /tmp/drv/at", S_FATAL);
#else /* 386 */
		fp = fopen(LDKERFILE, "a");
		fprintf(fp, "HD=\"${HD} at.a\"\n");
		fprintf(fp, "HDUNDEF=\"${HDUNDEF} -u atcon\"\n");
		fprintf(fp, "HDPATCH=\"${HDPATCH} drvl+220=atcon\"\n");
		fclose(fp);
		fp = fopen(PATCHFILE,"a");
		fprintf(fp, "/conf/patch /mnt/coherent drvl+220=atcon \n");
		fclose(fp);
		sys("/conf/start_dev at", S_FATAL);
#endif
		fp = fopen(PATCHFILE, "a");
#if !_I386
		fprintf(fp, "/conf/patch /mnt/drv/at %s\n", at_patch);
		if (rootflag)
#endif
			fprintf(fp,
			  "/conf/patch /mnt/coherent %s\n", at_patch);
		fclose(fp);
	} else
#if _I386
		printf("Patched in-core AT driver\n");
#else
		printf("Patched driver at /tmp/drv/at\n");
#endif
}

/*
 * floppy_tape:	add support for floppy tape (a really discusting device)
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
