/* $Header: /usr/src/sys/ldrv/RCS/nonedev.c,v 1.1 88/03/24 16:30:54 src Exp $ */

/*
 * $Log:	/usr/src/sys/ldrv/RCS/nonedev.c,v $
 * Revision 1.1	88/03/24  16:30:54	src
 * Initial revision
 * 
 */
#include <sys/coherent.h>
#include <sys/uproc.h>
#include <errno.h>

/*
 * Non existant device.
 */
nonedev()
{
	u.u_error = ENXIO;
}
