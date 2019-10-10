//////////
/ i8086 C string library.
/ strncpy()
/ ANSI 4.11.2.4.
//////////

//////////
/ char *
/ strncpy(To, From, Count)
/ char *To, *From;
/ int Count;
/
/ Copy up to Count bytes from From to To,
/ then NUL-pad if Count not reached.
//////////

#include <larges.h>

To	=	LEFTARG
From	=	To+DPL
Count	=	From+DPL

	Enter(strncpy_)
	Les	di, To(bp)	/ To address to ES:DI
	mov	cx, Count(bp)	/ Count to CX
	or	cx, cx
	je	2f		/ Copy nothing
	Lds	si, From(bp)	/ From address to DS:SI
	cld

1:	lodsb			/ Fetch From character
	stosb			/ and store through To
	orb	al, al
	loopne	1b		/ Continue if AL!=0 && CX!=0
	rep			/ Finish clearing line
	stosb			/ (does nothing if CX==0)

2:	mov	ax, To(bp)	/ Return the destination
#if	LARGEDATA
	mov	dx, es
#endif
	Leave
