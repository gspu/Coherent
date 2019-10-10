/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_CRED_H__
#define	__SYS_CRED_H__

#include <common/__cred.h>

typedef	__cred_t	cred_t;


/*
 * Complete the type of __cred_t while we are at it.
 */

struct __cred {
	unsigned short	cr_ref;		/* reference count */

	unsigned short	cr_ngroups;	/* number of groups in cr_grps */
	n_uid_t		cr_uid;		/* effective user ID */
	n_gid_t		cr_gid;		/* effective group ID */
	n_uid_t		cr_ruid;	/* real user ID */
	n_gid_t		cr_rgid;	/* real group ID */
	n_uid_t		cr_suid;	/* "saved" user ID */
	n_gid_t		cr_sgid;	/* "saved" group ID */
	n_gid_t	      * cr_groups;	/* array of group memberships */

	unsigned long	cr_pad [2];	/* reserved for future use */
};

#endif	/* ! defined (__SYS_CRED_H__) */

