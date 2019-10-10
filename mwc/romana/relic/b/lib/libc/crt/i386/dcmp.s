//////////
/ libc/crt/i386/dcmp.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ _dcmp(d) double d;
/ 
/ Compare %edx:%eax to double d on stack, return flags set appropriately.
/ Pops argument d before returning!
/ Bogus for denormals.
//////////

d	.equ	4
SGNMASK	.equ	0x80000000

	.globl	_dcmp

_dcmp:					/ Stack: hid lod ra
	xchgl	%ebx, d(%esp)		/ hid EBX ra, lod in EBX
	xchgl	%ecx, (%esp)		/ hid EBX ECX, ra in ECX
	xchgl	%ecx, d+4(%esp)		/ ra EBX ECX, d in ECX:EBX
	push	%esi			/ ra EBX ECX ESI
	movl	%esi, $1		/ value for > to ESI
	cmpl	%ecx, $SGNMASK		/ be wary of arg -0.0
	jnz	?L1
	subl	%ecx, %ecx		/ replace arg -0.0 with 0.0

?L1:	
	orl	%edx, %edx
	jns	?lhspos
	cmpl	%edx, $SGNMASK		/ be wary of lhs -0.0
	jnz	?lhsneg
	subl	%edx, %edx		/ turn lhs -0.0 into 0.0

?lhspos:				/ lhs is positive
	orl	%ecx, %ecx
	jns	?samesign		/ both positive
					/ else lhs positive, arg negative
?done:
	orl	%esi, %esi

?quit:
	pop	%esi
	pop	%ecx
	pop	%ebx
	ret

?samesign:
	cmpl	%edx, %ecx		/ both positive, compare magnitudes
	ja	?done			/ lhs greater
	jb	?less			/ lhs less
	cmpl	%eax, %ebx		/ hi equal, compare lo mantissa words	
	ja	?done
	jz	?quit

?less:
	negl	%esi			/ negate result sense
	jmp	?quit

?lhsneg:				/ lhs is negative
	negl	%esi			/ negate result sense
	orl	%ecx, %ecx
	jns	?done			/ lhs negative, arg positive, return less
	jmp	?samesign		/ both negative, compare magnitudes

/ end of libc/crt/i386/dcmp.s
