
/ i8086 coherent clist hack.

///////

/LXXX:	.long	NCPCL+4

/cltinit:
/	pushf			/ s = sphi()
/	cli
/	mov	NCLIST,%eax
/	imull	LXXX,%eax
/	addl	clistp,%eax
/	sub	%ecx,%ecx
/	jmp	loc32

/loc33:	mov	%ecx,(%eax)
/	mov	%eax,%ecx
/loc32:	sub	$NCPCL+4, %eax
/	cmp	clistp,%eax
/	jnb	loc33

/	mov	%ecx,cltfree
/	call	spl
/	add	$0x04,%esp
/	ret

/getq:
/	push	%esi

/	mov	8(%esp),%edx
/	sub	%ecx,%ecx
/	cmp	%ecx,(%edx)	/ if (cqp->cq_cc==0)
/	je	loc21

/	pushf			/ s = sphi()
/	cli
/	mov	12(%edx),%esi	/ op = cqp->cq_op	[%esi]
/	mov	16(%edx),%eax	/ ox = cqp->cq_ox	[%eax]
/	movb	4(%eax,%esi),%cl
/	push	%ecx		/ save = op->cl_ch[ox]
/	decl	(%edx)		/ if (--cqp->cq_cc == 0)
/	je	loc23
/	inc	%eax		/ ++ox
/	mov	%eax,16(%edx)	/ cqp->cq_ox = ox
/	cmp	$NCPCL,%eax	/ if (ox == NCPL)
/	jne	loc24

/loc23:	sub	%eax,%eax
/	mov	%eax,16(%edx)	/ cqp->cq_ox = 0
/	mov	(%esi),%ecx	/ np = op->cl_fp
/	mov	%ecx,12(%edx)	/ cqp->cq_op = np
/	cmp	%eax,%ecx	/ if (np==0)
/	jne	loc25
/	mov	%eax,4(%edx)		/ cqp->cq_ip = 0
/	mov	%eax,8(%edx)		/ cqp->cq_ix = 0

/loc25:	mov	cltfree,%ecx
/	mov	%ecx,(%esi)	/ op->cl_fp = cltfree
/	mov	%esi,cltfree	/ cltfree = op
/	cmp	%eax,cltwant	/ if (cltwant)
/	je	loc24
/	mov	%eax,cltwant		/ cltwant = 0
/	push	$cltwant
/	call	wakeup			/ wakeup(&cltwant)
/	pop	%eax

/loc24:	pop	%esi
/	call	spl		/ spl(s)
/	pop	%eax
/	mov	%esi,%eax	/ return save
/	pop	%esi
/	ret
/loc21:
/	mov	$-1,%eax			/ return -1
/	pop	%esi
/	ret

/putq:
/	push	%esi
/	sub	%eax,%eax
/	pushf			/ s = sphi();
/	cli
/	mov	12(%esp),%edx	/ ebp = cqp
/	mov	4(%edx),%esi	/ ip = cqp->cq_ip	[%esi]
/	mov	8(%edx),%ecx	/ ix = cqp->cq_ix	[%ecx]
/	cmp	%eax,%ecx	/ if (ix==0) {
/	jne	loc26
/	mov	cltfree,%esi		/ ip = cltfree
/	cmp	%eax,%esi		/ if (ip==0)
/	je	loc27			/ goto bad;
/	mov	(%esi),%ecx
/	mov	%ecx,cltfree		/ cltfree = cltfree->cl_fp
/	mov	%eax,(%esi)		/ ip->cl_fp = 0
/	mov	4(%edx),%ecx		/ np = cqp->cq_ip
/	cmp	%eax,%ecx		/ if (np==0)
/	jne	loc29
/	mov	%esi,12(%edx)			/ cqp->cq_op = ip
/	jmp	loc30
/					/ else
/loc29:	mov	%esi,(%ecx)			/ np->cl_fp = ip

/loc30:	mov	%esi,4(%edx)		/ cqp->cq_ip = ip
/	mov	%eax,%ecx		/ ix = 0
/				/ }

/loc26:
/	movb	16(%esp),%al	/ ip->cl_ch[ix] = c
/	movb	%al, 4(%esi,%ecx) 
/	inc	%ecx		/ ix++
/	cmp	$NCPCL,%ecx	/ if (ix==NCPCL)
/	jne	loc31
/	sub	%ecx,%ecx		/ ix = 0

/loc31:	mov	%ecx,8(%edx)	/ cqp->cq_ix = ix
/	incl	(%edx)		/ cqp->cq_cc++
/	call	spl		/ spl(s)
/	add	$4,%esp
/	mov	12(%esp),%eax	/ return (c)
/loc28:
/	pop	%esi
/	ret
/loc27:
/	call	spl		/ 		spl(s)
/	add	$4,%esp
/	mov	$-1,%eax	/ 		return -1
/	jmp	loc28

/clrq:
/	mov	4(%esp),%edx
/	pushf
/	cli

/loc34:	push	%edx
/	call	getq
/	pop	%edx
/	or	%eax,%eax
/	jge	loc34

/	call	spl
/	pop	%eax
/	ret

/loc35:	movl	$0x01,cltwant
/	push	%eax
/	push	%eax
/	push	$0x0100
/	push	$cltwant
/	call	sleep
/	add	$16,%esp
/waitq:
/	sub	%eax,%eax
/	cmp	%eax,cltfree
/	jne	loc35
/	ret

///////

/ atbsyw()	-- wait for AT disk controller to become not busy

/	Return:	0 = timeout
/		* = not busy

///////
/atbsyw:
/	mov	$0x3FFFF, %ecx 
/	mov	ATSREG, %edx
/loc16:	inb	(%dx)
/	testb	$BSY_ST, %al
/	loopne	loc16
/	mov	%ecx, %eax 
/	ret

///////

/ AT Hard Disk Assembler Support
/ atbsyw()	     - wait while controller is busy
/ atdrqw()	     - wait for controller to request data transfer

///////
///////

/ atdrqw()	-- wait for AT disk controller to initiate data request

/	Return:	0 = timeout
/		* = data requested

///////

/atdrqw:
/	mov	$0x3FFFF, %ecx
/	mov	ATSREG, %edx 
/loc17:	inb	(%dx)
/	testb	$DRQ_ST, %al 
/	loope	loc17
/	mov	%ecx, %eax 
/	ret


/ Read master PIC state
/ return 00:xx:yy:zz 4-byte int value
/	xx: interrupt mask
/	yy: isr
/	zz: irr

/	.globl	mchirp
/FOO	.macro	ch
/	push	ch
/	call	mchirp
/	add	$4,%esp
/	.endm

/	.globl	rd_m_pic
/rd_m_pic:
/	pushfl
/	cli
/	sub	%eax,%eax
/	inb	$PICM		/ interrupt mask to %eax:16..23
/	shl	$8,%eax
/	movb	$0x0B,%al	/ OCW3 - read isr
/	outb	$PIC
/	IODELAY
/	inb	$PIC		/ in-service register to %eax:8..15
/	shl	$8,%eax
/	movb	$0x0A,%al	/ OCW3 - read irr
/	outb	$PIC
/	IODELAY
/	inb	$PIC		/ irpt request register to %eax:0..7
/	popfl
/	ret

/ Read slave PIC state
/ return 00:xx:yy:zz 4-byte int value
/	xx: interrupt mask
/	yy: isr
/	zz: irr

/	.globl	rd_s_pic
/rd_s_pic:
/	pushfl
/	cli
/	sub	%eax,%eax
/	inb	$SPICM		/ interrupt mask to %eax:16..23
/	shl	$8,%eax
/	movb	$0x0B,%al	/ OCW3 - read isr
/	outb	$SPIC
/	IODELAY
/	inb	$SPIC		/ in-service register to %eax:8..15
/	shl	$8,%eax
/	movb	$0x0A,%al	/ OCW3 - read irr
/	outb	$SPIC
/	IODELAY
/	inb	$SPIC		/ irpt request register to %eax:0..7
/	popfl
/	ret

