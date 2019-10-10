/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_CRED_LIB_H__
#define	__KERNEL_CRED_LIB_H__

/*
 * Internal layering for credentials access.
 */

#include <common/ccompat.h>
#include <common/__cred.h>

__EXTERN_C_BEGIN__

n_gid_t	      *	groups_alloc	__PROTO ((unsigned short _ngroups));
void		groups_ref	__PROTO ((n_gid_t * _groups,
					  unsigned short _ngroups));
void		groups_unref	__PROTO ((n_gid_t * _groups,
					  unsigned short _ngroups));

__cred_t      *	cred_execid	__PROTO ((__cred_t * _credp, n_uid_t _uid,
					  n_gid_t _gid));
int		cred_match	__PROTO ((__cred_t * _credp, n_uid_t _uid,
					  n_gid_t _gid));
__cred_t      *	cred_newgid	__PROTO ((__cred_t * _credp, n_gid_t _gid));
__cred_t      *	cred_newegid	__PROTO ((__cred_t * _credp, n_gid_t _gid));
__cred_t      *	cred_newuid	__PROTO ((__cred_t * _credp, n_uid_t _uid));
__cred_t      *	cred_neweuid	__PROTO ((__cred_t * _credp, n_uid_t _uid));
__cred_t      *	cred_setgrp	__PROTO ((__cred_t * _credp,
					  unsigned short _ngroups));
__cred_t      *	cred_fake	__PROTO ((__cred_t * _dest,
					  __CONST__ __cred_t * _src));

__cred_t      *	cred_copy	__PROTO ((__cred_t * _credp));
__cred_t      *	cred_alloc	__PROTO ((void));
void		cred_ref	__PROTO ((__cred_t * _credp));
void		cred_unref	__PROTO ((__cred_t * _credp));

__EXTERN_C_END__

#endif	/* ! defined (__KERNEL_CRED_LIB_H__) */

