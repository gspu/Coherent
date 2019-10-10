//////////
/ libc/string/i386/rindex.s
/ i386 C string library.
/ rindex() entry point to strrchr(), retained for backward compatability.
//////////

//////////
/ char *
/ rindex(char *String, int Char)
/
/ Find rightmost occurence of Char in String, return pointer or NULL.
//////////

	.globl	strrchr
	.globl	rindex

rindex:
	jmp	strrchr

/ end of libc/string/i386/rindex.s
