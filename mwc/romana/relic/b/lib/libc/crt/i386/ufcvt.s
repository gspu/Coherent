//////////
/ libc/crt/i386/ufcvt.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ unsigned int	_ufcvt()
/
/ Convert float in %eax to unsigned int, return in %eax.
//////////

	.globl	_udcvt
	.globl	_ufcvt
	.globl	_dfcvt

_ufcvt:
	push	%edx
	call	_dfcvt			/ (double)f to EDX:EAX
	call	_udcvt			/ (double)f to unsigned in EAX
	pop	%edx
	ret

/ end of libc/crt/i386/ufcvt.s
