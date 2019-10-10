/* $Header: /src386/STREAMS/coh.386/RCS/sys6.c,v 2.1 93/08/09 13:36:55 bin Exp Locker: bin $ */
/*
 * POSIX.1-oriented system calls for Coherent.
 *
 * Conventions: as elsewhere, system call handlers have the same name as the
 * user would use but prefixed by a 'u'. Internal data interfaces have the
 * same name as a user function if they have the same signature, which is a
 * good thing for testing, and can save on redundant prototypes. Wherever
 * possible, we use such function-call interfaces rather than get involved in
 * the disgusting mess that is the U area or process-table mechanisms.
 */
/*
 * $Log:	sys6.c,v $
 * Revision 2.1  93/08/09  13:36:55  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  14:55:33  nigel
 * Nigel's R80
 * 
 */

#include <common/ccompat.h>
#include <sys/signal.h>
#include <unistd.h>


/*
 *-STATUS:
 *	POSIX.1
 *
 *-NAME:
 *	sigaction ()	Detailed signal management
 *
 *-SYNOPSIS:
 *	#include <signal.h>
 *
 *	int sigaction (int sig, const struct sigaction * act,
 *		       struct sigaction * oact);
 *
 *-DESCRIPTION:
 *	sigaction () allows the calling process to examine and/or specify the
 *	action to be taken on delivery of a specific signal.
 *
 *	"sig" specifies the signal and can be assigned any of the signals
 *	specified in signal(5) except SIGKILL and SIGSTOP.
 *
 *	If the argument "arg" is not NULL, it points to a structure specifying
 *	the new action to be taken when delivering "sig". If the argument
 *	"oact" is not NULL, it points to a structure where the action
 *	previously associated with "sig" is to be stored on return from
 *	sigaction ().
 *
 *	The "sigaction" structure includes the following members:
 *		void	     (*	sa_handler) ();
 *		sigset_t	sa_mask;
 *		int		sa_flags;
 *
 *	"sa_handler" specifies the disposition of the signal and may take any
 *	of the values specified in signal (5).
 *
 *	"sa_mask" specifies a set of signals to be blocked while the signal
 *	handler is active. On entry to the signal handler, that set of signals
 *	is added to the set of signals already being blocked when the signal
 *	is delivered. In addition, the signal that caused the handler to be
 *	executed will also be blocked, unless the SA_NODEFER flag has been
 *	specified. SIGSTOP and SIGKILL cannot be blocked (the system silently
 *	enforces this restriction).
 *
 *	"sa_flags" specifies a set of flags used to modify the behaviour of
 *	the signal. It is formed by a logical OR of any of the following
 *	values (only SA_CLDSTOP is available to System V, Release 3
 *	applications):
 *
 *	SA_NOCLDSTOP	If set and "sig" equals SIGCHLD, "sig" will not be
 *			sent to the calling process when its child processes
 *			stop or continue.
 *
 *	SA_NOCLDWAIT	If set and "sig" equals SIGCHLD, the system will not
 *			create zombie processes when children of the calling
 *			process exit. If the calling process subsequently
 *			issues a wait (), it blocks until all of the calling
 *			process's child processes terminate, and then returns
 *			a value of -1 with "errno" set to ECHILD.
 *
 *	SA_ONSTACK	If set and the signal is caught and an alternate
 *			signal stack has been declared with sigaltstack (),
 *			the signal is delivered to the calling process on that
 *			stack. Otherwise, the signal is delivered on the
 *			same stack as the main program.
 *
 *	SA_RESETHAND	If set and the signal is caught, the disposition of
 *			the signal is reset to SIG_DFL, and the signal will
 *			not be blocked on entry to the signal handler (SIGILL,
 *			SIGTRAP, and SIGPWR cannot be automatically reset when
 *			delivered; the system silently enforces this
 *			restriction).
 *
 *	SA_NODEFER	If set and the signal is caught, the signal will not
 *			be automatically blocked by the kernel while it is
 *			being caught.
 *
 *	SA_RESTART	If set and the signal is caught, a system call that
 *			is interrupted by the execution of this signal's
 *			handler is transparently restarted by the system.
 *			Otherwise, the system call returns an EINTR error.
 *
 *	SA_SIGINFO	If cleared and the signal is caught, "sig" is passed
 *			as the only argument to the signal-catching function.
 *			If set and the signal is caught, pending signals of
 *			type "sig" are reliably queued to the calling process
 *			and two additional arguments are passed to the signal-
 *			catching function. If the second argument is not equal
 *			to NULL, it points to a "siginfo_t" structure
 *			containing the reason why the signal was generated;
 *			the third argument points to a "ucontext_t" structure
 *			containing the receiving process's context when the
 *			signal was delivered.
 *
 *	sigaction () fails if any of the following is true:
 *
 *	EINVAL		The value of the "sig" argument is not a valid signal
 *			number or is equal to SIGKILL or SIGSTOP.
 *
 *	EFAULT		"act" or "oact" points outside the process's allocated
 *			address space.
 *
 *-DIAGNOSTICS:
 *	On success, sigaction () returns zero. On failure, it returns -1 and
 *	sets "errno" to indicate the error.
 */

#if	__USE_PROTO__
int usigaction (int sig, __CONST__ struct sigaction * act,
		struct sigaction * oact)
#else
int
usigaction (sig, act, oact)
int			sig;
__CONST__ struct sigaction
		      *	act;
struct sigaction      *	oact;
#endif
{
	struct sigaction temp;
	/*
	 * Once we validate the user pointers, we *must* take a local copy of
	 * either the previous signal setting or the data pointed to by "act"
	 * so that the caller is free to use the same pointer for both input
	 * and output arguments. Naturally, we must totally validate the new
	 * data before storing anything back to the user, irrespective of how
	 * convenient it might be to let a lower layer deal with this.
	 */
	return -1;
}


/*
 *-STATUS:
 *	POSIX.1
 *
 *-NAME:
 *	sigpending ()	Examine signals that are blocked and pending
 *
 *-SYNOPSIS:
 *	#include <signal.h>
 *
 *	int sigpending (sigset_t * set);
 *
 *-DESCRIPTION:
 *	The sigpending () function retrieves those signals that have been sent
 *	to the calling process but are being blocked from delivery by the
 *	calling process's signal mask. The signals are stored in the space
 *	pointed to by the argument "set".
 *
 *	sigpending () fails if any of the following are true:
 *
 *	EFAULT		The "set" argument points outside the process's
 *			allocated address space.
 *
 *-DIAGNOSTICS:
 *	On success, sigpending () returns zero. On failure, it returns -1 and
 *	sets "errno" to indicate the error.
 */

#if	__USE_PROTO__
int usigpending (o_sigset_t * set)
#else
int
usigpending (set)
o_sigset_t    *	set;
#endif
{
	return -1;
}


/*
 *-STATUS:
 *	POSIX.1
 *
 *-NAME:
 *	sigprocmask ()	Change or examine signal mask
 *
 *-SYNOPSIS:
 *	#include <signal.h>
 *
 *	int sigprocmask (int how, const sigset_t * set, sigset_t * oset);
 *
 *-DESCRIPTION:
 *	The sigprocmask () function is used to examine and/or change the
 *	calling process's signal mask. If the value of "how" is SIG_BLOCK, the
 *	set pointed to by the argument "set" is added to the current signal
 *	mask. If "how" is SIG_UNBLOCK, the set pointed to by the argument
 *	"set" is removed from the current signal mask. If "how" is
 *	SIG_SETMASK, the current signal mask is replaced by the set pointed
 *	to by the argument "set". If the argument "oset" is not NULL, the
 *	previous mask is stored in the space pointed to by "oset". If the
 *	value of the argument "set" is NULL, the value "how" is not
 *	significant and the process's signal mask is unchanged; thus, the call
 *	can be used to enquire about currently blocked signals.
 *
 *	If there are any pending unblocked signals after the call to
 *	sigprocmask (), at least one of those signals will be delivered before
 *	the call to sigprocmask () returns.
 *
 *	It is not possible to block those signals that cannot be ignored [see
 *	sigaction ()]; this restriction is silently imposed by the system.
 *
 *	If sigprocmask () fails, the process's signal mask is not changed.
 *
 *	sigprocmask () fails if any of the following are true:
 *
 *	EINVAL		The value of the "how" argument is not equal to one of
 *			the defined values.
 * 
 *	EFAULT		The value of "set" or "oset" points outside the
 *			process's allocated address space.
 *
 *-DIAGNOSTICS:
 *	On success, sigprocmask () returns zero. On failure, it returns -1 and
 *	sets "errno" to indicate the error.
 */

#if	__USE_PROTO__
int usigprocmask (int how, __CONST__ o_sigset_t * set, o_sigset_t * oset)
#else
int
usigprocmask (how, set, oset)
int		how;
__CONST__ o_sigset_t
	      *	set;
o_sigset_t    *	oset;
#endif
{
	o_sigset_t	tmp;
	/*
	 * Once we validate the user pointers, we *must* either take a local
	 * copy of the previous signal mask or the data pointed at by "set" so
	 * that the user is free to use the same pointer for both input and
	 * output arguments.
	 */
	return -1;
}


/*
 *-STATUS:
 *	POSIX.1
 *
 *-NAME:
 *	fpathconf ()
 *	pathconf ()	get configurable pathname variables
 *
 *-SYNOPSIS:
 *	#include <unistd.h>
 *
 *	long fpathconf (int fildes, int name);
 *	long pathconf (const char * path, int name);
 *
 *-DESCRIPTION:
 *	The functions fpathconf () and pathconf () return the current value of
 *	a configurable limit or option associated with a file or directory.
 *	The "path" argument points to the pathname of a file or directory;
 *	"fildes" is an open file descriptor; and "name" is the symbolic
 *	constant (defined in <unistd.h>) representing the configurable system
 *	limit or option to be returned.
 *
 *	The values returned by pathconf () or fpathconf () depend on the type
 *	of file specified by "path" or "fildes". The following table contains
 *	the symbolic constants supported by pathconf () and fpathconf ()
 *	along with the POSIX defined return value. The return value is based
 *	on the type of file specified by "path" or "fildes".
 *
 *	_PC_LINK_MAX	The maximum value of a file's link count. If "path" or
 *			"fildes" refers to a directory, the value returned
 *			applies to the directory itself.
 *
 *	_PC_MAX_CANON	The number of bytes in a terminal canonical input
 *			queue. The behaviour is undefined if "path" or
 *			"fildes" does not refer to a terminal file.
 *
 *	_PC_MAX_INPUT	The number of bytes for which space will be available
 *			in a terminal input queue. The behaviour is undefined 
 *			"path" or "fildes" does not refer to a terminal file.
 *
 *	_PC_NAME_MAX	The number of bytes in a filename. The behaviour is
 *			undefined if "path" or "fildes" does not refer to a
 *			directory. The value returned applies to the filenames
 *			within the directory.
 *
 *	_PC_PATH_MAX	The number of bytes in a pathname. The behaviour is
 *			undefined if "path" or "fildes" does not refer to a
 *			directory. The value returned is the maximum length of
 *			a relative pathname when the specified directory is
 *			the working directory.
 *
 *	_PC_PIPE_BUF	The number of bytes that can be written atomically
 *			when writing to a pipe. If "path" or "files" refers to
 *			a pipe or FIFO, the value returned applies to the FIFO
 *			itself. If "path" or "fildes" refers to a directory,
 *			the value returned applies to any FIFOs that exist or
 *			can be created within the directory. If "path" or
 *			"fildes" refer to any other type of file, the
 *			behaviour is undefined.
 *
 *	_PC_CHOWN_RESTRICTED	The use of the chown () function is restricted
 *			to a process with appropriate priveleges, and to
 *			changing the group ID of a file only to the effective
 *			group ID of the process or to one of its supplementary
 *			group IDs. If "path" or "fildes" refers to a
 *			directory, the value returned applies to any files,
 *			other than directories, that exist or can be created
 *			within the directory.
 *
 *	_PC_NO_TRUNC	Pathname components longer than NAME_MAX generate an
 *			error. The behaviour is	undefined if "path" or
 *			"fildes" does not refer to a directory. The value
 *			returned applies to the filenames within the
 *			directory.
 *	_PC_VDISABLE	Terminal special characters can be disabled using this
 *			character value, if it is defined. The behaviour is
 *			undefined if "path" or "filedes" does not refer to a
 *			terminal file.
 *
 *	The value of the configurable system limit or option specified by
 *	"name" does not change during the lifetime of the calling process.
 *
 *	fpathconf () fails if the following is true:
 *
 *	EBADF		"fildes" is not a valid file descriptor.
 *
 *	pathconf () fails if any of the following are true:
 *
 *	EACCES		Search permission is denied for a component of the
 *			path prefix.
 *
 *	ELOOP		Too many symbolic links are encountered while
 *			translating "path".
 *
 *	EMULTIHOP	Components of "path" require hopping to multiple
 *			remote machines and the file system type does not
 *			allow it.
 *
 *	ENAMETOOLONG	The length of a pathname component exceeds PATH_MAX,
 *			or a pathname component is longer than NAME_MAX while
 *			_POSIX_NO_TRUNC is in effect.
 *
 *	ENOENT		"path" is needed for the command specified and the
 *			named file does not exist or if the "path" argument
 *			points to an empty string.
 *
 *	ENOLINK		"path" points to a remote machine and the link to that
 *			machine is no longer active.
 *
 *	ENOTDIR		A component of the path prefix is not a directory.
 *
 *	Both fpathconf () and pathconf () fail if the following is true:
 *
 *	EINVAL		if "name" is an invalid value.
 *
 *-DIAGNOSTICS:
 *	If fpathconf () or pathconf () is invoked with an invalid symbolic
 *	constant or the symbolic constant corresponds to a configurable system
 *	limit or option not supported on the system, a value of -1 is returned
 *	to the invoking process. If the function fails because the
 *	configurable system limit or option corresponding to "name" is not
 *	supported on the system the value of "errno" is not changed.
 */

#if	__USE_PROTO__
int ufpathconf (int fildes, int name)
#else
int
ufpathconf (fildes, name)
int		fildes;
int		name;
#endif
{
	return -1;
}


#if	__USE_PROTO__
int upathconf (__CONST__ char * path, int name)
#else
int
upathconf (path, name)
__CONST__ char * path;
int		name;
#endif
{
	return -1;
}


/*
 *-STATUS:
 *	POSIX.1
 *
 *-NAME:
 *	sysconf ()	get configurable system variables
 *
 *-SYNOPSIS:
 *	#include <unistd.h>
 *
 *	long sysconf (int name);
 *
 *-DESCRIPTION:
 *	The sysconf () function provides a method for the application to
 *	determine the current value of a configurable system limit or option.
 *
 *	The "name" argument represents the system variable to be queried. The
 *	following table lists the minimal set of system variables from
 *	<limits.h> and <unistd.h> that can be returned by sysconf (), and the
 *	symbolic constants that are the corresponding values used for "name":
 *
 *	NAME:			RETURN VALUE:
 *	_SC_ARG_MAX		ARG_MAX
 *	_SC_CHILD_MAX		CHILD_MAX
 *	_SC_CLK_TCK		CLK_TCK
 *	_SC_NGROUPS_MAX		NGROUPS_MAX
 *	_SC_OPEN_MAX		OPEN_MAX
 *	_SC_PASS_MAX		PASS_MAX
 *	_SC_PAGESIZE		PAGESIZE
 *	_SC_JOB_CONTROL		_POSIX_JOB_CONTROL
 *	_SC_SAVED_IDS		_POSIX_SAVED_IDS
 *	_SC_VERSION		_POSIX_VERSION
 *	_SC_XOPEN_VERSION	_XOPEN_VERSION
 *	_SC_LOGNAME_MAX		LOGNAME_MAX
 *
 *	The value of CLK_TCK may be variable and it should not be assumed that
 *	CLK_TCK is a compile-time constant. The value of CLK_TCK is the same
 *	as the value of sysconf (_SC_CLK_TCK).
 *
 *-DIAGNOSTICS:
 *	If "name" is an invalid value, sysconf () will return -1 and set
 *	"errno" to indicate the error. If sysconf () fails due to a value of
 *	"name" that is not defined on the system, the function will returne
 *	a value of -1 without changing the value of "errno".
 *
 *-NOTES:
 *	A call to setrlimit () may cause the value of OPEN_MAX to change on
 *	System V, Release 4-compatible systems.
 */

#if	__USE_PROTO__
long usysconf (int name)
#else
long
usysconf (name)
int		name;
#endif
{
	return -1;
}
