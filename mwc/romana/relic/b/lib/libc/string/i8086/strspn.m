//////////
/ i8086 C string library.
/ strspn()
/ ANSI 4.11.5.6.
//////////

//////////
/ size_t
/ strspn(String1, String2)
/ char *String1, *String2;
/
/ Return the length of the initial segment of String1 which consists of
/ characters in String2
//////////

#include <larges.h>

String1	=	LEFTARG
String2	=	String1+DPL

	Enter(strspn_)
	Les	di, String1(bp)	/ String1 address String1 ES:DI
	mov	dx, di		/ Save for length calc
	cld
1:	movb	ah, Pes (di)	/ Get char from String 1
	orb	ah, ah
	je	3f		/ If equal done

	Lds	si, String2(bp)	/ address of String2 DS:SI
2:	lodsb			/ From character String1 AL
	orb	al, al
	je	3f		/ End of scan no match means done
	cmpb	ah, al
	jne	2b		/ Try next in String2
	inc	di		/ Try next in String1
	jmp	1b
3:	mov	ax, di
	sub	ax, dx
	Leave
