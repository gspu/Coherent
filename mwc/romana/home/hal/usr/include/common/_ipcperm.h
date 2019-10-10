/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__IPCPERM_H__
#define	__COMMON__IPCPERM_H__

/*
 * The System V IPC data type "struct ipc_perm" is used by several headers
 * to declare data members.  Rather than implicitly importing the header when
 * this type is exported to the user, we place just this one definition in a
 * common space.
 */

#include <common/feature.h>
#include <common/_uid.h>

struct ipc_perm {
	__uid_t		uid;		/* owner's user id */
	__gid_t		gid;		/* owner's group id */
	__uid_t		cuid;		/* creator's user id */
	__gid_t		cgid;		/* creator's group id */
	__mode_t	mode;		/* access modes */
	__seq_t		seq;		/* slot usage sequence number */
	__key_t		key;		/* key */
#if	_SYSV4
	long		_pad [4];
#endif
};

#endif	/* ! defined (__COMMON__IPCPERM_H__) */
