//////////
/ libc/string/i386/strchr.s
/ i386 C string library.
/ ANSI 4.11.5.2.
//////////

//////////
/ char *
/ strchr(char *String, int Char)
/
/ Find Char in String, return pointer or NULL.
/ Previously known as index();
/ index() entry point retained for backward compatability.
//////////

String	.equ	8
Char	.equ	String+4

	.globl	strchr
	.globl	index

strchr:
index:
	push	%esi

	movl	%esi, String(%esp)	/ String address to ESI
	movb	%ah, Char(%esp)		/ Char to AH
	cld

?loop:
	lodsb				/ Fetch String character to AL
	cmpb	%al, %ah		/ and compare to Char
	je	?matched
	orb	%al, %al		/ Test for NUL
	jne	?loop			/ Not done
	subl	%eax, %eax		/ End of String, return NULL
	jmp	?done

?matched:
	decl	%esi			/ Back up to match
	movl	%eax, %esi		/ Return pointer in EAX

?done:
	pop	%esi
	ret

/ end of libc/string/i386/strchr.s
