//////////
/ libc/gen/i386/alloca.s
/ Dynamic stack allocation.
//////////

//////////
/ char *
/ alloca(int n)
/ Grab n bytes directly from the stack.
//////////

	.globl	alloca
	.text

alloca:
	popl	%edx		/ return address to EDX
	popl	%eax		/ nbytes to EAX
	negl	%eax		/ -nbytes
	andl	%eax, $-4	/ round up to dword
	movl	%ecx, %esp	/ saved register pointer to ECX
	addl	%eax, %esp	/ return pointer to EAX
	movl	%esp, %eax	/ allocate
	pushl	8(%ecx)		/ copy saved registers
	pushl	4(%ecx)
	pushl	(%ecx)
	pushl	%eax		/ restore arg
	jmp	*%edx		/ and return

/ end of alloca.s
