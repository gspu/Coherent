/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_PROC_LIB_H__
#define	__KERNEL_PROC_LIB_H__

/*
 * This file contains definitions for some internal process-related operations
 * that are now properly sealed away.
 */

#include <common/ccompat.h>
#include <common/__pid.h>
#include <kernel/_proc.h>
#include <kernel/_sleep.h>

/*
 * Standard process-table iterator callback function type.
 */

typedef	int	     (*	__proc_iter_t)	__PROTO ((__proc_t * _procp,
						  __VOID__ * _arg));

__EXTERN_C_BEGIN__

__proc_t      *	new_process_init
				__PROTO ((__proc_t * _process));
void		process_destroy	__PROTO ((__proc_t * _process));
void		process_wake	__PROTO ((__proc_t * _process, int _reason));
void		process_set_runnable
				__PROTO ((__proc_t * _process));
__proc_t      *	process_find_pid
				__PROTO ((__pid_t _pid));
int		process_find_ppid
				__PROTO ((__pid_t _ppid, __proc_iter_t _iter,
					  __VOID__ * _arg));
int		process_find_zombies
				__PROTO ((__pid_t _ppid, __proc_iter_t _iter,
					  __VOID__ * _arg));
int		process_find_pgid
				__PROTO ((__pid_t _pgid, __proc_iter_t _iter,
					  __VOID__ * _arg));
int		process_find_sid
				__PROTO ((__pid_t _sid, __proc_iter_t _iter,
					  __VOID__ * _arg));
int		process_find_all
				__PROTO ((__proc_iter_t _iter,
					  __VOID__ * _arg));

int		session_find_pgid
				__PROTO ((__proc_t * _sesp, __pid_t _pgid,
					  __proc_iter_t _iter,
					  __VOID__ * _arg));

int		iter_find_any	__PROTO ((__proc_t * _proc, __VOID__* _arg));

__pid_t		current_pid	__PROTO ((void));

__EXTERN_C_END__


#define	process_wake_signal(proc)	process_wake (proc, PROCESS_SIGNALLED)

#endif	/* ! defined (__KERNEL_PROC_LIB_H__) */
 
