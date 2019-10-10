//////////
/ libc/gen/i386/ldiv.s
/ i386 C standard library.
/ ANSI 4.10.6.2, 4.10.6.4.
//////////

//////////
/ #include <stdlib.h>
/
/ div_t
/ div(int numer, int denom)
/
/ ldiv_t
/ ldiv(long numer, long denom)
/
/ Compute quotient and remainder of 'numer' by 'denom'.
/ Return a pointer to a statically allocated div_t/ldiv_t object.
/ Since int and long are both 32 bits, div() and ldiv() are identical.
//////////

	.data
quot:	.long	0
rem:	.long	0

numer	.equ	4
denom	.equ	numer + 4

	.text

	.globl	div
	.globl	ldiv

div:
ldiv:
	movl	%eax, numer(%esp)	/ Dividend to EAX
	cdq				/ Sign-extend to EDX:EAX
	idivl	denom(%esp)		/ Divide by divisor
	movl	quot, %eax		/ Store quotient
	movl	rem, %edx		/ Store remainder
	movl	%eax, $quot		/ Return pointer to result in EAX
	ret

/ end of libc/gen/i386/ldiv.s
