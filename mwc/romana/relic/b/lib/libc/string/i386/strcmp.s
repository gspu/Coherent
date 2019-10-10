//////////
/ libc/string/i386/strcmp.s
/ i386 C string library.
/ ANSI 4.11.4.2, 4.11.4.4.
//////////

//////////
/ int strcmp(char *String1, char *String2)
/ Compare String1 and String2 until mismatch or NUL seen.
/ Return -1 if <, 0 if ==, 1 if >.
/
/ int strncmp(char *String1, char *String2, size_t Count)
/ Compare up to Count bytes of String1 and String2.
//////////

String1	.equ	12
String2	.equ	String1+4
Count	.equ	String2+4

	.globl	strcmp
	.globl	strncmp

strncmp:
	movl	%ecx, Count-8(%esp)	/ Count to ECX
	orl	%ecx, %ecx
	jnz	strcmp0			/ fall in to common code...
	subl	%eax, %eax		/ Count 0, return NULL
	ret

strcmp:
	movl	%ecx, $-1		/ Max count to ECX

strcmp0:
	push	%esi
	push	%edi

	movl	%esi, String2(%esp)	/ String2 address to ESI
	movl	%edi, String1(%esp)	/ String1 address to EDI
	subl	%edx, %edx		/ Result 0 to EDX
	cld

?loop:
	lodsb				/ String2 character to AL
	scasb				/ Compare to String1 character
	jne	?noteq			/ Mismatch
	orb	%al, %al
	loopne	?loop			/ Continue if ECX!=0 && AL!=0
	jmp	?done			/ String1 == String2, return 0

?noteq:
	ja	?less
	incl	%edx			/ String1 > String2, return 1
	jmp	?done

?less:
	decl	%edx			/ String1 < String2, return -1

?done:
	movl	%eax, %edx		/ Result to EAX

	pop	%edi
	pop	%esi
	ret

/ end of libc/string/i386/strcmp.s
