/*
 * File:	main.c
 *
 * Purpose:	part of COHERENT kernel
 *
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 * $Log:	main.c,v $
 * Revision 1.4  92/01/22  09:48:40  bin
 * update by hal... post 321 beta
 * 
 * Revision 1.4  92/01/21  16:08:34  hal
 * Merged with 386 main.c.  Call read_cmos().
 * 
 * Revision 1.3  92/01/15  10:16:02  hal
 * Trivial banner change.
 * 
 * Revision 1.2	88/06/29  12:00:29 	src
 * Real/Protected mode status now printed during boot sequence.
 * Three part serial numbers now supported, moved to optional fourth line.
 * 
 * Revision 1.1	88/03/24  16:13:58	src
 * Initial revision
 * 
 * 87/04/09	Allan Cornish		/usr/src/sys/coh/main.c
 * Serial numbers changed to support group.
 *
 * 87/01/05	Allan Cornish		/usr/src/sys/coh/main.c
 * Copyright notice revised to include 1987.
 */

/*
 * Includes.
 */
#include <sys/coherent.h>
#include <sys/devices.h>
#include <sys/fdisk.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <sys/stat.h>
#include <sys/typed.h>
#include <sys/uproc.h>

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#ifndef VERSION		/* This should be specified at compile time */
#define VERSION	"..."
#endif
typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int read_cmos();

static void atcount();
static void rpdev();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
extern dev_t rootdev;
extern dev_t pipedev;
extern int ronflag;

short n_atdr;
char version[] = VERSION;
#ifdef _I386
char copyright[] = "\
Copyright 1982,1992 Mark Williams Company\n\
Copyright 1991 Inst. O'Donnell, Bievres, France\n";
#else
char copyright[] = "\
Copyright 1982,1992 Mark Williams Company\n";
#endif

#ifdef _I386
unsigned serialnum0 = 0;
unsigned serialnum1 = 0;
static long sntime  = 870409113L;
static long snm	    = 0;
static long snmcopy = 0;
#else
unsigned long	_entry = 0;		/* really the serial number */
unsigned long	__ = 0;			/* really the serial number also */
#endif

main()
{
	register SEG *sp;
#ifndef _I386
	extern int realmode;
#endif

	u.u_error = 0;
	bufinit();
	cltinit();
	pcsinit();
	seginit();
	atcount();
	rpdev();
	devinit();
#ifdef _I386
	printf("\nCOHERENT Version %s - 386 Mode (mem=%u Kbytes)\n",
		version, ctob(allocno())/1024);
#else
	printf("\Mark Williams COHERENT Version %s - %s Mode (mem=%u Kbytes)\n",
		version, (realmode ? "Real" : "Protected"), msize );
#endif
	printf(copyright);

#ifdef _I386
	if ( snm ) {
		printf("Serial Number %u", (unsigned) snm );
		if ( serialnum0 ) {
			printf(":%u", (unsigned) serialnum0 );
			if ( serialnum1 )
				printf(":%u", (unsigned) serialnum1 );
		}
		printf("\n");
	}

	/*
	 * Verify correct serial number
	 */
	if (snm != snmcopy)
		panic("Verification error - call Mark Williams Company at 1-800-MWC-1700\n");
#else
	if ( _entry ) {
		printf("Serial Number ");
		printf("%U\n", _entry);
	}

	/*
	 * Verify correct serial number
	 */
	if (_entry != __)
		panic("Verification error - call Mark Williams Company at 1-800-MARK-WMS\n");
#endif

	/*
	 * Turn on clock, mount root device, start off processes
	 * and return.
	 */
	batflag = 1;
#ifdef _I386
	iprocp = SELF;
	if (pfork()) {
		idle();
	} else {
		fsminit();
		eprocp = SELF;
		eveinit();
	}
#else
	if ((sp=salloc((fsize_t)UPASIZE, SFNCLR|SFNSWP)) == NULL)
		panic("Cannot allocate user area");
	if ((iprocp=process(idle))==NULL || (eprocp=process(NULL))==NULL)
		panic("Cannot create process");
	eveinit(sp);
	fsminit();
#endif
}

/*
 * atcount()
 *
 * Read CMOS and return 0,1, or 2 as number of installed "at" drives.
 */
static void atcount()
{
	int u;
	n_atdr = 0;

	/*
	 * Count nonzero drive types.
	 *
	 *	High nibble of CMOS 0x12 is drive 0's type.
	 *	Low  nibble of CMOS 0x12 is drive 1's type.
	 */
	u = read_cmos(0x12);
	if (u & 0x00F0)
		n_atdr++;
	if (u & 0x000F)
		n_atdr++;
}

/*
 * rpdev()
 *
 * If rootdev is zero, try to use data from tboot to set it.
 * If pipedev is zero, make it 0x883 if ronflag == 1, else make it rootdev.
 * Call rpdev() AFTER calling atcount().
 */
static void rpdev()
{
	FIFO *ffp;
	typed_space *tp;
	int found;
	extern typed_space boot_gift;
	unsigned root_ptn, root_drv, root_maj, root_min;

	if (rootdev == makedev(0,0)) {
		found = 0;
		if (F_NULL != (ffp = fifo_open(&boot_gift, 0))) {

			for (; !found && T_NULL != (tp = fifo_read(ffp)); ) {
				BIOS_ROOTDEV *brp = (BIOS_ROOTDEV *)tp->ts_data;
				if (T_BIOS_ROOTDEV == tp->ts_type) {
					found = 1;
					root_ptn = brp->rd_partition;
				}
			}
			fifo_close(ffp);

		}
		if (found) {
			/*
			 * root_drv = BIOS # of root drive
			 * root_ptn = partition # in range 0..3
			 * if root on second "at" device, add 4 to minor #
			 * if root on second scsi device, add 16 to minor #
			 */
			root_drv = root_ptn/NPARTN;
			root_ptn %= NPARTN;
			if (n_atdr > root_drv) {
				root_maj = AT_MAJOR;
				root_min = root_drv*NPARTN + root_ptn;
			} else { /* root on SCSI device */
				root_maj = SCSI_MAJOR;
				root_min = (root_drv-n_atdr)*16 + root_ptn;
			}
			rootdev = makedev(root_maj, root_min);
printf("rootdev=(%d,%d)\n", root_maj, root_min);
		}
	}
	if (pipedev == makedev(0,0)) {
		if (ronflag)
			pipedev = makedev(RM_MAJOR, 0x83);
		else
			pipedev = rootdev;
printf("pipedev=(%d,%d)\n", (pipedev>>8)&0xff, pipedev&0xff);
	}
}
