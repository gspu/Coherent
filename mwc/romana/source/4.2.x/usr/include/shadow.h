/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SHADOW_H__
#define	__SHADOW_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/_uid.h>

#if _SYSV4
struct spwd {
	char		*sp_namp;	/* User Name */
	char		*sp_pwdp;	/* Encrypted password */
	long		sp_lstchg;	/* Last changed date */
	long		sp_min;		/* Min time between changes */
	long		sp_max;		/* Max time between changes */
	long		sp_warn;
	long		sp_inact;
	long		sp_expire;	/* Acct expiration date. */
	unsigned long	sp_flag;
};

#else

struct	spwd {
	char		*sp_name;		/* User name */
	char		*__NON_POSIX(sp_passwd);
					/* Encrypted password */
	int		sp_uid;		/* User id */
	int		sp_gid;		/* Group id */
	int		__NON_POSIX(sp_quota);
					/* File space quota */
	char		*__NON_POSIX(sp_comment);
					/* Comments */
	char		*__NON_POSIX(sp_gecos);
					/* Gecos box number */
	char		*sp_dir;	/* Working directory */
	char		*sp_shell;	/* Shell */
};

#endif	/* _SYSV4 */

__EXTERN_C_BEGIN__

#if	! _POSIX_C_SOURCE

struct spwd	*getspent	__PROTO ((void));
struct spwd	*getspnam	__PROTO ((__CONST__ char *name));
void		setspent	__PROTO ((void));
void		endspent	__PROTO ((void));

#endif

__EXTERN_C_END__

#endif	/* ! defined (__SHADOW_H__) */
