//////////
/ libc/crt/i386/ducvt.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ double _ducvt()
/ Convert unsigned int in %eax to double, return in %edx:%eax.
/
/ There is no 32-bit shift, so the code below uses an n-1 bit shift
/ followed by a 1 bit shift rather than an n-bit shift;
/ this gets (double)1 right without a special case.
//////////

BIAS	.equ	1023

	.globl	_ducvt

_ducvt:
	bsrl	%edx, %eax		/ high bit position 0-31 to EDX (DL)
	jz	?zero			/ return 0.0
	push	%ecx
	movb	%cl, $31
	subb	%cl, %dl		/ shift count - 1 to CL
	shll	%eax, %cl		/ high bit lands at bit 31
	shll	%eax, $1		/ hidden bit into CF
	movl	%ecx, %edx
	addl	%ecx, $BIAS		/ biased exponent to ECX
	shll	%ecx, $20		/ position biased exponent
	movl	%edx, %eax
	shrl	%edx, $12		/ hi manissa to EDX
	shll	%eax, $20		/ lo mantissa to EAX
	orl	%edx, %ecx		/ exponent
	pop	%ecx
	ret

?zero:
	sub	%edx, %edx
	ret

/ end of libc/crt/i386/ducvt.s
