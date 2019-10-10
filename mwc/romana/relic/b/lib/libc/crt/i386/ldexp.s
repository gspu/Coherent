//////////
/ libc/crt/i386/ldexp.s
/ i386 C runtime library.
/ IEEE software floating point support.
/ i8086 IEEE software floating point library.
/ ldexp()
//////////

//////////
/ double
/ ldexp(dbl, exp) double dbl; int exp;
/
/ Returns dbl * 2^^exp.
//////////

	.globl	ldexp

dbl	.equ	4
exp	.equ	dbl+8
EXPMASK	.equ	0x7FF00000
SGNMASK	.equ	0x80000000
MAXEXP	.equ	2047

ldexp:
	movl	%ecx, dbl+4(%esp)		/ high word of dbl to ECX
	movl	%edx, $EXPMASK
	andl	%edx, %ecx			/ shifted exponent to EDX
	xorl	%ecx, %edx			/ mask off exponent in ECX
	shrl	%edx, $20			/ exponent in EDX
	addl	%edx, exp(%esp)			/ bump by exp
	jle	?underflow
	cmpl	%edx, $MAXEXP
	jge	?overflow
	shll	%edx, $20
	orl	%edx, %ecx			/ hi mantissa bits to EDX
	movl	%eax, dbl(%esp)			/ lo manitssa dword to EAX
	ret

?underflow:
	andl	%ecx, $SGNMASK			/ preserve sign
	jmp	?done

?overflow:
	andl	%ecx, $SGNMASK			/ preserve sign
	orl	%ecx, $EXPMASK			/ max exponent
?done:
	movl	%edx, %ecx			/ sign/exp to EDX
	subl	%eax, %eax			/ 0 lo mantissa to EAX
	ret

/ end of libc/crt/i386/ldexp.s
