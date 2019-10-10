//////////
/ libc/crt/i386/_fxam.s
/ i386 C runtime library.
/ IEEE software floating point support.
//////////

//////////
/ int _fxam(double d)
/ Return a value indicating the type of d,
/ corresponding to the return value of the "fxam" opcode, as follows:
/	1	+ NAN
/	3	- NAN
/	4	+ Normal
/	5	+ Infinity
/	6	- Normal
/	7	- Infinity
/	8	+ 0
/	10	- 0
/	12	+ Denormal
/	14	- Denormal
//////////

d	.equ	4
EXPMASK	.equ	0x7FF00000
MANMASK	.equ	0x000FFFFF

	.globl	_fxam

_fxam:
	movl	%edx, d+4(%esp)
	movl	%eax, d(%esp)		/ d to EDX:EAX
	movl	%ecx, %edx
	andl	%ecx, $MANMASK		/ extract hi mantissa dword
	orl	%eax, %ecx		/ EAX nonzero if any mantissa bits set
	movl	%ecx, %edx
	andl	%ecx, $EXPMASK		/ extract exponent in ECX
	jz	?zeroexp
	cmpl	%ecx, $EXPMASK
	jz	?maxexp
	movl	%eax, $4		/ arg is normal	
?setsign:
	orl	%edx, %edx
	jns	?L0			/ arg positive, return value from EAX
	addl	%eax, $2		/ bump result by 2 for negative arg
?L0:
	ret

?zeroexp:				/ exp is 0, arg is 0.0 or denormal
	orl	%eax, %eax		/ check if any mantissa bits set
	movl	%eax, $8
	jz	?setsign		/ arg is zero
	movl	%eax, $12		/ arg is denormal
	jmp	?setsign

?maxexp:				/ exp is max, arg is infinity or NaN
	orl	%eax, %eax		/ check if any mantissa bits set
	movl	%eax, $5
	jz	?setsign		/ arg is infinity
	movl	%eax, $1		/ arg is NaN
	jmp	?setsign

/ end of libc/crt/i386/_fxam.s
