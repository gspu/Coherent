#ifndef __FTXREQ_H__
#define __FTXREQ_H__

#include <common/__paddr.h>
#include <common/ccompat.h>

/* Block transfer requests (one per 1 Kbyte block). */
struct FtxReq {
	__paddr_t	ftr_dmaPaddr;	/* Physical RAM address for DMA. */
	int		ftr_blockno;	/* For debugging. */
	char		ftr_type;	/* 0=skip this block; 1=transfer. */
	unsigned char	ftrC;		/* pseudo cylinder */
	unsigned char	ftrH;		/* pseudo head */
	unsigned char	ftrR;		/* pseudo sector */
};

/* Values for the ftr_type field above. */
enum {
	FTXR_SKIP = 100,
	FTXR_XFER = 101
};

/* ftxqueue.c */

int			ftxReqFreeSize	__PROTO	((void));
struct FtxReq *		ftxReqGet	__PROTO	((void));
void	    		ftxReqPut	__PROTO	((struct FtxReq *));
int			ftxReqSize	__PROTO	((void));
void			ftxQinit	__PROTO	((void));

#endif /* __FTXREQ_H__ */
