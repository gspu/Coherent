#ifndef	__KERNEL_SIGPROC_H__
#define	__KERNEL_SIGPROC_H__

#include <common/ccompat.h>
#include <common/_signal.h>
#include <common/_siginfo.h>
#include <kernel/_proc.h>

/*
 * iBCS2 specifies that signal () shares a system call number with sigset (),
 * sighold (), sigrelse (), sigignore (), and sigpause (). The latter are
 * distinguished according to the high 16 bits of the signal number, as below.
 */

#define	SIGSET		0x100
#define	SIGHOLD		0x200
#define	SIGRELSE	0x400
#define	SIGIGNORE	0x800
#define	SIGPAUSE	0x1000


/*
 * Common kernel equivalent for the various user-level flavours of "struct
 * sigaction".
 */

typedef	struct {
	__sigactfl_t	sa_flags;
	__sigfunc_t	sa_handler;
	__sigset_t	sa_mask;
} __sigaction_t;


/*
 * The functions declared below provide kernel modules access to data items
 * stored in various ways as part of the U area or process structure. All
 * access to these items should be done through these accessors to ensure
 * binary compatibility between object modules independent of the exact layout
 * of these structures. We return pointers because C lacks references...
 */

__EXTERN_C_BEGIN__

void	curr_signal_action	__PROTO ((int _signal,
					  __CONST__ __sigaction_t * _act,
					  __sigaction_t * _oact));
void	curr_signal_mask	__PROTO ((__CONST__ __sigset_t * _mask,
					  __sigset_t * _omask));
void	curr_signal_addmask	__PROTO ((__CONST__ __sigset_t * _mask));

int	proc_signal_masked	__PROTO ((__proc_t * _process, int _signal));

void	proc_send_signal	__PROTO ((__proc_t * _process,
					  __CONST__ __siginfo_t * _siginfo));

void	proc_unkill		__PROTO ((__proc_t * _process, int _signal));

int	curr_signal_pending	__PROTO ((void));

__sigmiscfl_t	curr_signal_misc	__PROTO ((__sigmiscfl_t _clear,
						  __sigmiscfl_t _set));
__sigmiscfl_t	proc_signal_misc	__PROTO ((__proc_t * _process));

__EXTERN_C_END__

#endif	/* ! defined (__KERNEL_SIGPROC_H__) */


