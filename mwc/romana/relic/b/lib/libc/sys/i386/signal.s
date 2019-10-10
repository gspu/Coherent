//////////
/ libc/sys/i386/signal.s
/ Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991.
//////////

/ #include <signal.h>

	.globl	sighold
	.globl	sigrelse
	.globl	sigignore
	.globl	sigpause
	.globl	sigset
	.globl	signal
	.globl	.cerror

	.set	SIGDEFER,	0x100
	.set	SIGHOLD,	0x200
	.set	SIGRELSE,	0x400
	.set	SIGIGNORE,	0x800
	.set	SIGPAUSE,	0x1000
	.set	SIGDEFAULT,	0x8000


sighold:
	or	$SIGHOLD,4(%esp)
	jmp	signal

sigrelse:
	or	$SIGRELSE,4(%esp)
	jmp	signal

sigignore:
	or	$SIGIGNORE,4(%esp)
	jmp	signal

sigpause:
	or	$SIGPAUSE,4(%esp)
	jmp	signal

/	sigset(sig, SIG_DFL)
/	sigset(sig, SIG_IGN)
/	sigset(sig, proc) 

sigset:
	or	$SIGDEFER,4(%esp)

/	signal(sig, SIG_DFL)
/	signal(sig, SIG_IGN)
/	signal(sig, proc) 

signal:
	movl	$sigreturn,%edx		/ proc = &sigreturn
	movl	$48, %eax
	lcall	$0x7, $0		/ if old_sys = sigsys(sig)<0
	jc	.cerror			/	return -1
	orl	%eax,%eax
	ret		

sigreturn:
	addl	$4, %esp
	lcall	$0xf, $0

/ end of libc/sys/i386/signal.s
