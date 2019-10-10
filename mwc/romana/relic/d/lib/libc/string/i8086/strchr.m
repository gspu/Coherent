//////////
/ i8086 C string library.
/ strchr()
/ ANSI 4.11.5.2.
/ Previously known as index().
/ Entry point for index() retained for backward compatability.
//////////

//////////
/ char *
/ strchr(String, Char)
/ char *String;
/ int Char;
/
/ Find Char in String, return pointer or NULL.
//////////

#include <larges.h>

String	=	LEFTARG
Char	=	String+DPL

	.globl	strchr_
strchr_:

	Enter(index_)
	Lds	si, String(bp)	/ String address to DS:SI
	movb	ah, Char(bp)	/ Char to AH
	cld

1:	lodsb			/ String character to AL
	cmpb	al, ah
	je	2f		/ Got it, return address
	orb	al, al		/ Test for NUL
	jne	1b		/ Not done
	movb	ah, al		/ End of String, return NULL
#if	LARGEDATA
	mov	dx, ax
#endif
	jmp	3f

2:	dec	si		/ Back up to match
	mov	ax, si		/ AX = offset
#if	LARGEDATA
	mov	dx, ds		/ DX = segment
#endif

3:	Leave
