/ Coherent C library
/ i8086 small model

	.globl	setjmp_
	.globl	longjmp_

/ #include <setjmp.h>
/ int
/ setjmp(env);
/ jmp_buf env;
/
/ int
/ longjmp(env, value);
/ jmp_buf env;
/ int value;
/
/ These two routines implement non local gotos. "setjmp" treasures away,
/ int the supplied "env", the frame pointer, the stack pointer and the
/ return address. It then returns 0. A call to "longjmp" restores the stack
/ to that saved in the "env", and causes the call to "setjmp" to return
/ again, returning "value".

setjmp_:
	pop	cx			/ cx = return address.

	mov	bx, sp			/ bx = pointer to the
	mov	bx, 0(bx)		/ env. block.
	mov	0(bx), bp		/ save frame.
	mov	2(bx), sp		/ save stack (ret addr popped).
	mov	4(bx), cx		/ save ret addr.

	sub	ax, ax			/ return 0 to the
	ijmp	cx			/ caller.

longjmp_:
	mov	bx, sp			/ point bx into the stack and
	mov	ax, 4(bx)		/ grab the value
	mov	bx, 2(bx)		/ and the saved env. pointer.

	mov	cx, 0(bx)		/ cx is desired bp.

l0:	cmp	cx, 0(bp)		/ correct frame ??
	je	l1			/ yes.
	mov	bp, 0(bp)		/ unwind a frame.
	or	bp, bp			/ hit top ??
	jne	l0			/ nope, unwind another.

	mov	bp, cx			/ take a stab at
	jmp	l2			/ restoring the frame.

l1:	mov	sp, bp			/ reset stack to correct place
	pop	bp			/ and restore frame
	pop	di			/ and the two
	pop	si			/ register variables.

l2:	mov	sp, 2(bx)		/ reload sp and
	ijmp	4(bx)			/ return from the setjmp.
