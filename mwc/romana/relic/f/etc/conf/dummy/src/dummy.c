int dummyInt = 0x98547632;
short dummyShort = 0x9876;
char dummyChar = 'Z';

/*
 * KER/conf/dummy.c
 *
 * dummy device driver
 *
 * Revised: Fri Jul 23 12:42:16 1993 CDT
 */

/*
 * -----------------------------------------------------------------
 * Includes.
 */
#include <sys/coherent.h>
#include <sys/cmn_err.h>
#include <sys/con.h>
#include <sys/errno.h>

/*
 * -----------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/*
 * -----------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int nulldev();

/*
 * Configuration functions (local functions).
 */
static int dummyclose();
static int dummyioctl();
static int dummyload();
static int dummyopen();
static int dummyread();
static int dummyunload();
static int dummywrite();

/*
 * Support functions (local functions).
 */

/*
 * -----------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * Configuration table (export data).
 */
CON dummycon ={
	DFCHR,				/* Flags */
	0,				/* Major index */
	dummyopen,			/* Open */
	dummyclose,			/* Close */
	nulldev,			/* Block */
	dummyread,			/* Read */
	dummywrite,			/* Write */
	dummyioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	dummyload,			/* Load */
	dummyunload,			/* Unload */
	nulldev				/* Poll */
};

/*
 * -----------------------------------------------------------------
 * Code.
 */

/*
 * dummyload()
 */
static int
dummyload()
{
	cmn_err(CE_NOTE, "Dummy: load");
}

/*
 * dummyunload()
 */
static int
dummyunload()
{
	cmn_err(CE_NOTE, "Dummy: unload");
}

/*
 * dummyopen()
 */
static int
dummyopen(dev, mode)
dev_t dev;
int mode;
{
	cmn_err(CE_NOTE, "Dummy: open");
}

/*
 * dummyclose()
 */
static int
dummyclose(dev, mode)
dev_t dev;
int mode;
{
	cmn_err(CE_NOTE, "Dummy: close");
}

/*
 * dummyread()
 */
static int
dummyread(dev, iop)
dev_t dev;
register IO * iop;
{
	cmn_err(CE_NOTE, "Dummy: read");
}

/*
 * dummywrite()
 */
static int
dummywrite(dev, iop)
dev_t dev;
register IO * iop;
{
	cmn_err(CE_NOTE, "Dummy: write");
}

/*
 * dummyioctl()
 */
static int
dummyioctl(dev, com, vec)
dev_t	dev;
int	com;
struct sgttyb *vec;
{
	cmn_err(CE_NOTE, "Dummy: ioctl");
}
