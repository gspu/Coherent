/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__ERRNO_H__
#define	__ERRNO_H__

#include <common/feature.h>
#include <sys/errno.h>

extern	int		errno;

#if	! _STDC_SOURCE && ! _POSIX_C_SOURCE

extern	int		sys_nerr;
extern	char	      *	sys_errlist [];

#endif

#endif	/* ! defined (__ERRNO_H__) */
