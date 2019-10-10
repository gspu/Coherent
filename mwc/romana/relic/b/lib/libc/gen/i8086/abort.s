/
/ Abort.
/ Blow up with illegal instruction.
/

	.globl	abort_

abort_:
	push	si		/ For debugger's ease
	push	di
	push	bp
	mov	bp,sp
	sys	0		/ illegal
	mov	sp,  bp
	pop	bp
	pop	di
	pop	si
	ret
