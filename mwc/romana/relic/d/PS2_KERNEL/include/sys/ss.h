/*
 * ss.h
 *
 * Header for Seagate ST01/ST02 and Future Domain SCSI host adapters.
 *
 * $Log
 */

/*
 * Constants.
 */
#define SS_RAM		0x1800	/* Offset of parameter RAM */
#define SS_CSR		0x1A00	/* Offset of control/status register */
#define SS_DAT		0x1C00	/* Offset of data port */

#define SS_RAM_LEN	128	/* ST0x has 128 bytes of RAM */
#define SS_DAT_LEN	0x400	/* Byte range mapped to data port */
#define SS_SEL_LEN	0x2000	/* Total size of memory-mapped area */

#define WC_ENABLE_SCSI	0x80	/* Write Control (WC) register bits */
#define WC_ENABLE_IRPT	0x40
#define WC_ENABLE_PRTY	0x20
#define WC_ARBITRATE	0x10
#define WC_ATTENTION	0x08
#define WC_BUSY  	0x04
#define WC_SELECT  	0x02
#define WC_SCSI_RESET  	0x01

#define RS_ARBIT_COMPL	0x80	/* Read STATUS (RS) register bits */
#define RS_PRTY_ERROR	0x40
#define RS_SELECT	0x20
#define RS_REQUEST	0x10
#define RS_CTRL_DATA	0x08
#define RS_I_O  	0x04
#define RS_MESSAGE  	0x02
#define RS_BUSY  	0x01

/*
 * Import Functions.
 */

/* functions from ssqueue.c */
extern void ssq_wr_tail();
extern BUF * ssq_rd_head();
extern BUF * ssq_rm_head();

/* functions from ssas.s */
extern void ss_get();
extern int ss_put();
