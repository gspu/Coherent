/***********************************************************************
 *  Module: haiscsi.c
 *
 *  This is the interface between the Coherent kernel, the host
 *  adapter module and the SCSI device modules. It's just a simple
 *  dispatcher that determines which routine to call based upon the
 *  calling device's Target ID.  The target ID should be set in bits
 *  4-6 of the device's minor number.  
 *
 *  Copyright (c) 1993, Christopher Sean Hilton, All Rights Reserved.
 *
 *  Last Modified: Sat Jul 24 08:08:28 1993 by [chris]
 */

#include <stddef.h>
#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/buf.h>
#include <sys/io.h>
#include <sys/sched.h>
#include <sys/stat.h>

#include <sys/uproc.h>
#include <errno.h>

#include <sys/haiscsi.h>
#include <sys/haiioctl.h>

/*
 *  Constants.
 */
#define GROUPMASK   0xe0
#define GROUP0	  0x00		/* SCSI-1/2 */
#define GROUP1	  0x20		/* SCSI-1/2 */
#define GROUP2	  0x40		/* SCSI-2 */
#define GROUP5	  0xa0		/* SCSI-1/2 */

/* Configurable variables - see /etc/conf/hai/Space.c. */
extern int HAI_DISK;
extern int HAI_TAPE;

extern int nonedev();	   /* Set error and exit. */
extern int nulldev();	   /* Do nothing and exit. */

/*
 * Device type entry points.
 */
extern dca_t    sddca;		/* Fixed disk control routines */
extern dca_t    ctdca;		/* Cartridge tape control routines */
extern dca_t	haict3600;	/* Cartridge tape (Tandberg TDC3600) */

dca_t  mdca[MAXDEVS];		/* Initialized by setup_mdca(). */

static void scsi_open();	/* Open dispatcher */
static void scsi_close();	/* Close dispatcher */
static void scsi_block();	/* Block dispatcher */
static void scsi_read();	/* Read dispatcher */
static void scsi_write();	/* Write dispatcher */
static void scsi_ioctl();	/* I/O Control dispatcher */
static int scsi_load();		/* Load driver */
static int scsi_unload();	/* Unload driver */

static void setup_mdca();	/* Put device handlers into SCSI id table. */

CON scsicon = {
	DFBLK | DFCHR,
	SCSIMAJOR,
	scsi_open,		/* Open entry point */
	scsi_close,		/* Close entry point */
	scsi_block,		/* Block entry point. */
	scsi_read,		/* Read Entry point */
	scsi_write,		/* write entry point */
	scsi_ioctl,		/* IO control entry point */
	nulldev,		/* No powerfail entry (yet?) */
	hatimer,		/* timeout entry point */
	scsi_load,		/* Load entry point */
	scsi_unload,		/* Unload entry point */
	nulldev			/* No poll entry yet either. */
};

static char *errstr[] = {
	"No sense",
	"Recovered error",
	"Not ready",
	"Medium error",
	"Hardware error",
	"Illegal request",
	"Unit attention",
	"Data protect",
	"Blank check",
	"Vendor unique error",
	"Copy Aborted",
	"Aborted command",
	"Equal",
	"Volume overflow",
	"Miscompare",
	"Reserved"
};

char iofailmsg[] = "%s: status(0x%x)";
int  HAI_HAID	= 7;

extern int hapresent;		/* Provided/Controled by host adapter module */
extern dca_p mdca[MAXDEVS];	/* See haicfg.c */

/***********************************************************************
 *  int scsi_open(dev_t dev, int mode)
 *
 *  Open a device on the SCSI bus at target ID: tid(dev). This is
 *  Accomplished by calling the open routine at mdca[tid(dev)]->d_open
 */

static void scsi_open(dev, mode)
register dev_t   dev;
int mode;
{
	register dca_p d = mdca[tid(dev)];

	if (!hapresent || !d)
		u. u_error = EINVAL;
	else
		(*(d->d_open))(dev, mode);
}   /* scsi_open() */

/***********************************************************************
 *  void scsi_close()
 *
 *  Close entry point for all devices at major index SCSIMAJOR.
 */

static void scsi_close(dev)
register dev_t dev;
{
	register dca_p d = mdca[tid(dev)];

	if (!hapresent || !d)
		u. u_error = EINVAL;
	else
		(*(d->d_close))(dev);
}   /* scsi_close() */

/***********************************************************************
 *  void scsi_block()
 *
 *  Block Entry point.
 */

static void scsi_block(bp)
register BUF *bp;
{
	register dca_p d = mdca[tid(bp->b_dev)];

	if (!hapresent || !d) {
		bp->b_resid = bp->b_count;
		bp->b_flag |= BFERR;
		bdone(bp);
	}
	else
		(*(d->d_block))(bp);
}   /* scsi_block() */

/***********************************************************************
 *  void scsi_read()
 *
 *  Read entry point.
 */

void scsi_read(dev, iop)
register dev_t  dev;
register IO  *iop;
{
	register dca_p d = mdca[tid(dev)];
	if (!hapresent || !d)
		u. u_error = EINVAL;
	else
		(*(d->d_read))(dev, iop);
}   /* scsi_read() */

/***********************************************************************
 *  int scsi_write()
 *
 *  Write entry point.
 */

void scsi_write(dev, iop)
register dev_t  dev;
IO  *iop;
{
	register dca_p d = mdca[tid(dev)];
	if (!hapresent || !d)
		u. u_error = EINVAL;
	else
		(*(d->d_write))(dev, iop);
}   /* scsi_write() */

/***********************************************************************
 *  scsi_ioctl()
 *
 *  IO Control entry point.
 */

static void scsi_ioctl(dev, cmd, vec)
register dev_t  dev;
int			 cmd;
char			*vec;
{
	register dca_p d = mdca[tid(dev)];

	if (!hapresent || !d)
		u. u_error = EINVAL;
	else
		(*(d->d_ioctl))(dev, cmd, vec);
}   /* scsi_ioctl() */

/***********************************************************************
 *  scsi_load()
 *
 *  Load Entry point.
 */

static int scsi_load()

{
	register int	id;
	register dca_p  d;

	printf("Host Adapter Independent SCSI Driver v1.1\n");
	hainit();
	if (!hapresent)
		printf("Host Adapter Initialization failed.\n");

	setup_mdca();

	for (id = 0; id < MAXDEVS; ++id) {
		if ((d = mdca[id]) && d->d_load) {
			printf("ID %d: ", id);
			if (!(*(d->d_load))(id))
				printf("\tLoad() failed.\n");
		}
	}
	return;
}   /* scsi_load() */

/***********************************************************************
 *  scsi_unload()
 *
 *  SCSI unload routine.
 */

int scsi_unload()

{
	register int	id;
	register dca_p  d;

	for (id = 0; id < MAXDEVS; ++id)
		if ((d = mdca[id]) && d->d_unload)
			(*(d->d_unload))(id);
}   /* scsi_unload() */

/***********************************************************************
 *  Utility Routines
 */

/***********************************************************************
 *  char *swapbytes()
 *
 *  Swap bytes in an object from big to little endian or vice versa.
 */

char *swapbytes(mem, size)
char	*mem;
size_t  size;

{
	register char *p = mem;
	register char *q = p + size - 1;

	while (q > p) {
		*p ^= *q;
		*q ^= *p;
		*p ^= *q;
		p++;
		q--;
	}
	return mem;
}   /* swapbytes() */

/***********************************************************************
 *  cpycdb()
 *
 *  Copy a SCSI Command/Data Block. Return the number of bytes copied.
 */

int cpycdb(dst, src)
register cdb_p dst;
register cdb_p src;
{
		switch (src->g0. opcode & GROUPMASK) {
		case GROUP0:
			memcpy(dst, src, sizeof(g0cmd_t));
			return sizeof(g0cmd_t);
		case GROUP1:
		case GROUP2:
			memcpy(dst, src, sizeof(g1cmd_t));
			return sizeof(g1cmd_t);
		case GROUP5:
			memcpy(dst, src, sizeof(g5cmd_t));
			return sizeof(g5cmd_t);
		default:
			return 0;
	}
}   /* cpycdb() */

/***********************************************************************
 *  dumpmem()
 *
 *  Dump memory from (p) for (s) bytes.
 */

static char hexchars[] = "0123456789abcdef",
			linebuf[] = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n";

void dumpmem(m, p, s)
char m[];
register unsigned char *p;
register size_t s;
{
	register int i;
	char *l;

	if (m)
		printf(m);
	printf(" (0x%x)\n", (unsigned) p);
	memset(linebuf, ' ', sizeof(linebuf) - 2);
	linebuf[48] = '|';
	l = linebuf;
	for (i = 0; i < s; ++i, ++p) {
		*l++ = hexchars[(*p >> 4) & 0x0f];
		*l++ = hexchars[*p & 0x0f];
		*l++ = ' ';
		linebuf[50 + (i & 15)] = (*p >= ' ' && *p <= '~') ? *p : '.';
		if ((i & 15) == 15) {
			printf(linebuf);
			memset(linebuf, ' ', sizeof(linebuf) - 2);
			linebuf[48] = '|';
			l = linebuf;
		}
	}
	if ((s & 15) != 0)
		printf(linebuf);
}   /* dumpmem() */

/***********************************************************************
 *  scsidone()
 *
 *  Wake up processes sleeping on SRB (r).
 */

static int scsidone(r)
register srb_p	r;
{
	wakeup(&(r->status));
}

/***********************************************************************
 *  reqsense()
 *
 *  Issue a request sense command device loaded into the target/lun
 *  fields of the given srb r.  Uses v_sleep().
 */

void reqsense(r)
register srb_p r;
{
	int			 s;
	unsigned short  status;
	unsigned short  tries;
	unsigned short  timeout;
	bufaddr_t	   buf;
	unsigned short  xferdir;
	int			 (*cleanup)();
	cdb_t		   cdb;

	if (r->status == ST_CHKCOND) {
		status = ST_CHKCOND;
		tries = r->tries;
		timeout = r->timeout;
		memcpy(&buf, &(r->buf), sizeof(bufaddr_t));
		xferdir = r->xferdir;
		cleanup = r->cleanup;
		memcpy(&cdb, &(r->cdb), sizeof(cdb_t));

		r->timeout = 4;
		r->buf. space = KRNL_ADDR;
		r->buf. addr. caddr = (caddr_t) r->sensebuf;
		r->buf. size = sizeof(r->sensebuf);
		r->xferdir = DMAREAD;
		r->cleanup = &scsidone;
		memset(&(r->cdb), 0, sizeof(cdb_t));
		r->cdb. g0. opcode = 0x03;
		r->cdb. g0. lun_lba = (r->lun << 5);
		r->cdb. g0. xfr_len = r->buf. size;
		s = sphi();
		startscsi(r);
		while (r->status == ST_PENDING) {
			if (x_sleep(&(r->status), pritape, slpriSigCatch, "reqsense")) {
				u. u_error = EINTR;
				status = ST_USRABRT;
				break;
			}
		}
		spl(s);

		r->tries = tries;
		r->timeout = timeout;
		memcpy(&(r->buf), &buf, sizeof(bufaddr_t));
		r->xferdir = xferdir;
		r->cleanup = cleanup;
		memcpy(&(r->cdb), &(cdb), sizeof(cdb_t));
		r->status = status;
	}
}   /* reqsense() */

/***********************************************************************
 *  doscsi()
 *
 *  An alternative to startscsi() which handles everything including
 *  any request sense commands necessary if the command failed.  All
 *  information is returned in given srb.  Note:  you can only use
 *  this routine when the u structure for a process is available (from
 *  an open, close, read, write, or ioctl routine).  Since this calls
 *  sleep it will screw things up something fierce if you call it from
 *  a load, unload, block, timer or interrupt routine.  Also note
 *  that some host adapters do the sense part this automatically.
 */

void doscsi(r, retrylimit, schedPri, sleepPri, reason)
register srb_p  r;
int			 retrylimit;
int			 schedPri;
int			 sleepPri;
char			reason[];
{
	int	 s;

	r->cleanup = &scsidone;
	for (r->tries = 0; r->tries < retrylimit; ) {
		if (startscsi(r)) {
			s = sphi();
			while (r->status == ST_PENDING) {
				if (x_sleep(&(r->status), schedPri, sleepPri, reason)) {
					abortscsi(r);
					r->status = ST_USRABRT;
					u. u_error = EINTR;
				}
			}
			spl(s);
			if (r->status == ST_GOOD)
				return;

			if (r->status == ST_CHKCOND)
				reqsense(r);
		}
		else
			r->status = ST_TIMEOUT;
	}
}   /* doscsi() */

/***********************************************************************
 *  printsense()
 *
 *  Print out the results in the given sense buffer.  This is done
 *  in English, almost.
 */

void printsense(dev, msg, e)
register dev_t	  dev;
register char	   *msg;
register extsense_p e;
{
	long info;
	if ((e->errorcode & 0x70) != 0x70)
		devmsg(dev, "%s: Bad sensekey", msg);
	else {
		if ((e->errorcode & 0x80) != 0x80)
			devmsg(dev,
				   "%s: %s - key: (0x%x)",
				   msg,
				   errstr[e->sensekey & 0x0f],
				   (e->sensekey & 0xe0));
		else {
			info = (long) e->info;
			flip(info);
			devmsg(dev,
				   "%s: %s - addr: %d key: (0x%x)",
				   msg,
				   errstr[e->sensekey & 0x0f],
				   info,
				   (e->sensekey & 0xe0));
		}
	}
}   /* printsense() */

/***********************************************************************
 *  printerror()
 *  
 *  Print an error after command completion.  Be silent if the command
 *  was aborted by the user.
 */

int printerror(r, msg)
register srb_p r;
register char *msg;
{
	if (r->status == ST_USRABRT)
		return 0;
	else {
		if (r->status != ST_CHKCOND)
			devmsg(r->dev, iofailmsg, msg, r->status);
		else
			printsense(r->dev, msg, r->sensebuf);
		return 1;
	}
}   /* printerror() */

/***********************************************************************
 *  haiioctl()  --	  I/O Controls common to all devices.
 *  
 *  This function provides I/O Control functions common to all SCSI
 *  devices.  The chain of operation should be as follows:
 *  
 *  You:
 *	  1)  Make sure that the device is in an appropriate state to
 *		  perform the I/O Control.  (It might not be a good idea to
 *		  format the disk drive with the root partition).
 *  
 *	  2)  Call haiioctl() with your srb and cmd from I/O Control.
 */

void haiioctl(r, cmd, vec)
register srb_p  r;			  /* Device's srb */
register int	cmd;			/* Command to do */
register char   *vec;		   /* Additional information (if needed) */
{
	haiusercdb_t h;
	
	switch (cmd) {
	case HAIINQUIRY:
	case HAIMDSNS0:
	case HAIMDSLCT0:
	case HAIMDSNS2:
	case HAIMDSLCT2:
		u. u_error = ENXIO;
		return;
	case HAIUSERCDB:
		if (super()) {
			if (!ukcopy(vec, &h, sizeof(haiusercdb_t)))
				return;
			r->buf. space = USER_ADDR;
			r->buf. addr. caddr = vec + sizeof(haiusercdb_t);
			r->buf. size = h. buflen;
			r->xferdir = h. xferdir;
			r->timeout = h. timeout;
			memcpy(&(r->cdb), &(h. cdb), sizeof(cdb_t));
			doscsi(r, 1, pritty, slpriSigCatch, "haiioctl");
			if (!kucopy(r->sensebuf, ((haiusercdb_p) vec)->sensebuf, SENSELEN))
				return;
		}
		return;
	default:
		u. u_error = ENXIO;
		return;
	}
	return;
}   /* haiioctl() */

/***********************************************************************
 *  hainonblk()	 --  Block entry point for devices that shouldn't
 *					  have block entry points.
 *  
 *  Since this is a multiplexing driver and some devices behind it
 *  will have block entry points and some shouldn't.  ALL do.
 */

void hainonblk(bp)
register BUF	*bp;
{
	bp->b_flag |= BFERR;
	bdone(bp);
}   /* hainonblk() */

/*
 * setup_mdca
 *
 * Load mdca table based on globals HAI_DISK and HAI_TAPE.
 */
void
setup_mdca()
{
	int id, mask;
	extern dca_t sddca;
	extern dca_t ctdca;

	for (id = 0; id < 8; id ++) {
		mask = 1 << id;
		if (HAI_DISK & mask)
			mdca[id] = & sddca;
		if (HAI_TAPE & mask)
			mdca[id] = & ctdca;
	}
}

/* End of file */
