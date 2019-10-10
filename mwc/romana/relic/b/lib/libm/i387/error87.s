//////////
/ /usr/src/libm/i387/error87.s
/ i387 mathematics library
/ _edomain(), _erange()
//////////

	.globl  _edomain
	.globl	_erange
	.globl	errno

//////////
/ _edomain replaces the NDP stacktop %st with 0.0
/ and sets errno_ to the appropriate error code.
/ _erange just sets the error code.
//////////

EDOM	=	33		/ domain error code
ERANGE	=	34		/ range error code

_edomain:
	movl	errno, $EDOM	/ Set errno.
	fstp	%st		/ Pop the stack
	fldz			/ and load a 0.
	ret

_erange:
	movl	errno, $ERANGE	/ Set errno.
	ret

/ end of error87.s
