/***********************************************************************
 *  Module: haict.c
 *
 *  Unix device driver functions for accessing SCSI tape drives as
 *  character devices.  Conforms to Mark Williams Coherent definition
 *  of the Unix Device Driver interface for Coherent v4.0.1.
 *
 *  The philosophy of this driver is to support basic functions on
 *  the tape drive (read, write, retension, rewind, skip, etc). There
 *  are more features out there for all the SCSI tape drives out there
 *  than I know what to do with.  I leave custom support for these
 *  drives to the people who have them.  To this end this drive will
 *  blindly follow whatever information it can get using Mode Sense
 *  and Read Block Limits CDB's.  These tests are done at open time.
 *  An application can change the operation of the driver by applying
 *  the mode select command through I/O Control mechanism.
 *
 *  Copyright (c) 1993, Christopher Sean Hilton, All Rights Reserved.
 *
 *  Last Modified: Mon Jul 26 17:18:59 1993 by [chris]
 *
 *  $Id: haict.c,v 2.4 93/08/09 13:45:06 bin Exp Locker: bin $
 *
 *  $Log:	haict.c,v $
 * Revision 2.4  93/08/09  13:45:06  bin
 * Kernel 82 changes
 * 
 */

#include <stddef.h>
#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/inode.h>
#include <sys/stat.h>
#include <sys/sched.h>
#include <errno.h>
#include <sys/mtioctl.h>
#include <sys/file.h>		/* Louis */

#include <sys/haiscsi.h>
#include <sys/haiioctl.h>

#define REWINDTAPE      0x01
#define     IMMEDIATE   0x0010
#define REQSENSE        0x03
#define READBLKLMT      0x05
#define READ            0x08
#define WRITE           0x0a
#define WRITEFM         0x10
#define SPACE           0x11
#define     SKIP_BLK    0x00
#define     SKIP_FM     0x01
#define     SKIP_SEQFM  0x02
#define     SKIP_EOT    0x03
#define MODESELECT      0x15
#define ERASE           0x19
#define     ERASE_BLOCK 0x0000
#define     ERASE_TAPE  0x0001
#define MODESENSE       0x1a
#define LOAD            0x1b
#define     RETENSION   0x0020

/***********************************************************************
 *  The Tandberg TDC3600 requires special handling because it doesn't
 *  respond properly to a write when the tape is at the logical end
 *  of tape.  Tape should be organized as follows:
 *
 *  File 1 --------------| File 2-m
 *  DATA[1] | ...DATA[n] | FILEMARK | DATA[1-n] | FILEMARK | FILEMARK |
 *
 *  The double file marks signify the Logical End of Tape.  When a
 *  tape opened in write mode is written to and then closed on the
 *  no rewind device it is necessary to write a Logical End of tape
 *  and then skip backwards over one of the filemarks.  This leaves
 *  the tape positioned between the two filemarks. When this happens
 *  on my Tandberg the drive will then lock up (nice huh?) and fail
 *  to do anything until the tape is rewound or retensioned.
 */


#define CTDIRTY         0x0001
#define CTCLOSING       0x0002

#define CTILI           0x0020      /* Sensekey's Illegal Length Indicator */
#define CTEOM           0x0040      /* Sensekey's End OF Media bit */
#define CTFILEMARK      0x0080      /* Sensekey's Filemark bit */
#define     CTSKMASK    (CTILI | CTEOM | CTFILEMARK)
#define CTRDMD          0x0100      /* we are reading from the tape */
#define CTWRTMD         0x0200      /* we are writing to the tape */

#ifndef HAICACHESZ
/*
 *  There wasn't much of a difference in speed between 32 and 40 block
 *  in my experiance so save as much kalloc memory as possible.
 */

#define HAICACHESZ      (32 * BSIZE)/* 32 Block Cache for each device */
#endif

#ifndef HAICTVERBOSE
/*
 *  HAICTVERBOSE is done as a define because I wanted to be able to use two
 *  tape drives in my system at one time (why?!).  ld complains if it is a
 *  patchable constant.  This problem must be solved but products must also
 *  ship. [csh]
 */

#define HAICTVERBOSE    0x0001      /* Switch console messages on/off */
#endif

typedef enum {
    CTIDLE = 0,
    CTINIT,
    CTFBRD,
    CTVBRD,
    CTFBWRT,
    CTVBWRT,
    CTLASTWRT,
    CTSENSE,
    CTWRITEFM,
    CTSPACE,
    CTREWIND,
    CTERASE,
    CTLOADRETEN,
    CTIOCTL
} ctstate_t;

/* Block Descriptors in the mode sense command. */

typedef struct blkdscr_s *blkdscr_p;

typedef struct blkdscr_s {
    union {
        unsigned char   mediatype;
        unsigned long   totalblocks;
    } mt;
    union {
        unsigned char reserved;
        unsigned long blocksize;
    } rb;
} blkdscr_t;

typedef struct blklim_s *blklim_p;

typedef struct blklim_s {
    unsigned        blmax;      /* Maximum size for Reads/Writes */
    unsigned short  blmin;      /* Minimum size for Reads/Writes */
} blklim_t;

typedef struct ctctrl_s *ctctrl_p;

typedef struct ctctrl_s {
    char            *cache,     /* Transfer Cache */
                    *start;     /* Start of data in cache */
    size_t          cachesize,  /* Size of cache */
                    avail;      /* bytes availaible in cache */
    ctstate_t       state;
    unsigned        block,      /* Block size of device */
                    blmax;      /* Block limits maximum */
    unsigned short  blmin,      /* Block Limits minimum */
                    flags,      /* Flags from device */
                    inuse;      /* In Use flag */
    srb_t           srb;        /* SCSI Request block for transfers */
} ctctrl_t;

static int ctinit();        /* Initialize a SCSI device at (id) */
static void ctopen();       /* Open SCSI tape at (dev) */
static void ctclose();      /* Close a SCSI tape at (dev) */
static void ctread();       /* Read from SCSI tape at (dev) */
static void ctwrite();      /* Write SCSI tape at (dev) */
static void ctioctl();      /* I/O Control routine */
static int fillcache();     /* Fill the tape cache */
static int flushcache();    /* Flush the tape cache */

extern int nulldev();
extern int nonedev();

#define min(a, b)           ((a) < (b) ? (a) : (b))

#ifdef TDC3600
#define ctdca   haict3600
#endif

dca_t ctdca = {
    ctopen,         /* Open */
    ctclose,        /* Close */
    hainonblk,      /* No Block point here but don't just drop Buffers */
    ctread,         /* Read */
    ctwrite,        /* Write */
    ctioctl,        /* Ioctl */
    ctinit,         /* Load */
    nulldev,        /* Unload */
    nulldev         /* Poll */
};

static ctctrl_p         ctdevs[MAXDEVS];

/***********************************************************************
 *  Utility functions.                                                 *
 ***********************************************************************/

#define ctvmsg(l, cmd) { if (HAICTVERBOSE & (l)) { (cmd); } }

/***********************************************************************
 *  ctbusywait()
 *
 *  Wait for the tape drive state to return to idle.  This is easy
 *  for two reasons: 1) With no block entry point its safe to sleep
 *  at any time.  2) We shouldn't really need this anyhow.  This is
 *  unneccessary because without a block routine and with only one
 *  process able to open the tape drive at a time the state of the
 *  tape drive driver is well defined.  So, why is it here you ask?
 *  because one day some user might fork a process that owns the tape
 *  drive.  This would cause 40 days and nights worth of rain etc.
 *  Now all that will happen is both processes will be able to write/read
 *  from the tape drive and the data that they get will be complete
 *  garbage.  However, the kernel will not break.
 */

static int ctbusywait(c, newstate)
register ctctrl_p   c;
register ctstate_t  newstate;
{
    register int    s;
    int             retval;

    s = sphi();
    retval = 1;
    while (c->state != CTIDLE)
        if (x_sleep(&(c->srb. status), pritape, slpriSigCatch, "ctbsywt")) {
            u. u_error = EINTR;
            retval = 0;
            break;
        }
    c->state = newstate;
    spl(s);
    return retval;
}   /* ctbusywait() */

/***********************************************************************
 *  loadtape()
 *
 *  Move the tape to the load point.
 */

static int loadtape(c, opt)
register ctctrl_p c;
int opt;
{
    register srb_p      r = &(c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);

    if (!ctbusywait(c, CTLOADRETEN))
        return 0;
    r->buf. space = (int) r->buf. addr. caddr = r->buf. size = 0;
    r->xferdir = 0;
    r->timeout = 300;
    memset(g0, 0, sizeof(cdb_t));
    g0->opcode = LOAD;
    g0->xfr_len = 1;        /* Move tape to load point. */
    if (opt & IMMEDIATE)
        g0->lun_lba |= 1;
    if (opt & RETENSION)
        g0->xfr_len |= 2;
    doscsi(r, 4, pritape, slpriSigCatch, "loadtape");
    if (r->status != ST_GOOD && printerror(r, "Load failed"))
        u. u_error = EIO;

    c->state = CTIDLE;
    c->flags &= ~(CTFILEMARK | CTEOM);
    return (r->status == ST_GOOD);
}   /* loadtape() */

/***********************************************************************
 *  writefm()
 *
 *  Write Filemarks on the tape.
 */

static void writefm(c, count)
register ctctrl_p c;
int count;
{
    register srb_p      r = &(c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);

    if (!ctbusywait(c, CTWRITEFM))
        return;
    r->buf. space = (int) r->buf. addr. caddr = r->buf. size = 0;
    r->xferdir = 0;
    r->timeout = 40;
    g0->opcode = WRITEFM;
    g0->lun_lba = (r->lun << 5);
    g0->lba_mid = ((unsigned char *) &count)[2];
    g0->lba_low = ((unsigned char *) &count)[1];
    g0->xfr_len = ((unsigned char *) &count)[0];
    g0->control = 0;
    doscsi(r, 4, pritape, slpriSigCatch, "writefm");
    if (r->status != ST_GOOD && printerror(r, "Write filemarks failed"))
        u. u_error = EIO;
    c->state = CTIDLE;
}   /* writefm() */

/***********************************************************************
 *  space()
 *
 *  Space over blocks/filemarks/etc.
 */

static void space(c, count, object)
register ctctrl_p c;
int count;
int object;
{
    register srb_p      r = &(c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);

    if (!ctbusywait(c, CTSPACE))
        return;
    r->buf. space = (int) r->buf. addr. caddr = r->buf. size = 0;
    r->xferdir = 0;
    r->timeout = 300;
    g0->opcode = SPACE;
    g0->lun_lba = (r->lun << 5) | (object & 3);
    g0->lba_mid = ((unsigned char *) &count)[2];
    g0->lba_low = ((unsigned char *) &count)[1];
    g0->xfr_len = ((unsigned char *) &count)[0];
    g0->control = 0;
    doscsi(r, 2, pritape, slpriSigCatch, "space");
    if (r->status != ST_GOOD && printerror(r, "Space failed"))
        u. u_error = EIO;
    c->state = CTIDLE;
}   /* space() */

/***********************************************************************
 *  rewind()
 *
 *  Rewind the tape drive back to the load point.
 */

static void rewind(c, wait)
register ctctrl_p c;
int wait;
{
    register srb_p      r = &(c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);

    if (!ctbusywait(c, CTREWIND))
        return;
    r->buf. space = (int) r->buf. addr. caddr = r->buf. size = 0;
    r->timeout = 300;
    r->xferdir = 0;
    memset(g0, 0, sizeof(cdb_t));
    g0->opcode = REWINDTAPE;
    if (!wait)
        g0->lun_lba = (r->lun << 5) | 1;
    doscsi(r, 2, pritape, slpriSigCatch, "rewind");
    if (r->status != ST_GOOD && printerror(r, "Rewind failed"))
        u. u_error = EIO;
    c->flags = 0;
    c->state = CTIDLE;
}   /* rewind() */

static void erase(c, to_eot)
register ctctrl_p c;
int to_eot;
{
    register srb_p      r = &(c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);

    if (!ctbusywait(c, CTERASE))
        return;
    r->buf. space = (int) r->buf. addr. caddr = r->buf. size = 0;
    r->timeout = 300;
    r->xferdir = 0;
    memset(g0, 0, sizeof(cdb_t));
    g0->opcode = ERASE;
    g0->lun_lba = (r->lun << 5);
    if (to_eot)
        g0->lun_lba |= 1;
    doscsi(r, 2, pritape, slpriSigCatch, "erase");
    if (r->status != ST_GOOD && printerror(r, "Erase failed"))
        u. u_error = EIO;
    if (to_eot)
        c->flags &= ~(CTFILEMARK | CTEOM | CTILI | CTDIRTY);
    c->state = CTIDLE;
}   /* erase() */

/***********************************************************************
 *  Device Driver Entry Point routines.                                *
 ***********************************************************************/

/***********************************************************************
 *  ctinit()
 *
 *  Initialize the tape device at (id).  This doesn't do anything,
 *  not even verify that the drive is there because it could be powered
 *  off.
 */

static int ctinit(id)
register int id;
{
    register ctctrl_p c = kalloc(sizeof(ctctrl_t));

    if (!c) {
        printf("\tTape Driver: Could not allocate control structure.\n");
        return 0;
    }

    printf("\tCoherent SCSI Tape driver v1.1\n");
    memset(c, 0, sizeof(ctctrl_t));
    c->inuse = 0;
    c->cache = c->start = NULL;
    c->cachesize = HAICACHESZ;
    c->srb. target = id;
    c->srb. lun = 0;
    c->state = CTIDLE;
    ctdevs[id] = c;
    return 1;
}

static void ctopen(dev, mode)
dev_t   dev;
int mode;
{
    register ctctrl_p   c = ctdevs[tid(dev)];
    register srb_p      r = &(c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);
    int                 rblerf;         /* read block limits error flag */
    int                 s;
    char                buf[64];
    blkdscr_p           bd = (blkdscr_p) (buf + 4);
    blklim_p            bl = (blklim_p) (buf);

    if (!c) {
        u. u_error = ENXIO;
        return;
    }
    if ((mode != IPR) && (mode != IPW)) {
        u. u_error = EINVAL;
        return;
    }

    s = sphi();

    if (c->inuse) {
        u. u_error = EDBUSY;
        goto done;
    }

    c->inuse = 1;
    c->state = CTINIT;
    r->dev = dev;      /* Save the rewind bit for close. */

/***********************************************************************
 *  Repeat the test unit ready command to the tape drive.  This tells
 *  the state of the tape drive.  Repeating the command also clears out
 *  the request sense condition which comes after each time the user
 *  changes tapes.
 */

    r->timeout = 2;
    r->buf. space = (int) r->buf. addr. caddr = r->buf. size = 0;
    r->xferdir = 0;
    memset(g0, 0, sizeof(cdb_t));       /* Test Unit Ready */
    memset(r->sensebuf, 0, sizeof(r->sensebuf));
    doscsi(r, 4, pritape, slpriSigCatch, "ctopen");

/***********************************************************************
 *  If the command fails there probably wasn't a tape in the drive.
 */

    if (r->status != ST_GOOD) {     /* Is there a tape in the drive? */
        if (r->status != ST_USRABRT) {
            /* Otherwise assume no tape. */
            u. u_error = ENXIO;
            devmsg(r->dev, "Tape drive not ready.");
            goto openfailed;
        }
    }
#ifdef TDC3600

/***********************************************************************
 *  TDC3600's do a retension after all bus device resets and tape changes
 *  so I block in the open routine waiting for that operation to complete.
 *  This is a kludge to fix the author's system.  I anticipate that
 *  other host adapters may not be so kind about retrying commands
 *  until a device reports that it is no longer busy.  Hence the error
 *  message when this command fails. All this really does is allows
 *  me to safely start applications while the initial retension is still
 *  happening [csh].
 */

    else {
        if (r->sensebuf[0] == 0x70 && r->sensebuf[2] == 0x06) {

            /* Yep, The user just put it there */

            r->buf. space = (int) r->buf. addr. caddr = r->buf. size = 0;
            r->xferdir = 0;
            r->timeout = 300;
            memset(g0, 0, sizeof(cdb_t));
            g0->opcode = LOAD;
            g0->xfr_len = 1;        /* Move tape to load point. */
            doscsi(r, 4, pritape, slpriSigCatch, "ctopen");
            if (r->status != ST_GOOD &&
                printerror(r, "Load failed - TDC3600 not ready")) {
                u. u_error = ENXIO;
                goto openfailed;
            }
        }
    }
#endif

    ctvmsg(0x0100, (devmsg(r->dev, "Read block limits.")));
#ifdef TDC3600
    c->blmin = c->blmax = 512;
    ctvmsg(0x0100, (devmsg(r->dev, "Tandberg TDC36XX - Block limits set")));
    rblerf = 0;
#else
    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) bl;
    r->buf. size = sizeof(blklim_t);
    r->xferdir = DMAREAD;
    r->timeout = 2;
    memset(g0, 0, sizeof(cdb_t));
    g0->opcode = READBLKLMT;
    g0->xfr_len = 6;
    doscsi(r, 3, pritape, slpriSigCatch, "ctopen");

/***********************************************************************
 *  This is a bit oblique:  The Read Block limits is used to determine
 *  what the tape drive that we have can do.  I have yet to run into
 *  a tape drive that can do variable mode yet so I'm just winging
 *  this.  In any case all I did was make it okay for the Archive Tape
 *  drives to say "I don't support that command".
 */

    if (rblerf = (r->status != ST_GOOD)) {
        ctvmsg(0x0010, (printerror(r, "Read Block LImits")));
        c->blmax = c->blmin = 0;
    }
    else {
        flip(bl->blmax);    /* SCSI to INTEL order */
        flip(bl->blmin);    /* Ditto */
        c->blmax = (bl->blmax & 0x00ffffff);
        c->blmin = bl->blmin;
    }
#endif

/***********************************************************************
 *  Use mode sense to find out if the tape is write protected and also
 *  to find out what the current tape blocksize is.
 */

    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) buf;
    r->buf. size = sizeof(buf);
    r->xferdir = DMAREAD;
    r->timeout = 2;
    memset(g0, 0, sizeof(cdb_t));
    g0->opcode = MODESENSE;
    g0->xfr_len = sizeof(buf);
    doscsi(r, 3, pritape, slpriSigCatch, "ctopen");
    if (r->status != ST_GOOD) {
        if (printerror(r, "Mode sense failed"))
            u. u_error = EIO;
        goto openfailed;
    }

/***********************************************************************
 *  If tape drive opened in write mode make sure the tape is not write
 *  protected now.
 */

    if (mode == IPW && (buf[2] & 0x80) != 0) {
        devmsg(dev, "Tape is write protected");
        u. u_error = ENXIO;
        goto openfailed;
    }

/***********************************************************************
 *  If mode sense returned any media descriptors take the first one,
 *  it's the default, and use it.
 *
 *  There are two possible pitfalls here:
 *
 *      1) According to SCSI-1 it not an error to return zero block
 *  descriptors.  If you're favorite drive returns zero block descriptors
 *  this driver will fail here.
 *
 *      2)  I am assuming that the first media descriptor returned is
 *  the "current" media type which the drive expects to use.  This
 *  works with SCSI-2 but SCSI-1 says that the first descriptor is
 *  the "default" media type.  SCSI-1 is lacking in the definition
 *  of "default" for my conservative taste (when programming is
 *  concerned).
 *
 *  I would like to put the issue of supporting real oddball tape drives
 *  like my Tandberg to rest and only have one source source file for
 *  scsi tape drives but I don't think that will work.  In the future
 *  there will probably be two or maybe three if DAT proves to be as
 *  much fun as this is.
 */

    if (buf[3]) {   /* If mode sense returned any media descriptors */
        bd->rb. blocksize &= 0xffffff00;
        flip(bd->rb. blocksize);
        c->block = bd->rb. blocksize;
        if (c->block && c->cachesize % c->block)
            c->cachesize -= (c->cachesize % c->block);
    }
    else {
        devmsg(r->dev, "No media descriptors: Contact Mark Williams Tech support");
        u. u_error = ENXIO;
        goto openfailed;
    }
    ctvmsg(0x0001, devmsg(dev, "Blocksize: %d bytes.", c->block));

/***********************************************************************
 *  One last check:  If we aren't using block mode (!c->block)
 *  and we didn't get any block limits then we cannot support this
 *  drive.
 */

    if (!c->block && rblerf) {
        devmsg(r->dev, "<No block limits on variable mode tape drive>");
        devmsg(r->dev, "<Contact Mark Williams Tech Support>");
        u. u_error = ENXIO;
        goto openfailed;
    }

    c->flags = (mode == IPR) ? CTRDMD : CTWRTMD;
    if (c->block) {
        c->cache = kalloc(HAICACHESZ);
        if (!c->cache) {
            devmsg(dev, "Could not allocate tape cache");
            u. u_error = ENOMEM;
            goto openfailed;
        }
        c->avail = (c->flags & CTRDMD) ? 0 : HAICACHESZ;
        c->start = c->cache;
    }
    c->state = CTIDLE;
    goto done;

openfailed:
    c->state = CTIDLE;
    c->inuse = 0;

done:
    spl(s);
}   /* ctopen() */

/***********************************************************************
 *  ctclose()
 *
 *  Close the SCSI Device at (dev).
 */

static void ctclose(dev)
register dev_t  dev;
{
    register ctctrl_p   c = ctdevs[tid(dev)];
    register srb_p      r = &(c->srb);
    int                 s;

    if (!c) {
        u. u_error = ENXIO;
        return;
    }

    s = sphi();
    if (c->block && (c->flags & CTWRTMD)) {
        if (ctbusywait(c, CTLASTWRT))
            flushcache(c);
        c->state = CTIDLE;
    }
    spl(s);
    if (c->cache) {
        kfree(c->cache);
        c->cache = c->start = NULL;
        c->avail = 0;
    }

#ifndef TDC3600
/*
 *  Write two filemarks (Logical End of Tape) and then skip backwards
 *  over one of them to ready for the another write operation on the
 *  no rewind device.  This code guarantees properly formed tapes according
 *  to the wisened old nine-track hacker that I work with.  The problem
 *  is that...
 */
    if (c->flags & CTDIRTY) {
        writefm(c, 1);         /* Write a file mark */
        writefm(c, 1);         /* Write another filemark */
        space(c, -1, SKIP_FM); /* Go back to the first filemark */
    }
#else
/*
 *  ... problem is that my Tandberg considers it an error to do anything
 *  after it has skipped past a filemark. So all commands except load and
 *  rewind fail after the previous code.  The following will work in
 *  all situations but there is a risk that a user's tapes will only
 *  have one filemark at Logical End-of-Tape if the user isn't careful
 *  to use the rewind device the last time he uses tape drive.  This is
 *  only a problem with drives which insist upon doing a Retension each
 *  time the tape is changed or the drive gets reset.
 */
    if (c->flags & CTDIRTY) {
        writefm(c, 1);
        if (r->dev & REWIND)
            writefm(c, 1);
    }
#endif

    if (r->dev & REWIND)
        rewind(c, 0);

    c->inuse = 0;
    return;
}   /* ctclose() */

/***********************************************************************
 *  fillcache() --  Read from the tape into the cache (really?)
 *
 *  return 0 and set u. u_error on any errors.
 */

static int fillcache(c)
register ctctrl_p   c;
{
    register srb_p      r = (&c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);
    size_t              blocks;
    extsense_p          e;
    int                 info;
    int                 retval = 0;

    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) c->cache;
    r->buf. size = HAICACHESZ;
    r->xferdir = DMAREAD;
    r->timeout = 30;
    r->tries = 0;
    g0->opcode = READ;
    g0->lun_lba = (r->lun << 5) | 1;
    blocks = HAICACHESZ / c->block;
    g0->lba_mid = ((unsigned char *) &blocks)[2];
    g0->lba_low = ((unsigned char *) &blocks)[1];
    g0->xfr_len = ((unsigned char *) &blocks)[0];
    g0->control = 0;
    doscsi(r, 1, pritape, slpriSigCatch, "ctblkrd");
    switch (r->status) {
    case ST_GOOD:
        c->start = c->cache;
        c->avail = r->buf. size;
        retval = 1;
        break;
    case ST_CHKCOND:
        e = r->sensebuf;
        if ((e->errorcode & 0x70) == 0x70) {
            info = 0;
            if (e->errorcode & 0x80) {
                info = e->info;
                flip(info);
            }
            if (e->sensekey & (CTFILEMARK | CTEOM)) {
                c->flags |= (e->sensekey & (CTFILEMARK | CTEOM));
                c->start = c->cache;
                c->avail = HAICACHESZ - (info * c->block);
                retval = 1;
                break;
            }
        }
        printsense(r->dev, "Read failed", r->sensebuf);
        u. u_error = EIO;
        retval = 0;
        break;
    case ST_USRABRT:
        u. u_error = EINTR;
        c->start = c->cache;
        c->avail = 0;
        retval = 0;
        break;
    default:
        devmsg(r->dev, "Read failed: status (0x%x)", r->status);
        u. u_error = EIO;
        retval = 0;
        break;
    }
    return retval;
}   /* fillcache() */

/***********************************************************************
 *  ctfbrd()    --  Fixed block read handler.  Reads from the tape
 *                  drive through the cache when the tape drive is
 *                  in fixed block mode.
 */

static void ctfbrd(c, iop)
register ctctrl_p   c;
register IO         *iop;
{
    register size_t reqcount,   /* Total bytes transfered toward request */
                    xfrsize;    /* Current transfer size */
    size_t          total,      /* System global memory total transfer size */
                    size;       /* System global memory current transfer size */

    if (!ctbusywait(c, CTFBRD))
        return;
    reqcount = 0;
    while (iop->io_ioc) {
        xfrsize = min(c->avail, iop->io_ioc);
        if (xfrsize > 0) {
            switch (iop->io_seg) {
            case IOSYS:
                memcpy(iop->io. vbase + reqcount, c->start, xfrsize);
                break;
            case IOUSR:
                kucopy(c->start, iop->io. vbase + reqcount, xfrsize);
                break;
            case IOPHY:
                total = 0;
                while (total < xfrsize) {
                    size = min(xfrsize - total, NBPC);
                    xpcopy(c->start + total,
                           iop->io. pbase + reqcount + total,
                           size,
                           SEG_386_KD | SEG_VIRT);
                    total += size;
                }
                break;
            }
            c->start += xfrsize;
            c->avail -= xfrsize;
            reqcount += xfrsize;
            iop->io_ioc -= xfrsize;
        }
        if (iop->io_ioc) {
            if (c->flags & CTFILEMARK) {
                c->flags &= ~CTFILEMARK;
                break;
            }

            if (c->flags & CTEOM) {
                u. u_error = EIO;
                break;
            }

            if (!fillcache(c))
                break;
        }
    }   /* while */
    c->state = CTIDLE;
}   /* ctfbrd() */

/***********************************************************************
 *  ctvbrd()    --  Variable block read entry point.
 */

static void ctvbrd(c, iop)
register ctctrl_p   c;
IO                  *iop;
{
    register srb_p      r = &(c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);
    size_t              xfrsize;
    extsense_p          e;
    int                 info;

    if (!ctbusywait(c, CTVBRD))
        return;

    if (c->flags & CTEOM) {
        u. u_error = EIO;
        return;
    }
    if (c->flags & CTFILEMARK) {
        c->flags &= ~CTFILEMARK;
        return;
    }
    switch (iop->io_seg) {
    case IOSYS:
        r->buf. space = KRNL_ADDR;
        r->buf. addr. caddr = iop->io. vbase;
        break;
    case IOUSR:
        r->buf. space = USER_ADDR;
        r->buf. addr. caddr = iop->io. vbase;
        break;
    case IOPHY:
        r->buf. space = PHYS_ADDR;
        r->buf. addr. paddr = iop->io. pbase;
        break;
    }
    r->buf. size = xfrsize = iop->io_ioc;
    r->xferdir = DMAREAD;
    r->timeout = 30;
    g0->opcode = READ;
    g0->lun_lba = (r->lun << 5);
    g0->lba_mid = ((unsigned char *) &xfrsize)[2];
    g0->lba_low = ((unsigned char *) &xfrsize)[1];
    g0->xfr_len = ((unsigned char *) &xfrsize)[0];
    g0->control = 0;
    doscsi(r, 1, pritape, slpriSigCatch, "ctvbrd");
    switch (r->status) {
    case ST_GOOD:
        iop->io_ioc -= r->buf. size;
        break;
    case ST_CHKCOND:
        e = r->sensebuf;
        if ((e->errorcode & 0x70) == 0x70) {
            info = 0;
            if (e->errorcode & 0x80) {
                info = (long) e->info;
                flip(info);
            }
            if (e->sensekey & (CTFILEMARK | CTEOM)) {
                c->flags |= (e->sensekey & (CTFILEMARK | CTEOM));
                break;
            }
            else if (e->sensekey & CTILI) {
                devmsg(r->dev,
                       "Read failed buffer size %d blocksize %d",
                       xfrsize,
                       xfrsize - info);
                if (info > 0)
                    iop->io_ioc -= (xfrsize - info);
                else
                    u. u_error = EIO;
                break;
            }
        }
        printsense(r->dev, "Read failed", r->sensebuf);
        u. u_error = EIO;
        break;
    case ST_USRABRT:
        break;
    default:
        devmsg(r->dev, "Read failed: status (0x%x)", r->status);
        u. u_error = EIO;
        break;
    }
    c->state = CTIDLE;
}   /* ctvbrd() */

/***********************************************************************
 *  ctread()    --  OS Read entry point.
 */

static void ctread(dev, iop)
dev_t       dev;
register IO *iop;
{
    register ctctrl_p   c = ctdevs[tid(dev)];

    if (!c) {
        u. u_error = EINVAL;
        return;
    }

    if (c->block)
        ctfbrd(c, iop);
    else
        ctvbrd(c, iop);
}   /* ctread() */

/***********************************************************************
 *  flushcache()    --  flush the data in the cache to the tape.
 *
 *  returns 0 and sets u. u_error on failure else returns 1.
 */

static int flushcache(c)
register ctctrl_p   c;
{
    register srb_p      r = (&c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);
    size_t              xfrsize;
    extsense_p          e;
    int                 info;
    int                 retval = 0;

    if (c->avail >= HAICACHESZ)
        return 1;

    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) c->cache;
    r->buf. size = xfrsize = HAICACHESZ - c->avail;
    r->xferdir = DMAWRITE;
    r->timeout = 30;
    r->tries = 0;
    g0->opcode = WRITE;
    g0->lun_lba = (r->lun << 5);
    if (c->block) {
        g0->lun_lba |= 1;
        xfrsize = (xfrsize + c->block - 1) / c->block;
    }
    g0->lba_mid = ((unsigned char *) &xfrsize)[2];
    g0->lba_low = ((unsigned char *) &xfrsize)[1];
    g0->xfr_len = ((unsigned char *) &xfrsize)[0];
    g0->control = 0;
    doscsi(r, 1, pritape, slpriSigCatch, "ctblkwrt");
    switch (r->status) {
    case ST_GOOD:
        c->start = c->cache;
        c->avail = HAICACHESZ;
        retval = 1;
        break;
    case ST_CHKCOND:
        e = r->sensebuf;
        if ((e->errorcode & 0x70) == 0x70) {
            info = 0;
            if (e->errorcode & 0x80) {
                info = e->info;
                flip(info);
            }
            if (e->sensekey & CTEOM) {
                c->flags |= CTEOM;
                devmsg(r->dev, "End of tape on block write");
            }
        }
        printsense(r->dev, "Read failed", r->sensebuf);
        u. u_error = EIO;
        retval = 0;
        break;
    case ST_USRABRT:
        retval = 0;
        break;
    default:
        devmsg(r->dev, "Read failed: status (0x%x)", r->status);
        u. u_error = EIO;
        retval = 0;
        break;
    }
    return retval;
}   /* flushcache() */

/***********************************************************************
 *  ctfbwrt()   --  Fixed block write.  This should be fast because
 *                  it uses the tapes drives optimum setting and it
 *                  goes through a cache.
 */

static void ctfbwrt(c, iop)
register ctctrl_p   c;
register IO         *iop;
{
    register size_t reqcount,   /* Total bytes transfered */
                    xfrsize;    /* Current transfer size */
    size_t          total,      /* System global memory total transfer size */
                    size;       /* System global memory current transfer size */

    if (!ctbusywait(c, CTFBWRT))
        return;

    reqcount = 0;
    while (iop->io_ioc) {
        xfrsize = min(c->avail, iop->io_ioc);
        if (xfrsize) {
            switch (iop->io_seg) {
            case IOSYS:
                memcpy(c->start, iop->io. vbase + reqcount, xfrsize);
                break;
            case IOUSR:
                ukcopy(iop->io. vbase + reqcount, c->start, xfrsize);
                break;
            case IOPHY:
                total = 0;
                while (total < xfrsize) {
                    size = min(xfrsize - total, NBPC);
                    pxcopy(iop->io. pbase + reqcount + total,
                           c->start + total,
                           size,
                           SEG_386_KD | SEG_VIRT);
                    total += size;
                }
                break;
            }
            c->start += xfrsize;
            c->avail -= xfrsize;
            reqcount += xfrsize;
            iop->io_ioc -= xfrsize;
        }
        if (iop->io_ioc) {
            if (!flushcache(c))
                break;
        }
    }   /* while */
    c->state = CTIDLE;
}   /* ctfbwrt() */

/***********************************************************************
 *  ctvbwrt()   --  Variable block writes.
 */

static void ctvbwrt(c, iop)
register ctctrl_p   c;
register IO         *iop;
{
    register srb_p      r = &(c->srb);
    register g0cmd_p    g0 = &(r->cdb. g0);
    size_t              xfrsize;
    extsense_p          e;
    int                 info;

    if (!ctbusywait(c, CTVBWRT))
        return;

    if (c->blmax && iop->io_ioc > c->blmax) {
        devmsg(r->dev, "Tape Error: maximum read/write size is %d bytes.", c->blmax);
        u. u_error = EIO;
        return;
    }
    switch (iop->io_seg) {
    case IOSYS:
        r->buf. space = KRNL_ADDR;
        r->buf. addr. caddr = iop->io. vbase;
        break;
    case IOUSR:
        r->buf. space = USER_ADDR;
        r->buf. addr. caddr = iop->io. vbase;
        break;
    case IOPHY:
        r->buf. space = PHYS_ADDR;
        r->buf. addr. paddr = iop->io. pbase;
        break;
    }
    xfrsize = min(iop->io_ioc, c->blmin);
    r->buf. size = xfrsize;
    r->xferdir = DMAWRITE;
    r->timeout = 30;
    g0->opcode = WRITE;
    g0->lun_lba = (r->lun << 5);
    g0->lba_mid = ((unsigned char *) &xfrsize)[2];
    g0->lba_low = ((unsigned char *) &xfrsize)[1];
    g0->xfr_len = ((unsigned char *) &xfrsize)[0];
    g0->control = 0;
    doscsi(r, 1, pritape, slpriSigCatch, "ctvbwrt");
    switch (r->status) {
    case ST_GOOD:
        iop->io_ioc -= r->buf. size;
        break;
    case ST_CHKCOND:
        e = r->sensebuf;
        if ((e->errorcode & 0x70) == 0x70) {
            info = 0;
            if (e->errorcode & 0x80) {
                info = (long) e->info;
                flip(info);
            }
            if (e->sensekey & CTEOM) {
                c->flags |= CTEOM;
                devmsg(r->dev, "End of tape");
            }
        }
        printsense(r->dev, "Write failed", r->sensebuf);
        u. u_error = EIO;
        break;
    case ST_USRABRT:
        break;
    default:
        devmsg(r->dev, "Read failed: status (0x%x)", r->status);
        u. u_error = EIO;
        break;
    }
    c->state = CTIDLE;
}   /* ctvbwrt() */

/***********************************************************************
 *  ctwrite()   -- Write entry point for tape drive.
 */

static void ctwrite(dev, iop)
register dev_t  dev;
register IO     *iop;
{
    register ctctrl_p c = ctdevs[tid(dev)];

    if (!c) {
        u. u_error = ENXIO;
        return;
    }

    c->flags |= CTDIRTY;
    if (c->block)
        ctfbwrt(c, iop);
    else
        ctvbwrt(c, iop);
}   /* ctwrite() */

/***********************************************************************
 *  ctioctl()
 *
 *  I/O Control Entry point for Cartridge tape devices.
 *
 *  This function had been modified to allow applications level programs
 *  to select modes and features for the tape drive. As stated above,
 *  the philosophy of this driver is to provide least common denominator
 *  support for all tape drives.  I know that you spend big bucks to
 *  get that (insert your favorite drive brand/model).  If I decide
 *  to support everything out there on the market then I won't be able
 *  to write network drivers, serial drivers, etc.  So if you need
 *  to do something to the tape drive to make it work (mode sense/select)
 *  you can do it through this ctioctl as an applications program.
 */

static void ctioctl(dev, cmd, vec)
dev_t           dev;
register int    cmd;
char            *vec;
{
    register ctctrl_p   c = ctdevs[tid(dev)];
    register srb_p      r = &(c->srb);
    int                 s;

    if (!c) {
        u. u_error = EINVAL;
        return;
    }

    switch (cmd) {
    case MTREWIND:      /* Rewind */
        rewind(c, 1);
        break;
    case MTWEOF:        /* Write end of file mark */
        writefm(c, 1);
        break;
    case MTRSKIP:       /* Record skip */
        space(c, 1, SKIP_BLK);
        break;
    case MTFSKIP:       /* File skip */
        space(c, 1, SKIP_FM);
        break;
    case MTTENSE:       /* Tension tape */
        loadtape(c, RETENSION);
        break;
    case MTERASE:       /* Erase tape */
        erase(c, ERASE_TAPE);
        break;
    case MTDEC:         /* DEC mode */
    case MTIBM:         /* IBM mode */
    case MT800:         /* 800 bpi */
    case MT1600:        /* 1600 bpi */
    case MT6250:        /* 6250 bpi */
        return;
    default:
        if (!ctbusywait(c, CTIOCTL))
            return;
        s = sphi();
        haiioctl(r, cmd, vec);
        c->state = CTIDLE;
        spl(s);
        break;
    }
}   /* ctioctl() */

/* End of file */
