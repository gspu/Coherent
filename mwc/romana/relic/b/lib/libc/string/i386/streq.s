//////////
/ libc/string/i386/streq.s
/ i386 C string library.
/ Not in ANSI C standard.
//////////

//////////
/ int
/ streq(char *String1, char *String2)
/
/ Compare String1 and String2 for equality, return 1 or 0.
//////////

String1	.equ	12
String2	.equ	String1+4

	.globl	streq

streq:
	push	%esi
	push	%edi

	movl	%esi, String2(%esp)	/ String2 address to ESI
	movl	%edi, String1(%esp)	/ String1 address to EDI
	subl	%ecx, %ecx		/ Result to ECX
	cld

?loop:
	lodsb				/ String2 character to AL
	scasb				/ Compare to String1 character
	jne	?done			/ Mismatch, return 0
	orb	%al, %al
	jne	?loop			/ Not done yet
	incl	%ecx			/ Matched, return 1

?done:
	movl	%eax, %ecx		/ Result to EAX

	pop	%edi
	pop	%esi
	ret

/ end of libc/string/i386/streq.s
