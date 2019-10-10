/*
 * /usr/include/sys/xl8237.h
 *
 * DMA compatibility for Archive floppy tape.
 * This header will disappear soon!
 *
 * Revised: Mon May 24 17:30:27 1993 CDT
 */
#ifndef _SYS_I8237A_H
#define	_SYS_I8237A_H

#define D37A_MAX_CHAN   8
#define D37A_DFR_ALIGN  0xf
#define D37A_MIN_CHAN   0x0

/*
 * Defines for PC AT DMA controllers.
 * The PC AT has two intel 8237A-5 dma controllers with page registers
 * for each channel, allowing access to the entire 16M address space.
 */

#define DMACH0PG        0x87    /* port address for dma chan. 0 page reg. */
#define DMACH1PG        0x83    /* port address for dma chan. 1 page reg. */
#define DMACH2PG        0x81    /* port address for dma chan. 2 page reg. */
#define DMACH3PG        0x82    /* port address for dma chan. 3 page reg. */
#define DMACH4PG        0x00    /* dummy address for dma chan. 4 page reg. */
#define DMACH5PG        0x8b    /* port address for dma chan. 5 page reg. */
#define DMACH6PG        0x89    /* port address for dma chan. 6 page reg. */
#define DMACH7PG        0x8a    /* port address for dma chan. 7 page reg. */

/*
 * The EISA has a high page address register giving access to the
 *      full 32-bit address space
 */

#define DMACH0HPG       0x487   /* port address for dma chan. 0 high page reg */
#define DMACH1HPG       0x483   /* port address for dma chan. 1 high page reg */
#define DMACH2HPG       0x481   /* port address for dma chan. 2 high page reg */
#define DMACH3HPG       0x482   /* port address for dma chan. 3 high page reg */
#define DMACH4HPG       0x000   /* dumy address for dma chan. 4 high page reg */
#define DMACH5HPG       0x48b   /* port address for dma chan. 5 high page reg */
#define DMACH6HPG       0x489   /* port address for dma chan. 6 high page reg */
#define DMACH7HPG       0x48a   /* port address for dma chan. 7 high page reg */

/*
 * I/O port addresses for controller 1 programming.
 */
#define DMA1BCA0        0x00    /* chan. 0 base and current address */
#define DMA1BCWC0       0x01    /* chan. 0 base and current word count */
#define DMA1BCA1        0x02    /* chan. 1 base and current address */
#define DMA1BCWC1       0x03    /* chan. 1 base and current word count */
#define DMA1BCA2        0x04    /* chan. 2 base and current address */
#define DMA1BCWC2       0x05    /* chan. 2 base and current word count */
#define DMA1BCA3        0x06    /* chan. 3 base and current address */
#define DMA1BCWC3       0x07    /* chan. 3 base and current word count */
#define DMA1RSWWCR      0x08    /* read status reg/write command reg */
#define DMA1WRR         0x09    /* write request register */
#define DMA1WSMR        0x0a    /* write single mask register bit */
#define DMA1WMR         0x0b    /* write mode register */
#define DMA1CBPFF       0x0c    /* clear byte pointer flip flop */
#define DMA1RTRWMC      0x0d    /* read temp reg/write master clear */
#define DMA1CMR         0x0e    /* clear mask register */
#define DMA1WAMRB       0x0f    /* write all mask register bits */

/*
 * I/O port addresses for controller 2 programming.
 */
#define DMA2BCA0        0xc0    /* chan. 0 base and current address */
#define DMA2BCWC0       0xc2    /* chan. 0 base and current word count */
#define DMA2BCA1        0xc4    /* chan. 1 base and current address */
#define DMA2BCWC1       0xc6    /* chan. 1 base and current word count */
#define DMA2BCA2        0xc8    /* chan. 2 base and current address */
#define DMA2BCWC2       0xca    /* chan. 2 base and current word count */
#define DMA2BCA3        0xcc    /* chan. 3 base and current address */
#define DMA2BCWC3       0xce    /* chan. 3 base and current word count */
#define DMA2RSWWCR      0xd0    /* read status reg/write command reg */
#define DMA2WRR         0xd2    /* write request register */
#define DMA2WSMR        0xd4    /* write single mask register bit */
#define DMA2WMR         0xd6    /* write mode register */
#define DMA2CBPFF       0xd8    /* clear byte pointer flip flop */
#define DMA2RTRWMC      0xda    /* read temp reg/write master clear */
#define DMA2CMR         0xdc    /* clear mask register */
#define DMA2WAMRB       0xde    /* write all mask register bits */

/*
 * defines for XENIX compatibility
 */

/*
 * Intel 8237 DMA Controller.
 */

/*
 * DMA I/O Port Assignments.
 */
/* 8 bit channel specific registers on controller 1 */
#define DMA_0ADR        0x00    /* Channel  address register */
#define DMA_0WCNT       0x01    /* Channel  word count */
#define DMA_1ADR        0x02    /* Channel  address register */
#define DMA_1WCNT       0x03    /* Channel  word count */
#define DMA_2ADR        0x04    /* Channel  address register */
#define DMA_2WCNT       0x05    /* Channel  word count */
#define DMA_3ADR        0x06    /* Channel  address register */
#define DMA_3WCNT       0x07    /* Channel  word count */

#define DMACH0CCH       0x401   /* chan. 0 base and current count high */
#define DMACH1CCH       0x403   /* chan. 1 base and current count high */
#define DMACH2CCH       0x405   /* chan. 2 base and current count high */
#define DMACH3CCH       0x407   /* chan. 3 base and current count high */


/* 16 bit channel specific registers on controller 1.
 * Chip A0 connected to bus A1, etc. so even address 
 * increments generated by this controller.  Hence, 
 * 16 bit dma, and only even i/o addresses. Channel 4
 * used to cascade controllers.
 */
#define DMA_4ADR        0xC0    /* (RESERVED) Channel  address register */
#define DMA_4WCNT       0xC2    /* (RESERVED) Channel  word count */
#define DMA_5ADR        0xC4    /* Channel  address register */
#define DMA_5WCNT       0xC6    /* Channel  word count */
#define DMA_6ADR        0xC8    /* Channel  address register */
#define DMA_6WCNT       0xCA    /* Channel  word count */
#define DMA_7ADR        0xCC    /* Channel  address register */
#define DMA_7WCNT       0xCE    /* Channel  word count */

#define DMACH4CCH       0x000   /* dummy chan. 0 base and current count high */
#define DMACH5CCH       0x4c6   /* chan. 1 base and current count high */
#define DMACH6CCH       0x4ca   /* chan. 2 base and current count high */
#define DMACH7CCH       0x4ce   /* chan. 3 base and current count high */

/* DMA controller 1, 8 bit channels */
#define CTL1_CMD        0x08    /* Command reg */
#define CTL1_REQ        0x09    /* request reg */
#define CTL1_STAT       0x08    /* Status reg */
#define CTL1_MASK       0x0A    /* Mask set/reset register */
#define CTL1_MODE       0x0B    /* Mode reg */
#define CTL1_CLFF       0x0C    /* Clear byte pointer first/last flip-flop */
#define CTL1_ALLMASK    0x0F    /* Mask all registers */

#define EISA_DMAIS      0x40a   /* interrupt status regiser */
#define CTL1_SCM        0x40a   /* set chain mode */
#define CTL1_EWM        0x40b   /* extended write mode */

/* DMA controller 2, 16 bit channels */
#define CTL2_CMD        0xD0    /* Command reg */
#define CTL2_REQ        0xD2    /* request reg */
#define CTL2_STAT       0xD0    /* Status reg */
#define CTL2_MASK       0xD4    /* Mask set/reset register */
#define CTL2_MODE       0xD6    /* Mode reg */
#define CTL2_CLFF       0xD8    /* Clear byte pointer first/last flip-flop */
#define CTL2_ALLMASK    0xDE    /* Mask all registers */

#define CTL2_SCM        0x40a   /* set chain mode */
#define CTL2_EWM        0x40b   /* extended write mode */

/* 8 bit channels */
#define DMA_0XADR       0x87    /* Channel 0 address extension reg */
#define DMA_1XADR       0x83    /* Channel 1 address extension reg */
#define DMA_2XADR       0x81    /* Channel 2 address extension reg */
#define DMA_3XADR       0x82    /* Channel 3 address extension reg */

/* 16 bit channels */
#define DMA_5XADR       0x8B    /* Channel 5 address extension reg */
#define DMA_6XADR       0x89    /* Channel 6 address extension reg */
#define DMA_7XADR       0x8A    /* Channel 7 address extension reg */


#define DMA_MSK         0x0A    /* Mask, enable disk, disable others */
#define DMA_CLEAR       0x1A    /* Master clear */
#define IOCR            0x56    /* IO controller */

/*
 * DMA Channels. d_chan field of dmareq.
 */

#define NCHANS          8

/* 8 bit channels */
#define DMA_CH0         0       /* Channel 0 */
#define DMA_CH1         1       /* Channel 1 */
#define DMA_CH2         2       /* Channel 2 */
#define DMA_CH3         3       /* Channel 3 */
/* 16 bit channels */
#define DMA_CH4         4       /* Channel 4 */
#define DMA_CH5         5       /* Channel 5 */
#define DMA_CH6         6       /* Channel 6 */
#define DMA_CH7         7       /* Channel 7 */

/*
 * DMA Masks.
 */
#define DMA_SETMSK      4       /* Set mask bit */
#define DMA_CLRMSK      0       /* Clear mask bit */
#define DMA_RD          1
#define DMA_WR          2
#define DMA_VR          0

/* from memory to device */
#define DMA_Wrmode      0x48    /* single, read, increment, no auto-init */
/* from device to memory */
#define DMA_Rdmode      0x44    /* single, write, increment, no auto-init */
#define DMA_Nomode      0x0C    /* illegal mode */

#define DMAPRI  PRIBIO

/* dma_alloc modes */
#define DMA_BLOCK       0       /* blocking task time allocation */
#define DMA_NBLOCK      1       /* non-blocking task time allocation */

#define EISA_DMA_8      0       /* 8-bit data path */
#define EISA_DMA_16     1       /* 16-bit data path, word count */
#define EISA_DMA_32     2       /* 32-bit data path */
#define EISA_DMA_16B    3       /* 16-bit data path, byte count */

#define EISA_ENCM       4       /* enable chaining mode */
#define EISA_CMOK       8       /* chaining mode completed (OK) */


/*
Channel Address Array - make's life much easier
*/
struct d37A_chan_reg_addr {
        unsigned char   addr_reg;       /* target address register */
        unsigned char   cnt_reg;        /* count register */
        unsigned char   page_reg;       /* page register */
        unsigned char   ff_reg;         /* first-last flipflop */
        unsigned char   cmd_reg;        /* command register */
        unsigned char   mode_reg;       /* mode register */
        unsigned char   mask_reg;       /* mask register */
        unsigned char   stat_reg;       /* status register */
        unsigned char   reqt_reg;       /* request register */
        unsigned short  hpage_reg;      /* high page register */
        unsigned short  hcnt_reg;       /* high count register */
        unsigned short  xmode_reg;      /* extended mode register */
        unsigned short  scm_reg;        /* set chaining mode register */
};

/*
macro to initialize array of d37A_chan_reg_addr structures
*/
#define D37A_BASE_REGS_VALUES \
        {DMA_0ADR, DMA_0WCNT, DMACH0PG, CTL1_CLFF, \
        CTL1_CMD, CTL1_MODE, CTL1_MASK, CTL1_STAT, CTL1_REQ, \
        DMACH0HPG, DMACH0CCH, CTL1_EWM, CTL1_SCM}, \
        {DMA_1ADR, DMA_1WCNT, DMACH1PG,  CTL1_CLFF, \
        CTL1_CMD, CTL1_MODE, CTL1_MASK, CTL1_STAT, CTL1_REQ, \
        DMACH1HPG, DMACH1CCH, CTL1_EWM, CTL1_SCM}, \
        {DMA_2ADR, DMA_2WCNT, DMACH2PG,  CTL1_CLFF, \
        CTL1_CMD, CTL1_MODE, CTL1_MASK, CTL1_STAT, CTL1_REQ, \
        DMACH2HPG, DMACH2CCH, CTL1_EWM, CTL1_SCM}, \
        {DMA_3ADR, DMA_3WCNT, DMACH3PG,  CTL1_CLFF, \
        CTL1_CMD, CTL1_MODE, CTL1_MASK, CTL1_STAT, CTL1_REQ, \
        DMACH3HPG, DMACH3CCH, CTL1_EWM, CTL1_SCM}, \
        {DMA_4ADR, DMA_4WCNT, DMACH4PG,  CTL2_CLFF, \
        CTL2_CMD, CTL2_MODE, CTL2_MASK, CTL2_STAT, CTL2_REQ, \
        DMACH4HPG, DMACH4CCH, CTL2_EWM, CTL2_SCM}, \
        {DMA_5ADR, DMA_5WCNT, DMACH5PG,  CTL2_CLFF, \
        CTL2_CMD, CTL2_MODE, CTL2_MASK, CTL2_STAT, CTL2_REQ, \
        DMACH5HPG, DMACH5CCH, CTL2_EWM, CTL2_SCM}, \
        {DMA_6ADR, DMA_6WCNT, DMACH6PG,  CTL2_CLFF, \
        CTL2_CMD, CTL2_MODE, CTL2_MASK, CTL2_STAT, CTL2_REQ, \
        DMACH6HPG, DMACH6CCH, CTL2_EWM, CTL2_SCM}, \
        {DMA_7ADR, DMA_7WCNT, DMACH7PG,  CTL2_CLFF, \
        CTL2_CMD, CTL2_MODE, CTL2_MASK, CTL2_STAT, CTL2_REQ, \
        DMACH7HPG, DMACH7CCH, CTL2_EWM, CTL2_SCM}

#endif
