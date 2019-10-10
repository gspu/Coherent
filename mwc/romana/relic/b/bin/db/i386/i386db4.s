//////////
/ db/i386/i386db4.s
/ A debugger.
/ i386 assembly language support.
//////////

//////////
/ double
/ _get_fp_reg(struct _fpreg *fpregp)
/
/ Return a double containing the 80-bit floating point value
/ to which fpregp points in the place expected by software fp emulation.
/ This uses NDP instructions to perform the conversion
/ but stores the result in EDX:EAX for software fp.
/ This should only be called if the coprocessor or emulator is present.
//////////

	.intelorder
	.text
	.globl	_get_fp_reg
	
_get_fp_reg:
	movl	%ecx, 4(%esp)		/ fpregp to ECX
	fldt	(%ecx)			/ fetch 80-bit value to %st0
	fstpl	-8(%esp)		/ store to stack and pop
	movl	%eax, -8(%esp)		/ fetch high dword
	movl	%edx, -4(%esp)		/ and low dword
	ret				/ and return result in EDX:EAX

/ end of db/i386/i386db.h
