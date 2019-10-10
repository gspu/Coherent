/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__PWD_H__
#define	__PWD_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/_uid.h>

#if	_SYSV4 || _SYSV3

struct	passwd {
	char	      *	pw_name;	/* User name */
	char	      *	__NON_POSIX (pw_passwd);
					/* Encrypted password */
#if	_SYSV4
	__uid_t		pw_uid;
	__gid_t		pw_gid;
#else
	int		pw_uid;		/* User id */
	int		pw_gid;		/* Group id */
#endif
	char	      *	__NON_POSIX (pw_age);
	char	      *	__NON_POSIX (pw_comment);
					/* Comments */
	char	      *	__NON_POSIX (pw_gecos);
					/* Gecos box number */
	char	      *	pw_dir;		/* Working directory */
	char	      *	pw_shell;	/* Shell */
};

#else	/* if ! _SYSV4 && ! _SYSV3 */

struct	passwd {
	char	      *	pw_name;	/* User name */
	char	      *	__NON_POSIX (pw_passwd);
					/* Encrypted password */
	int		pw_uid;		/* User id */
	int		pw_gid;		/* Group id */
	int		__NON_POSIX (pw_quota);
					/* FIle space quota */
	char	      *	__NON_POSIX (pw_comment);
					/* Comments */
	char	      *	__NON_POSIX (pw_gecos);
					/* Gecos box number */
	char	      *	pw_dir;		/* Working directory */
	char	      *	pw_shell;	/* Shell */
};

#endif	/* ! _SYSV4 && ! _SYSV3 */

__EXTERN_C_BEGIN__

struct passwd *	getpwnam	__PROTO ((__CONST__ char * _name));
struct passwd *	getpwuid	__PROTO ((__uid_t _uid));

#if	! _POSIX_C_SOURCE

struct passwd *	getpwent	__PROTO ((void));
void		setpwent	__PROTO ((void));
void		endpwent	__PROTO ((void));

#endif

__EXTERN_C_END__

#endif	/* ! defined (__PWD_H__) */
