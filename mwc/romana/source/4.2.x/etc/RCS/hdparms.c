head	1.1;
access
	hal
	udo
	ljg
	jrd
	nigel
	fred;
symbols
	r12:1.1;
locks; strict;
comment	@ *@;


1.1
date	93.09.01.15.50.14;	author rcsadmin;	state Exp;
branches;
next	;


desc
@@


1.1
log
@COHERENT r12
@
text
@/*
 * File:	hdparms.c
 *
 * Last Hacked:	06/25/92
 *
 * Purpose:	display and modify hard drive parameters
 *
 *	Called by "mkdev" command or usable as stand-alone.
 *	Non-build version expects driver in /tmp/drv, which is where mkdev
 *	leaves it.
 *
 * Usage:	hdparms [-bfrs] devname ...
 *
 * Options:
 *	-b	Use special processing when invoked from /etc/build
 *	-f	Future Domain SCSI
 *	-r	Specified device controls root partition (implies -b)
 *	-s	Seagate SCSI
 */

/*
 * Includes.
 */
#define __KERNEL__ 1  /* to get buf.h via scsiwork.h */

#include <stdio.h>
#include <sys/fdisk.h>
#include <sys/hdioctl.h>
#include <sys/stat.h>
#include <sys/scsiwork.h>

#include "build0.h"

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define	USAGEMSG	"Usage:\t/etc/hdparms [ -bfrs ] device...\n"
#define OPENMODE	2	/* Default open mode: read/write. */
#define BUFLEN		40
#define DEV_SCSI_ID(dev)	((dev >> 4) & 0x0007)
#define	VERSION		"V2.0"		/* version number */

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
#if DEFAULT_PARMS
static void cam_parms();
static void fd_parms();
#endif
static void getuint();
static int hdparms();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
static int bflag;	/* 1 for call via /etc/build */
static int fflag;	/* 1 for Future Domain SCSI */
static int rflag;	/* 1 for rootdev */
static int sflag;	/* 1 for Seagate SCSI */

/*
 * main()
 */
main(argc, argv)
int argc;
char *argv[];
{
	unsigned char *s;
	int success = 1;

	argv0 = argv[0];
	usagemsg = USAGEMSG;
	if (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s; ++s) {
			switch(*s) {
			case 'b':
				++bflag;
				break;
			case 'f':
				++fflag;
				break;
			case 'r':
				++rflag;
				++bflag;
				break;
			case 's':
				++sflag;
				break;
			default:
				usage();
			}
		}
		--argc;
		++argv;
	}

	while (--argc > 0) {
		success &= hdparms(argv[1]);
		++argv;
	}

	/*
	 * Exit with nonzero value if any call to hdparms() failed.
	 */
	exit(success == 0);
}

/*
 * hdparms()
 *
 * Return 0 if error occurred, else 1.
 */
static int hdparms(devname)
unsigned char *devname;
{
	int fd;
	hdparm_t parms;
	int ret = 0;
	struct stat dvstat;
	int s_id;		/* SCSI id */
	uint pl;		/* offset of device entry in drv_parm_ */
	unsigned char ss_patch[60];
	unsigned char * drv = "/drv/ss";
	unsigned char cmd[80];
	FILE * fp;

	if ((fd = open(devname, OPENMODE)) < 0) {
		printf("Can't open %s.\n", devname);
		goto noclose_fd;
	}

	if (sflag || fflag)
		if (stat(devname, &dvstat) != 0) {
			printf("Can't stat %s.\n", devname);
			goto close_fd;
		} else {
			s_id = DEV_SCSI_ID(dvstat.st_rdev);
			pl = sizeof(_drv_parm_t) * s_id;
		}

	if (ioctl(fd, HDGETA, (char *)(&parms)) == -1)
		printf("Can't get parameters for %s.\n", devname);
	else {
		uint ncyls, nheads, nspt;
		unsigned long nsectors;

		ncyls = (parms.ncyl[1] << 8) | parms.ncyl[0];
		nheads = parms.nhead;
		nspt = parms.nspt;
		nsectors = (unsigned long)ncyls * (unsigned long)nheads * (unsigned long)nspt;

printf("\nHere are the current parameters for SCSI device %d:\n", s_id);
printf("Number of cylinders = %d\n", ncyls);
printf("Number of heads = %d\n", nheads);
printf("Number of sectors per track = %d\n", nspt);

printf(
"\nIf the values above do not agree with those used by your host adapter's BIOS"
"\nprogramming, you will not be able to boot COHERENT from this hard drive.\n");

		if (ncyls > 1024) {
printf(
"\nThis device has more than 1024 cylinders.  In order to use the entire drive"
"\nfrom COHERENT, and possibly to be compatible with other operating systems,"
"\nyou will need to enter a set of translation-mode parameters.  Enter the"
"\nparameters your BIOS uses.  You can accept the default values shown by"
"\npressing <Enter> at each prompt.\n\n");
#if DEFAULT_PARMS
			if (fflag)
				fd_parms(&ncyls, &nheads, &nspt);
			else
				cam_parms(&ncyls, &nheads, &nspt);
#endif				
		}

		if (yes_no("Do you want to modify drive parameters")) {
			getuint(&ncyls, "Number of cylinders");
			getuint(&nheads, "Number of heads");
			getuint(&nspt, "Number of sectors per track");

			parms.ncyl[1] = ncyls >> 8;
			parms.ncyl[0] = ncyls & 0xFF;
			parms.nhead = nheads;
			parms.nspt = nspt;

			if (ioctl(fd, HDSETA, (char *)(&parms)) == -1)
printf("Couldn't write new parameters for %s.\n", devname);
			else {
printf("New parameters written for %s.\n", devname);
				ret = 1;
			}

			/* Prepare to patch "ss" driver. */
			if (sflag || fflag) {
				sprintf(ss_patch,
					"drv_parm+%d=%d drv_parm+%d=0x%04x",
					pl, ncyls, pl+sizeof(int),
					(nspt<<8) + nheads);

				if (bflag) {

					/*
					 * Second kernel.
					 * Write PATCHFILE which is run by build.
					 */
					fp = fopen(PATCHFILE, "a");
fprintf(fp, "/conf/patch /mnt/coherent %s\n", ss_patch);
					fclose(fp);

					/* Third kernel. */
{
	char line[120];
	char tag[40];

	sprintf(line, "_TAG(SD%d)\t{ %d, %d, %d },",
	  s_id, ncyls, nspt, nheads);
	sprintf(tag, "SD%d", s_id);
	cohtune_ent("ss", tag, line);
}

				}
			}

		} else
			ret = 1;
	}

close_fd:
	close(fd);

noclose_fd:
	return ret;
}

/*
 * getuint()
 *
 * get unsigned integer value - display prompt with default value
 */
static void getuint(np, prompt)
uint * np;
unsigned char * prompt;
{
	unsigned char buf[BUFLEN];

	printf("%s [%d]: ", prompt, *np);
	fgets(buf, BUFLEN, stdin);
	sscanf(buf, "%d", np);
}

#if DEFAULT_PARMS
/*
 * cam_parms()
 *
 * Use CAM algorithm to compute new drive parameters which will keep
 * number of cylinders under 1024.
 */
static void cam_parms(p_ncyls, p_nheads, p_nspt)
uint * p_ncyls, * p_nheads, * p_nspt;
{
	unsigned long capacity, ncyls, nheads, nspt;
	unsigned long ntracks, nsph, nspc;

	capacity = (unsigned long)*p_ncyls * (unsigned long)*p_nheads * (unsigned long)*p_nspt;
	if (capacity == 0L)
		goto frotz;
	ncyls = 1024L;
	nspt = 62L;
	nsph = ncyls * nspt;
	nheads = capacity / nsph;

	if (capacity % nsph) {
		nheads++;
		ntracks = ncyls * nheads;
		nspt = capacity / ntracks;

		if (capacity % ntracks) {
			nspt++;
			nspc = nheads * nspt;
			ncyls = capacity / nspc;
		}
	}

	*p_ncyls = ncyls;
	*p_nheads = nheads;
	*p_nspt = nspt;
frotz:
	return;
}

/*
 * fd_parms()
 *
 * Use Future Domain algorithm to compute new drive parameters which will
 * keep number of cylinders under 1024.
 */
static void fd_parms(p_ncyls, p_nheads, p_nspt)
uint * p_ncyls, * p_nheads, * p_nspt;
{
	unsigned long capacity, ncyls, nheads, nspt;
	unsigned long ntracks, foo, nspc;

	capacity = (unsigned long)*p_ncyls *
	  (unsigned long)*p_nheads * (unsigned long)*p_nspt;
	if (capacity == 0L)
		goto frotz;
	nspt = 17;	/* first try 17 spt */
	while (1) {
		foo = capacity / 1024;
		nheads = (foo / nspt) + 1;
		nspc = nheads * nspt;
		ncyls = capacity / nspc;
		ntracks = nheads * ncyls;
		if (ntracks < 32768L)
			break;
		if (nspt == 17)
			nspt = 34;	/* after 17, try 34 spt */
		else if (nspt == 34)
			nspt = 63;	/* after 34, try 63 spt */
		else
			break;
	}

	*p_ncyls = ncyls;
	*p_nheads = nheads;
	*p_nspt = nspt;
frotz:
	return;
}
#endif
@
