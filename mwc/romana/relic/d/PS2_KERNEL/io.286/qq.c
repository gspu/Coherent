/*
 * qq - sample device driver using absolute memory addressing
 *
 * All this device does is read/write video ram.
 * It assumes that there is a monochrome adapter in use, so that video
 * ram starts at segment B000;  if color, this should be changed to B800.
 *
 * This driver does not do anything useful;  it is intended to serve as
 * an example.
 *
 * Here is how to make the driver and test it (you will need a COHERENT
 * Driver Kit installed on your system):
 * 1.	put this file, "qq.c", in /usr/src/sys/i8086/drv/qq.c
 * 2.	cut out the make file and store it in /usr/src/sys/i8086/drv/Mf.qq
 * 3.	cut out the config file and store it at /usr/sys/confdrv/qq
 * 4.	execute the following commands
 *		cd /usr/src/sys/i8086/drv
 *		make -f Mf.qq
 *		cd /usr/sys
 *		ldconfig qq
 *		drvld ldrv/qq
 * 5.	the driver should now be loaded - try "date > /dev/qq" or
 *	"cat < /dev/qq" (you will have to use Ctrl-C to stop the "cat"
 *	command)
 * 6.	to unload the driver, do "ps -d" to get the PID number for the driver;
 *	then do "kill kill nnn" where nnn is the process number for "<qq>"
 */
/****
Here is the makefile for the "qq" driver (cut it out of this file):
--------------- cut here -----------------
# Make file for a loadable driver

AS=exec /bin/as
CC=exec /bin/cc
CPP=exec /lib/cpp
CFLAGS=-I.. -I../sys -I../.. -I../../sys \
	-I/usr/include/sys
AFLAGS=-gx

# Include directories
USRINC=/usr/include
SYSINC=/usr/include/sys
KERINC=/usr/src/sys/sys
DRVINC=/usr/src/sys/i8086/sys
USRSYS=/usr/sys

DRVOBJ=	objects/qq.o

qq: objects/qq.o
	rm -f $(USRSYS)/lib/qq.a
	ar rc $(USRSYS)/lib/qq.a objects/qq.o

objects/qq.o:				\
		$(KERINC)/coherent.h	$(SYSINC)/types.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(USRINC)/errno.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/seg.h		\
		$(SYSINC)/stat.h	\
		$(SYSINC)/types.h	\
		qq.c
	$(CC) $(CFLAGS) -c -o $@ qq.c
--------------- cut here -----------------

Here is the configuration file for the "qq" driver.
Cut it out of this file and copy it to "/usr/sys/confdrv/qq".
When "ldconfig" is run, it will create a node for /dev/qq.
--------------- cut here -----------------
:
: 'Dummy driver for write to absolute RAM area'
:
UNDEF="${UNDEF} -u qqcon_ lib/qq.a"
PATCH="${PATCH} drvl_+70=qqcon_"
:
: devices
:
umask 0111
/etc/mknod -f ${DEV-/dev}/qq c 7  0 || exit 1
--------------- cut here -----------------
****/
#include "coherent.h"
#include "ins8250.h"
#include <sys/stat.h>
#include <sys/uproc.h>
#include <sys/proc.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mmu.h>

/*
 * Definitions.
 *
 */
#define	MONOVIDEO	0xB000		/* monochrome text RAM segment */
#define	VIDLENGTH	(2048*2)	/* screen locations (2 bytes each) */

/*
 * Export Functions.
 */
int	qqload();
int	qqopen();
int	qqclose();
int	qqread();
int	qqwrite();
int	qqunload();

/*
 * Import Functions
 */
int	nulldev();
int	nonedev();

/*
 * Configuration table.
 */
CON qqcon ={
	DFCHR,				/* Flags */
	7,				/* Major index */
	qqopen,				/* Open */
	qqclose,			/* Close */
	nulldev,			/* Block */
	qqread,				/* Read */
	qqwrite,			/* Write */
	nulldev,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	qqload,				/* Load */
	qqunload,			/* Unload */
	nulldev				/* Poll */
};

/*
 * Local variables.
 */
static faddr_t	screen_fp;		/* (far *) to access screen */
static paddr_t	screen_base;		/* physical address of screen base */

/*
 * Load Routine.
 */
static qqload()
{
	/*
	 * Allocate a selector to map onto the video RAM.  ptov() will
	 * return the first available selector of the 8,192 possible.
	 * This is time consuming, so we only want to do this as part
	 * of our initialization code and not on every access.
	 *
	 * Since we are operating in 286 protected mode (ugh), the
	 * second argument to ptov() must not exceed 0x10000L.
	 */
	screen_base = (paddr_t)((long)(unsigned)MONOVIDEO << 4);
	screen_fp = ptov(screen_base, (fsize_t)VIDLENGTH);
}

static qqunload()
{
	/*
	 * We have to free up the selector now that we're done using it.
	 */
	vrelse(screen_fp);
}

/*
 * Open Routine.
 */
qqopen( dev, mode )
dev_t dev;
{
}

/*
 * Close Routine.
 */
qqclose( dev )
dev_t dev;
{
}

/*
 * Read Routine.
 */
qqread( dev, iop )
dev_t dev;
register IO * iop;
{
	static int offset;
	int c;
	/*
	 * Read a character code from video RAM
	 * Start reading RAM just after where previous read ended
	 *
	 * Note that "offset" is the value of the displacement into
	 * the screen RAM. Any expression which results in a value
	 * which is less than VIDLENGTH is OK here.
	 */
	while(iop->io_ioc) {
		c = ffbyte(screen_fp + offset); /* fetch a "far" byte */
		if(ioputc(c, iop) == -1)
			break;
		offset += 2;
		offset %= VIDLENGTH;
	}
}

/*
 * Write Routine.
 */
qqwrite( dev, iop )
dev_t dev;
register IO * iop;
{
	int offset = 0;
	int c;

	/*
	 * Write a character into the screen RAM
	 * Note that "offset" is the value of the displacement into
	 * the screen RAM. Any expression which results in a value
	 * which is less than VIDLENGTH is OK here.
	 */
	while ((c = iogetc(iop)) >= 0 && offset < VIDLENGTH) {
		sfbyte(screen_fp + offset, c);	   /* store a "far" byte */
		offset += 2;	/* skip attribute byte */
	}
}
