//////////
/ libc/string/i386/strstr.s
/ i386 C string library.
/ ANSI 4.11.5.7.
//////////

//////////
/ char *
/ strstr(char *String1, char *String2)
/
/ Return the first occurance in String1 of String2.
/ If String2 is empty, return String1.
//////////

String1	.equ	16
String2	.equ	String1+4

	.globl	strstr

strstr:
	push	%esi
	push	%edi
	push	%ebx

	movl	%ebx, String1(%esp)	/ String1 address to EBX
	movl	%edi, String2(%esp)	/ String2 address to EDI
	movl	%edx, %edi		/ Save String2 start in EDX
	movl	%ecx, $-1		/ Max count to ECX
	subb	%al, %al		/ NUL to AL
	cld
	repne
	scasb				/ Scan to NUL
	movl	%eax, $-2
	subl	%eax, %ecx		/ Length of String2 to EAX
	jz	?gotcha			/ String2 is empty, just return String1

?loop:
	movl	%ecx, %eax		/ Count to ECX
	movl	%edi, %edx		/ String2 address to EDI
	movl	%esi, %ebx		/ Current location within String1 to ESI
	repe
	cmpsb				/ Compare entire String2 to current String1
	je	?gotcha			/ Success, return current location
	incl	%ebx			/ Mismatched, try next String1 location
	cmpb	-1(%esi), $0		/ Check if end of String1 reached
	jne	?loop			/ No, keep trying
	subl	%ebx, %ebx		/ Failure, return NULL

?gotcha:
	movl	%eax, %ebx		/ Success, return location in String1

?done:
	pop	%ebx
	pop	%edi
	pop	%esi
	ret

/ end of libc/string/i386/strstr.s
