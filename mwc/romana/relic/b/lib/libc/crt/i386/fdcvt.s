//////////
/ libc/crt/i386/fdcvt.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ "float" _fdcvt()
/ Convert double in %edx:%eax to float, return in %eax.
//////////

BIAS		.equ	1023
FBIAS		.equ	127
SGNMASK		.equ	0x80000000
EXPMASK		.equ	0x7FF00000
MANMASK		.equ	0x000FFFFF
FEXPMASK	.equ	0x7F800000
FMANMASK	.equ	0x007FFFFF
FHIDDEN		.equ	0x00800000
FMAXEXP		.equ	255

	.globl	_fdcvt

_fdcvt:
	push	%ecx
	mov	%ecx, %edx
	andl	%ecx, $EXPMASK		/ extract exponent in ECX
	jz	?zero			/ return 0.0, ignoring denormals
	orl	%edx, %edx
	pushfl				/ save arg sign
	shrl	%ecx, $20		/ biased double exponent in ECX
	subl	%ecx, $BIAS-FBIAS	/ adjust to float exponent bias
	jle	?zerof			/ underflow, return 0.0
	cmpl	%ecx, $FMAXEXP
	jge	?inf			/ overflow, return +-infinity
	andl	%edx, $MANMASK		/ high mantissa bits in EDX, hidden bit implicit
	shrd	%eax, %edx, $29		/ float mantissa in EAX, hidden bit implicit
	jnc	?pack			/ no rounding required
	incl	%eax			/ round up float mantissa
	testl	%eax, $FHIDDEN		/ check for carry past hidden bit
	jz	?pack			/ ok as is
	xorl	%eax, %eax		/ clear hidden bit in mantissa, now 0
	incl	%ecx			/ adjust the exponent
	cmpl	%ecx, $FMAXEXP
	jge	?inf			/ overflow, return +-infinity
?pack:
	shll	%ecx, $23		/ position float exponent
	orl	%eax, %ecx		/ combine exponent and mantissa
?setsign:
	popfl				/ restore arg sign
	jns	?done
	orl	%eax, $SGNMASK		/ set result sign
?done:
	pop	%ecx
	ret

?inf:
	movl	%eax, $FEXPMASK
	jmp	?setsign		/ return +-infinity

?zerof:
	popfl
?zero:
	subl	%eax, %eax		/ return 0.0
	pop	%ecx
	ret

/ end of libc/crt/i386/fdcvt.s
