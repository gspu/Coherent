//////////
/ libc/crt/i386/_modf.s
/ i386 C compiler library.
/ IEEE software floating point support.
//////////

//////////
/ double _modf(real, dp, exp) double real; double *dp; int exp;
/ 
/ Stores g == greatest integer <= real through dp and returns real - g.
/ This assumes real >= 0, 1 <= exp = exponent(real) <= DMBITS+1,
/ where DMBITS is the number of mantissa bits in a double.
/ modf.c deals with the other cases.
//////////

d	.equ	4
dp	.equ	d+8
exp	.equ	dp+4
EXPMASK	.equ	0x7FF00000
MANMASK	.equ	0x000FFFFF
HIDDEN	.equ	0x00100000
DEFEXP	.equ	1022
DMBITS	.equ	52

	.globl	_modf

_modf:
	movl	%edx, d+4(%esp)
	andl	%edx, $MANMASK
	movl	%eax, d(%esp)		/ mantissa to EDX:EAX
	movl	%ecx, $DMBITS+1		/ mantissa bits
	subl	%ecx, exp(%esp)		/ bits in integer result to ECX

	/ Shift EDX:EAX right to clobber unwanted digits.
	shrd	%eax, %edx, %cl		/ shift by CL mod 32
	shrl	%edx, %cl
	cmpl	%ecx, $32
	jl	?L0
	subl	%eax, %eax		/ zap lo dword, same effect as long shifts
?L0:
	shld	%edx, %eax, %cl		/ shift back to original position
	shll	%eax, %cl

	/ EDX:EAX contains the integer part mantissa, now supply the exponent.
	movl	%ecx, d+4(%esp)
	andl	%ecx, $EXPMASK		/ grab old exponent
	orl	%edx, %ecx		/ combine mantissa and exponent
	movl	%ecx, dp(%esp)		/ destination to ECX
	movl	4(%ecx), %edx
	movl	(%ecx), %eax		/ store integer part through dp

	/ Compute remainder.  First, shift away the integer part bits.
	movl	%edx, d+4(%esp)
	movl	%eax, d(%esp)		/ mantissa to EDX:EAX
	movl	%ecx, exp(%esp)		/ exponent to ECX
	cmpl	%ecx, $32
	jl	?L1
	movl	%edx, %eax		/ shift EDX:EAX 32 bits left
	subl	%eax, %eax
?L1:
	shld	%edx, %eax, %cl		/ first fraction digit to hidden bit
	shll	%eax, %cl		/ shift by CL mod 32
	movl	%ecx, $DEFEXP		/ default exponent to ECX
	andl	%edx, $MANMASK|HIDDEN	/ mask result
	jnz	?loop
	orl	%eax, %eax
	jz	?done			/ no bits left, return 0

	/ Normalize.  This should use "bsrl" but currently uses a loop instead.
?loop:
	testl	%edx, $HIDDEN		/ watch for hidden bit
	jnz	?pack
	shld	%edx, %eax, $1
	shll	%eax, $1		/ shift left one bit
	decl	%ecx			/ and adjust the exponent
	jmp	?loop			/ and repeat until hidden bit appears

?pack:
	andl	%edx, $MANMASK		/ mask
	shll	%ecx, $20
	orl	%edx, %ecx		/ and supply appropriate exponent
?done:
	ret

/ end of libc/crt/i386/_modf.s
