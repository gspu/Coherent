	.intelorder
//////////
/ libc/string/i386/memcpy.s
/ i386 C string library.
/ ANSI 4.11.2.1.
//////////

//////////
/ void *
/ memcpy(void *To, void *From, size_t Count)
/
/ Copy Count bytes from From to To, return To.
/ Copies first by dwords, then by words, then by bytes, for efficiency;
/ does not try to align the arguments.
//////////

To	.equ	12
From	.equ	To+4
Count	.equ	From+4

	.globl	memcpy
	.globl	_memcpy

memcpy:
	push	%esi
	push	%edi

	movl	%esi, From(%esp)	/ From to ESI
	movl	%edi, To(%esp)		/ To to EDI
	movl	%ecx, Count(%esp)	/ Count to ECX
	movl	%eax, %edi		/ Return value to EAX

_memcpy:				/ memmove() entry point
	push	%es			/ save es
	push	%ds			/ copy ds to es
	pop	%es
	cld
	movl	%edx, %ecx		/ Save Count in EDX
	shrl	%ecx, $2		/ Count/4
	rep
	movsl				/ move Count/4 dwords
	jnc	?byte			/ CF contains Count bit 1 from shrl above
	movsw				/ move a word

?byte:
	shrl	%edx, $1
	jnc	?done
	movsb				/ move a byte

?done:
	pop	%es
	pop	%edi
	pop	%esi
	ret

/ end of libc/string/i386/memcpy.s
