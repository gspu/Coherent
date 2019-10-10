static int blocked;
static int sg_hal;

/*
 * qq - sample device driver using absolute memory addressing
 *
 * All this device does is read/write video ram.
 * It assumes that there is a monochrome adapter in use, so that video
 * ram starts at segment B000;  if color, this should be changed to B800.
 *
 * This driver does not do anything useful;  it is intended to serve as
 * an example.
 */
#include <sys/coherent.h>
#include <sys/stat.h>
#include <sys/uproc.h>
#include <sys/proc.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <sys/types.h>
#include <sys/mmu.h>

/*
 * Definitions.
 *
 */
#define	MONOVIDEO	0xB000		/* monochrome text RAM segment */
#define	VIDLENGTH	(2048*2)	/* screen locations (2 bytes each) */

#define QQ_MAJOR	7		/* device major number */

/*
 * Export Functions.
 */
int	qqload();
int	qqopen();
int	qqclose();
int	qqread();
int	qqwrite();
int	qqunload();
int	qqioctl();
int	qqtime();

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
	QQ_MAJOR,			/* Major index */
	qqopen,				/* Open */
	qqclose,			/* Close */
	nulldev,			/* Block */
	qqread,				/* Read */
	qqwrite,			/* Write */
	qqioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	qqtime,				/* Timeout */
	qqload,				/* Load */
	qqunload,			/* Unload */
	nulldev				/* Poll */
};

/*
 * Local variables.
 */
#ifdef _I386
static paddr_t	screen_fp;		/* virtual address of screen base */
#else
static faddr_t	screen_fp;		/* (far *) to access screen */
static paddr_t	screen_base;		/* physical address of screen base */
#endif
int	qqopct;

/*
 * Load Routine.
 */
qqload()
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
#ifdef _I386
	screen_fp = map_pv(MONOVIDEO<<4, VIDLENGTH);
	printf("qqload:  screen_fp = %x = map_pv(%x, %x)\n",
		screen_fp, MONOVIDEO<<4, VIDLENGTH);
#else
	screen_base = (paddr_t)((long)(unsigned)MONOVIDEO << 4);
	screen_fp = ptov(screen_base, (fsize_t)VIDLENGTH);
#endif
}

qqunload()
{
	/*
	 * We have to free up the selector now that we're done using it.
	 */
#ifdef _I386
	unmap_pv(screen_fp);
#else
	vrelse(screen_fp);
#endif
}

/*
 * Open Routine.
 */
qqopen( dev, mode )
dev_t dev;
{
	qqopct++;
	printf("qqopen %d\n", qqopct);
}

/*
 * Close Routine.
 */
qqclose( dev )
dev_t dev;
{
	qqopct--;
	printf("qqclose %d\n", qqopct);
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
		c = ((char *)screen_fp)[offset];
		if(ioputc(c, iop) == -1)
			break;
		offset += 2;
		offset %= VIDLENGTH;
	}
}

/*
 * Write Routine.
 */
static char flab [] = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z ";

qqwrite( dev, iop )
dev_t dev;
register IO * iop;
{
	int offset = 0;
	int c;

	struct foob { long la, lb; } f;

	while (iop->io_ioc >= sizeof(f)) {
		ioread(iop, &f, sizeof(f));
#if MEMCPY
		/* memcpy from struct f to virtual address */
		memcpy(screen_fp + offset, &f, sizeof(f));
#else
		/* struct assignment to virtual address (implicit memcpy) */
		* (struct foob *)(screen_fp + offset) = f;
#endif
		offset += 2 * sizeof(f);	/* skip attribute byte */
	}

	/* single character writes to virtual address */
	while ((c = iogetc(iop)) >= 0 && offset < VIDLENGTH) {
		*(char *)(screen_fp + offset) = c;
		*(char *)(screen_fp + offset + 1) ^= 0xff;
		offset += 2;	/* skip attribute byte */
	}


	/* no virtual address used - xpcopy to physical destination */
	xpcopy(flab, (MONOVIDEO<<4)+320, 52, SEG_386_KD);
}

int qqct;

int
qqioctl(dev, com, vec)
dev_t	dev;
int	com;
struct sgttyb *vec;
{
	if (com == 333) {
		ukcopy(vec, screen_fp, 20);
		sg_hal = vtosg(vec, 20);
		drvl[QQ_MAJOR].d_time = 30;
		blocked = 1;
qqct = 0;
	}
	while (blocked) {
#ifdef _I386
		if (x_sleep (& blocked, pritty, slpriSigCatch,
			     "qqioctl") == PROCESS_SIGNALLED) {
#else
		v_sleep(&blocked, CVTTOUT, IVTTOUT, SVTTOUT, "qqioctl");
		if (nondsig ()) {  /* signal? */
#endif
			u.u_error = EINTR;
			break;
		}
	}
}

int
qqtime()
{
	int j;
	char buffer[20 + 1];

	buffer[20] = '\0';

	j = --drvl[QQ_MAJOR].d_time;

	if ((j % 2) == 0) {
		if (sg_hal) {
qqct += 20;
printf("qqtime:  j=%d  pxcopy(sg_hal=%x, dest=%x, len=%x, space=%x  ",
  j, sg_hal, screen_fp + qqct, 20, SEG_386_KD|SEG_VIRT);
			pxcopy(sg_hal, buffer, 20,
			  SEG_386_KD|SEG_VIRT);
printf("buffer=%s\n", buffer);
			pxcopy(sg_hal, screen_fp + qqct, 20,
			  SEG_386_KD|SEG_VIRT);
		} else
			putchar('?');
	}

	if (j == 0) {
		printf("wakeup\n");
		blocked = 0;
		wakeup(&blocked);
	}
}

int
vtosg(virt, size)
int virt;
int size;
{
	register SR *srp;
	register SEG *sp;
	register caddr_t base;
	int ret = 0;

	for (srp=u.u_segl; srp<&u.u_segl[NUSEG]; srp++) {
		if ((sp=srp->sr_segp) == NULL)
			continue;
#if 0
		/*
		 * Doing this check means string constants in text
		 * are inaccessible to system global memory.
		 */
		if ((srp->sr_flag&SRFDATA) == 0)
			continue;
#endif
		/*
		 * The following calculation is because the system represents
		 * the 'base' of a stack as its upper limit (because it is the
		 * upper limit that is fixed).
		 */
		base = srp->sr_base;
		if (srp==&u.u_segl[SISTACK])
			base -= srp->sr_size;

		if (virt < base)
			continue;
		if (virt + size > base + sp->s_size)
			continue;
 		ret = MAPIO(sp->s_vmem, virt - base);
	}
	return ret;
}
