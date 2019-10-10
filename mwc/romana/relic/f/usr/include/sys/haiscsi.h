/***********************************************************************
 *  Module: haiscsi.h
 *
 *  Constants and structures used to access SCSI devices through the
 *  SCSI Driver in a Host Adapter inspecific manner.
 *
 *  Copyright (c) 1993, Christopher Sean Hilton, All rights reserved.
 *
 *  Last Modified: Fri Jul 23 15:38:08 1993 by [chris]
 *
 */

#ifndef __SYS_HAISCSI_H__
#define __SYS_HAISCSI_H__

#define SCSIMAJOR   13

#define MAXTID      7
#define MAXDEVS     (MAXTID + 1)
#define MAXLUN      7
#define MAXUNITS    (MAXLUN + 1)

#define ST_GOOD     0x00    /* Status Good. */
#define ST_CHKCOND  0x02    /* Check Condition */
#define ST_CONDMET  0x04    /* Condition Met */
#define ST_BUSY     0x08    /* Busy */
#define ST_INTERM   0x10    /* Intermediate */
#define ST_INTCDMET 0x14    /* Intermediate Condtion Met */
#define ST_RESCONF  0x18    /* Reservation Conflict */
#define ST_COMTERM  0x22    /* Command Terminated */
#define ST_QFULL    0x28    /* Queue Full */

#define ST_TIMEOUT  0x0101  /* Command Timed out */
#define ST_USRABRT  0x0102  /* User pressed ^C */
#define ST_DRVABRT  0x0103  /* Command Aborted by driver */
#define ST_HATMOUT  0x0201  /* Host adapter Timed out command */
#define ST_PENDING  0xffff  /* Command Pending */

#define DMAREAD     0x0001  /* Command Reads from SCSI device */
#define DMAWRITE    0x0002  /* Command Writes to SCSI device */

#define SENSELEN    18

#define PHYS_ADDR       0x0000      /* Physical Address - (who knows) */
#define KRNL_ADDR       0x0001      /* Kernal Address */
#define USER_ADDR       0x0002      /* User Address (Anything goes) */
#define SYSGLBL_ADDR    0x0003      /* System Global address (yeah) */

/***** Minor Device Number Bits *****/

#define SPECIAL     0x80    /* Special Bit to flag boot block / Tape */
#define TIDMASK     0x70
#define LUNMASK     0x0c
#define PARTMASK    0x03
#define TAPE        0x01
#define REWIND      0x02

#pragma align 1

typedef struct g0cmd_s *g0cmd_p;

typedef struct g0cmd_s {
    unsigned char   opcode;     /* From opcode Table */
    unsigned char   lun_lba;    /* LUN and high part of LBA */
    unsigned char   lba_mid;    /* LBA Middle. */
    unsigned char   lba_low;    /* LBA Low. */
    unsigned char   xfr_len;    /* Transfer Length */
    unsigned char   control;    /* Control byte. */
} g0cmd_t;

typedef struct g1cmd_s *g1cmd_p;

typedef struct g1cmd_s {
    unsigned char   opcode;     /* From opcode Table */
    unsigned char   lun;        /* LUN */
    unsigned long   lba;        /* LBA */
    unsigned char   pad1;       /* Reserved */
    unsigned short  xfr_len;    /* Transfer Length's MSB. */
    unsigned char   control;    /* Control byte. */
} g1cmd_t;

#define g2cmd_t g1cmd_t         /* SCSI-2 Added Group 2 commands */
#define g2cmd_s g1cmd_s         /* with the same size and layout as */
#define g2cmd_p g1cmd_p         /* g1 commands. */

typedef struct g5cmd_s *g5cmd_p;

typedef struct g5cmd_s {
    unsigned char   opcode;     /* From opcode Table */
    unsigned char   lun;        /* LUN */
    unsigned long   lba;        /* LBA's MSB */
    unsigned char   pad1[3];    /* Reserved */
    unsigned short  xfr_len;    /* Transfer Length */
    unsigned char   control;    /* Control byte. */
} g5cmd_t;

typedef union cdb_u *cdb_p;

typedef union cdb_u {
    g0cmd_t g0;
    g1cmd_t g1;
    g5cmd_t g5;
} cdb_t;

typedef struct sense_s *sense_p;

typedef struct sense_s {
    unsigned char   errorcode;  /* Error Code:  0x0? */
    unsigned char   lba_msb;    /* LSB's MS 5 Bits */
    unsigned char   lba_mid;    /*       Middle 8 bits */
    unsigned char   lba_lsb;    /*       LS 8 Bits */
} sense_t;

typedef struct extsense_s *extsense_p;

typedef struct extsense_s {
    unsigned char   errorcode;      /* Error Code (70H) */
    unsigned char   segmentnum;     /* Number of current segment descriptor */
    unsigned char   sensekey;       /* Sense Key(See bit definitions too) */
    long            info;           /* Information MSB */
    unsigned char   addlen;         /* Additional Sense Length */
    unsigned char   addbytes[1];    /* Additional Sense unsigned chars */
} extsense_t;

#ifdef __KERNEL__
/***** Device Control Array *****/

typedef struct dca_s *dca_p;

typedef struct dca_s {
    int (*d_open)();            /* open routine for device */
    int (*d_close)();           /* close routine */
    int (*d_block)();           /* Block request routine (Strategy) */
    int (*d_read)();            /* Character Read routine */
    int (*d_write)();           /* Character Write routine */
    int (*d_ioctl)();           /* I/O Control routine */
    int (*d_load)();            /* Load or Init routine */
    int (*d_unload)();          /* Unload routine */
    int (*d_poll)();            /* Poll routine */
} dca_t;

typedef struct bufaddr_s *bufaddr_p;

typedef struct bufaddr_s {
    int             space;      /* Address space */
    union {
        paddr_t     paddr;      /* Physical Address */
        caddr_t     caddr;      /* Virtual Address */
    } addr;
    size_t          size;       /* Size of buffer */
} bufaddr_t;

typedef struct srb_s *srb_p;    /* SCSI Request Block */

typedef struct srb_s {
    unsigned short  status;     /* SCSI Status Byte */
    unsigned short  hastat;     /* Host Adapter Status Byte */
    dev_t           dev;        /* Device number (major/minor) */
    unsigned char   target;     /* Target ID */
    unsigned char   lun;        /* Logical Unit Number */
    unsigned short  tries;      /* Current tries */
    unsigned short  timeout;    /* Seconds til timeout */
    bufaddr_t       buf;        /* Buffer to use */
    unsigned short  xferdir;    /* Transfer Direction */
    int             (*cleanup)();   /* Cleanup Function. */
    cdb_t           cdb;        /* Command to execute */
    char            sensebuf[SENSELEN];
} srb_t;

#pragma align

#ifdef HA_MODULE
extern dca_p mdca[MAXDEVS];
#else
extern int hapresent;
#endif

/***********************************************************************
 *  Host Adapter routines.
 *
 *  These must be defined by the host adapter module.  For each individual
 *  routine's functionality see the host adapter module aha154x.c.
 */

extern void hatimer();
extern void haintr();
extern int hainit();
extern int startscsi();
extern void abortscsi();
extern void resetdevice();
extern void haihdgeta();
extern void haihdseta();

extern char iofailmsg[];
extern int  HAI_HAID;

#define bit(n)          (1 << (n))
#define tid(d)          (((d) & TIDMASK) >> 4)
#define lun(d)          (((d) & LUNMASK) >> 2)
#define partn(d)        (((d) & SPECIAL) ? 4 : ((d) & PARTMASK))

char *swapbytes();
#define flip(o)         swapbytes(&(o), sizeof(o))
int cpycdb();
void reqsense();
void doscsi();
void printsense();
int printerror();
void haiioctl();
void hainonblk();
#endif	/* KERNEL */

#endif /* !defined( __SYS_HAISCSI_H__ ) */

/* End of file */
