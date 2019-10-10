/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef __SYS_SELECT_H__
#define __SYS_SELECT_H__

#include <sys/param.h>

#if NOFILE <= 32
typedef int fd_set;

#define FD_ZERO(fdp)	{*fdp = 0;}
#define FD_SET(b,fdp)	(*fdp |= 1 << (b))
#define FD_ISSET(b,fdp)	(*fdp & 1 << (b))
#define FD_SETSIZE 32
#else
typedef int fd_set[2];

#define FD_ZERO(fdp)	{(*fdp)[0]=(*fdp)[1]=0;}
#define FD_SET(b,fdp)	((*fdp)[((b)>>5)&1] |= 1 << ((b)&0x1F))
#define FD_ISSET(b,fdp)	((*fdp)[((b)>>5)&1] & 1 << ((b)&0x1F))
#define FD_SETSIZE 64

#endif
#endif
