/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/unistd.h
 * Cf. Intel iBSC2, pp. 6-82, 6-83.
 * Revised Sun Jun 20 1993 for prototypes		-- NB
 */

#ifndef	__UNISTD_H__
#define	__UNISTD_H__

#include <common/ccompat.h>
#include <common/__pid.h>
#include <common/__off.h>
#include <common/_size.h>
#include <common/_ssize.h>
#include <common/_uid.h>
#include <common/whence.h>

/* Access modes. */
#define	F_OK	0
#define	X_OK	1
#define	W_OK	2
#define	R_OK	4

/* lockf() commands. */
#define	F_ULOCK	0		/* unlock region			*/
#define	F_LOCK	1		/* sleep until available and lock	*/
#define	F_TLOCK	2		/* lock if available, EAGAIN if not	*/
#define	F_TEST	3		/* return 0 if available, EAGAIN if not	*/

/* File descriptors for standard FILEs. */
#define	STDIN_FILENO	0
#define	STDOUT_FILENO	1
#define	STDERR_FILENO	2

/*
 * Names for pathconf () and fpathconf ().
 */

#define	_PC_LINK_MAX		0
#define	_PC_MAX_CANON		1
#define	_PC_MAX_INPUT		2
#define	_PC_NAME_MAX		3
#define	_PC_PATH_MAX		4
#define	_PC_PIPE_BUF		5
#define	_PC_CHOWN_RESTRICTED	6
#define	_PC_NO_TRUNC		7
#define	_PC_VDISABLE		8


/*
 * Names for sysconf ().
 */

#define	_SC_ARG_MAX		0
#define	_SC_CHILD_MAX		1
#define	_SC_CLK_TCK		2
#define	_SC_NGROUPS_MAX		3
#define	_SC_OPEN_MAX		4
#define	_SC_JOB_CONTROL		5
#define	_SC_SAVED_IDS		6
#define	_SC_VERSION		7
#define	_SC_XOPEN_VERSION	8
#define	_SC_LOGNAME_MAX		9


__EXTERN_C_BEGIN__

__NO_RETURN__ void
		_exit		__PROTO ((int _status));
int		access		__PROTO ((__CONST__ char * _path,
					  int _mode));
unsigned int	alarm		__PROTO ((unsigned int _seconds));
int		chdir		__PROTO ((__CONST__ char * _path));
int		chown		__PROTO ((__CONST__ char * _path, 
					  __uid_t _owner, __gid_t _group));
int		close		__PROTO ((int _fildes));
char	      *	ctermid		__PROTO ((char * _s));
int		dup		__PROTO ((int _fildes));
int		dup2		__PROTO ((int _fildes, int _fildes2));
int		execl		__PROTO ((__CONST__ char * _path,
					  __CONST__ char * _arg, ...));
int		execle		__PROTO ((__CONST__ char * _path,
					  __CONST__ char * _arg, ...));
int		execlp		__PROTO ((__CONST__ char * _file,
					  __CONST__ char * _arg, ...));
int		execv		__PROTO ((__CONST__ char * _path,
					  char * __CONST__ _argv []));
int		execve		__PROTO ((__CONST__ char * _path,
					  char * __CONST__ _argv [],
					  char * __CONST__ _envp []));
int		execvp		__PROTO ((__CONST__ char * _file,
					  char * __CONST__ _argv []));
__pid_t		fork		__PROTO ((void));
char	      *	getcwd		__PROTO ((char * _buf, size_t _size));
__gid_t		getegid		__PROTO ((void));
__uid_t		geteuid		__PROTO ((void));
__gid_t		getgid		__PROTO ((void));
char	      *	getlogin	__PROTO ((void));
__pid_t		getpgrp		__PROTO ((void));
__pid_t		getpid		__PROTO ((void));
__pid_t		getppid		__PROTO ((void));
__uid_t		getuid		__PROTO ((void));
int		isatty		__PROTO ((int _fildes));
int		link		__PROTO ((__CONST__ char * _existing,
					  __CONST__ char * _new));
__off_t		lseek		__PROTO ((int _fildes, __off_t _offset,
					  int _whence));
int		pause		__PROTO ((void));
int		pipe		__PROTO ((int _fildes [2]));
int		read		__PROTO ((int _fildes, __VOID__ * _buf,
					  size_t _nbyte));
int		rmdir		__PROTO ((__CONST__ char * _path));
int		setgid		__PROTO ((__gid_t _gid));
int		setuid		__PROTO ((__uid_t _uid));
unsigned int	sleep		__PROTO ((unsigned int _seconds));
char	      *	ttyname		__PROTO ((int _fildes));
int		unlink		__PROTO ((__CONST__ char * _path));
int		write		__PROTO ((int _fildes,
					  __CONST__ __VOID__ * _buf,
					  size_t _nbyte));

#if	0		/* not supported */
long		fpathconf	__PROTO ((int _fildes, int _name));
int		getgroups	__PROTO ((int _gidsetsize,
					  __gid_t _grouplist []));
long		pathconf	__PROTO ((__CONST__ char * _path, int _name));
int		setpgid		__PROTO ((__pid_t _pid, __pid_t _pgid));
__pid_t		setsid		__PROTO ((void));
long		sysconf		__PROTO ((int _name));
__pid_t		tcgetpgrp	__PROTO ((int _fildes));
int		tcsetpgrp	__PROTO ((int _fildes, __pid_t _pgrp_id));
#endif	/* 0 */

#if	! _POSIX_SOURCE
int		brk		__PROTO ((__VOID__ * _endds));
__VOID__      *	sbrk		__PROTO ((int _incr));
#endif

__EXTERN_C_END__

#endif	/* ! defined (__UNISTD_H__) */
