//////////
/ libc/string/i386/index.s
/ i386 C string library.
/ index() entry point to strchr(), retained for backward compatability.
//////////

//////////
/ char *
/ index(char *String, int Char)
/
/ Find Char in String, return pointer or NULL.
//////////

	.globl	strchr
	.globl	index

index:
	jmp	strchr

/ end of libc/string/i386/index.s
