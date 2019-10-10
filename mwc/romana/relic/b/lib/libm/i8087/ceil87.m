//////////
/ libm 8087
/ ceil(d), floor(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	ceil_
	.globl	floor_
	.globl	floor

//////////
/ double
/ ceil(d)
/ double d;
//////////

cwup	=	0xBFF		/ round up 8087 control word
d	=	2+RASIZE		/ arg offset

ceil_:
	ifno8087(_ceil_)
	push	bp
	mov	bp, sp

	fdld	d(bp)		/ Load argument d.
	mov	ax, $cwup	/ Round up control word to AX.
	jmp	1f

//////////
/ double
/ floor(d)
/ double d;
/
/ floor() rounds 8087 stacktop down to the int below.
/ It saves and restores the current 8087 control word.
//////////

cwdown	=	0x7FF		/ round down 8087 control word
d	=	2+RASIZE	/ arg offset
autos	=	4		/ auto space needed
oldcw	=	-2		/ saved 8087 control word
newcw	=	-4		/ new 8087 control word

floor_:
	ifno8087(_floor_)
	push	bp
	mov	bp, sp

	fdld	d(bp)		/ Load argument d.
	jmp	0f

floor:				/ d
	push	bp
	mov	bp, sp

0:	mov	ax, $cwdown	/ Round down control word to AX.

1:	sub	sp, $autos	/ Claim two words of auto space.

	mov	newcw(bp), ax	/ Store new control word.
	fstcw	oldcw(bp)	/ Save old control word.
	fldcw	newcw(bp)	/ Load new control word.
	frndint			/ int(d)
	fldcw	oldcw(bp)	/ Restore saved control word.

	mov	sp, bp
	pop	bp
	Gret
