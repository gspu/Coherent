//////////
/ libc/string/i386/strcat.s
/ i386 C string library.
/ ANSI 4.11.3.1., 4.11.3.2.
//////////

//////////
/ char *
/ strcat(char *To, char *From)
/ Append From to To.
/
/ char *
/ strncat(char *To, char *From, size_t Count)
/ Append not more than Count characters (NOT including the NUL) from From to To.
/ Always append a NUL.
//////////

To	.equ	12
From	.equ	To+4
Count	.equ	From+4

	.globl	strcat
	.globl	strncat

strncat:
	movl	%edx, Count-8(%esp)	/ Limit of move to EDX
	orl	%edx, %edx
	jnz	strcat0			/ Fall in to common code
	ret				/ Move nothing

strcat:
	movl	%edx, $-1		/ Max count to EDX

strcat0:
	push	%esi
	push	%edi

	movl	%esi, From(%esp)	/ From to ESI
	movl	%edi, To(%esp)		/ To to EDI
	movl	%ecx, $-1		/ Max count to ECX
	subb	%al, %al		/ NUL to AL
	cld
	repne
	scasb				/ Find end of To
	decl	%edi			/ and back up to NUL
	movl	%ecx, %edx		/ Count to ECX

?loop:
	lodsb				/ Fetch From char to AL
	stosb				/ and store through To
	orb	%al, %al
	loopne	?loop			/ Count nonzero and NUL not seen, continue
	je	?done			/ NUL has been copied, done
	subb	%al, %al		/ Count ran out before NUL found,
	stosb				/ NUL-terminate the string

?done:
	movl	%eax, To(%esp)		/ Return the destination

	pop	%edi
	pop	%esi
	ret

/ end of libc/string/i386/strcat.s
