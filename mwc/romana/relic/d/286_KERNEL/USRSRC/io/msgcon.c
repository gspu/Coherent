/* $Header: /usr/src/sys/i8086/drv/RCS/msgcon.c,v 2.1 88/09/03 13:09:32 src Exp $
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
 * System V Compatible Message Device Driver
 *
 *	This device driver provides System V compatible messaging operations.
 *	Operations are performed through the message device (/dev/msg).
 *	and are implemented as ioctl calls from msgctl, msgget, msgsnd, msgrcv
 *	utilities.
 *
 *			Author: Allan Cornish, INETCO Systems Ltd., Oct 1984
 *
 * $Log:	/usr/src/sys/i8086/drv/RCS/msgcon.c,v $
 * Revision 2.1	88/09/03  13:09:32	src
 * *** empty log message ***
 * 
 * Revision 1.1	88/03/24  17:05:49	src
 * Initial revision
 * 
 * 87/03/02	Allan Cornish		/usr/src/sys/i8086/drv/msgcon.c
 * Msgioctl() now supports long key [was short] on MSGGET operations.
 * This allows compatability with System V.
 *
 * 85/08/06	Allan Cornish
 * Msg.c split into configuration (msgcon.c) and implementation (msg.c).
 *
 * 85/07/03	Allan Cornish
 * Simplified msgopen by ignoring minor device, which previously had to be 0.
 *
 * 84/01/30	Allan Cornish
 * Initial revision.
 */

#include <coherent.h>
#include <types.h>
#include <uproc.h>
#include <errno.h>
#include <con.h>
#include <msg.h>

/*
 * Functions.
 */

int msgopen();
int msgioctl();
int nulldev();
int nonedev();

/*
 * Device Configuration.
 */

CON msgcon = {
	DFCHR,			/* Flags			*/
	25,			/* Major Index			*/
	msgopen,		/* Open				*/
	nulldev,		/* Close			*/
	nonedev,		/* Block			*/
	nonedev,		/* Read				*/
	nonedev,		/* Write			*/
	msgioctl,		/* Ioctl			*/
	nulldev,		/* Power fail			*/
	nulldev,		/* Timeout			*/
	nulldev,		/* Load				*/
	nulldev			/* Unload			*/
};

/*
 * Message Device Open.
 */

static
msgopen( dev, mode )

dev_t dev;
int mode;

{
	extern struct msqid_ds * msqs; /* Pointer to array of message queues */

	if ( ! msqs )			/* message queues not initialized */
		msginit();

	if ( ! msqs )			/* no message queues */
		u.u_error = ENODEV;
}

/*
 * Message Device Ioctl.
 */

static
msgioctl( dev, com, vec )

dev_t dev;
int com;
register int *vec;

{
	switch ( com ) {

	case MSGCTL:
		putuwd( vec+0,
			umsgctl(getuwd( vec+1 ),
				getuwd( vec+2 ),
				getuwd( vec+3 ) ));
		return;

	case MSGGET:
		putuwd( vec+0,
			umsgget(getuwd( vec+1 ),
				getuwd( vec+2 ),
				getuwd( vec+3 ) ));
		return;

	case MSGSND:
		putuwd( vec+0,
			umsgsnd(getuwd( vec+1 ),
				getuwd( vec+2 ),
				getuwd( vec+3 ),
				getuwd( vec+4 ) ));
		return;

	case MSGRCV:
		putuwd( vec+0,
			umsgrcv(getuwd( vec+1 ),
				getuwd( vec+2 ),
				getuwd( vec+3 ),
				getuwd( vec+4 ),
				getuwd( vec+5 ),
				getuwd( vec+6 ) ));
		return;

	default:
		u.u_error = EINVAL;
		return;
	}
}
