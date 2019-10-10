//////////
/ libc/string/i386/memmove.s
/ i386 C string library.
/ ANSI 4.11.2.2.
//////////

////////
/ void *
/ memmove(void *To, void *From, size_t Count)
/
/ Copy a block of memory from one
/ place in physical address space to
/ another place in physical address space.
/ Copy backwards if the source precedes the destination in memory.
/ Forward copy uses memcpy(), q.v.
/ Backward copy does byte-by-byte copy rather than trying to use dwords.
////////

To	.equ	12			/ destination
From	.equ	To+4			/ source
Count	.equ	From+4			/ byte count

	.globl	memmove
	.globl	_memcpy

memmove:
	push	%esi
	push	%edi

	movl	%edi, To(%esp)		/ Load EDI with destination.
	movl	%eax, %edi		/ Return value to EAX.
	movl	%ecx, Count(%esp)	/ Pick up byte count.
	jecxz	?done			/ No work.
	movl	%esi, From(%esp)	/ Load ESI with source.
	cmpl	%esi, %edi		/ Compare pointers.
	jz	?done			/ From == To.
	ja	_memcpy			/ From > To, copy forward.
	addl	%esi, %ecx		/ From < To,
	addl	%edi, %ecx		/ copy backward.
	decl	%esi			/ Find end of From
	decl	%edi			/ and end of To.
	std				/ Set flag for backward copy.
	rep				/ Copy
	movsb				/ bytes.
	cld				/ Clear direction flag.

?done:
	pop	%edi
	pop	%esi
	ret

/ end of libc/string/i386/memmove.s
