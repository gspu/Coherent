////////
/
/ void ffcopy(from_fp, to_fp, count)
/ faddr_t from_fp, to_fp;
/ int count;
/
/ Copy count bytes from from_fp to to_fp.
/
/ Here is the stack after initial "push bp":
/
/	12(bp)	count
/	10(bp)	FP_SEL(to_fp)
/	8(bp)	FP_OFF(to_fp)
/	6(bp)	FP_SEL(from_fp)
/	4(bp)	FP_OFF(from_fp)
/	2(bp)	return IP
/	0(bp)	old bp
/
////////

ffcopy_:
	push	bp
	mov	bp, sp
	push	es
	push	di
	push	ds
	push	si

	lds	si, 4(bp)	/ from_fp  to DS:SI
	les	di, 8(bp)	/ to_fp to ES:DI
	mov	cx, 12(bp)	/ rep count to CX
	rep
	movsb

	pop	si
	pop	ds
	pop	di
	pop	es
	pop	bp
	ret
