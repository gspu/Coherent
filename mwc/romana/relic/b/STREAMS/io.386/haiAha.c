/***********************************************************************
 *  Module: aha154x.c
 *
 *  Host Adapter Interface routines for the Adaptec AHA154XX series
 *  of host adapters.
 *
 *  Much of the information used to create this driver was obtained
 *  from the AHA1540B/1542B Technical Reference Manual available from
 *  Adaptec by phone or snail mail at:
 *
 *      Adaptec - Literature Department
 *      691 South Milpitas Blvd.
 *      Milpitas, CA 95035
 *      (408) 945-8600
 *
 *  Copyright (c), 1993 Christopher Sean Hilton, All Rights Reserved.
 *
 *  Last Modified: Fri Jun  4 20:03:17 1993 by [chris]
 */

#define _COH_HA_                /* Host Adaptor Module */

#include <stddef.h>
#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/types.h>

#include <sys/haiscsi.h>

/***********************************************************************
 *  Constants - Patch for your own circumstances.
 */

unsigned short  AHABASE =       0x330,      /* Port Base */
                AHAINTR =       11,         /* Host Adapter Interrupt vector */
                AHADMACHAN =    0x05;       /* DMA Channel */

#define CTRLREG     (AHABASE + 0)   /* Control Register (Write) */
#define     HRST    bit(7)          /* Hard Reset */
#define     SRST    bit(6)          /* Soft Reset */
#define     IRST    bit(5)          /* Interrupt Reset */
#define     SCRST   bit(4)          /* SCSI Bus Reset */

#define STSREG      (AHABASE + 0)   /* Status Register (Read) */
#define     STST    bit(7)          /* Self Test in progress */
#define     DIAGF   bit(6)          /* Internal Diagnostic Failure */
#define     INIT    bit(5)          /* Mailbox Initialization Required */
#define     IDLE    bit(4)          /* SCSI Host Adapter Idle */
#define     CDF     bit(3)          /* Command/Data Out Port Full */
#define     DF      bit(2)          /* Data In Port Full */
#define     INVDCMD bit(0)          /* Invalid HA Command */

#define CMDDATAOUT  (AHABASE + 1)   /* Command/Data Out (Write) */
#define     NOP         0x00        /* No Operation (really?) */
#define     MBINIT      0x01        /* Mail Box Initialization */
#define     STARTSCSI   0x02        /* Start a SCSI Command */
#define     STARTBIOS   0x03        /* Start a BIOS Command */
#define     HAINQUIRY   0x04        /* HA Inquiry */
#define     ENBLMBOA    0x05        /* Enable Mailbox Out Available Interrupt */
#define     STSELTO     0x06        /* Set Selection Timeout */
#define     SETBUSON    0x07        /* Set Bus on time */
#define     SETBUSOFF   0x08        /* Set Bus off time */
#define     SETXFER     0x09        /* Set transfer speed */
#define     RETINSTDEV  0x0a        /* Return Installed Devices */
#define     RETCFGDATA  0x0b        /* Return Configuration Data */
#define     ENBLTRGTMD  0x0c        /* Enable Target Mode */
#define     RETSUDATA   0x0d        /* Return Setup Data */

#define DATAIN      (AHABASE + 1)

#define INTRFLGS    (AHABASE + 2)
#define     ANYINTR bit(7)          /* Any Interrupt */
#define     SCRD    bit(3)          /* SCSI Reset Detected */
#define     HACC    bit(2)          /* HA Command Complete */
#define     MBOA    bit(1)          /* MBO Empty */
#define     MBIF    bit(0)          /* MBI Full */

#define MBOFREE     0x00        /* Mailbox out is free */
#define MBOSTART    0x01        /* Start CCB in this Mailbox */
#define MBOABORT    0x02        /* Abort CCB in this Mailbox */

#define MBIFREE     0x00        /* Mailbox in is free */
#define MBISUCCESS  0x01        /* Mailbox's CCB Completed Successfully */
#define MBIABORTED  0x02        /* Mailbox's CCB Aborted */
#define MBIABRTFLD  0x03        /* CCB to Abort not found */
#define MBIERROR    0x04        /* CCB Completed with error */

#define TIMEOUT     -1          /* Timeout from pollxxx() functions. */

#define ST_HAINIT   0x0001      /* Host Adapter being initialized */
#define ST_HAIDLE   0x0002      /* Host Adapter is idle */

/***********************************************************************
 *  Types
 */

#pragma align 1

typedef union addr3_u {         /* addr3_u for big/little endian conversions */
    unsigned long   value;
    unsigned char   byteval[sizeof(unsigned long)];
} addr3_t;

typedef union mbo_u *mbo_p;     /* Out Box to host adapter */

typedef union mbo_u {
    unsigned char   cmd;
    paddr_t         ccbaddr;
} mbo_t;

typedef union mbi_u *mbi_p;     /* In Box from host adapter */

typedef union mbi_u {
    unsigned char   sts;
    paddr_t         ccbaddr;
} mbi_t;

typedef struct mb_s {           /* Host adapter mailbox type */
    mbo_t       o[MAXDEVS];     /* One out box for each device */
    mbi_t       i[MAXDEVS];     /* One in box for each possible reply */
} mb_t;

typedef struct haccb_s *haccb_p;    /* Host Adapter Command/Control Block */

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
    cdb_t           cdb;
} haccb_t;

typedef struct dsentry_s *dsentry_p;

typedef struct dsentry_s {
    unsigned char   size[3];
    unsigned char   addr[3];
} dsentry_t;

typedef struct dslist_s *dslist_p;

typedef struct dslist_s {
    dsentry_t       entries[16];
} dslist_t;

#pragma align

/***********************************************************************
 *  Variables
 */

static int      hastate;                /* Host Adapter State */
static mb_t     mb;
static haccb_t  ccb[MAXDEVS];
static paddr_t  ccbbase;
static srb_p    actv[MAXDEVS];
static dslist_t ds[MAXDEVS];

void abortscsi();

/***********************************************************************
 *  Support Functions
 */

#define min(a, b)   (((a) <= (b)) ? (a) : (b))

/***********************************************************************
 *  pollclr()
 *
 *  Wait for bit(s) to clear in a port.
 */

static int pollclr(port, bits)
unsigned port;
unsigned bits;

{
    long countdown = 1000000L;
    unsigned pval;

    while (countdown-- > 0L) {
        pval = inb(port) & 0xff;
        if ((pval & bits) == 0)
            return (pval & ~bits);
    }
    printf("pollclr() port 0x%x stuck at 0x%x waiting for 0x%x\n", port, pval, bits);
    return TIMEOUT;
}   /* pollclr() */

/***********************************************************************
 *  pollset()
 *
 *  Wait for bits in port to be set.
 */

static int pollset(port, bits)
unsigned port;
unsigned bits;
{
    long countdown = 1000000L;
    unsigned pval;

    while (countdown-- > 0L) {
        pval = inb(port) & 0xff;
        if ((pval & bits) == bits)
            return pval;
    }
    printf("pollset() port 0x%x stuck at 0x%x waiting for 0x%x\n", port, pval, bits);
    return TIMEOUT;
}   /* pollset() */

/***********************************************************************
 *  hacc()
 *
 *  Host Adapter Command Completed - Returns 1 if last host adapter
 *  command completed without error.
 */

static int hacc()

{
    unsigned stsreg;

    if (pollset(INTRFLGS, HACC) == TIMEOUT) {
        printf("Timeout waiting for Host Adapter Command Complete\n");
        return 0;
    }
    stsreg = inb(STSREG) & (IDLE | INIT | CDF | INVDCMD);
    if (stsreg != IDLE) {
        printf("Host Adapter Stuck - STSREG: 0x%x\n", stsreg);
        return 0;
    }
    return 1;
}   /* hacc() */

/***********************************************************************
 *  haidle()
 *
 *  Returns 1 if the Idle Bit is on in the adapter status register
 */

#define haidle()    (pollset(STSREG, IDLE) != TIMEOUT)

/***********************************************************************
 *  gethabyte()
 *
 *  Get a byte from the host adapter Data In register.
 */

static int gethabyte()

{
    return (pollset(STSREG, DF) == TIMEOUT) ? TIMEOUT : (inb(DATAIN) & 0xff);
}   /* gethabyte() */

/***********************************************************************
 *  puthabyte()
 *
 *  Write a byte to the host adapter Command/Data Out Register.
 */

static int puthabyte(b)
unsigned char b;

{
    if (pollclr(STSREG, CDF) == TIMEOUT)
        return 0;
    else {
        outb(CMDDATAOUT, b);
        return 1;
    }
}   /* puthabyte() */

/***********************************************************************
 *  isaha154x()
 *
 *  Returns a string indicating the type and revision of the AHA154x
 *  host adapter on the bus if it is out there.  Caveat: if something
 *  else is out there that can respond to this message I don't know
 *  what will happen.
 */

static char *isaha154x(haport)
unsigned haport;

{
    /*                      0123456789012345678901234 */
    static char brdid[] =  "Adaptec AHA-154x# Rev: ##";
    char buf[4], *p, *idstr = NULL;
    int ch, oldhaport, s;

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
            idstr = (inb(STSREG) & IDLE != IDLE) ? NULL : brdid;
            brdid[23] = buf[2];
            brdid[24] = buf[3];
            switch (buf[0]) {   /* Board Type Byte */
                case 'B':   /* AHA-1640 with 64 Head BIOS */
                    brdid[13] = '6';
                case '\0':  /* AHA-1540 with 16 head BIOS */
                case '0':   /* AHA-1540 with 64 head BIOS */
                    brdid[16] = ' ';
                    break;
                case 'A':   /* AHA-1540B with 64 head BIOS */
                    brdid[16] = 'B';
                    break;
                default:
                    idstr = "AHA-154x Conformant Host Adapter";
                    break;
            }
        }
    }
    AHABASE = oldhaport;
    spl(s);
    return idstr;
}   /* isaha154x() */

/***********************************************************************
 *  hareset()
 *
 *  Reset the host adapter and leave it ready for operation.
 */

static char *hareset()

{
    addr3_t mbaddr;
    int mbinitok, stsreg, i, s;
    char *p;

    if (p = isaha154x(AHABASE)) {
        s = sphi();
        outb(CTRLREG, SRST);
        if ((stsreg = pollclr(STSREG, STST)) == TIMEOUT)
            panic("SCSI: Timeout waiting for Host Adapter Initialization\n");

        if ((stsreg & DIAGF) || (stsreg & (INIT | IDLE)) != (INIT | IDLE))
            panic("SCSI: Host Adapter Initialization Failed.\n");

        memset(&mb, 0, sizeof(mb));
        mbaddr. value = vtop(&mb);
        mbinitok = 1;
        mbinitok &= puthabyte(MBINIT);
        mbinitok &= puthabyte(sizeof(mb) / (sizeof(mbo_t) + sizeof(mbi_t)));
        mbinitok &= puthabyte(mbaddr. byteval[2]);
        mbinitok &= puthabyte(mbaddr. byteval[1]);
        mbinitok &= puthabyte(mbaddr. byteval[0]);
        if (mbinitok && hacc())
            for (i = 0; i < MAXDEVS; ++i) {
                actv[i] = NULL;
                mb. o[i]. ccbaddr = vtop(ccb + i);
                flip(mb. o[i]. ccbaddr);
                mb. o[i]. cmd = MBOFREE;
            }
        else
            panic("SCSI: AHA-154XB Mailbox Initialization Failed.\n");

        ccbbase = vtop(ccb);
        spl(s);
    }
    return p;
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
 *      1)  The host adapter is installed in a Intel (big-endian)
 *          machine. Believe it or not there is (are) non-Intel CPU
 *          ISA bus machines. and the one that I know of is a M68000
 *          machine where this would not work.
 *
 *      2)  The kernel's data space is physically contiguous and is
 *          never swapped out.
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
                case MBISUCCESS:
                case MBIERROR:
                case MBIABRTFLD:
                    r->status = ccb[id]. trgtsts;
                    break;
                case MBIABORTED:
                    r->status = ST_DRVABRT;
                    break;
                default:
                    panic("Host Adapter Mailbox In value corrupted\n");
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
    register int    id;
    register srb_p  r;
    register int    active;
    int             s;

    s = sphi();
    checkmail();        /* Cleanup any missed interrupts, etc. */
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
    char *p;

    hastate = ST_HAINIT;
    printf("Host Adapter Module: AHA-154x v1.0.0 beta\n");
    setivec(AHAINTR, haintr);
    p = hareset();
    if (!p)
        panic("AHA154x not found or init failed at port: 0x%x\n", AHABASE);
    else
        printf("Initializing %s at port 0x%x, IRQ 0x%x, DMA 0x%x\n", p, AHABASE, AHAINTR, AHADMACHAN);

    dmacascade();
    hastate = ST_HAIDLE;
    return 1;
}   /* hainit() */

/***********************************************************************
 *  mkdslist()
 *
 *  Make an Adaptec Data Segment list for a Scatter/Gather Request.
 *  Input:      d - the memory area for the Scatter/Gather List.
 *              b - the bufaddr structure for the memory block.
 */

static int mkdslist(d, b)
register dsentry_p  d;
bufaddr_p           b;
{
    paddr_t                 p_start;
    size_t                  p_size;
    paddr_t                 p_next;
    int                     segments = 1;
    register unsigned long  start;
    register unsigned long  end;
    dsentry_p               l = d;

    switch (b->space) {
    case KRNL_ADDR:     /* Kernal Address */
    case USER_ADDR:     /* User Address (Anything goes) */
        start = b->addr. vaddr;
        p_start = vtop(start);
        break;
    case SYSGLBL_ADDR:  /* System Global address (yeah) */
        start = b->addr. paddr;
        p_start = P2P(start);
        break;
    case PHYS_ADDR:     /* Physical Address - (who knows) */
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
    register haccb_p    c;
    paddr_t             bufaddr;
    size_t              datalen;
    register int        s;

    if (r->target >= MAXDEVS || r->lun >= MAXUNITS) {
        printf("Illegal device ID: %d LUN: %d", r->target, r->lun);
        return 0;
    }

    ++r->tries;
    if (actv[r->target]) {
        devmsg(r->dev, "Device busy: opcode (0x%x)", r->cdb. g0. opcode);
        return 0;
    }

    s = sphi();
    r->status = ST_PENDING;
    c = ccb + r->target;
    memset(c, 0, sizeof(haccb_t));
    c->opcode = 0;              /* Start SCSI CDB */
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
    else
        panic("startscsi(): AHA-154x Command Start Failure\n");

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
        spin;

    printf("abortscsi(id: %d opcode: (0x%x)\n", r->target, r->cdb. g0. opcode);
    s = sphi();
    r->timeout = 0;
    mb. o[r->target]. cmd = MBOABORT;
    if (!puthabyte(STARTSCSI) || (inb(STSREG) & INVDCMD) != 0)
        panic("abortscsi(): AHA-154x Command Start Failure\n");

    spin = 0x200001;
    while (r->status == ST_PENDING && --spin > 0L)
        if ((spin & 0xFFF) == 0)
            checkmail();

    if (r->status == ST_PENDING) {
        if (r->xferdir == DMAREAD &&
            r->buf. space != KRNL_ADDR &&
            r->buf. size > 0)
            panic("abortscsi failed at id: %d\n", r->target);
        else {
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
 *  Reset a SCSI target.  This is not used yet so it isn't coded but if
 *  you need to use it all you need to do is set up a CCB with opcode =
 *  0x81. Then send it to the Host Adapter.  It will do the rest.
 */

int resetdevice(id)
int id;
{
    panic("resetdevice() Called for ID: %d\n", id);
}   /* resetdevice() */

/* End of file */
