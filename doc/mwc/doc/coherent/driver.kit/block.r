.ds TL "Block Driver"
.NH "Example of a Block Driver"
.PP
This section gives an example driver for a block device:
the \*(CO driver for the AT hard disk.
This driver is described in the article
.B at
in the \*(CO Lexicon.
The source is kept in directory
.BR /etc/conf/at/src/at.c .
.PP
In the following, code appears in monospaced font; comments appear in Roman.
.SH "Preliminaries"
.PP
The following code prefaces the driver.
.Sh "Header Files"
.PP
.B at
uses the following header files.
Because header files have changed drastically for \*(CO 4.2, you should note
carefully the suite included here:
.DM
#include <sys/cmn_err.h>
#include <sys/inline.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdlib.h>
.DE
.DM
#include <kernel/typed.h>
#include <sys/coherent.h>
#include <sys/uproc.h>
#include <sys/fdisk.h>
#include <sys/hdioctl.h>
#include <sys/buf.h>
#include <sys/con.h>
#include <sys/devices.h>
.DE
.Sh "Manifest Constants"
.PP
.B at
uses the following gives manifest constants and macros:
.DM
#define	LOCAL
.DE
.DM
.tm 3.0i
#define	HDBASE 0x01F0	/* Port base */
#define	SOFTLIM 6	/*  (7) num of retries before diag */
#define	HARDLIM 8	/* number of retries before fail */
#define	BADLIM 100	/* num to stop recov if flagged bad */
.DE
.DM
.tm 3.0i
#define	BIT(n)	(1 << (n))
.DE
.DM
.tm 3.0i
#define	CMOSA 0x70	/* write cmos address to this port */
#define	CMOSD 0x71	/* read cmos data through this port */
.DE
.DM
.ta 2.0i 3.5i
/*
 * I/O Port Addresses
 */
#define DATA_REG	(HDBASE + 0)	/* data (r/w) */
#define AUX_REG	(HDBASE + 1)	/* error(r), write precomp cyl/4 (w) */
#define NSEC_REG	(HDBASE + 2)	/* sector count (r/w) */
#define SEC_REG	(HDBASE + 3)	/* sector number (r/w) */
#define LCYL_REG	(HDBASE + 4)	/* low cylinder (r/w) */
#define HCYL_REG	(HDBASE + 5)	/* high cylinder (r/w) */
#define HDRV_REG	(HDBASE + 6)	/* drive/head (r/w) (D <<4)+(1 << H) */
#define CSR_REG	(HDBASE + 7)	/* status (r), command (w) */
#define HF_REG	(HDBASE + 0x206)	/* Usually 0x3F6 */
.DE
.DM
.ta 2.0i 3.5i
/*
 * Error from AUX_REG (r)
 */
#define DAM_ERR	BIT(0)	/* data address mark not found */
#define TR0_ERR	BIT(1)	/* track 000 not found */
#define ABT_ERR	BIT(2)	/* aborted command */
#define ID_ERR	BIT(4)	/* id not found */
#define ECC_ERR	BIT(6)	/* data ecc error */
#define BAD_ERR	BIT(7)	/* bad block detect */
.DE
.DM
.ta 2.0i 3.5i
/*
 * Status from CSR_REG (r)
 */
#define ERR_ST	BIT(0)	/* error occurred */
#define INDEX_ST	BIT(1)	/* index pulse */
#define SOFT_ST	BIT(2)	/* soft (corrected) ECC error */
#define DRQ_ST	BIT(3)	/* data request */
#define SKC_ST	BIT(4)	/* seek complete */
#define WFLT_ST	BIT(5)	/* improper drive operation */
#define RDY_ST	BIT(6)	/* drive is ready */
#define BSY_ST	BIT(7)	/* controller is busy */
.DE
.DM
.ta 2.0i 3.5i
/*
 * Commands to CSR_REG (w)
 */
#define RESTORE(rate)	(0x10 +(rate))	/* X */
#define SEEK(rate)	(0x70 +(rate))	/* X */
#define READ_CMD	(0x20)	/* X */
#define WRITE_CMD	(0x30)	/* X */
#define FORMAT_CMD	(0x50)	/* X */
#define VERIFY_CMD	(0x40)	/* X */
#define DIAGNOSE_CMD	(0x90)	/* X */
#define SETPARM_CMD	(0x91)	/* X */
.DE
.DM
.ta 2.0i 3.5i
/*
 * Device States.
 */
#define SIDLE	0	/* controller idle */
#define SRETRY	1	/* seeking */
#define SREAD	2	/* reading */
#define SWRITE	3	/* writing */
.DE
.Sh "Function Declarations"
.PP
The following declares the functions used in the driver.
.DM
.ta 2.0i 3.5i
/*
 * Forward Referenced Functions.
 */
LOCAL void	atreset ();
LOCAL int	atdequeue ();
LOCAL void	atstart ();
LOCAL void	atdefer ();
LOCAL int	aterror ();
LOCAL void	atrecov ();
LOCAL void	atdone ();
.DE
.Sh "Macros"
.PP
.B at
uses the following macros:
.DM
.ta 2.0i 3.5i
#define NOTBUSY()	((inb (ATSREG) & BSY_ST) == 0)
#define DATAREQUESTED()	((inb (ATSREG) & DRQ_ST) != 0)
#define ATDRQ()	(DATAREQUESTED () ? 1 : atdrq ())
#define ATBSYW(u)	(NOTBUSY () ? 1 : myatbsyw (u))
.DE
.Sh "Global and Static Variables"
.PP
.B at
uses the following global and static variables:
.DM
extern typed_space	boot_gift;
extern short	at_drive_ct;
.DE
The following are used throughout the driver:
.IP \fBATSECS\fR
This is number of seconds to wait for an expected interrupt.
.IP \fBATSREG\fR
This must be 3F6 for most new IDE drives; or
1F7 for Perstor controllers and some old IDE drives.
Either value works with most drives.
.IP \fBatparm\fR
This holds drive parameters.
If initialized to zero,
the driver will try to use values it read from the BIOS during real-mode
startup.
.DM
extern unsigned ATSECS;
extern unsigned ATSREG;
extern struct hdparm_s atparm [];
.DE
.PP
The next line gives the global variable that holds the partition parameters,
as copied from the disk.
There are
.B "N_ATDRV * NPARTN"
positions for the user partitions,
plus
.B N_ATDRV
additional partitions to span each drive.
.PP
When aligning partitions on cylinder boundaries, the 
optimal partition size is 14,280 blocks (2 * 3 * 4 * 5 * 7 * 17); whereas
an acceptable partition size is  7,140 blocks (3 * 4 * 5 * 7 * 17).
.DM
static struct fdisk_s pparm [N_ATDRV * NPARTN + N_ATDRV];
.DE
The following structure
.B at
holds information about the disk controller.
There exists one copy of this structure for each AT controller.
.DM
.ta 0.5i 3.5i
static struct at {
	BUF *at_actf;	/* Link to first */
	BUF *at_actl;	/* Link to last */
	paddr_t at_addr;	/* Source/Dest virtual address */
	daddr_t at_bno;	/* Block # on disk */
	unsigned at_nsec;	/* # of sectors on current transfer */
	unsigned at_drv;
	unsigned at_head;
	unsigned at_cyl;
	unsigned at_sec;
	unsigned at_partn;
	unsigned char at_dtype [N_ATDRV];	/* drive type, 0 if unused */
	unsigned char at_tries;
	unsigned char at_state;
	unsigned at_totalsec;
} at;
.DE
Finally, this last variable holds the template of the message
to be displayed when an AT drive times out.
.DM
static char timeout_msg [] = "at%d: TO\en";
.DE
.SH "Load Routine"
.PP
.B atload()
is the routine that the kernel executes when this driver is loaded.
Under \*(CO 4.2, it is executed once, when the kernel is booted.
.PP
This function resets the controller, grabs the interrupt vector, and
sets up the drive characteristics.
.DM
LOCAL void
atload ()
{
	unsigned int u;
	struct hdparm_s * dp;
	struct { unsigned short off, seg; } p;
.DE
.DM
	if (at_drive_ct <= 0)
		return;

	/* Flag drives 0, 1 as present or not. */
	at.at_dtype [0] = 1;
	at.at_dtype [1] = at_drive_ct > 1 ? 1 : 0;
.DE
.DM
.DE
.DM
	/* Obtain Drive Characteristics. */
	for (u = 0, dp = atparm; u < at_drive_ct; ++ dp, ++ u) {
		struct hdparm_s int_dp;
		unsigned short ncyl = _CHAR2_TO_USHORT (dp->ncyl);
.DE
.DM
		if (ncyl == 0) {
			/*
			 * Not patched.
			 *
			 * If tertiary boot sent us parameters,
			 *   Use "fifo" routines to fetch them.
			 *   This only gives us ncyl, nhead, and nspt.
			 *   Make educated guesses for other parameters:
			 *   Set landc to ncyl, wpcc to -1.
			 *   Set ctrl to 0 or 8 depending on head count.
			 *
			 * Follow INT 0x41/46 to get drive static BIOS drive
			 * parameters, if any.
			 *
			 * If there were no parameters from tertiary boot,
			 * or if INT 0x4? nhead and nspt match tboot parms,
			 *   use "INT" parameters (will give better match on
			 *   wpcc, landc, and ctrl fields, which tboot can't
			 *   give us).
			 */
.DE
.DM
			FIFO * ffp;
			typed_space * tp;
			int found, parm_int;
.DE
.DM
if (F_NULL != (ffp = fifo_open (& boot_gift, 0))) {
			for (found = 0; ! found && (tp = fifo_read (ffp)); ) {
				BIOS_DISK * bdp = (BIOS_DISK *)tp->ts_data;
				if ((T_BIOS_DISK == tp->ts_type) &&
				    (u == bdp->dp_drive) ) {
					found = 1;
					_NUM_TO_CHAR2(dp->ncyl,
					  bdp->dp_cylinders);
					dp->nhead = bdp->dp_heads;
					dp->nspt = bdp->dp_sectors;
					_NUM_TO_CHAR2(dp->wpcc, 0xffff);
					_NUM_TO_CHAR2(dp->landc,
					  bdp->dp_cylinders);
					if (dp->nhead > 8)
						dp->ctrl |= 8;
				}
			}
			fifo_close (ffp);
}
.DE
.DM
			if (u == 0)
				parm_int = 0x41;
			else /* (u == 1) */
				parm_int = 0x46;
			pxcopy ((paddr_t)(parm_int * 4), & p, sizeof p, SEL_386_KD);
			pxcopy ((paddr_t)(p.seg <<4L)+ p.off,
				& int_dp, sizeof (int_dp), SEL_386_KD);
.DE
.DM
			if (! found || (dp->nhead == int_dp.nhead &&
			     		dp->nspt == int_dp.nspt)) {
				* dp = int_dp;
				printf ("Using INT 0x%x", parm_int);
			} else
				printf ("Using INT 0x13(08)");
.DE
.DM
		} else {
			printf ("Using patched");
			/*
			 * Avoid incomplete patching.
			 */
			if (at.at_dtype [u] == 0)
				at.at_dtype [u] = 1;
			if (dp->nspt == 0)
				dp->nspt = 17;
		}
.DE
.DM
#if VERBOSE > 0
	printf (" drive %d parameters\en", u);

	cmn_err (CE_CONT,
		 "at%d: ncyl=%d nhead=%d wpcc=%d eccl=%d ctrl=%d landc=%d "
		 "nspt=%d\en", u, _CHAR2_TO_USHORT (dp->ncyl), dp->nhead,
		 _CHAR2_TO_USHORT (dp->wpcc), dp->eccl, dp->ctrl,
		 _CHAR2_TO_USHORT (dp->landc), dp->nspt);
#endif
	}
.DE
.DM
	/* Initialize Drive Size. */
	for (u = 0, dp = atparm; u < at_drive_ct; ++ dp, ++ u) {
		if (at.at_dtype [u] == 0)
			continue;

.DE
.DM
		pparm [N_ATDRV * NPARTN + u].p_size =
			(long) _CHAR2_TO_USHORT (dp->ncyl) * dp->nhead *
				dp->nspt;
	}
.DE
.DM
	/* Initialize Drive Controller.	 */
	atreset ();
}
.DE
.SH "Unload Routine"
.PP
Function
.B atunload()
is called when this driver is unloaded from memory \(em or would be, if
\*(CO 4.2 supported loadable drivers.
.DM
LOCAL void
atunload ()
{
}
.DE
.SH "Reset the Controller"
.PP
Function
.B atreset()
resets the hard-disk controller and defines drive characteristics.
.DM
LOCAL void
atreset ()
{
	int u;
	struct hdparm_s * dp;
.DE
.DM
	/* Reset controller for a minimum of 4.8 microseconds. */
	outb (HF_REG, 4);
	for (u = 100; -- u != 0;)
		/* DO NOTHING */ ;
	outb (HF_REG, atparm [0].ctrl & 0x0F);
	ATBSYW (0);
.DE
.DM	if (inb (AUX_REG) != 0x01) {
		/*
		 * Some IDE drives always timeout on initial reset.
		 * So don't report first timeout.
		 */
		static one_bad;
.DE
.DM
		if (one_bad)
			printf ("at: hd controller reset timeout\en");
		else
			one_bad = 1;
	}
.DE
.DM
	/* Initialize drive parameters. */
	for (u = 0, dp = atparm; u < at_drive_ct; ++ dp, ++ u) {
		if (at.at_dtype [u] == 0)
			continue;
		ATBSYW (u);
.DE
.DM
		/* Set drive characteristics. */
		outb (HF_REG, dp->ctrl);
		outb (HDRV_REG, 0xA0 + (u << 4) + dp->nhead - 1);
.DE
.DM
		outb (AUX_REG, _CHAR2_TO_USHORT (dp->wpcc) / 4);
		outb (NSEC_REG, dp->nspt);
		outb (SEC_REG, 0x01);
		outb (LCYL_REG, dp->ncyl [0]);
		outb (HCYL_REG, dp->ncyl [1]);
		outb (CSR_REG, SETPARM_CMD);
		ATBSYW (u);
.DE
.DM
		/* Restore heads. */
		outb (CSR_REG, RESTORE (0));
		ATBSYW (u);
	}
}
.DE
.SH "Open Routine"
.PP
Function
.B atopen()
is called when a user's application invokes the system call
.B open()
for an AT device.
A pointer to this function appears in field
.B c_open
of the
.B CON
structure at the end of this driver.
.PP
This function validating the minor device (that is, ensures that the
user is attempting to open a devices that exists), and updates
the paritition table if necessary.
.DM
LOCAL void
atopen (dev, mode)
dev_t dev;
{
	int d;		/* drive */
	int p;		/* partition */
.DE
.DM
	p = minor (dev) % (N_ATDRV * NPARTN);
	if (minor (dev) & SDEV) {
		d = minor (dev) % N_ATDRV;
		p += N_ATDRV * NPARTN;
	} else
		d = minor (dev) / NPARTN;
.DE
.DM
	if (d >= N_ATDRV || at.at_dtype [d] == 0) {
		printf ("atopen: drive %d not present ", d);
		set_user_error (ENXIO);
		return;
	}
	if (minor (dev) & SDEV)
		return;
.DE
.DM
	/* If partition not defined read partition characteristics. */
	if (pparm [p].p_size == 0)
		fdisk (makedev (major (dev), SDEV + d), & pparm [d * NPARTN]);
.DE
.DM
	/* Ensure partition lies within drive boundaries and is non-zero size. */
	if (pparm [p].p_base + pparm [p].p_size >
	    pparm [d + N_ATDRV * NPARTN].p_size) {
		printf ("atopen: p_size too big ");
		set_user_error (EINVAL);
	} else if (pparm [p].p_size == 0) {
		printf ("atopen: p_size zero ");
		set_user_error (ENODEV);
	}
}
.DE
.SH "Read Routine"
.PP
Function
.B atread()
is called when a user's application invokes the system call
.B read()
for an AT device.
A pointer to this function appears in field
.B c_read
of the
.B CON
structure at the end of this driver.
This function simply invokes the common code for processing raw I/O.
.DM
LOCAL void
atread (dev, iop)
dev_t dev;
IO *iop;
{
	ioreq (NULL, iop, dev, BREAD, BFRAW | BFBLK | BFIOC);
}
.DE
.SH "Write Routine"
.PP
Function
.B atwrite()
is called when a user's application invokes the system call
.B write()
for an AT device.
A pointer to this function appears in field
.B c_write
of the
.B CON
structure at the end of this driver.
This function simply invokes the common code for processing raw I/O.
.DM
LOCAL void
atwrite (dev, iop)
dev_t dev;
IO *iop;
{
	ioreq (NULL, iop, dev, BWRITE, BFRAW | BFBLK | BFIOC);
}
.DE
.SH "ioctl Routine"
.PP
Function
.B atioctl()
is called when a user's application invokes the system call
.B ioctl()
for an AT device.
A pointer to this function appears in field
.B c_ioctl
of the
.B CON
structure at the end of this driver.
This function validates the minor device and
updates the paritition table if necessary.
.DM
LOCAL void
atioctl (dev, cmd, vec)
dev_t dev;
int cmd;
char *vec;
{
	int d;
.DE
.DM
	/* Identify drive number. */
	if (minor (dev) & SDEV)
		d = minor (dev) % N_ATDRV;
	else
		d = minor (dev) / NPARTN;
.DE
.DM
	/* Identify input / output request. */
	switch (cmd) {
	case HDGETA:
		/* Get hard disk attributes. */
		kucopy (atparm + d, vec, sizeof (atparm [0]));
		break;
.DE
.DM
	case HDSETA:
		/* Set hard disk attributes. */
		ukcopy (vec, atparm + d, sizeof (atparm [0]));
		at.at_dtype [d] = 1;	/* set drive type nonzero */
		pparm [N_ATDRV * NPARTN + d].p_size =
			  (long) _CHAR2_TO_USHORT (atparm [d].ncyl) *
			  atparm [d].nhead * atparm [d].nspt;
		atreset ();
		break;
.DE
.DM
	default:
		set_user_error (EINVAL);
		break;
	}
}
.DE
.SH "Watch for Interrupts"
.PP
Function
.B atwatch()
watches for interrupts.
If
.B drvl[AT_MAJOR]
is greater than zero, this function decrements it.
If it decrements to zero, it simulates a hardware interrupt.
.DM
LOCAL void
atwatch()
{
	BUF * bp = at.at_actf;
	int s;
.DE
.DM
	s = sphi ();
	if (-- drvl [AT_MAJOR].d_time > 0) {
		spl (s);
		return;
	}
.DE
.DM
	/* Reset hard disk controller, cancel request. */
	atreset ();
	if (at.at_tries ++ < SOFTLIM) {
		atstart ();
	} else {
		printf ("at%d%c: bno=%lu head=%u cyl=%u nsec=%u tsec=%d "
			"dsec=%d <Watchdog Timeout>\en", at.at_drv,
			(bp->b_dev & SDEV) ? 'x' : at.at_partn % NPARTN + 'a',
			bp->b_bno, at.at_head, at.at_cyl, at.at_nsec,
			at.at_totalsec, inb (NSEC_REG));
.DE
.DM
		at.at_actf->b_flag |= BFERR;
		atdone (at.at_actf);
	}
	spl (s);
}
.DE
.SH "Block Function"
.PP
Function
.B atblock()
queues a block to the disk.
It also ensures that the transfer is within the disk partition.
.DM
LOCAL void
atblock (bp)
BUF	* bp;
{
	struct fdisk_s * pp;
	int partn = minor (bp->b_dev) % (N_ATDRV * NPARTN);
	int s;
.DE
.DM
	bp->b_resid = bp->b_count;
	if (minor (bp->b_dev) & SDEV)
		partn += N_ATDRV * NPARTN;
	pp = pparm + partn;
.DE
.DM
	/* Check for read at end of partition. */
	if (bp->b_req == BREAD && bp->b_bno == pp->p_size) {
		bdone (bp);
		return;
	}
.DE
.DM
	/* Range check disk region. */
	if (bp->b_bno + (bp->b_count / BSIZE) > pp->p_size ||
	    bp->b_count % BSIZE != 0 || bp->b_count == 0) {
		bp->b_flag |= BFERR;
		bdone (bp);
		return;
	}
.DE
.DM
	s = sphi ();
	bp->b_actf = NULL;
	if (at.at_actf == NULL)
		at.at_actf = bp;
	else
		at.at_actl->b_actf = bp;
	at.at_actl = bp;
	spl (s);
.DE
.DM
	if (at.at_state == SIDLE)
		if (atdequeue ())
			atstart ();
}
.DE
.SH "Dequeue a Request"
.PP
Function
.B atdequeue()
obtains the next request for disk I/O.
.DM
LOCAL int
atdequeue ()
{
	BUF * bp;
	struct fdisk_s * pp;
	struct hdparm_s * dp;
	unsigned int nspt;
	ldiv_t		addr;
	unsigned short	secs;
	unsigned short	newsec;
.DE
.DM
	at.at_tries = 0;
	if ((bp = at.at_actf) == NULL)
		return 0;
.DE
.DM
	at.at_partn = minor (bp->b_dev) % (N_ATDRV * NPARTN);
	if (minor (bp->b_dev) & SDEV) {
		at.at_partn += N_ATDRV * NPARTN;
		at.at_drv = minor (bp->b_dev) % N_ATDRV;
	} else
		at.at_drv = minor (bp->b_dev) / NPARTN;
.DE
.DM
	nspt = atparm [at.at_drv].nspt;
	at.at_addr = bp->b_paddr;
	pp = pparm + at.at_partn;
	at.at_bno = pp->p_base + bp->b_bno;
.DE
.DM
	dp = atparm + at.at_drv;
	addr = ldiv (at.at_bno, dp->nspt);
	at.at_sec = addr.rem + 1;
	addr = ldiv (addr.quot, dp->nhead);
	at.at_cyl = addr.quot;
	at.at_head = addr.rem;
.DE
The following code was added to the driver for \*(CO 4.2, to speed I/O
on the AT disk.
The following explains how this speed-up works.
.PP
It is unclear why, but IDE writes appear always to lose a revolution,
even though reads work comfortably.
This may be caused by IDE drives trying to maintain the synchronous
semantics of the write, or it may be due to
the \*(CO kernel's not making the read time and the
slack being taken up by track-buffering.
.PP
In either case, \*(CO gains a vast improvement in throughput for writes and
a modest gain for reads by looking ahead in the request chain and
coalescing separate requests to consecutive blocks into a single
multi-sector request.
.DM
	newsec = secs = bp->b_count / BSIZE;
	while (bp->b_actf != NULL && bp->b_actf->b_bno == bp->b_bno + secs &&
	       bp->b_actf->b_req == bp->b_req &&
	       bp->b_actf->b_dev == bp->b_dev) {
		/*
		 * Take care to bound the length of the combined request to a
		 * single byte count of sectors.
		 */
		bp = bp->b_actf;
.DE
.DM
		if (newsec + (secs = bp->b_count / BSIZE) > 256)
			break;
		newsec += secs;
	}
	at.at_totalsec = at.at_nsec = newsec;
	return 1;
}
.DE
.SH "Send Data to the Disk"
.PP
Function
.B atsend()
actually moves data onto the disk.
.DM
LOCAL void
atsend (addr)
paddr_t addr;
{
	addr = P2P (addr);
	repoutsw (DATA_REG, (unsigned short *) __PTOV (addr), BSIZE / 2);
}
.DE
.SH "Receive Data from the Disk"
.PP
Function
.B atrecv()
actually receives data from the disk.
.DM
LOCAL void
atrecv (addr)
paddr_t addr;
{
	addr = P2P (addr);
	repinsw (DATA_REG, (unsigned short *) __PTOV (addr), BSIZE / 2);
}
.DE
.SH "Abandon a Request"
.PP
Function
.B atabandon()
abandons a request for disk I/O.
.DM
LOCAL void
atabandon ()
{
	buf_t *bp;
.DE
.DM
	/* Abandon this operation. */
	while ((bp = at.at_actf) != NULL) {
		at.at_actf = bp->b_actf;
		bp->b_flag |= BFERR;
		bdone (bp);
	}
	at.at_state = SIDLE;
}
.DE
.SH "Start a Read/Write Operation"
.PP
Function
.B atstart()
starts or restarts the next disk read/write operation.
.DM
LOCAL void
atstart ()
{
	struct hdparm_s * dp;
.DE
.DM
	/* Check for repeated access to most recently identified bad track. */
	ATBSYW (at.at_drv);
	dp = atparm + at.at_drv;
	outb (HF_REG, dp->ctrl);
	outb (HDRV_REG, (at.at_drv << 4) + at.at_head + 0xA0);
.DE
.DM
	outb (NSEC_REG, at.at_nsec);
	outb (SEC_REG, at.at_sec);
	outb (LCYL_REG, at.at_cyl);
	outb (HCYL_REG, at.at_cyl >> 8);
.DE
.DM
	if (inb (NSEC_REG) != (at.at_nsec & 0xFF)) {
		/*
		 * If we get here, things are confused. We should reset the
		 * controller and retry whatever operation we want to start
		 * now.
		 */
		drvl [AT_MAJOR].d_time = 1;
		return;
	}
.DE
.DM
	if (at.at_actf->b_req == BWRITE) {
		outb (CSR_REG, WRITE_CMD);
		while (ATDRQ () == 0) {
			atabandon ();
			return;
		}
		atsend (at.at_addr);
		at.at_state = SWRITE;
	} else {
		outb (CSR_REG, READ_CMD);
		at.at_state = SREAD;
	}
.DE
.DM
	drvl [AT_MAJOR].d_time = ATSECS;
}
.DE
.SH "Interrupt Handler"
.PP
Function
.B atintr()
handles interrupts.
It clears the interrupt, and defers its processing until a more suitable time.
.DM
void
atintr ()
{
	(void) inb (CSR_REG);	/* clears controller interrupt */
	atdefer ();
}
.DE
.SH "Defer Service of an Interrupt"
.PP
Function
.B atdefer()
actually services the hard-disk interrupt.
It transfers the required data, and updates the state of the device.
.DM
LOCAL void
atdefer ()
{
	BUF * bp = at.at_actf;
	switch (at.at_state) {
	case SRETRY:
		atstart ();
		break;
.DE
.DM
	case SREAD:
		/* Check for I/O error before waiting for data. */
		if (aterror ()) {
			atrecov ();
			break;
		}
.DE
.DM
		/* Wait for data, or forever. */
		if (ATDRQ () == 0) {
			atabandon ();
			break;
		}
.DE
.DM
		/* Read data block.*/
		atrecv (at.at_addr);
.DE
.DM
		/* Check for I/O error after reading data. */
		if (aterror ()) {
			atrecov ();
			break;
		}
.DE
.DM
		/*
		 * Every time we transfer a block, bump the timeout to prevent
		 * very large multisector transfers from timing out due to
		 * sheer considerations of volume.
		 */
		drvl [AT_MAJOR].d_time = ATSECS * 2;
.DE
.DM
		at.at_addr += BSIZE;
		bp->b_resid -= BSIZE;
		at.at_tries = 0;
		at.at_bno ++;
.DE
.DM
		/*
		 * Check for end of transfer (total, or simply part of a large
		 * combined request).
		 */
		if (-- at.at_nsec == 0)
			atdone (bp);
		else if (bp->b_resid == 0) {
			at.at_addr = (at.at_actf = bp->b_actf)->b_paddr;
			bdone (bp);
		}
		break;
.DE
.DM
	case SWRITE:
		/* Check for I/O error. */
		if (aterror ()) {
			atrecov ();
			break;
		}
.DE
.DM
		/* bump timeout again, for reasons given above. */
		drvl [AT_MAJOR].d_time = ATSECS * 2;
.DE
.DM
		at.at_addr += BSIZE;
		bp->b_resid -= BSIZE;
		at.at_tries = 0;
		at.at_bno ++;
.DE
.DM
		/*
		 * Check for end of transfer, either the real end or the end
		 * of a block boundary in a combined transfer.
		 */
		if (-- at.at_nsec == 0) {
			atdone (bp);
			break;
		} else if (bp->b_resid == 0)
			at.at_addr = bp->b_actf->b_paddr;
.DE
.DM
		/* Wait for ability to send data, or forever. */
		while (ATDRQ () == 0) {
			atabandon ();
			break;
		}
.DE
.DM
		/* Send data block. */
		atsend (at.at_addr);
		if (bp->b_resid == 0) {
			at.at_actf = bp->b_actf;
			bdone (bp);
		}
	}
}
.DE
.SH "Check for an Error"
.PP
Function
.B aterror()
checks for drive error.
If it finds an error, it increments the error count and prints a
message that reports the error.
It returns zero if it did not find an error, and one if it did.
.DM
LOCAL int
aterror ()
{
	BUF * bp = at.at_actf;
	int csr;
	int aux;
.DE
.DM
	if ((csr = inb (ATSREG)) & (ERR_ST | WFLT_ST)) {
		aux = inb (AUX_REG);
.DE
.DM
		if (aux & BAD_ERR) {
			at.at_tries	= BADLIM;
		} else if (++ at.at_tries < SOFTLIM)
			return 1;
.DE
.DM
		printf ("at%d%c: bno =%lu head =%u cyl =%u",
			at.at_drv,
			(bp->b_dev & SDEV) ? 'x' : at.at_partn % NPARTN + 'a',
			(bp->b_count / BSIZE) + bp->b_bno - at.at_nsec,
			at.at_head, at.at_cyl);
.DE
.DM
		if ((csr & (RDY_ST | WFLT_ST)) != RDY_ST)
			printf (" csr =%x", csr);
		if (aux & (DAM_ERR | TR0_ERR | ID_ERR | ECC_ERR | ABT_ERR))
			printf (" aux =%x", aux);
.DE
.DM
		if (aux & BAD_ERR)
			printf (" <Block Flagged Bad>");
		if (at.at_tries < HARDLIM)
			printf (" retrying...");
		printf ("\en");
		return 1;
	}
	return 0;
}
.DE
.SH "Attempt to Recover from an Error"
.PP
Function
.B atrecov()
attempts to recover from a reported error.
.DM
LOCAL void
atrecov ()
{
	BUF * bp = at.at_actf;
	int cmd = SEEK (0);
	int cyl = at.at_cyl;
.DE
.DM
	switch (at.at_tries) {
	case 1:
	case 2:
		/* Move in 1 cylinder, then retry operation */
		if (--cyl < 0)
			cyl += 2;
		break;
.DE
.DM
	case 3:
	case 4:
		/* Move out 1 cylinder, then retry operation */
		if (++ cyl >= _CHAR2_TO_USHORT (atparm [at.at_drv].ncyl))
			cyl -= 2;
		break;
.DE
.DM
	case 5:
	case 6:
		/* Seek to cylinder 0, then retry operation */
		cyl = 0;
		break;
.DE
.DM
	default:
		/* Restore drive, then retry operation */
		cmd = RESTORE (0);
		cyl = 0;
		break;
	}
.DE
.DM
	/* Retry operation [after repositioning head] */
	if (at.at_tries < HARDLIM) {
		drvl [AT_MAJOR].d_time = cmd == RESTORE (0) ? ATSECS * 2 :
							      ATSECS;
		outb (LCYL_REG, cyl);
		outb (HCYL_REG, cyl >> 8);
		outb (HDRV_REG, (at.at_drv << 4) + 0xA0);
		outb (CSR_REG, cmd);
		at.at_state = SRETRY;
	} else {
.DE
.DM
		/* Give up on block. */
		bp->b_flag |= BFERR;
		atdone (bp);
	}
}
.DE
.SH "Release the Current I/O Buffer"
.PP
Function
.B atdone()
releases the current I/O buffer.
.DM
LOCAL void
atdone (bp)
BUF * bp;
{
	at.at_actf = bp->b_actf;
	drvl [AT_MAJOR].d_time = 0;
	at.at_state = SIDLE;
.DE
.DM
	if (atdequeue ())
		atstart ();
	bdone (bp);
}
.DE
.SH "Indicate the Drive Is Not Busy"
.PP
Function
.B notBusy()
indicates that the drive is not busy.
See macro
.BR NOTBUSY() ,
defined above.
.DM
int
notBusy ()
{
	return NOTBUSY ();
}
.DE
.SH "Indicate Whether Data Have Been Requested"
.PP
Function
.B dataRequested()
indicates whether data have been requested.
See macro
.BR DATAREQUESTED() ,
defined above.
.DM
int
dataRequested ()
{
	return DATAREQUESTED ();
}
.DE
.SH "Report a Timeout, First Version"
.PP
Function
.B _report_timeout()
actually prints the message that reports that an I/O operation has
timed out.
.DM
static	int report_scheduled;
static	int report_drv;
LOCAL void
_report_timeout ()
{
	printf (timeout_msg, report_drv);
	report_scheduled = 0;
}
.DE
.SH "Report a Timeout, Second Version"
.PP
Function
.B report_timeout()
manages the task of reporting that an I/O request has timed out.
.DM
LOCAL void
report_timeout (unit)
int unit;
{
	short s = sphi();
	if (report_scheduled == 0) {
		report_scheduled = 1;
		spl(s);
.DE
.DM
		report_drv = unit;
		defer (_report_timeout);
	} else
		spl (s);
}
.DE
.SH "Wait Until the Controller Is Freed"
.PP
Function
.B myatbsyw()
waits while the controller is busy.
It returns zero if the driver timed out while executing this I/O task;
or a non-zero value if it did not.
.DM
int
myatbsyw (unit)
int unit;
{
	if (busyWait (notBusy, ATSECS * HZ))
		return 1;
	report_timeout (unit);
	return 0;
}
.DE
.SH "Wait for Controller to Initiate Request"
.PP
Function
.B atdrq()
waits for the controller to initiate a request.
It returns zero if the driver timed out while waiting; or one if it
did not.
.DM
int
atdrq ()
{
	if (busyWait (dataRequested, ATSECS /* * HZ */))
		return 1;
	report_timeout (at.at_drv);
	return 0;
}
.DE
.SH "The CON Structure"
.PP
Finally, the following gives the
.B CON
structure for this driver.
This structure contains pointers to the functions to be invoked by
the kernel's system calls.
For details on this structure, see the entry for
.B CON
in this manual's Lexicon.
.DM
CON	atcon	= {
	DFBLK | DFCHR,			/* Flags */
	AT_MAJOR,			/* Major index */
	atopen,				/* Open */
	NULL,				/* Close */
	atblock,			/* Block */
	atread,				/* Read */
	atwrite,			/* Write */
	atioctl,			/* Ioctl */
	NULL,				/* Powerfail */
	atwatch,			/* Timeout */
	atload,				/* Load */
	atunload			/* Unload */
};
.DE
.SH "Where To Go From Here"
.PP
The following section gives an example of a driver for a character device.
The kernel functions invoked in this driver are described in this manual's
Lexicon.
