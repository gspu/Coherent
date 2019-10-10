/* $Header: /usr/src/sys/i8086/drv/RCS/semcon.c,v 2.1 88/09/03 13:11:55 src Exp $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1987, 1985, 1984.
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

/*
 * System V Compatible Semaphore Device Driver
 *
 *	This device driver provides System V compatible semaphore operations.
 *	Operations are performed through the semaphore device (/dev/sem).
 *	and are implemented as ioctl calls from semctl, semget, semop
 *	utilities.
 *
 *			Author: Allan Cornish, INETCO Systems Ltd., Sep 1984
 *
 * $Log:	/usr/src/sys/i8086/drv/RCS/semcon.c,v $
 * Revision 2.1	88/09/03  13:11:55	src
 * *** empty log message ***
 * 
 * Revision 1.1	88/03/24  17:06:26	src
 * Initial revision
 * 
 * 87/03/02	Allan Cornish		/usr/src/sys/i8086/drv/semcon.c
 * Semioctl() now supports long key [was short] on SEMGET operations.
 * This allows compatability with System V.
 *
 * 85/08/06	Allan Cornish
 * Sem.c split into configuration (semcon.c) and implementation (sem.c).
 * Semload() renamed to seminit().
 *
 * 85/07/03	Allan Cornish
 * Eliminated semopen() and semclose() functions.
 *
 * 84/09/30	Allan Cornish
 * Initial Revision.
 */

#include <coherent.h>
#include <types.h>
#include <uproc.h>
#include <errno.h>
#include <con.h>
#include <sem.h>

/*
 * Functions.
 */

int seminit();
int semioctl();
int nulldev();
int nonedev();

/*
 * Device Configuration.
 */

CON semcon = {
	DFCHR,			/* Flags			*/
	23,			/* Major Index			*/
	nulldev,		/* Open				*/
	nulldev,		/* Close			*/
	nonedev,		/* Block			*/
	nonedev,		/* Read				*/
	nonedev,		/* Write			*/
	semioctl,		/* Ioctl			*/
	nulldev,		/* Power fail			*/
	nulldev,		/* Timeout			*/
	seminit,		/* Load				*/
	nulldev			/* Unload			*/
};

/*
 * Semaphore Device Ioctl.
 */

static
semioctl( dev, com, vec )

dev_t dev;
int com;
register int *vec;

{
	switch ( com ) {

	case SEMCTL:
		putuwd( vec+0,
			usemctl(getuwd( vec+1 ),
				getuwd( vec+2 ),
				getuwd( vec+3 ),
				getuwd( vec+4 ) ));
		return;

	case SEMGET:
		putuwd( vec+0,
			usemget(getuwd( vec+1 ),
				getuwd( vec+2 ),
				getuwd( vec+3 ),
				getuwd( vec+4 ) ));
		return;

	case SEMOP:
		putuwd( vec+0,
			usemop(	getuwd( vec+1 ),
				getuwd( vec+2 ),
				getuwd( vec+3 ) ));
		return;

	default:
		u.u_error = EINVAL;
		return;
	}
}
