	.globl	sout_
	.globl	scrbase_

/ sout(p, c)
/ char *p;
/
sout_:
	cli
	mov	bx,sp
	mov	ax,4(bx)
	mov	bx,2(bx)
	mov	cx,scrbase_	/screen address
	mov	es,cx
	movb	es:(bx),al
	mov	ax,ds
	mov	es,ax
	sti
	ret
