//////////
/ rts87.s
/ cc386 80x87 runtime support.
//////////

	.text

	.globl	_cfcc
	.globl	_tstcc
	.globl	_tstccp
	.globl	_dp87
	.globl	_fdcvt
	.globl	_vdcvt
	.globl	_ldcvt
	.globl	_pdcvt
	.globl	_udcvt
	.globl	_idcvt
	.globl	_dfcvt
	.globl	_dvcvt
	.globl	_dlcvt
	.globl	_ducvt
	.globl	_dicvt
	.globl	_dpcvt
	.globl	ldexp
	.globl	frexp
	.globl	modf

/ The following should be unnecessary but...
	.globl	_fvcvt
	.globl	_flcvt
	.globl	_fucvt
	.globl	_ficvt

cwchop	.word	0x0F3F			/ chop control word
two	.word	2			/ constant 2

RASIZE	=	4			/ size of a return address
EBPSIZE	=	4			/ size of saved %ebp

//////////
/ _tstcc()	Test %st and copy NDP condition codes.
/ _tstccp()	Test %st and pop and copy NDP condition codes.
/ _cfcc()	Copy NDP condition codes.
/
/ _tstcc() tests %st against 0 and then does a _cfcc().
/ _tstccp() tests %st against 0, pops it, and then does a _cfcc.
/ _cfcc() copies the C0 and C3 status flags from the NDP
/ into the Cf and Zf flags of the 80x86.
/ This uses the otherwise useless opcode that copies the flags
/ from %ah in 8080 format.
/
/ Input:
/	%st for _tstcc() and _tstccp(), NDP status word for _cfcc().
/ Output:
/	80x86 flags Cf and Zf.
/	NDP stack popped for _tstccp().
//////////

_tstcc:
	ftst				/ Test %st against 0
	jmp	_cfcc			/ and set the condition codes.
_tstccp:
	ftst				/ Test st against 0,
	fstp	%st			/ pop the 80x87 stack,
					/ and fall through...
_cfcc:
	fstsw	%ax			/ Store the 80x87 status word to %eax
	fwait				/ and wait for it to finish up.
	sahf				/ Load Cf and Zf correctly.
	ret

//////////
/ _dp87()	Double push from NDP stacktop to 80x86 stack.
/
/ Push a double from the NDP stacktop %st to the system stack.
/ There is some magic to keep the return address in one piece.
/
/ Input:
/	Double in %st.
/ Outputs:
/	NDP stack popped.
/	Double on 80x86 stack.
//////////

_dp87:
	pop	%eax			/ Return address to %eax.
	subl	%esp, $8		/ Claim a double on stack and
	fstpl	(%esp)			/ pop 80x87 to memory.
	fwait				/ Make sure pop is completed.
	ijmp	%eax			/ Return to the caller.

//////////
/ _dicvt()	Convert integer to double.
/ _ducvt()	Convert unsigned integer to double.
/ _dpcvt()	Convert pointer to double.
/ _dlcvt()	Convert long integer to double.
/ _dvcvt()	Convert unsigned long integer to double.
/ _dfcvt()	Convert float to double.
/
/ Widen to double.
/ Some simple widens can be compiled inline,
/ but the unsigned widens and the signed
/ widens when the object lacks an lvalue cannot be.
/
/ Input:
/	Operand passed in standard C fashion on 80x86 stack.
/ Output:
/ 	Result in %st.
//////////

IARG	=	RASIZE

_dicvt:
_ficvt:
_dlcvt:
_flcvt:
	fildl	IARG(%esp)		/ Load and convert the integer.
	ret

UARG	=	RASIZE+EBPSIZE
INT64	=	-8			/ Auto int64 offset.

_ducvt:
_dpcvt:
_fucvt:
_dvcvt:
_fvcvt:
	push	%ebp
	movl	%ebp, %esp
	subl	%esp, $8		/ Claim a 64 bit integer.

	movl	%eax, UARG(%ebp)	/ Grab unsigned integer
	movl	INT64(%ebp), %eax	/ and store in 64 bit integer low half.
	movl	INT64+4(%ebp), $0	/ Zero-extend to high half.
	fildll	INT64(%ebp)		/ Load and convert to double.

	movl	%esp, %ebp
	pop	%ebp
	ret

DARG	=	RASIZE+EBPSIZE
FLOAT	=	-4			/ Auto float offset.

_dfcvt:
	push	%ebp
	movl	%ebp, %esp
	push	%eax			/ Claim a float.

	fldl	DARG(%ebp)		/ Grab double and
	fstps	FLOAT(%ebp)		/ round to a float.
	flds	FLOAT(%ebp)		/ Load result.

	movl	%esp, %ebp
	pop	%ebp
	ret

//////////
/ _idcvt()	Convert double to integer.
/ _udcvt()	Convert double to unsigned integer.
/ _pdcvt()	Convert double to pointer.
/ _ldcvt()	Convert double to long integer.
/ _vdcvt()	Convert double to unsigned long integer.
/ _fdcvt()	Convert double to float.
/
/ Shrinks from double.
/ The task is a little harder than would be expected.
/ Shift the 80x87 into chop mode for integer conversions
/ so that conversions work the right way.
/
/ Input:
/ 	Double argument on the 80x86 stack.
/ Output:
/	Result in %eax for [iulv]dcvt(), %st for _fdcvt().
//////////

SAVECW	=	-6			/ Auto saved control word offset.
INT	=	-4			/ Auto int32 offset.

_idcvt:
_ldcvt:
	push	%ebp
	movl	%ebp, %esp
	subl	%esp, $6		/ Claim a word and a dword.

	fstcw	SAVECW(%ebp)		/ Save old control word.
	fldcw	cwchop			/ Load chopping control word.
	fldl	DARG(%ebp)		/ Load up double and
	fistpl	INT(%ebp)		/ convert to integer and
	fldcw	SAVECW(%ebp)		/ put control word back.
	movl	%eax, INT(%ebp)		/ Result to %eax.

	movl	%esp, %ebp
	pop	%ebp
	ret

SAVECW	=	-10			/ Auto saved control word offset.
INT64	=	-8			/ Auto int64 offset.

_pdcvt:
_udcvt:
_vdcvt:
	push	%ebp
	movl	%ebp, %esp
	subl	%esp, $10		/ Claim a word and a 64-bit int.

	fstcw	SAVECW(%ebp)		/ Save old control word.
	fldcw	cwchop			/ Load chopping control word.
	fldl	DARG(%ebp)		/ Load up double and
	fistpll	INT64(%ebp)		/ convert to integer and
	fldcw	SAVECW(%ebp)		/ put control word back.
	movl	%eax, INT64(%ebp)	/ Unsigned long result to %eax.
	cmpl	INT64+4(%ebp), $0	/ If hi result dword is zero,
	je	?0			/ return lo result dword.
	movl	%eax, $-1		/ Overflow, return UINTMAX.

?0:
	movl	%esp, %ebp
	pop	%ebp
	ret

FLOAT	=	-4			/ Auto float offset.

_fdcvt:
	push	%ebp
	movl	%ebp, %esp
	push	%eax			/ Claim a float.

	fldl	DARG(%ebp)		/ Load the double and
	fstps	FLOAT(%ebp)		/ convert it to a float.
	flds	FLOAT(%ebp)		/ Load the float.

	movl	%esp, %ebp
	pop	%ebp
	ret

//////////
/ double
/ ldexp(fraction, exponent) double fraction; int exponent;
/
/ Assemble a double precision floating point number
/ from the given fraction and integer exponent.
//////////

FRAC	=	RASIZE+EBPSIZE
EXP	=	FRAC+8

ldexp:
	push	%ebp
	movl	%ebp, %esp

	fildl	EXP(%ebp)		/ Grab exponent
	fldl	FRAC(%ebp)		/ and fraction.
	fscale				/ Put it all together
	fstp	%st(1)			/ and pop stack.

	pop	%ebp
	ret

//////////
/ double
/ frexp(value, expp) double value; int *expp;
/
/ Split a double float into its fraction and its exponent.
/ The "fxtract" instruction almost does the job.
/ The IEEE standard says that the significand is not from 0.5 to 1
/ but from 1 to 2.
/ The extracted significand and exponent must be adjusted if nonzero.
//////////

VALUE	=	RASIZE+EBPSIZE
EXPP	=	VALUE+8

frexp:
	push	%ebp
	movl	%ebp, %esp

	fldl	VALUE(%ebp)		/ Load the value.
	fxtract				/ Significand to %st, unbiased exp to %st1.
	call	_tstcc			/ Check if the significand is zero.
	je	frexp0			/ Jump if it is.
	filds	two			/ %st=2.0, %st1=sig, %st2=exp.
	fdivp	%st(1), %st		/ %st=sig/2.0, %st1=exp.
	fld1				/ %st=1.0, %st1=sig/2.0, %st2=exp.
	faddp	%st(2), %st		/ %st=sig/2.0, %st1=exp+1.0.

frexp0:					/ Significand in %st, exponent in %st1.
	fxch				/ Exponent to %st, significand to %st1.
	movl	%eax, EXPP(%ebp)	/ Save the exponent
	fistpl	(%eax)			/ through the supplied pointer and pop.
	fwait				/ Make sure it is good.

	pop	%ebp
	ret

//////////
/ double
/ modf(value, intpart) double value; double *intpart;
/
/ Split a double (another way!).
//////////

VALUE	=	RASIZE+EBPSIZE
INTPART	=	VALUE+8
SAVECW	=	-2			/ Auto saved control word offset.

modf:
	push	%ebp
	movl	%ebp, %esp
	push	%eax			/ Claim one dword (only 2 bytes used).

	fstcw	SAVECW(%ebp)		/ Save the old control word and
	fldcw	cwchop			/ load chopping control word.
	fldl	VALUE(%ebp)		/ Pick up argument.
	fld	%st			/ %st=arg, %st1=arg.
	frndint				/ %st=intpart, %st1=arg.
	fsub	%st(1), %st		/ %st=intpart, %st1=fracpart.
	movl	%eax, INTPART(%ebp)	/ Load integer pointer.
	fstpl	(%eax)			/ Store intpart through it and pop.
	fldcw	SAVECW(%ebp)		/ Put old control word back.

	movl	%esp, %ebp
	pop	%ebp
	ret

/ end of rts87.s

