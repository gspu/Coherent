//////////
/ libc/crt/i386/dicvt.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ double _dicvt()
/ Convert int in %eax to double, return in %edx:%eax.
//////////

SGNMASK	.equ	0x80000000

	.globl	_dicvt
	.globl	_ducvt

_dicvt:
	orl	%eax, %eax
	pushfl				/ save arg sign
	jns	?L0
	negl	%eax			/ complement negative arg

?L0:
	call	_ducvt			/ convert to double in EDX:EAX
	popfl
	jns	?L1
	xorl	%edx, $SGNMASK		/ negative arg, flip result sign

?L1:
	ret

/ end of libc/crt/i386/dicvt.s
