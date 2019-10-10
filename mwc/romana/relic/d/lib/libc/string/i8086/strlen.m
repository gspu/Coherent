//////////
/ i8086 C string library.
/ strlen()
/ ANSI 4.11.6.3.
//////////

//////////
/ size_t
/ strlen(String)
/ char *String;
/
/ Find length of String.
//////////

#include <larges.h>

String	=	LEFTARG

	Enter(strlen_)
	Les	di, String(bp)	/ String address to ES:DI
	cld
	mov	dx, di		/ Save start
	mov	cx, $-1		/ Max count to CX
	sub	ax, ax
	repne
	scasb			/ Scan to NUL
	dec	di		/ Back up to match
	mov	ax, di
	sub	ax, dx		/ &NUL - &start = length
	Leave
