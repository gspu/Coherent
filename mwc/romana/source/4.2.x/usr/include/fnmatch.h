/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__FNMATCH_H__
#define	__FNMATCH_H__

/*
 * Shell-style file-name pattern matching facility from POSIX.2
 */

#include <common/feature.h>
#include <common/ccompat.h>

#if	_POSIX_C_SOURCE != 1

enum {
	FNM_NOESCAPE = 1,
	FNM_PATHNAME = 2,
	FNM_PERIOD = 4,
	__MAX_FNMFLAG = 7
};

enum {
	__FNM_MATCH,
	FNM_NOMATCH,
	__FNM_ERROR
};

__EXTERN_C_BEGIN__

int		fnmatch		__PROTO ((__CONST__ char * _pattern,
					  __CONST__ char * _string,
					  int _flags));
__EXTERN_C_END__

#endif	/* _POSIX_C_SOURCE != 1 */

#endif	/* ! defined (__FNMATCH_H__) */

