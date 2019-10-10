/* $Header: /src386/kernel/io.386/RCS/msgstub.c,v 1.6 93/04/16 06:59:26 bin Exp Locker: bin $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1985
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

/*
 * Message Device Driver Stub.
 *
 *	This module provides stubs for messaging routines accessed by the
 *	operating system, and is linked in when messaging is not specified.
 *
 *			Author: Allan Cornish, INETCO Systems Ltd., Nov 1986.
 *
 * $Log:	msgstub.c,v $
 * Revision 1.6  93/04/16  06:59:26  bin
 * Hal: kernel 76 update
 * 
 * Revision 1.2  92/01/06  12:29:17  hal
 * Compile with cc.mwc.
 * 
 * Revision 2.1	88/09/03  13:09:42	src
 * *** empty log message ***
 * 
 * Revision 1.1	88/03/24  17:05:52	src
 * Initial revision
 * 
 * 86/12/12	Allan Cornish		/usr/src/sys/i8086/drv/msgstub.c
 * Added 3rd argument to msgpoll() to support non-blocking polls.
 *
 * 85/11/21	Allan Cornish		/usr/src/sys/i8086/drv/msgstub.c
 * Initial Revision.
 */

#include <sys/coherent.h>
#include <errno.h>

msgpoll( qid, ev, msec )
int qid;
int ev;
int msec;
{
	return POLLNVAL;
}

msgget()
{
	u.u_error = ENXIO;
}

msgrcv()
{
	u.u_error = ENXIO;
}

msgsnd()
{
	u.u_error = ENXIO;
}

msgctl()
{
	u.u_error = ENXIO;
}
