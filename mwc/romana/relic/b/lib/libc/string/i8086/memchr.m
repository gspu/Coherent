//////////
/ i8086 C string library.
/ memchr()
/ ANSI 4.11.5.1.
//////////

//////////
/ char *
/ memchr(String, Char, Count)
/ char *String;
/ int Char, Count;
/
/ Find Char in String, return pointer or NULL.
//////////

#include <larges.h>

String	=	LEFTARG
Char	=	String+DPL
Count	=	Char+2

	Enter(memchr_)
	mov	cx, Count(bp)	/ Count to CX
	or	cx, cx
	je	1f		/ No match possible in first 0 characters
	Les	di, String(bp)	/ String address to ES:SI
	movb	al, Char(bp)	/ Char to AL
	cld
	repne
	scasb			/ Look for character
	je	2f		/ Matched

1:	sub	ax, ax		/ No match, return NULL
#if	LARGEDATA
	mov	dx, ax
#endif
	jmp	3f

2:	dec	di		/ Back up to match
	mov	ax, di		/ AX = offset
#if	LARGEDATA
	mov	dx, es		/ DX = segment
#endif

3:	Leave
