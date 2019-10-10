//////////
/ libc/gen/i386/setjmp.s
/ i386 C library.
/ ANSI 4.6.1.1, 4.6.2.1.
//////////

//////////
/ #include <setjmp.h>
/
/ int
/ setjmp(env) jmp_buf env;
/ Store the current register "environment"
/ (frame pointer, stack pointer, return address, register variables)
/ into "env" and return 0.
/
/ void
/ longjmp(env, value) jmp_buf env; int value;
/ Restore the registers from "env", causing the setjmp() call to
/ return again, this time returning the given "value".
/
/ For i386, the "jmp_buf" structure is an array of six dwords.
//////////

SAVEEBP	.equ	0
SAVEESP	.equ	SAVEEBP+4
SAVERA	.equ	SAVEESP+4
SAVEESI	.equ	SAVERA+4
SAVEEDI	.equ	SAVEESI+4
SAVEEBX	.equ	SAVEEDI+4

	.globl	setjmp
	.globl	longjmp

setjmp:
	pop	%ecx			/ return address to ECX
	movl	%edx, (%esp)		/ env to EDX
	movl	SAVEEBP(%edx), %ebp	/ save EBP
	movl	SAVEESP(%edx), %esp	/ save ESP (after return address popped)
	movl	SAVERA(%edx), %ecx	/ save return address
	movl	SAVEESI(%edx), %esi	/ save ESI
	movl	SAVEEDI(%edx), %edi	/ save EDI
	movl	SAVEEBX(%edx), %ebx	/ save EBX
	subl	%eax, %eax		/ return value 0 to EAX
	ijmp	%ecx			/ and return

longjmp:
	pop	%edx			/ longjmp return address to bit bucket
	pop	%edx			/ env to EDX
	pop	%eax			/ return value to EAX
	orl	%eax, %eax		/ check if return value 0
	jnz	?0			/ nope, ok
	incl	%eax			/ ANSI 4.6.2.1: return 1 if arg is 0

?0:
	mov	%ebp, SAVEEBP(%edx)	/ restore EBP
	mov	%esp, SAVEESP(%edx)	/ restore ESP
	mov	%esi, SAVEESI(%edx)	/ restore ESI
	mov	%edi, SAVEEDI(%edx)	/ restore EDI
	mov	%ebx, SAVEEBX(%edx)	/ restore EBX
	ijmp	SAVERA(%edx)		/ return through saved return address

/ end of libc/gen/i386/setjmp.s
