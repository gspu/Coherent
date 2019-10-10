//////////
/ libc/gen/i386/canon.s
/ i386 C library.
//////////

//////////
/ long
/ _canl(l) long l;
/ 
/ Transform long to/from canonical format.
//////////

l	.equ	4

	.globl	_canl

_canl:
	movl	%eax, l(%esp)		/ l to EAX
	roll	%eax, $16		/ exchange high and low words
	ret

/ end of libc/gen/i386/canon.s
