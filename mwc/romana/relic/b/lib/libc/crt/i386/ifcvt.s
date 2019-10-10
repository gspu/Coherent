//////////
/ libc/crt/i386/ifcvt.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ int _ifcvt()
/
/ Convert float in %eax to int, return in %eax.
//////////

MAXINT	.equ	0x7FFFFFFF
SGNMASK	.equ	0x80000000

	.globl	_idcvt
	.globl	_ifcvt
	.globl	_dfcvt

_ifcvt:
	push	%edx
	call	_dfcvt			/ (double)f to EDX:EAX
	call	_idcvt			/ (double)f to int in EAX
	pop	%edx
	ret

/ end of libc/crt/i386/ifcvt.s
