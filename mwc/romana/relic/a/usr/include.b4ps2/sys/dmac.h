/* (-lgl
 * 	COHERENT Driver Kit Version 2.0.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef	DMAC_H
#define	DMAC_H

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
