//////////
/ libc/crt/i386/dfcvt.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ double _dfcvt()
/ Convert float in %eax to double, return in %edx:%eax.
/ Ignores denormals and NaNs.
//////////

BIAS		.equ	1023
FBIAS		.equ	127
SGNMASK		.equ	0x80000000
EXPMASK		.equ	0x7FF00000
FEXPMASK	.equ	0x7F800000
FMANMASK	.equ	0x007FFFFF
FMAXEXP		.equ	255

	.globl	_dfcvt

_dfcvt:
	push	%ecx
	subl	%edx, %edx		/ 0 to EDX
	movl	%ecx, %eax		/ copy float to ECX
	andl	%ecx, $FEXPMASK		/ extract float exponent
	jz	?zero			/ return 0.0, ignoring denormals
	orl	%eax, %eax
	pushfl				/ save arg flag
	shrl	%ecx, $23		/ unshift float exponent
	cmpl	%ecx, $FMAXEXP		/ check for big exponent
	jz	?inf			/ return +-infinity, ignoring NaNs
	addl	%ecx, $BIAS-FBIAS	/ adjust to double exponent bias
	shll	%ecx, $20		/ double exponent to correct position
	and	%eax, $FMANMASK		/ extract float mantissa in EAX
	movl	%edx, %eax		/ and copy to EDX
	shrl	%edx, $3		/ shift hi mantissa
	shll	%eax, $29		/ and lo mantissa
	orl	%edx, %ecx		/ exponent

?setsign:
	popfl				/ restore arg sign flag
	jns	?L0
	orl	%edx, $SGNMASK		/ set sign bit
?L0:
	pop	%ecx
	ret

?inf:
	movl	%edx, $EXPMASK
	subl	%eax, %eax
	jmp	?setsign

?zero:
	subl	%eax, %eax		/ EDX is already 0, return 0.0
	pop	%ecx
	ret

/ end of libc/crt/i386/dfcvt.s
