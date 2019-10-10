/* $Header: /kernel/kersrc/coh.386/RCS/null.c,v 1.2 92/08/04 12:33:53 bin Exp Locker: bin $ */
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
 * Null and memory driver.
 *  Minor device 0 is /dev/null
 *  Minor device 1 is /dev/mem, physical memory
 *  Minor device 2 is /dev/kmem, kernel data
 *  Minor device 3 is /dev/cmos
 *  Minor device 4 is /dev/boot_gift
 *
 * $Log:	null.c,v $
 * Revision 1.2  92/08/04  12:33:53  bin
 * changed for ker 59
 * 
 * Revision 1.2  92/01/06  11:59:49  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:14:04	src
 * Initial revision
 * 
 */

/*
 * The symbol "DANGEROUS" should be undefined for a production system.
 */
#define NULL_IOCTL	/* Allow ioctl()s for /dev/kmem.  */
#define DANGEROUS	/* Allow dangerous ioctl()s for /dev/null.  */

#include <sys/coherent.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/typed.h>
#ifdef NULL_IOCTL
#include <sys/null.h>
#endif /* NULL_IOCTL */

/* These are minor numbers.  */
#define DEV_NULL	0	/* /dev/null	*/
#define DEV_MEM		1	/* /dev/mem	*/
#define DEV_KMEM	2	/* /dev/kmem	*/
#define DEV_CMOS	3	/* /dev/cmos	*/
#define DEV_BOOTGIFT	4	/* /dev/bootgift  */

/*
 * Functions for configuration.
 */
int	nlread();
int	nlwrite();
int	nlioctl();
int	nulldev();
int	nonedev();

/*
 * Configuration table.
 */
CON nlcon ={
	DFCHR,				/* Flags */
	0,				/* Major index */
	nulldev,			/* Open */
	nulldev,			/* Close */
	nulldev,			/* Block */
	nlread,				/* Read */
	nlwrite,			/* Write */
#ifdef NULL_IOCTL
	nlioctl,			/* Ioctl */
#else /* NULL_IOCTL */
	nonedev,			/* Ioctl */
#endif /* NULL_IOCTL */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	nulldev,			/* Load */
	nulldev				/* Unload */
};

/*
 * Null/memory read routine.
 */
nlread(dev, iop)
dev_t dev;
register IO *iop;
{
	register unsigned n;
	unsigned char tmp, read_cmos();
	extern typed_space boot_gift;

	switch (minor(dev)) {
	case DEV_NULL:
		n = 0;
		break;

	case DEV_MEM:
		n = pxcopy((long)iop->io_seek, iop->io.pbase, iop->io_ioc,
			SEG_386_UD);
		break;

	case DEV_KMEM:
		n = kucopy((vaddr_t)iop->io_seek, iop->io.vbase, iop->io_ioc);
		break;

	case DEV_CMOS:
		for (n = iop->io_ioc; n > 0; --n) {
			tmp = read_cmos(iop->io_seek + n);
			if (0 == kucopy(&tmp, iop->io.vbase + n, sizeof(tmp))){
			/* Abort the loop if we run out of destination.  */
				break;
			}
		}
		n = iop->io_ioc - n;
		break;

	case DEV_BOOTGIFT:
		if (iop->io_seek < BG_LEN) {
			n = iop->io_ioc;
			/* Copy no more than to the end of boot_gift.  */
			if (iop->io_seek + n > BG_LEN) {
				n = BG_LEN - (iop->io_seek);
			}

			n = kucopy(&boot_gift, iop->io.vbase, n);
		} else {
			n = 0;
		}
		break;

	default:
		u.u_error = ENXIO;
		return;
	}
	iop->io_ioc -= n;
	if (u.u_error == EFAULT)
		u.u_error = 0;
}

/*
 * Null/memory write routine.
 */
nlwrite(dev, iop)
dev_t dev;
register IO *iop;
{
	register unsigned n;

	switch (minor(dev)) {
	case DEV_NULL:
		n = iop->io_ioc;
		break;

	case DEV_MEM:
		n = xpcopy(iop->io.pbase, (long)iop->io_seek, iop->io_ioc,
			SEG_386_UD);
		break;

	case DEV_KMEM:
		n = ukcopy(iop->io.vbase, (vaddr_t)iop->io_seek, iop->io_ioc);
		break;

	case DEV_CMOS:
		n = 0;	/* /dev/cmos is not writable.  */
		break;

	case DEV_BOOTGIFT:
		n = 0;	/* /dev/bootgift is not writable.  */
		break;

	default:
		u.u_error = ENXIO;
		return;
	}
	iop->io_ioc -= n;
	if (u.u_error == EFAULT)
		u.u_error = 0;
}

#ifdef NULL_IOCTL /* Includes all of nlioctl().  */

/*
 * Do an ioctl call for /dev/null.
 */
int
nlioctl(dev, cmd, vec)
	dev_t dev;
	int cmd;
	char * vec;
{
	/* Only /dev/kmem has an ioctl.  */
	if (minor(dev) != DEV_KMEM) {
		u.u_error = EINVAL;
		return (-1);
	}

	switch (cmd) {
#ifdef DANGEROUS
	case NLCALL:	/* Call a function.  */
		return docall(vec);
#endif /* DANGEROUS */
	default:
		u.u_error = EINVAL;
		return(-1);
	}
} /* nlioctl() */

#endif /* NULL_IOCTL */

#ifdef DANGEROUS /* Includes all of docall().  */
/*
 * MASSIVE SECURITY HOLE!  This should NOT be included in a distribution
 * system.  Among other problems, it becomes possible to do "setuid(0)".
 *
 * Call a function with arguments.
 *
 * Takes an array of unsigned ints.  The first element is the length of
 * the whole array, the second element is a pointer to the function to
 * call, all other elements are arguments.  At most 5 arguments may be
 * passed.
 *
 * Returns the return value of the called fuction in uvec[0].
 */
int
docall(uvec)
	unsigned uvec[];
{
	int (* func)();
	unsigned kvec[7];
	int retval;

	printf("NLCALL security hole.\n");

	/* Fetch the first element of vec.  */
	ukcopy(uvec, kvec, sizeof(unsigned));

	if ((kvec[0] < 2) || (kvec[0] > 7)) {
		/* Invalid number of elements in uvec.  */
		u.u_error = EINVAL;
		return(-1);
	}
	
	/* Fetch the whole vector.  */
	ukcopy(uvec, kvec, kvec[0] * sizeof(unsigned));

	/* Extract the function.  */
	func = (int (*)()) kvec[1];

	/* Call the function with all arguments.  */
	retval = (*func)(kvec[2], kvec[3], kvec[4], kvec[5], kvec[6]);

	kucopy(&retval, uvec, sizeof(unsigned));

} /* docall() */

#endif /* DANGEROUS */
