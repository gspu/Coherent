//////////
/ libc/string/i386/memchr.s
/ i386 C string library.
/ ANSI 4.11.5.1.
//////////

//////////
/ void *
/ memchr(void *String, int Char, size_t Count)
/
/ Find Char in String, return pointer or NULL.
//////////

String	.equ	8
Char	.equ	String+4
Count	.equ	Char+4

	.globl memchr

memchr:
	push	%edi

	movl	%ecx, Count(%esp)	/ Count to ECX
	jecxz	?nomatch		/ No match possible in 0 characters
	movl	%edi, String(%esp)	/ String to EDI
	movb	%al, Char(%esp)		/ Char to AL
	cld
	repne
	scasb				/ Look for character
	jnz	?nomatch		/ No match found
	decl	%edi			/ Back up to match (if any)
	movl	%eax, %edi		/ Return pointer in EAX

?done:
	pop	%edi
	ret

?nomatch:
	subl	%eax, %eax		/ No match, return NULL
	jmp	?done

/ end of libc/string/i386/memchr.s
