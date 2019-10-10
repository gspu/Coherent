//////////
/ MSDOS MWC86 runtime.
/ Copy memory.
/ ANSI 4.11.2.2.
//////////

////////
/ Copy a block of memory from one
/ place in the physical address space to
/ another place in the physical address space.
/ Copy backwards if the source precedes the destination in memory.
/
/ char *
/ memmove(dst, src, nbytes)
/ char *dst, *src;
/ int nbytes;
////////

#include <larges.h>

dst	=	LEFTARG			/ destination
src	=	dst+DPL			/ source
nbytes	=	src+DPL			/ byte count

c16:	.word	16			/ Constant for multiply

	Enter(memmove_)
	Les	di, dst(bp)		/ Load ES:DI with destination.
	mov	cx, nbytes(bp)		/ Pick up byte count.
	jcxz	4f			/ No work, return dst.

	cld				/ Set forward copy bit.
	Lds	si, src(bp)		/ Load DS:SI with source.

#if	LARGEDATA
/ LARGE model requires some computation to deduce whether
/ the source precedes the destination.
	mov	ax, ds
	mov	bx, es
	sub	ax, bx			/ src segment - dst segment
	jne	0f			/ Segments match,
	cmp	si, di			/ just compare offsets.
	jmp	1f
0:	imul	cs:c16			/ Convert paragraph difference to bytes.
	add	ax, si			/ Add in source offset
	adc	dx, $0
	sub	ax, di			/ and subtract destination offset.
	sbb	dx, $0
	jne	2f			/ High word of difference is nonzero.
	cmp	ax, $0
#else
/ SMALL model can just compare offsets.
	cmp	si, di			/ Compare offsets.
#endif
1:	jz	4f			/ src == dest, return dst.
2:	jae	3f			/ src > dest, copy forward.
	add	si, cx			/ src < dest,
	add	di, cx			/ copy backward.
	dec	si			/ Find end of src
	dec	di			/ and end of dest
	std				/ and set flag for backward copy.
3:
	rep				/ Copy the
	movsb				/ block of bytes.
4:
	cld				/ Clear direction flag for DOS.
	mov	ax, dst(bp)		/ return the destination
#if LARGEDATA
	mov	dx, es
#endif
	Leave
