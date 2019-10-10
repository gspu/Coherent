////////
/
/ Clocked Functions - schedule function 'f' to be periodically invoked.
/		    - invocation is at clock interrupt level
/		    - only one function can be clocked
/
/	clocked( f, hz )
/	void (*f)();
/	int hz;
/
////////

	.globl	clocked_

////////
/
/ Hardware Dependant Constants
/
////////

	EOI	= 0x20		/ Non-specific End of Interrupt command
	PIC	= 0x20		/ 8259 pic   base address
	PIT	= 0x40		/ 8253 timer base address

////////
/
/ Private Data Declarations
/
////////

	.bssd
stack:	.blkb	256		/ Run time stack for the clocked function.
stktop:

	.shri	/ CODE SPACE!
uss:	.word	0		/ User stack segment
usp:	.word	0		/ User stack pointer
kds:	.word	0		/ Kernel data segment
oldclk:	.word	0		/ Previous clock interrupt entry point.
reload:	.word	0		/ Number of polls per logical clock tick.
resid:	.word	0		/ Number of polls left in logical clock tick.
cfunc:	.word	0		/ Clocked function
	.shri

////////
/
/ clocked( f, hz )
/ void (*f)();
/ int hz;
/
/	Input:	f  = function to be clocked.
/		hz = desired clock rate in invocations/second.
/
/	Action:	Calculate the number of polls per logical clock tick.
/		Seize the clock hardware interrupt.
/		Preserve the previous clock interrupt handler.
/		Reprogram the hardware clock to the desired rate.
/
/	Return:	 0 = clock interrupt seized.
/		-1 = clock interrupt previously seized.
/
/	Notes:	The logical clock rate to the OS will be unchanged.
/
////////

clocked_:
	pop	ax		/ Convert IP into PSW,CS,IP so can use iret.
	pushf
	push	cs
	push	ax

	mov	cs:kds, ds	/ Remember the kernel data segment

	cli			/ Disable interrupts

	push	es		/ Seize clock interrupt vector
	sub	ax, ax
	mov	es, ax
	mov 	ax, es:0x0020
	cmp	ax, $clocker
	jne	0f
	mov	ax, $-1
	pop	es
	iret

0:	mov	cs:oldclk, ax
	mov	es:0x0020, $clocker
	pop	es

	mov	bx, sp		/ Remember the function to be clocked.
	mov	ax, ss:6(bx)
	mov	cs:cfunc, ax

	mov	ax, ss:8(bx)	/ Compute polling rate relative to 20 hz.
	sub	dx, dx
	mov	cx, $20
	div	cx
	mov	cs:reload, ax
	mov	cs:resid, ax

	movb	al, $0x36	/ Timer 0, LSB, MSB, mode 3
	outb	PIT+3, al
	mov	ax, $59659
	sub	dx, dx
	div	cs:reload
	outb	PIT, al		/ LSB of 59659/reload
	jmp	0f
0:	movb	al, ah
	outb	PIT, al		/ MSB of 59659/reload

	sub	ax, ax
	iret

////////
/
/	clocker()	- initiate clocked function.
/
/	Action:	Every hardware clock tick, invoke the clocked function.
/		Every 'reload' hardware clock ticks,
/			simulate a logical clock interrupt to the OS.
/
////////

clocker:
	push	ax		/ Save registers.
	push	bx
	push	cx
	push	dx
	push	ds
	push	es

	mov	cs:uss, ss	/ Save current stack.
	mov	cs:usp, sp

	mov	ax, cs:kds	/ Remap Data/Extra/Stack Segments into Kernel.
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, $stktop	/ Remap stack pointer onto private intr stack.

	icall	cs:cfunc	/ Call the clocked function.

	mov	ss, cs:uss	/ Restore previous stack.
	mov	sp, cs:usp
	
	pop	es		/ Restore registers.
	pop	ds
	pop	dx
	pop	cx
	pop	bx

	dec	cs:resid
	je	0f
	movb	al, $EOI
	outb	PIC, al
	pop	ax
	iret

0:	mov	ax, cs:reload
	mov	cs:resid, ax
	pop	ax
	ijmp	cs:oldclk
