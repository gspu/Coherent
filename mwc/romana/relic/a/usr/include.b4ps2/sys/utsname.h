/* (-lgl
 * 	COHERENT Version 3.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef UTSNAME_H
#define UTSNAME_H	UTSNAME_H

#define SYS_NMLN	9

struct	utsname {
	char	sysname[SYS_NMLN];
	char	nodename[SYS_NMLN];
	char	release[SYS_NMLN];
	char	version[SYS_NMLN];
	char	machine[SYS_NMLN];
};

extern	struct	utsname	utsname;

#endif
