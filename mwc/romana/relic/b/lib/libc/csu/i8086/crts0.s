/ C run-time start-off.
/ Coherent native version.
/ Must be loaded at 0000

	.globl	main_
	.globl	environ_
	.globl	errno_
	.globl	_exit_

errno_	=	0002

loc_0:	jmp	start
	.blkw	3		/ spare space

start:
	mov	0, $0		/ clear location 0
	mov	bp, sp
	mov	ax, 4(bp)
	mov	environ_, ax
	call	main_
	push	ax
	call	exit_
_exit_:
	sys	1

	.prvd
environ_:
	.word	0

	.shrd
	.blkw	4
