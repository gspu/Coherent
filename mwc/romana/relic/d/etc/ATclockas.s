/
/	The  information  contained herein  is a trade secret  of INETCO
/	Systems, and is confidential information.   It is provided under
/	a license agreement,  and may be copied or disclosed  only under
/	the terms of that agreement.   Any reproduction or disclosure of
/	this  material  without  the express  written  authorization  of
/	INETCO Systems or persuant to the license agreement is unlawful.
/
/	Copyright (c) 1985
/	An unpublished work by INETCO Systems, Ltd.
/	All rights reserved.
/

/	Used for setting and reading the AT clock (circa Sept 1985)
/
/					Krish Nair (Sept 1985)

.globl zput_
.globl zget_
.globl sphi_
.globl splo_

//////////////////////////////
/ zput will write the 1st argument to 0x70 and then write the 
/ 2nd argument to 0x71
/////////////////////

zput_:	mov 	bx, sp
	mov	ax, 2(bx)
	mov 	dx, $0x70
	cli
	outb	dx, al
	inc	dx
	mov	ax, 4(bx)
	outb 	dx, al
	sti
	ret

//////////////////////////
/ zput get will write the argument to 0x70 and then read 0x71
/////////////////

zget_:	mov	bx, sp
	mov	ax, 2(bx)
	mov	dx, $0x70
	cli
	outb	dx, al
	jmp	0f
     0: inc 	dx
	inb 	al, dx
	sti
	ret

////////////////////
/ sphi will disable interrupts
//////////

sphi_:	cli
	ret

///////////////////
/ splo will enable interrupts
///////////////

splo_:	sti
	ret
