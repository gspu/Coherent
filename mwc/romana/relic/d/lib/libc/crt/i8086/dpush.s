////////
/
/ Intel 8086 floating point.
/ push accumulator.
/ small model.
/
////////

	.globl	dpush
	.globl	_fpac_

////////
/
/ ** dpush -- push double accumulator.
/
/ this utility routine is called by the compiler to push the double
/ accumulator (a quad word called _fpac_) onto the 8086 stack. this
/ is a bit funny, because the return address has to be popped  into
/ a safe place first, or it will be captured.
/
/ compiler calling sequence:
/	call	dpush
/
/ outputs:
/	_fpac_ pushed onto stack.
/	sp=updated (decremented by 8)
/
/ uses:
/	bx
/
////////

dpush:	pop	bx			/ bx = return address
	push	_fpac_+6		/ push
	push	_fpac_+4		/ all
	push	_fpac_+2		/ four
	push	_fpac_+0		/ words
	ijmp	bx			/ return
