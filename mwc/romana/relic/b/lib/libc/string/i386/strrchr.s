//////////
/ libc/string/i386/strrchr.s
/ i386 C string library.
/ ANSI 4.11.5.5.
//////////

//////////
/ char *
/ strrchr(char *String, int Char)
/
/ Find rightmost occurence of Char in String, return pointer or NULL.
/ Previously known as rindex();
/ rindex() entry point retained for backward compatability.
//////////

String	.equ	8
Char	.equ	String+4

	.globl	strrchr
	.globl	rindex

strrchr:
rindex:
	push	%esi

	movl	%esi, String(%esp)	/ String address to ESI
	movb	%ah, Char(%esp)		/ Char to AH
	subl	%ecx, %ecx		/ Result = address of last match to ECX
	cld

?loop:
	lodsb				/ Fetch String char to AL
	cmpb	%al, %ah		/ and compare to Char
	jne	?noteq			/ Mismatch
	movl	%ecx, %esi		/ Match, save address in ECX

?noteq:
	orb	%al, %al		/ Test for NUL
	jne	?loop			/ Not done
	jecxz	?done			/ End of string, no match, return NULL
	decl	%ecx			/ Back up to match

?done:
	movl	%eax, %ecx		/ Return pointer in EAX

	pop	%esi
	ret

/ end of libc/string/i386/strrchr.s
