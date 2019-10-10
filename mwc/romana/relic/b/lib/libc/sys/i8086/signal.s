/
/ C interface routines for the signal system call.
/ This is not just a direct call, but must save
/ and restore user state before and after processing
/ user signal code.
/
/ int (*signal(signo, function))() int signo; int (*function)();
/

NSIG	= 16

	.globl	signal_
	.globl	_fpac_

signal_:
	mov	bx,sp		/ get stack base pointer
	mov	cx,02(bx)	/ get signo
	cmp	cx,$NSIG
	ja	0f		/ > NSIG illegal

	cmp	04(bx),$1	/ function
	jbe	0f		/ 0 or 1 (SIG_DFL or SIG_IGN)

	mov	ax,$sigsave
	push	ax		/ user function changed to sigsave
	push	cx		/ signo
	push	bx		/ and junk word where pc would be
	sys	060		/ 48
	pop	bx		/ pop args
	pop	cx
	pop	cx
	jmp	1f		/ return value

0:	sys	060		/ 48
1:	cmp	ax,$-1
	je	1f		/ error return

	mov	cx,04(bx)	/ new user function
	mov	bx,02(bx)	/ signo
	add	bx,bx		/ check old signal setting
	cmp	sigtable-2(bx),$-1	/ in sigtable[signo-1]
	je	0f		/ unknown: yield syscall return

	mov	ax,sigtable-2(bx)	/ else yield old user function
0:	mov	sigtable-2(bx),cx	/ and save new one in table
1:	ret

sigsave:
	push	_fpac_+6	/ save pseudo-regs
	push	_fpac_+4
	push	_fpac_+2
	push	_fpac_
	push	ax		/ save regs
	push	cx
	push	dx
	push	bx
	push	bp

	mov	bp,sp
	mov	bx,2*9(bp)	/ get signo (pushed by system)
	push	bx		/ pass to user function
	add	bx,bx
	icall	sigtable-2(bx)	/ (*sigtable[signo-1])(signo);
	pop	bx

	mov	bx,2*10(bp)	/ swap return address
	mov	cx,2*11(bp)	/ and flags
	mov	2*11(bp),bx	/ to facilitate return
	mov	2*10(bp),cx

	pop	bp		/ restore regs
	pop	bx
	pop	dx
	pop	cx
	pop	ax
	pop	_fpac_		/ restore pseudo-regs
	pop	_fpac_+2
	pop	_fpac_+4
	pop	_fpac_+6
	add	sp,$2		/ discard signo
	popf			/ get flags
	ret

	.prvd
sigtable:
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.word	-1
