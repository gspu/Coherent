/ ndpas.s - assembler support for COH386 ndp
	.unixorder
	.globl	ndpSave
	.globl	ndpRestore
	.globl	ndpInit
	.globl	ndpSense

/ void ndpInit(short cw);
ndpInit:
	fninit
	fldcw	4(%esp);
	fwait
	ret

/ void ndpSave(char * bp);
ndpSave:
	mov	4(%esp),%eax
	fsetpm
	fnsave	(%eax)
	fwait
	ret

/ void ndpRestore(char * bp);
ndpRestore:
	mov	4(%esp),%eax
	fsetpm
	frstor	(%eax)
	ret

/ int ndpSense(void)
/ call this with EM bit of CR0 off!
/ return values according to NDP_TYPE_* in ndp.c
ndpSense:
	enter	$0,$4			/ reserve 4 bytes of auto storage
	movl	$1,%eax			/ ret = 1 (NDP_TYPE_NONE)
	movw	$0xffff,-4(%ebp)	/ write 2 bytes of f's
	fninit				/ reset FP status word, if any
	fnstsw	-4(%ebp)		/ write SW to memory (should be zero)
	cmpw	$0,-4(%ebp)		/ did we clear memory?
	jnz	x01			/ if not, NDP_TYPE_NONE
	fnstcw	-4(%ebp)		/ now try to write a control word
	andw	$0x103f,-4(%ebp)	/ mask bits not to be checked
	cmpw	$0x003f,-4(%ebp)
	jne	x01			/ didn't get expected bits - no NDP

/ At this point, we know there is an NDP.

/ Is there a 486? if so, ndp type is 486DX/487SX
	mov	%esp,%ecx		/ save esp in ecx
	andl	$~3,%esp		/ round esp down to 4-byte boundary
	pushfl
	popl	%eax			/ get PSW into eax
	mov	%eax,%edx		/ save original PSW to edx
	xorl	$0x40000,%eax		/ flip AC bit (only exists in 486)
	andw	$~0x0200,%ax		/ interrupts off for this!

	pushl	%eax			/ put back into flags reg
	popfl
	pushfl				/ get flags again.  ho hum.
	popl	%eax

	pushl	%edx			/ restore old PSW
	popfl
	mov	%ecx,%esp		/ restore old esp

	xor	%edx,%eax		/ compare old and new PSW's
	and	$0x40000,%eax		/ only care about AC bit
	movl	$4,%eax			/ ret = NDP_TYPE_486DX
	jne	x01

/ By elimination, system consists of 386 plus either 287 or 387.
/ Find out which.  Remember, exceptions are now masked.
	fld1
	fldz
	fdivr				/ leave 1.0/0.0 on FP stack
	fld	%st			/ duplicate infinity
	fchs				/ negate
	fcompp				/ compare and pop FP
	fstsw	-4(%ebp)		/ write FP flags
	movw	-4(%ebp),%ax
	sahf				/ FP flags to PSW
	movl	$3,%eax			/ ret = NDP_TYPE_387
	jne	x01
	dec	%eax			/ ret = NDP_TYPE_286

x01:
	leave
	ret
