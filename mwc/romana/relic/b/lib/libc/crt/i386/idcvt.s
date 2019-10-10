//////////
/ libc/crt/i386/idcvt.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ int _idcvt()
/
/ Convert double in %edx:%eax to int, return in %eax.
//////////

MAXINT	.equ	0x7FFFFFFF
SGNMASK	.equ	0x80000000

	.globl	_idcvt
	.globl	_udcvt

_idcvt:
	orl	%edx, %edx		/ check arg sign
	pushfl				/ and save
	jns	?posarg
	xorl	%edx, $SGNMASK		/ force arg positive
?posarg:
	call	_udcvt			/ convert to unsigned
	orl	%eax, %eax		/ check high result bit
	js	?overflow		/ too big to represent as int
	popfl				/ restore arg sign
	jns	?done
	negl	%eax			/ negative arg, complement result
?done:
	ret

?overflow:
	movl	%eax, $MAXINT
	popfl				/ restore arg sign
	jns	?done
	incl	%eax			/ MAXINT+1 is MININT
	ret

/ end of libc/crt/i386/idcvt.s
