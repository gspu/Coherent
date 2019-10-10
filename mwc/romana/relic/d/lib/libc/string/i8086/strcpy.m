//////////
/ i8086 C string library.
/ strcpy()
/ ANSI 4.11.2.3.
//////////

//////////
/ char *
/ strcpy(To, From)
/ char *To, *From;
/
/ Copy From to To until NUL.
//////////

#include <larges.h>

To	=	LEFTARG
From	=	To+DPL

	Enter(strcpy_)
	Lds	si, From(bp)	/ From address to DS:SI
	Les	di, To(bp)	/ To address to ES:DI
	cld

1:	lodsb			/ From character to AL
	stosb			/ and stored to To
	orb	al, al
	jne	1b		/ Not done
	mov	ax, To(bp)	/ Return the destination
#if	LARGEDATA
	mov	dx, es
#endif
	Leave
