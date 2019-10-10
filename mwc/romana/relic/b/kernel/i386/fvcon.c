/* (lgl-
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	All rights reserved.
 -lgl) */
#include <coherent.h>
#include <con.h>
#include <mtype.h>
#include <stat.h>

extern	CON	nlcon[];		/* Null device */
extern	CON	ctcon[];		/* Console terminal */
extern	CON	iscon[];
extern	CON	lpcon[];
extern	CON	flcon[];
extern	CON	a0con[];
extern	CON	a1con[];
extern	CON	atcon[];
extern	CON	rmcon[];

/*
 * Device table.
 */
DRV	drvl[NDRV] = {
	{nlcon},	{ctcon},	{iscon },	{lpcon },  /*  0- 3 */
	{flcon },	{a0con },	{a1con },	{NULL },  /*  4- 7 */
	{rmcon },	{NULL },	{NULL },	{atcon }, /*  8-11 */
	{NULL },	{NULL },	{NULL },	{NULL },  /* 12-15 */
	{NULL },	{NULL },	{NULL },	{NULL },  /* 16-19 */
	{NULL },	{NULL },	{NULL },	{NULL },  /* 20-23 */
	{NULL },	{NULL },	{NULL },	{NULL },  /* 24-27 */
	{NULL },	{NULL },	{NULL },	{NULL }   /* 28 - 31 */
};

/*
 * Time.
 */
struct _TIME_OF_DAY timer ={
	0,				/* Initial time */
	0,				/* Ticks */
	-1*60,				/* Mittel Europa Zeit */
	1				/* Daylight saving time */
};

/*
 * Devices and sizes.
 */
dev_t	rootdev = makedev(4, 14);	/* Root device */
dev_t	pipedev = makedev(4, 14);	/* Pipe device */
dev_t	swapdev = makedev(0, 0);	/* Swap device */
daddr_t	swapbot = 0;			/* Swap base */
daddr_t	swaptop = 0;			/* Swap end */
int	ronflag	= 0;			/* Not read only root device */
int	drvn	= NDRV;			/* Maximum number of devices */
int	mactype	= M_386;		/* Machine type */

/*
 * Flexible param's
 */
int	NCLIST	= 8;	/* 8 clists per installed tty, never run out */
int	ALLSIZE = 7*1024;	/* 7K has been reasonable */
int	NINODE	= 64;		/* More than enough so far */
int	NBUF	= 16;		/* Stingy */
