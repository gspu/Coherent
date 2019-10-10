/* (lgl-
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	All rights reserved.
 -lgl) */
#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/ino.h>
#include <mtype.h>
#include <sys/stat.h>

extern	CON	atcon[];
extern	CON	ctcon[];		/* Console terminal */
extern	CON	fdccon[];
extern	CON	iscon[];
extern	CON	lpcon[];
extern	CON	msgcon[];
extern	CON	nlcon[];		/* Null device */
extern	CON	ptycon[];		/* pseudotty's */
extern	CON	rmcon[];
extern	CON	semcon[];
extern	CON	shmcon[];

/*
 * Device table.
 */
DRV	drvl[NDRV] = {
	{nlcon},	{ctcon},	{iscon },	{NULL },     /*  0- 3 */
	{fdccon },	{NULL },	{NULL },	{NULL },     /*  4- 7 */
	{rmcon },	{NULL },	{NULL },	{NULL },     /*  8-11 */
	{NULL },	{NULL },	{NULL },	{NULL },     /* 12-15 */
	{NULL },	{NULL },	{NULL },	{NULL },     /* 16-19 */
	{NULL },	{NULL },	{NULL },	{NULL },     /* 20-23 */
	{NULL },	{NULL },	{NULL },	{NULL },     /* 24-27 */
	{NULL },	{NULL },	{NULL },	{NULL }      /* 28-31 */
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
dev_t	rootdev = makedev(0, 0);	/* Root device */
dev_t	pipedev = makedev(0, 0);	/* Pipe device */
dev_t	swapdev = makedev(0, 0);	/* Swap device */
daddr_t	swapbot = 0;			/* Swap base */
daddr_t	swaptop = 0;			/* Swap end */
int	ronflag	= 0;			/* Not read only root device */
int	drvn	= NDRV;			/* Maximum number of devices */
int	mactype	= M_386;		/* Machine type */

/*
 * Flexible param's
 */
int	NCLIST	= 64;
int	ALLSIZE = 0;	/* main.c autosizes. Patching overrides autosize */
int	NINODE	= 128;	/* More than enough so far */
int	NBUF	= 0;	/* main.c autosizes. Patching overrides autosize */
int	NHASH	= 1021;
int	BPFMAX	= (ND + NBN + NBN*NBN + NBN*NBN*NBN);
			/* for ulimit - max # of blocks per file */
