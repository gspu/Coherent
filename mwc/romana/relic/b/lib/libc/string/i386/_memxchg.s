//////////
/ libc/string/i386/_memxchg.s
/ i386 C string library.
/ Not in ANSI C standard.
//////////


//////////
/ void _memxchg(char *S1, char *S2, size_t Count)
/
/ Exchange records of Count bytes pointed to by S1 and S2.
/ Copies first by dwords, then by words, then by bytes, for efficiency;
/ does not try to align the arguments.
//////////

S1	.equ	12
S2	.equ	S1+4
Count	.equ	S2+4

	.globl	_memxchg

_memxchg:
	push	%esi
	push	%edi

	movl	%ecx, Count(%esp)	/ Count to ECX
	movl	%esi, S2(%esp)		/ S2 address to ESI
	movl	%edi, S1(%esp)		/ S1 address to EDI
	cld
	movl	%edx, %ecx		/ Save Count in EDX
	shrl	%ecx, $2		/ Convert byte count to dword count
	jz	?word			/ No dwords to move

?dword:
	movl	%eax, (%edi)		/ S1 dword to EAX
	movsl				/ S2 dword to S1, bump EDI and ESI
	movl	-4(%esi), %eax		/ S1 word to old S2
	loop	?dword			/ Decrement dword count and repeat

?word:
	jnc	?byte			/ No word move, CF is set from shrl above
	movw	%ax, (%edi)		/ S1 word to AX
	movsw				/ S2 word to S1, bump EDI and ESI
	movw	-2(%esi), %ax		/ S1 word to old S2

?byte:
	shrl	%edx, $1		/ Check bit 0 of Count
	jnc	?done			/ No byte move
	movb	%al, (%edi)		/ S1 byte to AL
	movsb				/ S2 byte to S1, bump EDI and ESI
	movb	-1(%esi), %al		/ S1 byte to old S2

?done:
	pop	%edi
	pop	%esi
	ret

/ end of libc/string/i386/_memxchg.s
