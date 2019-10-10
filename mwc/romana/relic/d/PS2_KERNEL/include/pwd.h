/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef	PWD_H
#define	PWD_H	PWD_H

struct	passwd
{
	char	*pw_name;	/* User name */
	char	*pw_passwd;	/* Encrypted password */
	int	pw_uid;		/* User id */
	int	pw_gid;		/* Group id */
	int	pw_quota;	/* FIle space quota */
	char	*pw_comment;	/* Comments */
	char	*pw_gecos;	/* Gecos box number */
	char	*pw_dir;	/* Working directory */
	char	*pw_shell;	/* Shell */
};

struct	passwd	*getpwent();
struct	passwd	*getpwuid();
struct	passwd	*getpwnam();

#endif
