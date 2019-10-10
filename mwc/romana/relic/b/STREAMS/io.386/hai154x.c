/***********************************************************************
 *  Module: hai154x.c
 *
 *  Host Adapter Interface routines for the Adaptec AHA154XX series
 *  of host adapters.
 *
 *  Much of the information used to create this driver was obtained
 *  from the AHA1540B/1542B Technical Reference Manual available from
 *  Adaptec by phone or snail mail at:
 *
 *	  Adaptec - Literature Department
 *	  691 South Milpitas Blvd.
 *	  Milpitas, CA 95035
 *	  (408) 945-8600
 *
 *  Copyright (c), 1993 Christopher Sean Hilton, All Rights Reserved.
 *
 *  Last Modified: Sat Jul 24 08:44:54 1993 by [chris]
 *
 */

#define HA_MODULE	   /* Host Adaptor Module */
#define LOUIS_HACK	1	/* defined for Louis' hacks */

#include <stddef.h>
#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/devices.h>
#include <sys/types.h>
#include <sys/hdioctl.h>

#include <sys/haiscsi.h>

/*
 * Configurable variables - see /etc/conf/hai/Space.c
 */
extern unsigned short	AHABASE;
extern unsigned short	AHAINTR;
extern unsigned short	AHADMACHAN;

extern int	HAI_SD_HDS;
extern int	HAI_SD_SPT;

#define CTRLREG	 (AHABASE + 0)   /* Control Register (Write) */
#define	 HRST	bit(7)		  /* Hard Reset */
#define	 SRST	bit(6)		  /* Soft Reset */
#define	 IRST	bit(5)		  /* Interrupt Reset */
#define	 SCRST   bit(4)		  /* SCSI Bus Reset */

#define STSREG	  (AHABASE + 0)   /* Status Register (Read) */
#define	 STST	bit(7)		  /* Self Test in progress */
#define	 DIAGF   bit(6)		  /* Internal Diagnostic Failure */
#define	 INIT	bit(5)		  /* Mailbox Initialization Required */
#define	 IDLE	bit(4)		  /* SCSI Host Adapter Idle */
#define	 CDF	 bit(3)		  /* Command/Data Out Port Full */
#define	 DF	  bit(2)		  /* Data In Port Full */
#define	 INVDCMD bit(0)		  /* Invalid HA Command */

#define CMDDATAOUT  (AHABASE + 1)   /* Command/Data Out (Write) */
#define	 NOP		 0x00		/* No Operation (really?) */
#define	 MBINIT	  0x01		/* Mail Box Initialization */
#define	 STARTSCSI   0x02		/* Start a SCSI Command */
#define	 STARTBIOS   0x03		/* Start a BIOS Command */
#define	 HAINQUIRY   0x04		/* HA Inquiry */
#define	 ENBLMBOA	0x05		/* Enable Mailbox Out Available Interrupt */
#define	 STSELTO	 0x06		/* Set Selection Timeout */
#define	 SETBUSON	0x07		/* Set Bus on time */
#define	 SETBUSOFF   0x08		/* Set Bus off time */
#define	 SETXFER	 0x09		/* Set transfer speed */
#define	 RETINSTDEV  0x0a		/* Return Installed Devices */
#define	 RETCFGDATA  0x0b		/* Return Configuration Data */
#define	 ENBLTRGTMD  0x0c		/* Enable Target Mode */
#define	 RETSUDATA   0x0d		/* Return Setup Data */

#define DATAIN	  (AHABASE + 1)

#define INTRFLGS	(AHABASE + 2)
#define	 ANYINTR bit(7)		  /* Any Interrupt */
#define	 SCRD	bit(3)		  /* SCSI Reset Detected */
#define	 HACC	bit(2)		  /* HA Command Complete */
#define	 MBOA	bit(1)		  /* MBO Empty */
#define	 MBIF	bit(0)		  /* MBI Full */

#define MBOFREE	 0x00		/* Mailbox out is free */
#define MBOSTART	0x01		/* Start CCB in this Mailbox */
#define MBOABORT	0x02		/* Abort CCB in this Mailbox */

#define MBIFREE	 0x00		/* Mailbox in is free */
#define MBISUCCESS  0x01		/* Mailbox's CCB Completed Successfully */
#define MBIABORTED  0x02		/* Mailbox's CCB Aborted */
#define MBIABRTFLD  0x03		/* CCB to Abort not found */
#define MBIERROR	0x04		/* CCB Completed with error */

#define TIMEOUT	 -1		  /* Timeout from pollxxx() functions. */

#define ST_HAINIT   0x0001	  /* Host Adapter being initialized */
#define ST_HAIDLE   0x0002	  /* Host Adapter is idle */

/***********************************************************************
 *  Types
 */

#pragma align 1

typedef union addr3_u {		 /* addr3_u for big/little endian conversions */
	unsigned long   value;
	unsigned char   byteval[sizeof(unsigned long)];
} addr3_t;

typedef union mbo_u *mbo_p;	 /* Out Box to host adapter */

typedef union mbo_u {
	unsigned char   cmd;
	paddr_t		 ccbaddr;
} mbo_t;

typedef union mbi_u *mbi_p;	 /* In Box from host adapter */

typedef union mbi_u {
	unsigned char   sts;
	paddr_t		 ccbaddr;
} mbi_t;

typedef struct mb_s {		   /* Host adapter mailbox type */
	mbo_t	   o[MAXDEVS];	 /* One out box for each device */
	mbi_t	   i[MAXDEVS];	 /* One in box for each possible reply */
} mb_t;

typedef struct haccb_s *haccb_p;	/* Host Adapter Command/Control Block */

typedef struct haccb_s {
	unsigned char   opcode;
	unsigned char   addrctrl;
	unsigned char   cdblen;
	unsigned char   senselen;
	unsigned char   datalen[3];
	unsigned char   bufaddr[3];
	unsigned char   linkaddr[3];
	unsigned char   linkid;
	unsigned char   hoststs;
	unsigned char   trgtsts;
	unsigned char   pad[2];
	cdb_t		   cdb;
} haccb_t;

typedef struct dsentry_s *dsentry_p;

typedef struct dsentry_s {
	unsigned char   size[3];
	unsigned char   addr[3];
} dsentry_t;

typedef struct dslist_s *dslist_p;

typedef struct dslist_s {
	dsentry_t	   entries[16];
} dslist_t;

#pragma align

/***********************************************************************
 *  Variables
 */

static int	  hastate;		/* Host Adapter State */
static mb_t	 mb;			 /* Mailboxes for host adapter */	
static haccb_t  ccb[MAXDEVS];   /* CCB's for mailboxes */
static paddr_t  ccbbase;		/* ccbbase address for quick checkmail */
static srb_p	actv[MAXDEVS];  /* Active srb's for each target */
static dslist_t ds[MAXDEVS];	/* Data segment lists one for each target */
static unsigned chkport = 0,	/* Port for chkset/ckhclr */
				chkstop = 0,	/* Target value for chkset/chkclr */
				chkval  = 0;	/* Value in port chkset/chkclr */
int			 hapresent;	  /* Host adapter present flag */

/***********************************************************************
 *  Support Functions
 */

#define min(a, b)   (((a) <= (b)) ? (a) : (b))

/***********************************************************************
 *  chkclr()	--  Check port (chkport) for bits (chkstop) to be clear.
 *				  If clear return 1 else return 0.  Leave value of
 *				  port in chkval;
 */

static int chkclr()

{
	return ((chkval = inb(chkport)) & chkstop) == 0;
}   /* chkclr() */

/***********************************************************************
 *  chkset()	--  Check port (chkport) for bits (chkstop) to be set.
 *				  If all bits are set return 1 else return 0. Leave
 *				  value of port in chkval.
 */

static int chkset()

{
	return ((chkval = inb(chkport)) & chkstop) == chkstop;
}   /* chkset() */

/***********************************************************************
 *  pollclr()   --  Wait usec milliseconds for bit(s) to clear in a
 *				  port.
 */

static int pollclr(port, bits, usec)
register unsigned   port;	   /* port to watch */
register unsigned   bits;	   /* bits to watch for */
unsigned			usec;	   /* number of milliseconds to wait for */
{
	register s;

	s = sphi();
	chkport = port;
	chkstop = bits;
	busyWait(chkclr, usec);
	spl(s);
	
#if 1   /* DEBUG */
	if ((chkval & bits) == 0)
		return chkval;
	else {
		printf("pollclr: <Timeout Reg: 0x%x mask 0x%x value 0x%x>\n", port, bits, chkval);
		return TIMEOUT;
	}
#else
	return ((chkval & bits) == 0) ? chkval : TIMEOUT;
#endif	
}   /* pollclr() */

/***********************************************************************
 *  pollset()   --  Wait usec milliseconds for bit(s) set in a port.
 */

static int pollset(port, bits, usec)
register unsigned   port;	   /* port to watch */
register unsigned   bits;	   /* bits to watch for */
unsigned			usec;	   /* number of milliseconds to wait for */
{
	register s;

	s = sphi();
	chkport = port;
	chkstop = bits;
	busyWait(chkset, usec);
	spl(s);

#if 1   /* DEBUG */
	if ((chkval & bits) == bits)
		return chkval;
	else {
		printf("pollset: <Timeout Reg: 0x%x mask 0x%x value 0x%x>\n", port, bits, chkval);
		return TIMEOUT;
	}
#else
	return ((chkval & bits) == bits) ? chkval : TIMEOUT;
#endif	
}   /* pollset() */

/***********************************************************************
 *  hacc()
 *
 *  Host Adapter Command Completed - Returns 1 if last host adapter
 *  command completed without error.
 */

static int hacc()

{
	register unsigned stsreg;

#ifdef LOUIS_HACK
	register unsigned louis_stsreg;
	if((louis_stsreg = pollset(INTRFLGS, HACC, 350)) == TIMEOUT)
	 return 0;
	printf("HACC - louis_stsreg: 0x%x\n", louis_stsreg);
#else
	if (pollset(INTRFLGS, HACC, 350) == TIMEOUT)
		return 0;
#endif

	stsreg = pollset(STSREG, IDLE, 350) & (IDLE | INIT | CDF | INVDCMD);
	if (stsreg != IDLE) {
		printf("Aha154x stuck - STSREG: 0x%x\n", stsreg);
		return 0;
	}
	return 1;
}   /* hacc() */

/***********************************************************************
 *  haidle()
 *
 *  Returns 1 if the Idle Bit is on in the adapter status register
 */

#define haidle()	(pollset(STSREG, IDLE, 350) != TIMEOUT)

/***********************************************************************
 *  gethabyte()
 *
 *  Get a byte from the host adapter Data In register.
 */

static int gethabyte()

{
	if (pollset(STSREG, DF, 350) == TIMEOUT) {
		printf("haiscsi: <gethabyte timeout (0x%x) 0x%x 0x%x>\n", STSREG, DF, chkval);
		return TIMEOUT;
	}
	else
		return (inb(DATAIN) & 0xff);
}   /* gethabyte() */

/***********************************************************************
 *  puthabyte()
 *
 *  Write a byte to the host adapter Command/Data Out Register.
 */

static int puthabyte(b)
unsigned char b;

{
	if (pollclr(STSREG, CDF, 350) == TIMEOUT)
		return 0;
	else {
		outb(CMDDATAOUT, b);
		return 1;
	}
}   /* puthabyte() */

/***********************************************************************
 *  is_154x()
 *
 *  Returns a string indicating the type and revision of the AHA154x
 *  host adapter on the bus if it is out there.  Caveat: if something
 *  else is out there that can respond to this message I don't know
 *  what will happen.
 */

static int is_154x(haport)
unsigned haport;
{
	char	buf[4],
			*p;
	int	 ch,
			oldhaport,
			s,
			r = 0;

	oldhaport = AHABASE;
	s = sphi();
	AHABASE = haport;
	if (haidle() && puthabyte(HAINQUIRY)) {
		for (p = buf; p < buf + sizeof(buf); ++p) {
			ch = gethabyte();
			if (ch == TIMEOUT)
				break;
			else
				*p = ch;
		}
		if (p >= buf + sizeof(buf)) {
			outb(CTRLREG, IRST);
			if ((pollset(STSREG, IDLE, 350) & IDLE) == IDLE) {
				if (buf[0] == '\0') {
					printf("AHA-1540 with 16 Head BIOS Detected\n");
					/* SDS_HDS = 16; Should we? */
				}
				r = 1;
			}
		}
	}
	AHABASE = oldhaport;
	spl(s);
	return r;
}   /* is_154x() */

/***********************************************************************
 *  hareset()
 *
 *  Reset the host adapter and leave it ready for operation.
 */

static int hareset()

{
	paddr_t mbaddr;	 /* Mail box array's paddr. */
	int	 mbiok,	  /* Mail box initialization proceding ok */
			stsreg,	 /* local copy of STSREG */
			retval,	 /* return value */
			s;

	retval = 0;
	s = sphi();
	outb(CTRLREG, SRST);
	if ((stsreg = pollclr(STSREG, STST, 350)) == TIMEOUT)
		printf("AHA-154x: Reset failed, host adapter stuck.\n");
	else
#ifdef LOUIS_HACK
	printf("Resetting - stsreg: 0x%x\n", stsreg);
#endif
		if ((stsreg & DIAGF) || (stsreg & (INIT | IDLE)) != (INIT | IDLE))
			printf("AHA-154x: Reset/diagnostics failed.\n");
		else {

		mbaddr = vtop(&mb);
#ifdef LOUIS_HACK
	printf("init commands bytes will be:\n");
	printf(" MBINIT: 0x%x, COUNT: 0x%x, ADDR_MSB: 0x%x, ADDR_2: 0x%x, ADDR_LSB: 0x%x\n", \
		MBINIT, (sizeof(mb) / (sizeof(mbo_t) + sizeof(mbi_t))), ((unsigned char *) &mbaddr)[2], \
		((unsigned char *) &mbaddr)[1], ((unsigned char *) &mbaddr)[0]);
#endif 
			mbiok = 1;
			mbiok &= puthabyte(MBINIT);
			mbiok &= puthabyte(sizeof(mb) / (sizeof(mbo_t) + sizeof(mbi_t)));
			mbiok &= puthabyte(((unsigned char *) &mbaddr)[2]);
			mbiok &= puthabyte(((unsigned char *) &mbaddr)[1]);
			mbiok &= puthabyte(((unsigned char *) &mbaddr)[0]);
			if (!mbiok || !hacc())
				printf("AHA-154x: Mailbox initialization failed.\n");
			else
				retval = 1;
		}
	spl(s);
	return retval;
}   /* hareset() */

/***********************************************************************
 *  dmacascade()
 *
 *  Set the selected (AHADMACHAN) dma channel to cascade mode.
 */

static void dmacascade()

{
	int dmaporta, dmaportb, s;

	s = sphi();
	if (AHADMACHAN == 0) {
		dmaporta = 0x0b;
		dmaportb = 0x0a;
	} else {
		dmaporta = 0xd6;
		dmaportb = 0xd4;
	}
	outb(dmaporta, 0xc0 | (AHADMACHAN & 3));
	outb(dmaportb, (AHADMACHAN & 3));
	spl(s);
}   /* dmacascade() */

/***********************************************************************
 *  checkmail()
 *
 *  Check the incoming mailboxes for messages.  Do this on any Mail
 *  Box In Full interrupt and before you fail a command for timeout.
 *  The code to determine which target device finished by tid is a
 *  bit tricky and relies on the following assumptions:
 *
 *	  1)  The host adapter is installed in a Intel (big-endian)
 *		  machine. Believe it or not there is (are) non-Intel CPU
 *		  ISA bus machines. and the one that I know of is a M68000
 *		  machine where this would not work.
 *
 *	  2)  The kernel's data space is physically contiguous and is
 *		  never swapped out.
 */

static int checkmail()

{
	static int startid = 0;
	int msgs = 0;
	int sts;
	int s;
	register int id;
	register srb_p r;
	register int i = startid;

	do {
		if (mb. i[i]. sts != MBIFREE) {
			s = sphi();
			sts = mb. i[i]. sts;
			flip(mb. i[i]. ccbaddr);
			id = (unsigned) ((mb. i[i]. ccbaddr & 0x00ffffffL) - ccbbase) / sizeof(haccb_t);
			if (r = actv[id]) {
				switch (sts) {
				case MBIABRTFLD:
					devmsg(r->dev, "Command 0x%x abort failed\n", r->cdb. g0. opcode);
				case MBISUCCESS:
				case MBIERROR:
					r->status = ccb[id]. trgtsts;
					break;
				case MBIABORTED:
					devmsg(r->dev, "Command 0x%x aborted successfully\n", r->cdb. g0. opcode);
					r->status = ST_DRVABRT;
					break;
				default:
					printf("Host Adapter Mailbox In value corrupted\n");
					break;
				}

				actv[id] = NULL;
				if (r->cleanup)
					(*(r->cleanup))(r);
				msgs |= bit(id);
			}
			mb. i[i]. ccbaddr = 0;
			spl(s);
		}
		else if (msgs)
			break;
		i = (i + 1) & 7;
	} while (i != startid);
	startid = i;

	return msgs;
}   /* checkmail() */

/***********************************************************************
 *  Driver Interface Functions
 */

/***********************************************************************
 *  hatimer()
 *
 *  Host adapter Timeout handler.
 */

void hatimer()

{
	register int	id;
	register srb_p  r;
	register int	active;
	int			 s;

	s = sphi();
	checkmail();		/* Cleanup any missed interrupts, etc. */
	active = 0;
	for (id = 0; id < MAXDEVS; ++id) {
		if ((r = actv[id]) != NULL && r->timeout != 0) {
			if (--r->timeout == 0) {
				abortscsi(r);
				r->status = ST_DRVABRT;
			}
			else
				active = 1;
		}
	}
	drvl[SCSIMAJOR]. d_time = active;
	spl(s);
}   /* hatimer() */

/***********************************************************************
 *  haintr()
 *
 *  SCSI interrupt handler for host adapter.
 */

void haintr()

{
	int intrflgs;

	intrflgs = inb(INTRFLGS);
	if (intrflgs & ANYINTR)
		outb(CTRLREG, IRST);

	if (hastate == ST_HAINIT)
		return;
	if (intrflgs & MBIF) {
		checkmail();
		return;
	}
}   /* haintr() */

/***********************************************************************
 *  hainit()
 *
 *  Initialize the host adapter for operation.
 */

int hainit()

{
	register int i;

	hastate = ST_HAINIT;
	hapresent = 0;
	printf("Host Adapter Module: AHA-154x v1.1\n");
	if (!is_154x(AHABASE)) {
		printf("hainit() failed: Adaptec AHA-154x not found.\n");
		return 0;
	}

	setivec(AHAINTR, haintr);
	if (!(hapresent = hareset())) {
		printf("hainit() failed: Could not initialize AHA-154x at (0x%x)\n", AHABASE);
		return 0;
	}

	for (i = 0; i < MAXDEVS; ++i) {
		actv[i] = NULL;
		mb. o[i]. ccbaddr = vtop(ccb + i);
		flip(mb. o[i]. ccbaddr);
		mb. o[i]. cmd = MBOFREE;
	}
	ccbbase = vtop(ccb);
	dmacascade();
	hastate = ST_HAIDLE;
	return 1;
}   /* hainit() */

/***********************************************************************
 *  mkdslist()
 *
 *  Make an Adaptec Data Segment list for a Scatter/Gather Request.
 *  Input:	  d - the memory area for the Scatter/Gather List.
 *			  b - the bufaddr structure for the memory block.
 */

static int mkdslist(d, b)
register dsentry_p  d;
bufaddr_p		   b;
{
	paddr_t				 p_start;
	size_t				  p_size;
	paddr_t				 p_next;
	int					 segments = 1;
	register unsigned long  start;
	register unsigned long  end;
	dsentry_p			   l = d;

	switch (b->space) {
	case KRNL_ADDR:	 /* Kernal Address */
	case USER_ADDR:	 /* User Address (Anything goes) */
		start = b->addr. caddr;
		p_start = vtop(start);
		break;
	case SYSGLBL_ADDR:  /* System Global address (yeah) */
		start = b->addr. paddr;
		p_start = P2P(start);
		break;
	case PHYS_ADDR:	 /* Physical Address - (who knows) */
	default:
		return 0;
	}
	end = start + b->size;
	p_size  = min(NBPC - (p_start & (NBPC - 1)), end - start);

	for ( ; ; ) {
		d->size[2] = ((unsigned char *) &p_size)[0];
		d->size[1] = ((unsigned char *) &p_size)[1];
		d->size[0] = ((unsigned char *) &p_size)[2];
		d->addr[2] = ((unsigned char *) &p_start)[0];
		d->addr[1] = ((unsigned char *) &p_start)[1];
		d->addr[0] = ((unsigned char *) &p_start)[2];
		if (start + p_size >= end)
			return segments;

		p_next = (b->space == SYSGLBL_ADDR) ? P2P(start + p_size) :
											  vtop(start + p_size);
		if (p_next == p_start + p_size)
			/* Continue Last Segment */
			p_size += min(NBPC, end - start - p_size);
		else {
			/* Start New Segment */
			p_start = p_next;
			start += p_size;
			p_size = min(NBPC, end - start);
			++d;
			if (++segments > (sizeof(dslist_t) / sizeof(dsentry_t)))
				return 0;
		}
	}
}   /* mkdslist() */

/***********************************************************************
 *  startscsi()
 *
 *  Send a SCSI CDB to a target device on the bus.
 */

int startscsi(r)
register srb_p   r;
{
	register haccb_p	c;
	paddr_t			 bufaddr;
	size_t			  datalen;
	register int		s;

	if (r->target >= MAXDEVS || r->lun >= MAXUNITS) {
		printf("Illegal device ID: %d LUN: %d", r->target, r->lun);
		return 0;
	}

	++r->tries;
	if (actv[r->target]) {
		devmsg(r->dev,
			   "Device busy: old opcode (0x%x) new opcode (0x%x)",
			   ccb[r->target]. cdb. g0. opcode,
			   r->cdb. g0. opcode);
		return 0;
	}

	s = sphi();
	r->status = ST_PENDING;
	c = ccb + r->target;
	memset(c, 0, sizeof(haccb_t));
	c->opcode = 0;			  /* Start SCSI CDB */
	c->addrctrl = (r->target << 5) | (r->lun & 7);
	if (r->xferdir & DMAREAD)  c->addrctrl |= 0x08;
	if (r->xferdir & DMAWRITE) c->addrctrl |= 0x10;
	c->cdblen = cpycdb(&(c->cdb), &(r->cdb));
	c->senselen = 1;

/***********************************************************************
 *  Set up the CCB's Address here. This turned out to be a bit more
 *  complicated than I thought it would be.
 */

	if (r->buf. space == PHYS_ADDR) {
		c->datalen[0] = ((unsigned char *) &(r->buf. size))[2];
		c->datalen[1] = ((unsigned char *) &(r->buf. size))[1];
		c->datalen[2] = ((unsigned char *) &(r->buf. size))[0];
		c->bufaddr[0] = ((unsigned char *) &(r->buf. addr. paddr))[2];
		c->bufaddr[1] = ((unsigned char *) &(r->buf. addr. paddr))[1];
		c->bufaddr[2] = ((unsigned char *) &(r->buf. addr. paddr))[0];
	}
	else {
		datalen = mkdslist(ds[r->target]. entries, &(r->buf));
		if (datalen == 0) {
			printf("SCSI ID %d - Bad Scatter/Gather list\n", r->target);
			spl(s);
			return 0;
		}
		else if (datalen == 1)
			memcpy(c->datalen, ds[r->target]. entries, 6);
		else {
			c->opcode = 2;
			bufaddr = vtop(ds[r->target]. entries);
			datalen *= 6;
			c->datalen[0] = ((unsigned char *) &datalen)[2];
			c->datalen[1] = ((unsigned char *) &datalen)[1];
			c->datalen[2] = ((unsigned char *) &datalen)[0];
			c->bufaddr[0] = ((unsigned char *) &bufaddr)[2];
			c->bufaddr[1] = ((unsigned char *) &bufaddr)[1];
			c->bufaddr[2] = ((unsigned char *) &bufaddr)[0];
		}
	}

	mb. o[r->target]. cmd = MBOSTART;
	if (puthabyte(STARTSCSI) && (inb(STSREG) & INVDCMD) == 0) {
		actv[r->target] = r;
		if (r->timeout)
			drvl[SCSIMAJOR]. d_time = 1;
	}
	else {
		printf("startscsi() failed: Resetting host adapter.\n");
		mb. o[r->target]. cmd = MBOFREE;
		actv[r->target] = NULL;
		r->status = ST_DRVABRT;
		if (r->cleanup)
			(*r->cleanup)(r);
		hapresent = hareset();
		return 0;
	}

	spl(s);
	return 1;
}   /* startscsi() */

/***********************************************************************
 *  abortscsi()
 *
 *  Abort the SCSI Command at a target device on the bus.
 */

void abortscsi(r)
register srb_p   r;
{
	int s,
		tries;

	printf("abortscsi(id: %d opcode: (0x%x))\n", r->target, r->cdb. g0. opcode);
	s = sphi();
	r->timeout = 0;
	for (tries = 10; hapresent && tries > 0; --tries) {
		mb. o[r->target]. cmd = MBOABORT;
		if (puthabyte(STARTSCSI) && (inb(STSREG) & INVDCMD) == 0)
			break;

		printf("abortscsi(): AHA-154x Command start failed.\n");
		hapresent = hareset();
	}

	if (tries <= 0)
		printf("abortscsi() failed: Cannot reach host adapter\n");
	else {
		busyWait(checkmail, 1000);

		if (r->status == ST_PENDING) {
			printf("abortscsi() failed: id %d\n", r->target);
			resetdevice(r->target);

			actv[r->target] = NULL;
			r->status = ST_DRVABRT;
			if (r->cleanup)
				(*r->cleanup)(r);
		}
	}

	spl(s);
}   /* abortscsi() */

/***********************************************************************
 *  resetdevice()
 *
 *  Reset a SCSI target.
 */

void resetdevice(id)
register int id;
{
	register haccb_p	c = &(ccb[HAI_HAID]);
	int				 tries;

	c->opcode = 0x81;
	c->addrctrl = (id << 5);
	for (tries = 10; tries > 0; --tries) {
		if (!hapresent)
			hapresent = hareset();
		if (hapresent) {
			mb. o[HAI_HAID]. cmd = MBOSTART;
			if (puthabyte(STARTSCSI) && (inb(STSREG) & INVDCMD) == 0)
				break;
		}
	}
}   /* resetdevice() */

/***********************************************************************
 *  haihdgeta()	 --  Get disk driver paramters.
 *  
 *	This function is provided to support the HDGETA/HDSETA I/O Controls
 *  so you don't need the old Adaptec SCSI driver to set up the partition
 *  table on initial setup.  There is a catch-22 with this.  You need
 *  to know the drive's geometry in order to set up the partition table
 *  but cannot get the drive's geometry until you have set up the partition
 *  table.  We solve this by using the drive's size and then dividing
 *  down based upon SDS_HDS heads and SDS_SPT sectors per track.  Every
 *  host adapter will want to do this differently.  The Adaptec solution
 *  is the best that I've seen so far. (It allows you to use Huge (1.0
 *  GB) disks under DOS without trouble.
 */

void haihdgeta(hdp, diskcap)
register hdparm_t   *hdp;
register unsigned   diskcap;
{
	unsigned short  ncyl = (unsigned short) (diskcap / (HAI_SD_HDS * HAI_SD_SPT)),
					landc = ncyl,
					rwccp = 0xffff,
					wpcc = 0xffff;

	memset(hdp, 0, sizeof(hdparm_t));
	*((unsigned short *) hdp->ncyl) = ncyl;
	*((unsigned short *) hdp->rwccp) = rwccp;
	*((unsigned short *) hdp->wpcc) = wpcc;
	*((unsigned short *) hdp->landc) = landc;

	hdp->nhead = HAI_SD_HDS;
	if (hdp->nhead > 8)
		hdp->ctrl |= 8;
	hdp->nspt = HAI_SD_SPT;
}   /* haihdseta() */

/***********************************************************************
 *  haihdseta()	 --  Set disk parameters in accordance with HDSETA
 *					  I/O Control.
 *  
 *  Set the disk paramters accordingly for a request from the fdisk
 *  program.  This call really doesn't do anything on the adaptec or
 *  in the SCSI driver in general because SCSI Disks use Logical Block
 *  addressing rather than Cylinder/Head/Track addressing found with
 *  less intelligent Disk drive types.  What this will do is allow
 *  the fdisk program to work so a user can format his disk and install
 *  Coherent on it (A Good Thing).  This boils down to a fancy way to
 *  patch SDS_HDS and SDS_SPT.
 */

void haihdseta(hdp)
register hdparm_t *hdp;
{
	HAI_SD_HDS = hdp->nhead;
	HAI_SD_SPT = hdp->nspt;
}   /* haihdseta() */

/* End of file */
