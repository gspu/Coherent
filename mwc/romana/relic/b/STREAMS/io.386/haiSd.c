/***********************************************************************
 *  Module: haisd.c
 *
 *  Unix device driver functions for accessing SCSI hard drives as
 *  block devices.  Conforms to Mark Williams Coherent definition of
 *  the Unix Device Driver interface.
 *
 *  Copyright (c) 1993, Christopher Sean Hilton. All rights reserved.
 *
 *  Last Modified: Mon Jul 26 17:16:43 1993 by [chris]
 *
 *  This code assumes BSIZE == (1 << 9).
 *
 *  $Id: haisd.c,v 2.3 93/08/09 13:45:26 bin Exp Locker: bin $
 *
 *  $Log:	haisd.c,v $
 * Revision 2.3  93/08/09  13:45:26  bin
 * Kernel 82 changes
 * 
 */

#include <stddef.h>
#include <sys/fdisk.h>
#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/inode.h>
#include <sys/stat.h>
#include <sys/sched.h>
#include <sys/sdioctl.h>    /* This is not the Coherent sdioctl.h */
#include <sys/hdioctl.h>    /* All this is to make fdisk work... */
#include <errno.h>

#include <sys/haiscsi.h>

#define REMOVABLE_MEDIA 0       /* Support Removable media? */

#define INQBUFSZ        64

#define SDIDLE          0
#define SDINIT          1
#define SDIO            2
#define SDSENSE         3
#define SDIOCTL         4

#define INQUIRY         0x12
#define GETCAPACITY     0x25
#define REQSENSE        0x03
#define G1READ          0x28
#define G1WRITE         0x2a

typedef struct partlim_s *partlim_p;

typedef struct partlim_s {
    unsigned long   base;       /* base of the partition (blocks) */
    unsigned long   size;       /* size of the partition (blocks) */
} partlim_t;

typedef struct sdctrl_s *sdctrl_p;

typedef struct sdctrl_s {
    unsigned short  state;
    unsigned short  lastclose;
    BUF             *actf,
                    *actl;
    srb_t           srb;
    partlim_t       plim[1 + 4];    /* special device (1) + all partitions (4) */
    BUF             buf;
} sdctrl_t;

static int sdload();            /* Initialize a SCSI device at (id) */
static void sdopen();           /* Open SCSI DASD at (dev) */
static void sdclose();          /* Close SCSI DASD at (dev) */
static void sdblock();          /* Block Entry Point */
static void sdread();           /* Read SCSI DASD at (dev) */
static void sdwrite();          /* Write SCSI DASD at (dev) */
static void sdioctl();          /* I/O Control for DASD. */

extern int nulldev();
extern int nonedev();

static void sdstart();
static void sdfinish();

#define partindex(d)    ((((d) & (SPECIAL | PARTMASK)) == 0x80) ? 0 : ((d) & PARTMASK) + 1)

dca_t sddca = {
    sdopen,         /* Open */
    sdclose,        /* Close */
    sdblock,        /* Block */
    sdread,         /* Read */
    sdwrite,        /* Write */
    sdioctl,        /* Ioctl */
    sdload,         /* Load */
    nulldev,        /* Unload */
    nulldev         /* Poll */
};

#if REMOVABLE_MEDIA
static int rmsdload();          /* Removable Media Disks */

dca_t rmsddca = {
    sdopen,         /* Open */
    sdclose,        /* Close */
    sdblock,        /* Block */
    sdread,         /* Read */
    sdwrite,        /* Write */
    sdioctl,        /* Ioctl */
    rmsdload,       /* Load */
    nulldev,        /* Unload */
    nulldev         /* Poll */
};
#endif

static sdctrl_p sddevs[MAXDEVS];

/***********************************************************************
 *  sdload()
 *
 *  Start up a DASD device at (id).
 *
 *  1)  Make sure that it's a disk drive and that we can support it.
 *  2)  Get its size and blocksize to make sure that we can use it.
 *  3)  Set up a control structure for it.
 */

static int sdload(id)
register int id;
{
    register sdctrl_p c;
    register srb_p r;
    int timeout;
    char inqbuf[INQBUFSZ];
    long diskcap[2];

	_CHIRP('Q', 143);
    c = kalloc(sizeof(sdctrl_t));
    if (!c) {
        printf("\tout of memory in sdload(): ");
        return 0;
    }

    memset(c, 0, sizeof(sdctrl_t));
    c->state = SDINIT;
    r = &(c->srb);
    r->dev = makedev(SCSIMAJOR, SPECIAL | (id << 4));
    r->target = id;
    r->lun = 0;
    r->timeout = 0;
    r->cleanup = NULL;
    r->xferdir = DMAREAD;

    /* Request Sense to clear reset condition. */
    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) r->sensebuf;
    r->buf. size = sizeof(r->sensebuf);
    memset(&(r->cdb), 0, sizeof(cdb_t));
    r->cdb. g0. opcode = REQSENSE;
    r->cdb. g0. xfr_len = sizeof(r->sensebuf);
    startscsi(r);
    timeout = 1000000L;
    while (r->status == ST_PENDING && --timeout > 0L)
        ;

    if (r->status != ST_GOOD) {
        printf("\tRequest sense failed: status (0x%x)\n", r->status);
        kfree(c);
        return 0;
    }

    /* Inquiry to make sure that this is a disk drive */
    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) inqbuf;
    r->buf. size = sizeof(inqbuf);
    memset(&(r->cdb), 0, sizeof(cdb_t));
    r->cdb. g0. opcode = INQUIRY;
    r->cdb. g0. xfr_len = sizeof(inqbuf);
    startscsi(r);
    timeout = 1000000L;
    while (r->status == ST_PENDING && --timeout > 0L)
        ;

    if (r->status != ST_GOOD) {
        printf("\tInquiry failed status: (0x%x)\n", r->status);
        kfree(c);
        return 0;
    }
    if (inqbuf[0] != 0) {
        printf("\tDevice type byte: (0x%x) - not a DASD\n", inqbuf[0]);
        kfree(c);
        return 0;
    }
    else if (inqbuf[1] & 0x80) {
        printf("\tRemovable Media Not supported yet.\n");
        kfree(c);
        return 0;
    }

    /* Get Capacity to set up the drive for use */
    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) diskcap;
    r->buf. size = sizeof(diskcap);
    diskcap[0] = diskcap[1] = 0;
    memset(&r->cdb, 0, sizeof(cdb_t));
    r->cdb. g1. opcode = GETCAPACITY;
    startscsi(r);
    timeout = 1000000L;
    while (r->status == ST_PENDING && --timeout > 0L)
        ;

    if (r->status != ST_GOOD) {
        printf("\tGet Capacity Failed: 0x%x\n", r->status);
        kfree(c);
        return 0;
    }
    flip(diskcap[0]);
    flip(diskcap[1]);
    if (diskcap[1] != BSIZE) {
        printf("\tInvalid Block Size %d Reformat with %d Bytes/Block\n", diskcap[1], BSIZE);
        kfree(c);
        return 0;
    }

    inqbuf[36] = '\0';
    printf("\t%s %d MB\n", (inqbuf + 8), (diskcap[0] + bit(10)) >> 11);
    sddevs[id] = c;
    sddevs[id]->state = SDIDLE;
    sddevs[id]->plim[0]. base = 0;
    sddevs[id]->plim[0]. size = diskcap[0];
    sddevs[id]->actf = sddevs[id]->actl = NULL;
    return 1;
}   /* sdload() */

#if REMOVABLE_MEDIA
static int rmsdload(id)
register int id;
{
    register sdctrl_p c;
    register srb_p r;
    int timeout;
    long diskcap[2];

    c = kalloc(sizeof(sdctrl_t));
    if (!c) {
        printf("\tout of memory in rmsdload(): ");
        return 0;
    }

    memset(c, 0, sizeof(sdctrl_t));
    c->state = SDINIT;
    r = &(c->srb);
    r->dev = makedev(SCSIMAJOR, SPECIAL | (id << 4));
    r->target = id;
    r->lun = 0;
    r->timeout = 0;
    r->cleanup = NULL;
    r->xferdir = DMAREAD;

    /* Request Sense to clear reset condition. */
    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) r->sensebuf;
    r->buf. size = sizeof(r->sensebuf);
    memset(&(r->cdb), 0, sizeof(cdb_t));
    r->cdb. g0. opcode = REQSENSE;
    r->cdb. g0. xfr_len = sizeof(r->sensebuf);
    startscsi(r);
    timeout = 1000000L;
    while (r->status == ST_PENDING && --timeout > 0L)
        ;

    if (r->status != ST_GOOD) {
        printf("\tRequest sense failed: status (0x%x)\n", r->status);
        kfree(c);
        return 0;
    }

    /* Inquiry to make sure that this is a disk drive */
    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) c->inqbuf;
    r->buf. size = sizeof(c->inqbuf);
    memset(&(r->cdb), 0, sizeof(cdb_t));
    r->cdb. g0. opcode = INQUIRY;
    r->cdb. g0. xfr_len = sizeof(c->inqbuf);
    startscsi(r);
    timeout = 1000000L;
    while (r->status == ST_PENDING && --timeout > 0L)
        ;

    if (r->status != ST_GOOD) {
        printf("\tInquiry failed status: (0x%x)\n", r->status);
        kfree(c);
        return 0;
    }
    if (c->inqbuf[0] != 0) {
        printf("\tDevice type byte: (0x%x) - not a DASD\n", c->inqbuf[0]);
        kfree(c);
        return 0;
    }
    else if ((c->inqbuf[1] & 0x80) == 0)
        printf("\tConfiguration error ID %d is fixed media.\n", id);

    /* Get Capacity to set up the drive for use */
    r->buf. space = KRNL_ADDR;
    r->buf. addr. caddr = (caddr_t) diskcap;
    r->buf. size = sizeof(diskcap);
    diskcap[0] = diskcap[1] = 0;
    memset(&r->cdb, 0, sizeof(cdb_t));
    r->cdb. g1. opcode = GETCAPACITY;
    startscsi(r);
    timeout = 1000000L;
    while (r->status == ST_PENDING && --timeout > 0L)
        ;

    if (r->status != ST_GOOD) {
        printf("\tGet Capacity Failed: 0x%x\n", r->status);
        kfree(c);
        return 0;
    }
    flip(diskcap[0]);
    flip(diskcap[1]);
    printf("Get Capacity results count: %d, size %d\n", diskcap[0], diskcap[1]);
/*    if (diskcap[1] != BSIZE) {
        printf("\tInvalid Block Size %d Reformat with %d Bytes/Block\n", diskcap[1], BSIZE);
        kfree(c);
        return 0;
    }
*/
    c->inqbuf[36] = '\0';
    printf("\t%s %d MB\n", (c->inqbuf + 8), (diskcap[0] + bit(10)) >> 11);
    sddevs[id] = c;
    sddevs[id]->state = SDIDLE;
    sddevs[id]->plim[0]. base = 0;
    sddevs[id]->plim[0]. size = diskcap[0];
    sddevs[id]->actf = sddevs[id]->actl = NULL;
    return 1;
}   /* rmsdload() */
#endif

#if 0
/***********************************************************************
 *  sdunload()
 *
 *  Unload routine.  Right now unused so ifdefed out.
 */

static void sdunload(id)
register int id;
{
    if (sddevs[id]) {
        kfree(sddevs[id]);
        sddevs[id] = NULL;
    }
}   /* sdunload() */
#endif

/***********************************************************************
 *  loadptable()
 *
 *  Read the master boot record from the Fixed disk and set the block
 *  limits on the individual partition devices.  Wouldn't it be nice if
 *  there were more than four partition slots available?!
 */

static int loadptable(dev)
register dev_t   dev;
{
    struct fdisk_s      fp[4];
    register sdctrl_p   c;
    register int        i;

    if (!partindex(dev))
        return 0;

    if (fdisk(makedev(major(dev), (minor(dev) & ~PARTMASK) | SPECIAL), fp)) {
        for (c = sddevs[tid(dev)], i = 1; i < 5; ++i) {
            c->plim[i]. base = fp[i-1]. p_base;
            c->plim[i]. size = fp[i-1]. p_size;
        }
        return 1;
    }
    else {
        printf("fdisk failed\n");
        return -1;
    }
}   /* loadptable() */

/***********************************************************************
 *  sdopen()
 *
 *  Open Entry point for SCSI DASD devices.
 */

static void sdopen(dev /*, mode */)
dev_t   dev;
/* int     mode; */
{
    register sdctrl_p c;

    c = sddevs[tid(dev)];
    if (!c || loadptable(dev) == -1) {
        u. u_error = ENXIO;
        return;
    }
    ++c->lastclose;
    return;
}   /* sdopen() */

/***********************************************************************
 *  sdclose()
 *
 *  Close the SCSI DASD device at dev.
 */

static void sdclose(dev)
dev_t   dev;
{
    register sdctrl_p c;

    c = sddevs[tid(dev)];
    if (!c)
        u. u_error = ENXIO;
    else if (c->lastclose)
        --c->lastclose;
}   /* sdclose() */

/***********************************************************************
 *  sdfinish()
 *
 *  Finish up a fixed disk srb.
 */

static void sdfinish(r)
register srb_p r;
{
    register sdctrl_p   c;
    register BUF        *bp;
    extsense_p          e;

    c = sddevs[r->target];
    bp = c->actf;
    switch (c->state) {
    case SDIO:
        switch (r->status) {
        case ST_GOOD:
            bp->b_resid = bp->b_count - r->buf. size;
            break;
        case ST_CHKCOND:
            r->timeout = 4;
            r->buf. space = KRNL_ADDR;
            r->buf. addr. caddr = (caddr_t) r->sensebuf;
            r->buf. size = sizeof(r->sensebuf);
            r->xferdir = DMAREAD;
            memset(&(r->cdb), 0, sizeof(cdb_t));
            r->cdb. g0. opcode = REQSENSE;
            r->cdb. g0. lun_lba = (r->lun << 5);
            r->cdb. g0. xfr_len = r->buf. size;
            if (startscsi(r))
                c->state = SDSENSE;
            return;
        default:
            devmsg(r->dev,
                   "%s failed at block %d: status (0x%x)",
                   (bp->b_req == BREAD) ? "Read" : "Write",
                   bp->b_bno,
                   r->status);
            bp->b_resid = bp->b_count;
            bp->b_flag |= BFERR;
            break;
        }
        break;
    case SDSENSE:
        if (r->status != ST_GOOD)
            devmsg(r->dev, "%s sense failed at block %d",
                       (bp->b_req == BREAD) ? "Read" : "Write",
                       bp->b_bno);
        else {
            e = r->sensebuf;
            printsense(r->dev,
                       (bp->b_req == BREAD) ? "Read failed" : "Write failed",
                       e);
            if ((e->errorcode & 0x70) == 0x70 && (e->sensekey & 0x0f) == 0x01)
                bp->b_resid = bp->b_count - r->buf. size;
            else {
                bp->b_resid = bp->b_count;
                bp->b_flag |= BFERR;
            }
        }
        break;
    default:
        bp->b_resid = bp->b_count;
        bp->b_flag |= BFERR;
        break;
    }
    c->actf = c->actf->b_actf;
    bdone(bp);
    c->state = SDIDLE;
    sdstart(c);
    return;
}   /* sdfinish() */

/***********************************************************************
 *  sdstart()
 *
 *  Start/restart the Fixed disk request queue.
 */

static void sdstart(c)
sdctrl_p c;
{
    register BUF        *bp;
    register g1cmd_p    g1;
    register srb_p  r = &(c->srb);
    unsigned long       blkcnt;
    int                 i;

    if (!(bp = c->actf) || c->state != SDIDLE)
        return;

    i = partindex(bp->b_dev);
    blkcnt = bp->b_count >> 9;
    if (bp->b_bno + blkcnt > c->plim[i]. size)
        blkcnt = c->plim[i]. size - bp->b_bno;

    r->dev = bp->b_dev;
    r->target = tid(bp->b_dev);
    r->lun = lun(bp->b_dev);
    r->timeout = 4;
    r->buf. space = SYSGLBL_ADDR;
    r->buf. addr. paddr = bp->b_paddr;
    r->buf. size = (blkcnt << 9);
    r->xferdir = (bp->b_req == BREAD) ? DMAREAD : DMAWRITE;
    r->cleanup = &sdfinish;
    g1 = &(r->cdb. g1);
    memset(g1, 0, sizeof(cdb_t));
    g1->opcode = (bp->b_req == BREAD) ? G1READ : G1WRITE;
    g1->lun = (r->lun << 5);
    g1->lba = c->plim[i]. base + bp->b_bno;
    flip(g1->lba);
    g1->xfr_len = blkcnt;
    flip(g1->xfr_len);
    if (startscsi(r))
        c->state = SDIO;
}   /* sdstart() */

/***********************************************************************
 *  sdblock()
 *
 *  Block/strategy entry point for SCSI fixed disks.
 */

static void sdblock(bp)
register BUF    *bp;
{
    int i, s;
    register sdctrl_p c;

    c = sddevs[tid(bp->b_dev)];
    i = partindex(bp->b_dev);

    bp->b_resid = bp->b_count;
    if (bp->b_bno > c->plim[i]. size || (bp->b_count & (BSIZE-1)) != 0) {
        bp->b_flag |= BFERR;
        bdone(bp);
        return;
    }

    if (bp->b_bno == c->plim[i]. size) {
        if (bp->b_req != BREAD)
            bp->b_flag |= BFERR;
        bdone(bp);
        return;
    }

    s = sphi();
    bp->b_actf = NULL;
    if (!c->actf)
        c->actf = bp;
    else
        c->actl->b_actf = bp;
    c->actl = bp;
    while (c->state == SDIDLE && c->actf)
        sdstart(c);
    spl(s);
}   /* sdblock() */

/***********************************************************************
 *  sdread()
 *
 *  Read entry point for SCSI DASD devices.
 */

static void sdread(dev, iop)
dev_t       dev;
register IO *iop;
{
    register sdctrl_p c;

    c = sddevs[tid(dev)];
    ioreq(&(c->buf), iop, dev, BREAD, BFIOC | BFRAW);
}   /* sdread() */

/***********************************************************************
 *  sdwrite()
 *
 *  Write entry point for SCSI DASD devices.
 */

static void sdwrite(dev, iop)
dev_t       dev;
register IO *iop;
{
    register sdctrl_p c;

    c = sddevs[tid(dev)];
    ioreq(&(c->buf), iop, dev, BWRITE, BFIOC | BFRAW);
}   /* sdwrite() */

/***********************************************************************
 *  sdioctl()
 *
 *  I/O Control for DASD's right now.  The options here are for the
 *  self configuring SCSI kernel.
 */

static void sdioctl(dev, cmd, vec)
register dev_t  dev;
register int    cmd;
char            *vec;
{
    register sdctrl_p   c = sddevs[tid(dev)];
    int                 s;
    hdparm_t            hdp;

    switch (cmd) {
    case HDGETA:
        haihdgeta(&hdp, c->plim[0]. size);
        kucopy(&hdp, vec, sizeof(hdparm_t));
        break;
    case HDSETA:
        if (ukcopy(vec, &hdp, sizeof(hdparm_t)))
            haihdseta(&hdp);
        break;
    default:
        if (c->lastclose > 1) {
            u. u_error = EACCES;    /* Only one open on this device */
            return;
        }

        s = sphi();
        while (c->state != SDIDLE)
            /* Where is the wakeup for this sleep? */
            if (x_sleep(&(c->state), pridisk, slpriSigCatch, "sdioctl")) {
                u. u_error = EINTR;
                return;
            }
        c->state = SDIOCTL;
        haiioctl(&(c->srb), cmd, vec);
        c->state = SDIDLE;
        spl(s);
        break;
    }
}   /* sdioctl() */

/* End of file */
