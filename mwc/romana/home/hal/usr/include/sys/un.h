/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_UN_H__
#define	__SYS_UN_H__

/*
 * Socket address format for UNIX domain sockets.
 */

struct sockaddr_un {
	short		sun_family;	/* AF_UNIX */
	char		sun_path [108];
};

#endif	/* ! defined (__SYS_UN_H__) */

