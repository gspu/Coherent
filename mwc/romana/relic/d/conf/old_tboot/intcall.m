//////////
/ From MSDOS MWC86 system call interface.
/ Interrupt simulation.
/ Modified for use with tertiary boot code for COHERENT.
/ 
/ Modified September 1991 by La Monte H. Yarroll <piggy@mwc.com>
/
/ DANGER!! THIS IS SELF MODIFYING CODE.  IT WILL ONLY WORK IN A STRICTLY
/ SINGLE THREADED ENVIRONMENT.
//////////

#include "larges.h"

//////////
/ void
/ intcall(src, dest, intnum)
/ struct reg *src;		/* All regs are loaded except flags */
/ struct reg *dest;		/* All regs are stored here */
/ int intnum;			/* Int number */
//////////

src	=	6+RASIZE
dest	=	src+PTRSIZE
intnum	=	dest+PTRSIZE

	.globl	intcall_

intcall_:
	push	si			/ Save register variables.
	push	di
	push	bp
	mov	bp, sp

	movb	al, intnum(bp)		/ Get intnum in AL
	movb	cs:myint+1, al		/ Modify the code

	Lds	si, src(bp)		/ Load DS:SI with src pointer.
	mov	ax, (si)		/ Set AX
	mov	bx, 2(si)		/ and BX
	mov	cx, 4(si)		/ and CX
	mov	dx, 6(si)		/ and DX
	mov	di, 10(si)		/ and DI,
	push	12(si)			/ save src DS,
	mov	es, 14(si)		/ set ES
	mov	si, 8(si)		/ and SI
	pop	ds			/ and DS.
					/ Actually do the interrupt.
myint:	int	0			/ This instruction gets modified.

/ The interrupt handler preserves SS:SP.
/ The iret to the handler pops the handler address and flags pushed above,
/ the iret from the handler pops the retint address and flags.
retint:
	mov	bp, sp			/ Restore BP.
	pushf				/ Save result flags
	push	es			/ and ES
	push	ds			/ and DS
	push	si			/ and SI.

#if	!LARGEDATA
	mov	si, ss
	mov	ds, si			/ Restore DS
	mov	es, si			/ and ES.
#endif

	Lds	si, dest(bp)		/ Get dest in DS:SI.
	mov	(si), ax		/ Load dest with returned AX
	mov	2(si), bx		/ and BX
	mov	4(si), cx		/ and CX
	mov	6(si), dx		/ and DX
	pop	8(si)			/ and SI
	mov	10(si), di		/ and DI
	pop	12(si)			/ and DS
	pop	14(si)			/ and ES
	pop	16(si)			/ and flags.

	pop	bp
	pop	di
	pop	si
	Gret

/ end of intcall.m
