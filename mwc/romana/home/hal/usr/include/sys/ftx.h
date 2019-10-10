#ifndef __FTX_H__
#define __FTX_H__

#include <common/ccompat.h>

/* Producer-consumer state variables. */

struct Ftx {
	int	ftx_dmaDir;		/* Dma direction - read/write. */
	int	ftx_bytesXferred;	/* Bytes transferred on this call. */
	int	ftx_residual;		/* Bytes left after incomplete DMA. */
	int	ftx_blockno;		/* Most recent block transferred. */
	char	ftx_IRQpending;		/* A read/write command is pending. */
	char	ftx_error;		/* FDC error has been detected. */
	char	ftx_DMA_on;		/* DMA has been enabled. */
	char	ftx_timedOut;		/* An FDC command timed out. */
	char	ftx_stopio;		/* Tells IRQ process to stop. */
};

extern __VOLATILE__ struct Ftx ftx;

#endif /* __FTX_H__ */
