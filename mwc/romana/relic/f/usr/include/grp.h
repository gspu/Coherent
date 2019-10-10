/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure for the /etc/group file.
 */

#ifndef	__GRP_H__
#define	__GRP_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/_uid.h>

struct group {
	char	      *	gr_name;
	char	      *	__NON_POSIX (gr_passwd);
#if	_SYSV4
	__gid_t		gr_gid;
#else
	int		gr_gid;
#endif
	char	     **	gr_mem;
};


__EXTERN_C_BEGIN__

struct group  *	getgrgid	__PROTO ((__gid_t _gid));
struct group  *	getgrnam	__PROTO ((__CONST__ char * _name));

#if	! _POSIX_SOURCE

struct group  *	getgrent	__PROTO ((void));

#endif

__EXTERN_C_BEGIN__

#endif	/* ! defined (__GRP_H__) */
