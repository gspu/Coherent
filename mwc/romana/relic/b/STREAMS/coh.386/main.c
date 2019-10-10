/*
 * File:	main.c
 */

/*
 * Includes.
 */
#include <kernel/param.h>
#include <kernel/typed.h>

#include <sys/coherent.h>
#include <sys/devices.h>
#include <sys/fdisk.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <sys/stat.h>

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
#ifndef RELEASE
#define RELEASE "0"
#endif

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int read_cmos();

static void set_at_drive_ct();
static void rpdev();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * Configurable variables - see ker/conf/cohmain/Space.c
 *
 * at_drive_ct must be initialized at the number of "at" drives for PS1
 * and ValuePoint systems as the number of hard drives is not in CMOS.
 */
extern short at_drive_ct;

extern dev_t rootdev;
extern dev_t pipedev;
extern int ronflag;
extern int PHYS_MEM;
extern unsigned long _entry;		/* really the serial number */
extern unsigned long _bar;		/* really the serial number also */

/* End of configurable variables. */

char version[] = VERSION;
char release[] = RELEASE;
char copyright[] = "Copyright 1982,1993 Mark Williams Company\n";

#ifdef	TRACER

/*
 * Take a checksum of the kernel text.
 */
static unsigned		theSum;

static unsigned
checkSum ()
{
	extern	char	stext [], __end_text [];
	char	      *	tmp = stext;
	unsigned	sum = 0;
	while (tmp != __end_text)
		sum = sum * 5 + * tmp ++;
	return sum;
}

/*
 * Check the current kernel text segment against the boot checksum
 */

int
checkTheSum ()
{
	unsigned	newSum;
	
	if ((newSum = checkSum ()) != theSum)
		panic ("Kernel checksum failure, %x vs %x", theSum, newSum);
	return 0;
}
#endif	/* defined (TRACER) */


main()
{
	extern int BPFMAX;
	char * ndpTypeName();
	extern int (*ndpEmFn)();
	extern short ndpType;

#ifdef	TRACER
	theSum = checkSum ();
#endif
	
	CHIRP('a');
	while (& u + 1 < & u) {
		_CHIRP ('t', 8);
		_CHIRP ('k', 6);
		_CHIRP ('u', 4);
		_CHIRP ('h', 2);
		_CHIRP ('p', 0);
	}

#if	_I386
	wrNdpUser(0);
	wrNdpSaved(0);
	u.u_bpfmax = BPFMAX;
#endif
	u.u_error = 0;
	bufinit();
	_CHIRP('0', 156);
	cltinit();
	_CHIRP('1', 156);
	pcsinit();
	_CHIRP('2', 156);
	seginit();
	_CHIRP('3', 156);
	set_at_drive_ct();
	_CHIRP('4', 156);
	rpdev();
	_CHIRP('5', 156);
	devinit();
	_CHIRP('6', 156);
	rlinit();
	_CHIRP('7', 156);

	putchar_init();
	_CHIRP('8', 156);
	printf("*** COHERENT Version %s - 386 Mode.  %uKB free memory. ***\n",
		release, ctob(allocno())/1024);

	/* Print default display type, based on BIOS int11h call. */
	if ((int11 () & 0x30) == 0x30)
		printf ("Monochrome.  ");
	else
		printf ("Color.  ");

	/* Display FP coprocessor/emulation setting. */
	senseNdp ();
	printf (ndpTypeName ());
	if (ndpType <= 1 && ndpEmFn)
		printf ("FP Emulation.  ");

	/* Display general system configuration parameters. */
	printf ("%u buffers.  %u buckets.  %u clists.\n",
	  NBUF, NHASH, NCLIST);
	printf ("%uKB kalloc pool.  %u KB phys pool.\n",
	  ALLSIZE / 1024, PHYS_MEM / 1024);
	printf (copyright);

	if (_entry) {
		printf ("Serial Number ");
		printf ("%lu\n", _entry);
	}

	/*
	 * Verify correct serial number
	 */
	if (_entry != _bar)
panic("Verification error - call Mark Williams Company at +1-708-291-6700\n");

	/*
	 * Turn on clock, mount root device, start off processes
	 * and return.
	 */
	batflag = 1;

	iprocp = SELF;
	CHIRP('b');
	if (pfork()) {
		CHIRP('i');
		idle();
	} else {
		fsminit();
		CHIRP('-');
		eprocp = SELF;
		eveinit();
		CHIRP('=');
	}

#ifdef	TRACER
	checkTheSum ();
#endif
	CHIRP ('c');
}

/*
 * set_at_drive_ct()
 *
 * If "at_drive_ct" is already nonzero, don't change it.
 * Else, read CMOS and return 0,1, or 2 as number of installed "at" drives.
 */
void
set_at_drive_ct()
{
	int u;

        if (at_drive_ct <= 0) {
		/*
		 * Count nonzero drive types.
		 *
		 *	High nibble of CMOS 0x12 is drive 0's type.
		 *	Low  nibble of CMOS 0x12 is drive 1's type.
		 */
		u = read_cmos(0x12);
		if (u & 0x00F0)
			at_drive_ct++;
		if (u & 0x000F)
			at_drive_ct++;
	}
}

/*
 * rpdev()
 *
 * If rootdev is zero, try to use data from tboot to set it.
 * If pipedev is zero, make it 0x883 if ronflag == 1, else make it rootdev.
 * Call rpdev() AFTER calling set_at_drive_ct().
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
		if (ffp = fifo_open(&boot_gift, 0)) {
			int rc;

			for (rc = 0; !found && (tp = fifo_read(ffp)); rc++) {
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
			if (at_drive_ct > root_drv) {
				root_maj = AT_MAJOR;
				root_min = root_drv*NPARTN + root_ptn;
			} else { /* root on SCSI device */
				root_maj = SCSI_MAJOR;
				root_min = (root_drv-at_drive_ct)*16 + root_ptn;
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
