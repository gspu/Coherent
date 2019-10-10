//////////
/ libc/string/i386/strspn.s
/ i386 C string library.
/ ANSI 4.11.5.6.
//////////

//////////
/ size_t
/ strspn(char *String1, char *String2)
/
/ Return the length of the initial segment of String1
/ which consists of characters in String2.
//////////

String1	.equ	12
String2	.equ	String1+4

	.globl	strspn

strspn:
	push	%esi
	push	%edi

	movl	%edi, String1(%esp)	/ String1 address to EDI
	movl	%edx, %edi		/ and save for length calculation
	cld

?fetch1:
	movb	%ah, (%edi)		/ String1 character to AH
	orb	%ah, %ah
	jz	?done			/ End of String1, done
	movl	%esi, String2(%esp)	/ String2 address to ESI

?fetch2:
	lodsb				/ String2 character to AL
	orb	%al, %al		/ If at end of String2, then
	jz	?done			/ String1 char is not in String2, done
	cmpb	%ah, %al
	jnz	?fetch2			/ No match, try next char in String2
	incl	%edi			/ Match, try next char in String1
	jmp	?fetch1

?done:
	movl	%eax, %edi
	subl	%eax, %edx		/ Return &last - &String1

	pop	%edi
	pop	%esi
	ret

/ end of libc/string/i386/strspn.s
