/* $Header: /src386/STREAMS/io.386/RCS/ct.c,v 2.3 93/08/09 13:44:24 bin Exp Locker: bin $ */
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
 * Coherent
 * Console terminal driver.
 *
 * $Log:	ct.c,v $
 * Revision 2.3  93/08/09  13:44:24  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  15:28:02  nigel
 * Nigel's R80
 * 
 * Revision 1.2  92/01/06  12:26:48  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:18:09	src
 * Initial revision
 * 
 * 86/11/19	Allan Cornish		/usr/src/sys/drv/ct.c
 * Added support for System V.3 compatible polls.
 */
#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <sys/stat.h>

/*
 * Functions for configuration.
 */
int	ctopen();
int	ctclose();
int	ctread();
int	ctwrite();
int	ctioctl();
int	ctpoll();
int	nulldev();
int	nonedev();

/*
 * Configuration table.
 */
CON ctcon ={
	DFCHR|DFPOL,			/* Flags */
	1,				/* Major index */
	ctopen,				/* Open */
	ctclose,			/* Close */
	nulldev,			/* Block */
	ctread,				/* Read */
	ctwrite,			/* Write */
	ctioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	nulldev,			/* Load */
	nulldev,			/* Unload */
	ctpoll				/* Poll */
};

/*
 * Open.
 */
ctopen(dev, m)
dev_t dev;
{
	register dev_t ttdev;

	if ((ttdev=SELF->p_ttdev) == NODEV) {
		u.u_error = ENXIO;
		return;
	}
	dopen(ttdev, m, DFCHR);
}

/*
 * Close.
 */
ctclose(dev)
dev_t dev;
{
	dclose(SELF->p_ttdev);
}

/*
 * Read.
 */
ctread(dev, iop)
dev_t dev;
IO *iop;
{
	dread(SELF->p_ttdev, iop);
}

/*
 * Write.
 */
ctwrite(dev, iop)
dev_t dev;
IO *iop;
{
	dwrite(SELF->p_ttdev, iop);
}

/*
 * Ioctl.
 */

ctioctl(dev, com, vec, mode)
dev_t dev;
struct sgttyb *vec;
{
	dioctl (SELF->p_ttdev, com, vec, mode, NULL);
}


/*
 * Poll.
 */

ctpoll(dev, ev)
dev_t dev;
int ev;
{
	return dpoll(SELF->p_ttdev, ev);
}
