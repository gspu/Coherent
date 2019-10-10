/* $Header: /x/usr/src/sys/io/RCS/pccon.c,v 1.1 91/12/02 17:15:52 hal Exp $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Configuration table.
 * 80286/80386 Coherent.
 * Minimal system.
 *
 * $Log:	pccon.c,v $
 * Revision 1.1  91/12/02  17:15:52  hal
 * Shipped with COH 3.2.0.
 * 
 */
#include <sys/coherent.h>
#include <sys/con.h>
#include <mtype.h>
#include <sys/stat.h>

extern	CON	nlcon[];		/* Null device */
extern	CON	ctcon[];		/* Console terminal */

/*
 * Device table.
 */
DRV	drvl[NDRV] = {
	{nlcon},	{ctcon},	{NULL },	{NULL },  /*  0 -  3 */
	{NULL },	{NULL },	{NULL },	{NULL },  /*  4 -  7 */
	{NULL },	{NULL },	{NULL },	{NULL },  /*  8 - 11 */
	{NULL },	{NULL },	{NULL },	{NULL },  /* 12 - 15 */
	{NULL },	{NULL },	{NULL },	{NULL },  /* 16 - 19 */
	{NULL },	{NULL },	{NULL },	{NULL },  /* 20 - 23 */
	{NULL },	{NULL },	{NULL },	{NULL },  /* 24 - 27 */
	{NULL },	{NULL },	{NULL },	{NULL }   /* 28 - 31 */
};

/*
 * Time.
 */
TIME timer ={
	0,				/* Initial time */
	0,				/* Ticks */
	8*60,				/* Pacific */
	1				/* Daylight saving time */
};

/*
 * Devices and sizes.
 */
dev_t	rootdev = makedev(0, 0);	/* Root device */
dev_t	pipedev = makedev(0, 0);	/* Pipe device */
dev_t	swapdev = makedev(0, 0);	/* Swap device */
daddr_t	swapbot = 0;			/* Swap base */
daddr_t	swaptop = 0;			/* Swap end */
int	ronflag	= 0;			/* Not read only root device */
int	drvn	= NDRV;			/* Maximum number of devices */
int	mactype	= M_8086;		/* Machine type */

/*
 * Flexible param's
 */
int	NCLIST	= 8;		/* 8 clists per installed tty, never run out */
int	ALLSIZE = 7*1024;	/* 7K has been reasonable */
int	NINODE	= 64;		/* More than enough so far */
int	NBUF	= 16;		/* Stingy */
