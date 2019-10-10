/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__GRP_H__
#define	__GRP_H__

/*
 * Structure for the /etc/group file.
 */

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

#if	! _POSIX_C_SOURCE

struct group  *	getgrent	__PROTO ((void));
void		setgrent	__PROTO ((void));
void		endgrent	__PROTO ((void));
int		initgroups	__PROTO ((__CONST__ char * _name,
					  __gid_t _basegid));

#endif

__EXTERN_C_BEGIN__

#endif	/* ! defined (__GRP_H__) */
