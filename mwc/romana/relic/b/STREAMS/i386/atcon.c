/*
 * i386/atcon.c
 *
 * System initializers.   This file will disappear soon, I hope. - hws
 *
 * Revised: Sun Aug  1 13:53:24 1993 CDT
 */
#include <sys/coherent.h>
#include <sys/ino.h>
#include <mtype.h>
#include <sys/stat.h>

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
int	mactype	= M_386;		/* Machine type */
dev_t	swapdev = makedev(0, 0);	/* Swap device */
daddr_t	swapbot = 0;			/* Swap base */
daddr_t	swaptop = 0;			/* Swap end */

/*
 * Flexible parameters.
 * See also conf/cohmain/Space.c and conf/mtune.
 */

/* for ulimit - max # of blocks per file */
int	BPFMAX	= (ND + NBN + NBN*NBN + NBN*NBN*NBN);
