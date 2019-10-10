/* (-lgl
 * 	COHERENT Driver Kit Version 2.0.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Header for system call table.
 */
#ifndef SYSTAB_H
#define SYSTAB_H
#ifdef _I386
#include <sys/param.h>
#else
#include <sys/const.h>
#endif

/*
 * Functions types.
 */
#define INT	2
#define LONG	4

/*
 * System call table structure.
 */
struct systab {
#ifdef _I386
	int	s_nargs;		/* number of arguments */
#else
	char	s_alen;			/* Size of argument list */
#endif
	char	s_type;			/* Type returned by function */
	int	(*s_func)();		/* Function */
};

/*
 * System call table.
 */
extern	struct	systab sysitab[NMICALL];
extern	struct	systab cohcall;

#endif
