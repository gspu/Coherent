/ long _canl(l) long l;
/ This is called by the routines that
/ transform longs to and from the
/ canonical formats.

	.globl	_canl_

_canl_:
	mov	bx, sp
	mov	dx, 2(bx)
	mov	ax, 4(bx)
	ret
