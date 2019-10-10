/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/*
 * /usr/include/sys/dmac.h
 *
 * Constants for the 8237 DMA controller.
 *
 * Revised: Mon May 24 18:10:51 1993 CDT
 */
#ifndef	__SYS_DMAC_H__
#define	__SYS_DMAC_H__

#define	DMA	0x00			/* Primary 8237 base port */
#define	SDMA	0xC0			/* Secondary 8237 base port */
#define	DMAPAGE	0x80			/* DMA page registers */
#define	CLEARFL	0x0C			/* Clear F/L offset */
#define	SETMASK	0x0A			/* Set DMA mask offset */
#define	SETMODE	0x0B			/* Set DMA mode offset */
#define	RDMEM	0x48			/* Mode, read memory */
#define	WRMEM	0x44			/* Mode, write memory */
#define	MASKOFF	0x00			/* Mask bit off */
#define	MASKON	0x04			/* Mask bit on */

#if 1
/* For compatibility with other DDK's. */
#define DMA_Wrmode      0x48    /* single, read, increment, no auto-init */
#define DMA_Rdmode      0x44    /* single, write, increment, no auto-init */

/* Channels 0-3 are for 8-bit transfers. */
#define DMA_CH0		0
#define DMA_CH1		1
#define DMA_CH2		2
#define DMA_CH3		3

/* Channels 4-7 are for 8-bit transfers. */
#define DMA_CH4		4
#define DMA_CH5		5
#define DMA_CH6		6
#define DMA_CH7		7
#endif

#ifdef _I386
#define DMASEG_SIZE	NBPC		/* Size of a DMA segment.  */
#include <sys/param.h>
/*
 * Identify the dma segment of a physical address.
 */
#define	dmaseg(p)	((p) & ~(NBPC-1))
#else
#define dmaseg(p)	((p)&0xF0000L)
#endif
                               
#endif
