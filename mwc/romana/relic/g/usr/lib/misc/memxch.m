/*
 * void * memxch(void *buf1, void *buf2, size_t size)
 * exchanges buf1 & buf2 for size.
 */
#include <larges.h>
buf1	=	LEFTARG
buf2	=	buf1+DPL
size	=	buf2+DPL
	Enter(memxch_)
	mov	cx, size(bp)
	Les	si, buf1(bp)
	Lds	di, buf2(bp)
	shr	cx, $1
	jnc	1f
	movb	al, Pes (di)
	movsb
	movb	-1(si), al
1:
	jcxz	3f
2:
	mov	ax, Pes (di)
	movs
	mov	-2(si), ax
	loop	2b
3:
	Leave
