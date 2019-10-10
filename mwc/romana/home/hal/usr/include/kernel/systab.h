/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/*
 * Header for system call table.
 */

#ifndef __KERNEL_SYSTAB_H__
#define __KERNEL_SYSTAB_H__

#include <common/ccompat.h>
#include <common/gregset.h>
#include <kernel/param.h>

/*
 * Type of a system-call function.  Note that we prototype a function here that
 * takes six arguments plus a pointer to a register set. In fact, the calling
 * sequence is different, but this seems more appropriate than "..." for an
 * argument list.
 *
 * Either method is non-portable, but this one should work in more cases. To
 * make it work in all cases, we need a selective dispatch based on the real
 * number of arguments.
 */

typedef	long	     (*	__sysfunc_t)	__PROTO ((int _arg1, int _arg2,
						  int _arg3, int _arg4,
						  int _arg5, int _arg6,
						  gregset_t * _regsetp));
typedef	long	     (*	__sysfunc0_t)	__PROTO ((gregset_t * _regsetp));
typedef	long	     (*	__sysfunc1_t)	__PROTO ((int _arg1,
						  gregset_t * _regsetp));
typedef	long	     (*	__sysfunc2_t)	__PROTO ((int _arg1, int _arg2,
						  gregset_t * _regsetp));
typedef	long	     (*	__sysfunc3_t)	__PROTO ((int _arg1, int _arg2,
						  int _arg3,
						  gregset_t * _regsetp));
typedef	long	     (*	__sysfunc4_t)	__PROTO ((int _arg1, int _arg2,
						  int _arg3, int _arg4,
						  gregset_t * _regsetp));
typedef	long	     (*	__sysfunc5_t)	__PROTO ((int _arg1, int _arg2,
						  int _arg3, int _arg4,
						  int _arg5,
						  gregset_t * _regsetp));

/*
 * Function return types, as in the "s_type" member below.
 */

enum {
	__SYSCALL_INT	= 2,
	__SYSCALL_LONG	= 4
};


/*
 * System call table structure.
 */

struct systab {
	unsigned char	s_nargs;		/* number of arguments */
	char		s_type;			/* Type returned by function */
	__sysfunc_t	s_func;			/* Function */
	char	      *	s_name;			/* Function name */
	int		s_stat;			/* call count */
};


#define	__DOSYSCALL(argc,func,args,regsetp) \
	((argc) == 0 ? (* (__sysfunc0_t) (func)) (regsetp) : \
	 (argc) == 1 ? (* (__sysfunc1_t) (func)) ((args) [0], (regsetp)) : \
	 (argc) == 2 ? (* (__sysfunc2_t) (func)) ((args) [0], (args) [1], \
						  (regsetp)) : \
	 (argc) == 3 ? (* (__sysfunc3_t) (func)) ((args) [0], (args) [1], \
						  (args) [2], (regsetp)) : \
	 (argc) == 4 ? (* (__sysfunc4_t) (func)) ((args) [0], (args) [1], \
						  (args) [2], (args) [3], \
						  (regsetp)) : \
	 (argc) == 5 ? (* (__sysfunc5_t) (func)) ((args) [0], (args) [1], \
						  (args) [2], (args) [3], \
						  (args) [4], (regsetp)) : \
	 (argc) == 6 ? (* (func)) ((args) [0], (args) [1], (args) [2], \
				   (args) [3], (args) [4], (args) [5], \
				   (regsetp)) : \
	 (panic ("Bad number of system call arguments"), 0))


/*
 * System call table.
 */

extern	struct systab	sysitab [NMICALL];
extern	struct systab	sys286tab [NMICALL];
extern	struct systab	h28itab [H28CALL];
extern	struct systab	cohcall;

#endif	/* ! defined (__KERNEL_SYSTAB_H__) */

