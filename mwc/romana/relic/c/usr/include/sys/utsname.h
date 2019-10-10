/* (-lgl
 * 	COHERENT Version 3.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef UTSNAME_H
#define UTSNAME_H	UTSNAME_H

struct	utsname {
	char	sysname[9];
	char	nodename[9];
	char	release[9];
	char	version[9];
	char	machine[9];
};

extern	struct	utsname	utsname;

#endif
