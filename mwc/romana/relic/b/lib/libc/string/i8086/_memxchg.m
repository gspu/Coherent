//////////
/ i8086 C string library internals.
/ _memxchg()
/ Exchange records of Count bytes pointed to by S1 and S2.
//////////

//////////
/ void _memxchg(char *S1, char *S2, size_t Count)
//////////

#include <larges.h>

S1	=	LEFTARG
S2	=	S1+DPL
Count	=	S2+DPL

	Enter(_memxchg_)
	mov	cx, Count(bp)	/ Count to CX
	jcxz	2f		/ Done if Count is 0

	Lds	si, S2(bp)	/ S2 address to DS:SI
	Les	di, S1(bp)	/ S1 address to ES:DI
	cld
	shr	cx, $1		/ Convert byte count to word count
	jnc	1f		/ Count even, skip byte exchange

	movb	al, Pes (di)	/ S1 byte to AL
	movsb			/ S2 byte to S1, bump DI and SI
	movb	-1(si), al	/ S1 byte to old S2
	jz	2f		/ Done if Count is 0

1:
	mov	ax, Pes (di)	/ S1 word to AX
	movs			/ S2 word to S1, bump DI and SI
	mov	-2(si), ax	/ S1 word to old S2
	loop	1b		/ Decrement byte count and repeat

2:
	Leave
