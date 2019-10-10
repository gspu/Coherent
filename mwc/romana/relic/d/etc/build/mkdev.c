/*
 * mkdev.c
 * 10/25/90
 * Allow the user to configure devices requiring loadable drivers.
 * Uses common routines in build0.c: cc -o mkdev mkdev.c build0.c
 * Usage: mkdev [ -bdv ] { scsi | at }
 * Options:
 *	-b	Use special processing when invoked from /etc/build
 *	-d	Debug; echo commands without executing
 *	-v	Verbose
 *
 * $Log:	mkdev.c,v $
 * Revision 1.6  92/01/17  11:31:17  bin
 * another hal update... looks like the final 321 ship version
 * 
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

#define	VERSION		"V1.4"		/* version number */
#define	USAGEMSG	"Usage:\t/etc/mkdev [ -bdv ] [ scsi | at ]\n"
#define BUFLEN		50
#define AHA_HDS		64
#define AHA_DMA		5
#define AHA_IRQ		11
#define AHA_BASE	0x330
#define TANDY_HDS	16

/* Forward. */
void	scsi();
void	at();

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
			if (strcmp(argv[1], "scsi") == 0)
				scsi();
			else if (strcmp(argv[1], "at") == 0)
				at();
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
	int i, id, lun, rootflag;
	int ss_dev = 0;
	int fut_dev = 0;
	short nsdrive = 0;
	int ss_int = 5, new_int;
	unsigned int ss_base = 0xCA00, new_base;
	unsigned char ss_patch[80], buf[BUFLEN];
	FILE *fp;
	int aha_dev = 0, sd_hds = AHA_HDS, sd_dma = AHA_DMA;
	int sd_irq = AHA_IRQ, sd_base = AHA_BASE;

	rootflag = 0;
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
	if ((i = is_dir("/tmp/drv")) == 0)
		sys("/bin/mkdir /tmp/drv", S_FATAL);
	else if (i == -1)
		fatal("/tmp/drv is not a directory");

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
printf("What is your 4-digit hexadecimal base address [%04X]? ", ss_base);
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
	 * Set rootflag if root partition is on SCSI device.
	 * If using both AT and SCSI devices, write to drvld.all for
	 * non-root device.
	 */
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

	/* Make device nodes. */
	cls(0);
	printf(
"You must specify a SCSI-ID (0 through 7) for each SCSI hard disk device.\n"
"Each SCSI hard disk device can contain up to four partitions.\n\n"
		);

newdev:
	id = get_int(0, 7, "Enter the SCSI-ID:");
#if	1
	lun = 0;
	nsdrive |= (1 << id);
#else
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

	/* Make the cooked devices. */
	for (i = 0; i < 4; i++) {
		sprintf(cmd, "/etc/mknod -f %s/sd%d%c b %d %d",
			dev, id, 'a'+i, SCSI_MAJOR, SCSI_minor(0, id, lun, i));
		sys(cmd, S_NONFATAL);
	}
	sprintf(cmd, "/etc/mknod -f %s/sd%dx b %d %d",
		dev, id, SCSI_MAJOR, SCSI_minor(1, id, lun, 0));
	sys(cmd, S_NONFATAL);

	/* Make the raw devices. */
	for (i = 0; i < 4; i++) {
		sprintf(cmd, "/etc/mknod -f %s/rsd%d%c c %d %d",
			dev, id, 'a'+i, SCSI_MAJOR, SCSI_minor(0, id, lun, i));
		sys(cmd, S_NONFATAL);
	}
	sprintf(cmd, "/etc/mknod -f %s/rsd%dx c %d %d",
		dev, id, SCSI_MAJOR, SCSI_minor(1, id, lun, 0));
	sys(cmd, S_NONFATAL);

	/* Set the device permissions. */
	sprintf(cmd, "/bin/chmod 0600 %s/sd* %s/rsd*", dev, dev);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/bin/chown sys %s/sd*[a-d] %s/rsd*[a-d]", dev, dev);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/bin/chgrp sys %s/sd*[a-d] %s/rsd*[a-d]", dev, dev);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/bin/chown root %s/sd*x %s/rsd*x", dev, dev);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/bin/chgrp root %s/sd*x %s/rsd*x", dev, dev);
	sys(cmd, S_NONFATAL);

	/* Append lines to /tmp/devices to pass device info to /etc/build. */
	if (bflag) {
		for (i = 0; i < 4; i++) {
			sprintf(cmd, "/bin/echo sd%dx sd%d%c %d %d >>/tmp/devices",
				id, id, 'a'+i, SCSI_MAJOR, SCSI_minor(0, id, lun, i));
			sys(cmd, S_NONFATAL);
		}
	}
	if (yes_no("Do you have another SCSI hard disk device on this host adapter"))
		goto newdev;

	/*
	 * Ugly patching stuff specific to "ss" driver.
	 * At this point all SCSI id's attached to the host are known.
	 */
	if (ss_dev) {
		int unit;

		/* "ss" device driver requires patching to work at all. */
		sprintf(ss_patch,
			"NSDRIVE_=0x%04x SS_INT_=%d SS_BASE_=0x%04x",
			nsdrive, ss_int, ss_base);
		sys("/bin/cp /drv/ss /tmp/drv/ss", S_FATAL);
		sprintf(cmd, "/conf/patch /tmp/drv/ss %s", ss_patch);
		sys(cmd, S_FATAL);
		if (bflag) {
			if (rootflag) {
				fp = fopen(LDKERFILE, "a");
				fprintf(fp, "HD=ss.a\n");
				fprintf(fp, "HDUNDEF=\"-u sscon_\"\n");
				fprintf(fp, "HDPATCH=\"drvl_+130=sscon_\"\n");
				fclose(fp);
			}
			sys("/etc/drvld -r /tmp/drv/ss", S_FATAL);
			fp = fopen(PATCHFILE, "a");
			fprintf(fp, "/conf/patch /mnt/drv/ss %s\n", ss_patch);
			if (rootflag)
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
					  (rootflag)?'r':'b',
					  (fut_dev)?'f':'s', dev, unit);
					sys(cmd, S_NONFATAL);
				}
		} else
			printf("Patched driver at /tmp/drv/ss\n");

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
		sprintf(ss_patch,
		  "SD_HDS_=%d SDDMA_=%d SDIRQ_=%d SDBASE_=0x%x ",
		  sd_hds, sd_dma, sd_irq, sd_base);
		sys("/bin/cp /drv/aha154x /tmp/drv/aha154x", S_FATAL);
		sprintf(cmd, "/conf/patch /tmp/drv/aha154x %s", ss_patch);
		sys(cmd, S_FATAL);
		if (bflag) {
			if (rootflag) {
				fp = fopen(LDKERFILE, "a");
				fprintf(fp, "HD=aha154x.a\n");
				fprintf(fp, "HDUNDEF=\"-u sdcon_\"\n");
				fprintf(fp, "HDPATCH=\"drvl_+130=sdcon_\"\n");
				fclose(fp);
			}
			sys("/etc/drvld -r /tmp/drv/aha154x", S_FATAL);
			fp = fopen(PATCHFILE, "a");
			fprintf(fp, "/conf/patch /mnt/drv/aha154x %s\n",
			  ss_patch);
			if (rootflag)
				fprintf(fp,
				  "/conf/patch /mnt/coherent %s\n", ss_patch);
			fclose(fp);
		} else
			printf("Patched driver at /tmp/drv/aha154x\n");
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
	int rootflag = 0;
	int i;

	/* Make /tmp/drv if not there. */
	if ((i = is_dir("/tmp/drv")) == 0)
		sys("/bin/mkdir /tmp/drv", S_FATAL);
	else if (i == -1)
		fatal("/tmp/drv is not a directory");

	cls(0);
	sys("/bin/cp /drv/at /tmp/drv/at", S_FATAL);
	printf(
"Most AT-compatible controllers work with NORMAL polling.\n\n"
"Perstor controllers and some IDE hard drives require ALTERNATE polling.\n\n"
"If you get \"<Watchdog Timeout>\" or \"at0:TO\" errors with normal polling,\n"
"use alternate polling.\n\n");

	if (yes_no("Use ALTERNATE polling"))
		strcpy(at_patch, "ATSREG_=0x1F7 ");
	else
		strcpy(at_patch, "ATSREG_=0x3F6 ");
	sprintf(cmd, "/conf/patch /tmp/drv/at %s", at_patch);
	sys(cmd, S_FATAL);

	if (bflag){
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
		fp = fopen(PATCHFILE, "a");
		fprintf(fp, "/conf/patch /mnt/drv/at %s\n", at_patch);
		if (rootflag)
			fprintf(fp,
			  "/conf/patch /mnt/coherent %s\n", at_patch);
		fclose(fp);
	} else
		printf("Patched driver at /tmp/drv/at\n");
}
/* end of mkdev.c */
