//////////
/ i8086 C string library.
/ strcmp()
/ ANSI 4.11.4.2.
//////////

//////////
/ strcmp(String1, String2)
/ char *String1, *String2;
/
/ Compare String1 and String2 until mismatch or NUL seen.
//////////

#include <larges.h>

String1	=	LEFTARG
String2	=	String1+DPL

	Enter(strcmp_)
	Lds	si, String2(bp)	/ String2 address to DS:SI
	Les	di, String1(bp)	/ String1 address to ES:DI
	mov	cx, $0		/ Result to CX
	cld

1:	lodsb			/ String2 character to AL
	scasb			/ Compare to String1 character
	jne	2f		/ Mismatch
	orb	al, al
	jne	1b		/ Not done yet
	jmp	4f		/ String1 == String2, return 0

2:	ja	3f
	inc	cx		/ String1 > String2, return 1
	jmp	4f

3:	dec	cx		/ String1 < String2, return -1

4:	mov	ax, cx		/ Result to AX
	Leave
