//////////
/ i8086 C string library.
/ strrchr()
/ ANSI 4.11.5.5.
/ Previously known as rindex().
/ Entry point for rindex() retained for backward compatability.
//////////

//////////
/ char *
/ strrchr(String, Char)
/ char *String;
/ int Char;
/
/ Find rightmost occurence of Char in String,
/ return pointer or NULL.
//////////

#include <larges.h>

String	=	LEFTARG
Char	=	String+DPL

	.globl	strrchr_
strrchr_:

	Enter(rindex_)
	Lds	si, String(bp)	/ String address to DS:SI
	movb	ah, Char(bp)	/ Char to AH
	sub	bx, bx		/ Address of last match to BX
	cld

1:	lodsb			/ Fetch String char to AL
	cmpb	al, ah		/ and compare to Char
	jne	2f		/ Mismatch
	mov	bx, si		/ Match, save address

2:	orb	al, al		/ Test for NUL
	jne	1b		/ Not done
	or	bx, bx		/ End of string, test if match found
	jne	3f		/ Match found
	mov	ax, bx		/ No match, return NULL
#if	LARGEDATA
	mov	dx, ax
#endif
	jmp	4f		/ Go home

3:	dec	bx		/ Back up to match
	mov	ax, bx		/ AX = offset
#if	LARGEDATA
	mov	dx, ds		/ DX = segment
#endif

4:	Leave
