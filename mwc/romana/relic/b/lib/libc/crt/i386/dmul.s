//////////
/ libc/crt/i386/dmul.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ double _dmul(double d)
/ Return d * %edx:eax in %edx:%eax.
/
/ In 32-bit dwords:
/ A	= hiA:loA
/ B	= hiB:loB
/ A*B	= hi(hiA*hiB):lo(hiA*hiB)
/	+	      hi(hiA*loB):lo(hiA*loB)
/	+	      hi(loA*hiB):lo(loA*hiB)
/	+			  lo(loA*loB):lo(loA*loB)
/ Multiplying two n bit quantities produces a 2*n-1 or 2*n bit result.
/ Thus, multiplying two 53 bit quantities produces a 105 or 106 bit result.
/ We want a normalized 53-bit result, so the lo-order dword above is irrelevant.
//////////

d	.equ	8
BIAS	.equ	1023
EXPMASK	.equ	0x7FF00000
MANMASK	.equ	0x000FFFFF
SGNMASK	.equ	0x80000000
HIDDEN	.equ	0x00100000

	.globl	_dmul

_dmul:
	push	%ebp
	movl	%ebp, %esp
	push	%esi
	push	%edi
	push	%ebx
	push	%ecx

	movl	%esi, d+4(%ebp)
	movl	%edi, d(%ebp)		/ d to ESI:EDI, call it A
					/ now done with EBP as index register

	/ Check for special cases +-0.0 and +-infinity on each side.
	movl	%ebx, %esi
	andl	%ebx, $EXPMASK
	jz	?retlhs			/ A is 0.0, return it; ignore denormal
	cmpl	%ebx, $EXPMASK
	jz	?retlhs			/ A is +-infinity or NaN, return it
	movl	%ecx, %edx
	andl	%ecx, $EXPMASK
	jz	?done			/ B is 0.0, return it; ignore denormal
	cmpl	%ecx, $EXPMASK
	jz	?done			/ B is +-infinity or NaN, return it

	/ Compute result sign.
	movl	%ebp, %edx
	xorl	%ebp, %esi
	andl	%ebp, $SGNMASK
	push	%ebp			/ save result sign bit

	/ Compute result exponent.
	shrl	%ebx, $20		/ A biased exp in EBX
	shrl	%ecx, $20		/ B biased exp in ECX
	subl	%ecx, $BIAS		/ unbiased
	addl	%ecx, %ebx		/ form biased result exponent
	jl	?zero			/ underflow, return 0.0
	cmpl	%ecx, $EXPMASK>>20
	jge	?inf			/ overflow, return +-infinity
	movl	%ebp, %ecx		/ save result exponent in EBP

	/ Perform the 64*64 bit multiply.
	andl	%esi, $MANMASK
	orl	%esi, $HIDDEN		/ extract A mantissa, restore hidden bit
	andl	%edx, $MANMASK
	orl	%edx, $HIDDEN		/ extract B mantissa, restore hidden bit
	movl	%ecx, %edx
	movl	%ebx, %eax		/ B mantissa to ECX:EBX

	mul	%edi			/ loA*loB
	push	%edx			/ save hi(loA*loB), ignore lo dword
	movl	%eax, %ebx		/ loB
	mul	%esi			/ hiA*loB
	xchgl	%edi, %eax		/ lo(hiA*loB) to EDI, loA to EAX
	movl	%ebx, %edx		/ hi(hiA*loB) to EBX
	mul	%ecx			/ loA*hiB
	xchgl	%esi, %eax		/ lo(loA*hiB) to ESI, hiA to EAX
	xchgl	%ecx, %edx		/ hi(loA*hiB) to ECX, hiB to EDX
	mul	%edx			/ hiA*hiB, leave in EDX:EAX

	/ Add partial products to obtain 96 bit product in EDX:EAX:ESI.
	/ This is really a 105-32=73 or 106-32=74 bit value, higher bits are 0.
	addl	%esi, %edi
	adcl	%eax, %ebx
	adcl	%edx, $0
	pop	%edi			/ hi(loA*loB)
	addl	%esi, %edi
	adcl	%eax, %ecx
	adcl	%edx, $0

	/ Normalize the result mantissa.
	/ The product presently contains 73 or 74 bits.
	/ The normalized result contains 53+32 = 85 bits.
	movb	%cl, $12		/ shift count to CL
	testl	%edx, $0x200		/ examine product bit 74
	jz	?L0			/ 73 bit product
	decb	%cl			/ 74 bit product, decrement shift count
	incl	%ebp			/ and bump the exponent

?L0:
	/ Shift EDX:EAX:ESI left by CL bits.
	shld	%edx, %eax, %cl
	shld	%eax, %esi, %cl
	shll	%esi, %cl

	/ Round up result if appropriate.
	shll	%esi, $1		/ high bit of lo 64 bits to CF
?addcarry:
	adcl	%eax, $0
	adcl	%edx, $0
	testl	%edx, $HIDDEN<<1
	jnz	?carry

	/ Pack result mantissa from EDX:EAX, exponent from EBP, sign from stack.
?pack:
	andl	%edx, $MANMASK		/ mask off hidden bit
	orl	%ebp, %ebp
	jle	?zero			/ exponent underflow, return 0.0
	cmp	%ebp, $EXPMASK>>20
	jge	?inf			/ exponent overflow, return infinity
	shll	%ebp, $20		/ position exponent
	orl	%edx, %ebp		/ pack mantissa and exponent
	pop	%ecx
	orl	%edx, %ecx		/ pack with sign

	/ Return the value from EDX:EAX.
?done:
	pop	%ecx
	pop	%ebx
	pop	%edi
	pop	%esi
	pop	%ebp
	ret

	/ Rounding generated carry past hidden bit, shift one bit right.
?carry:
	incl	%ebp			/ bump the exponent
	shrd	%eax, %edx, $1
	pushfl
	shrl	%edx, $1
	popfl
	jmp	?addcarry

	/ Return the value from ESI:EDI.
?retlhs:
	xchgl	%esi, %edx
	xchgl	%edi, %eax
	jmp	?done

?inf:
	pop	%edx			/ pop result sign bit
	orl	%edx, $EXPMASK		/ max exp, zero mantissa for infinity
	jmp	?L2

?zero:
	pop	%edx			/ pop sign bit and ignore
	subl	%edx, %edx		/ return 0.0
?L2:
	subl	%eax, %eax
	jmp	?done

/ end of libc/crt/i386/dmul.s
