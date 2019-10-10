//////////
/ i8086 C string library.
/ memcpy()
/ ANSI 4.11.2.1.
//////////

//////////
/ char *
/ memcpy(To, From, Count)
/ char *To, *From;
/ int Count;
/
/ Copy Count bytes from From to To.
//////////

#include <larges.h>

To	=	LEFTARG
From	=	To+DPL
Count	=	From+DPL

	Enter(memcpy_)
	mov	cx, Count(bp)	/ Count to CX
	Lds	si, From(bp)	/ From address to DS:SI
	Les	di, To(bp)	/ To address to ES:DI
	mov	ax, di		/ Result to AX
#if	LARGEDATA
	mov	dx, es		/ or DX:AX
#endif
	cld
	rep
	movsb
	Leave
