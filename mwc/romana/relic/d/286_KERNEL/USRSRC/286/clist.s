/ $Header: /usr/src/sys/i8086/src/RCS/clist.s,v 1.1 88/03/24 17:39:16 src Exp $

/ (lgl-
/	The information contained herein is a trade secret of Mark Williams
/	Company, and  is confidential information.  It is provided  under a
/	license agreement,  and may be  copied or disclosed  only under the
/	terms of  that agreement.  Any  reproduction or disclosure  of this
/	material without the express written authorization of Mark Williams
/	Company or persuant to the license agreement is unlawful.
/
/	COHERENT Version 2.3.37
/	Copyright (c) 1982, 1983, 1984.
/	An unpublished work by Mark Williams Company, Chicago.
/	All rights reserved.
/ -lgl)
////////
/
/ i8086 coherent clist hack.
/ cltinit, getq and putq have been tuned.
/ the remaining functions are as produced by cc -S coh/clist.c
/ with NCPCL substituted
/
/ $Log:	/usr/src/sys/i8086/src/RCS/clist.s,v $
/ Revision 1.1	88/03/24  17:39:16	src
/ Initial revision
/ 

#include <sys/const.h>

	.shri
L10001:	.word	NCPCL+2
	.globl	cltinit_
cltinit_:
	push	si
	push	di
	push	bp
	mov	bp, sp
	sub	sp, $0x0C

	pushf			/ s =
	cli			/ sphi()
	sub	di, di
	mov	ax, NCLIST_
	imul	cs:L10001
	mov	-0x0C(bp), ax
	add	ax, clistp_
	mov	-0x04(bp), ax

L3:	sub	-0x04(bp), $NCPCL+2
	mov	ax, -0x04(bp)
	cmp	ax, clistp_
	jb	L2

L10002:	mov	si, -0x04(bp)
	mov	(si), di
	mov	di, si
	jmp	L3

L2:	mov	cltfree_, di
	call	spl_
	add	sp, $0x02

	mov	sp, bp
	pop	bp
	pop	di
	pop	si
	ret

	.globl	getq_

getq_:
	mov	dx, si
	push	bp
	mov	bp, sp

	mov	bp, 4(bp)	/ bp = cqp
	sub	ax, ax		/ ax = 0
	cmp	(bp), ax	/ if (cqp->cq_cc == 0)
	jne	0f
	dec	ax		/ return (-1)
	jmp	2f

0:	pushf			/ s =
	cli			/ sphi()
	mov	si, 6(bp)	/ si = op = cqp->cq_op
	mov	bx, 8(bp)	/ bx = ox = cqp->cq_ox
	movb	al, 2(bx,si)	/ ax = op->cl_ch[ox]
	dec	(bp)		/ if (--cqp->cq_cc == 0)
	je	0f
	inc	bx		/ ++ox
	cmp	bx, $NCPCL	/ if (ox == NCPL)
	jne	1f

0:	sub	bx, bx		/ ox = 0;
	mov	cx, (si)	/ cx = np = op->cl_fp
	mov	6(bp), cx	/ cqp->cq_op = np
	cmp	cx, bx		/ if (np == 0)
	jne	0f
	mov	2(bp), bx	/ cqp->cq_ip = 0
	mov	4(bp), bx	/ cqp->cq_ix = 0

0:	mov	cx, cltfree_	/ cx = tmp = cltfree
	mov	(si), cx	/ op->cl_fp = tmp
	mov	cltfree_, si	/ cltfree = op
	cmp	cltwant_, bx	/ if (cltwant != 0)
	je	1f
	mov	cltwant_, bx	/ cltwant = 0
	mov	cx, $cltwant_	/ wakeup(&cltwant)
	push	bx	/ save
	push	dx	/ save
	push	ax	/ save
	push	cx
	call	wakeup_
	pop	cx	/ clear stack
	pop	ax	/ restore
	pop	dx	/ restore
	pop	bx	/ restore

1:	mov	8(bp), bx	/ cqp->cq_ox = ox
	mov	cx, ax
	call	spl_		/ spl(s)
	add	sp, $2
	mov	ax, cx

2:	pop	bp
	mov	si, dx
	ret

	.globl	putq_

putq_:
	mov	dx, si
	push	bp
	mov	bp, sp

	mov	cx, 6(bp)	/ cx = c
	mov	bp, 4(bp)	/ bp = cqp
	sub	ax, ax		/ ax = 0
	pushf			/ s =
	cli			/ sphi()
	mov	si, 2(bp)	/ si = ip = cqp->cq_ip
	mov	bx, 4(bp)	/ bx = ix = cqp->cq_ix
	cmp	bx, ax		/ if (ix == 0)
	jne	2f
	mov	si, cltfree_	/ ip = cltfree
	cmp	si, ax		/ if (ip == 0)
	jne	0f
	call	spl_		/ spl(s)
	add	sp, $2
	mov	ax, $-1		/ return (-1)
	jmp	3f

0:	mov	bx, (si)	/ tmp = ip->cl_fp
	mov	cltfree_, bx	/ cltfree = tmp
	mov	(si), ax	/ ip->cl_fp = 0
	mov	bx, 2(bp)	/ np = cqp->cq_ip
	cmp	bx, ax		/ if (np == 0)
	jne	0f
	mov	6(bp), si	/ cqp->cq_op = ip
	jmp	1f

0:	mov	(bx), si	/ np->cl_fp = ip

1:	mov	2(bp), si	/ cqp->cq_ip = ip
	mov	bx, ax		/ bx = ix = cqp->cq_ix = 0

2:	movb	2(bx,si), cl	/ ip->cl_ch[ix] = c
	inc	bx		/ ix++
	cmp	bx, $NCPCL	/ if (ix == NCPCL)
	jne	0f
	mov	bx, ax		/ ix = 0

0:	mov	4(bp), bx	/ cqp->cq_ix = ix
	inc	(bp)		/ cqp->cq_cc++
	call	spl_		/ spl(s)
	add	sp, $2
	mov	ax, cx		/ return (c)

3:	pop	bp
	mov	si, dx
	ret

	.globl	clrq_

clrq_:
	push	si
	push	di
	push	bp
	mov	bp, sp

	mov	si, 0x08(bp)
	call	sphi_
	mov	di, ax

L18:	push	si
	call	getq_
	add	sp, $0x02
	or	ax, ax
	jge	L18
	push	di
	call	spl_
	add	sp, $0x02

	pop	bp
	pop	di
	pop	si
	ret

	.globl	waitq_

waitq_:
	push	si
	push	di
	push	bp
	mov	bp, sp

L21:	cmp	cltfree_, $0x00
	jne	L19
	mov	cltwant_, $0x01
	sub	ax, ax
	push	ax
	push	ax
	mov	ax, $0x0100
	push	ax
	mov	ax, $cltwant_
	push	ax
	call	sleep_
	add	sp, $0x08
	jmp	L21

L19:	pop	bp
	pop	di
	pop	si
	ret
