/	module name testit

	.data

	.align	1
	.globl my_char
my_char:
	.byte	17
	.align	2
	.globl my_short
my_short:
	.word	17
	.align	4
	.globl my_int
my_int:
	.long	17
	.align	4
	.globl my_long
my_long:
	.long	17

	.text

	.globl main
main:

	.data

.L2:
	.byte	38
	.byte	109
	.byte	121
	.byte	95
	.byte	99
	.byte	104
	.byte	97
	.byte	114
	.byte	58
	.byte	32
	.byte	32
	.byte	37
	.byte	120
	.byte	10
	.byte	0

	.text


	.data

.L3:
	.byte	38
	.byte	109
	.byte	121
	.byte	95
	.byte	115
	.byte	104
	.byte	111
	.byte	114
	.byte	116
	.byte	58
	.byte	32
	.byte	37
	.byte	120
	.byte	10
	.byte	0

	.text


	.data

.L4:
	.byte	38
	.byte	109
	.byte	121
	.byte	95
	.byte	105
	.byte	110
	.byte	116
	.byte	58
	.byte	32
	.byte	32
	.byte	32
	.byte	37
	.byte	120
	.byte	10
	.byte	0

	.text


	.data

.L5:
	.byte	38
	.byte	109
	.byte	121
	.byte	95
	.byte	108
	.byte	111
	.byte	110
	.byte	103
	.byte	58
	.byte	32
	.byte	32
	.byte	37
	.byte	120
	.byte	10
	.byte	0

	.text


	.data

.L7:
	.byte	109
	.byte	121
	.byte	95
	.byte	99
	.byte	104
	.byte	97
	.byte	114
	.byte	32
	.byte	112
	.byte	97
	.byte	116
	.byte	99
	.byte	104
	.byte	101
	.byte	100
	.byte	32
	.byte	116
	.byte	111
	.byte	58
	.byte	32
	.byte	32
	.byte	37
	.byte	120
	.byte	10
	.byte	0

	.text


	.data

.L9:
	.byte	109
	.byte	121
	.byte	95
	.byte	99
	.byte	104
	.byte	97
	.byte	114
	.byte	32
	.byte	105
	.byte	115
	.byte	32
	.byte	100
	.byte	101
	.byte	102
	.byte	97
	.byte	117
	.byte	108
	.byte	116
	.byte	101
	.byte	100
	.byte	46
	.byte	10
	.byte	0

	.text


	.data

.L11:
	.byte	109
	.byte	121
	.byte	95
	.byte	115
	.byte	104
	.byte	111
	.byte	114
	.byte	116
	.byte	32
	.byte	112
	.byte	97
	.byte	116
	.byte	99
	.byte	104
	.byte	101
	.byte	100
	.byte	32
	.byte	116
	.byte	111
	.byte	58
	.byte	32
	.byte	32
	.byte	37
	.byte	120
	.byte	10
	.byte	0

	.text


	.data

.L13:
	.byte	109
	.byte	121
	.byte	95
	.byte	115
	.byte	104
	.byte	111
	.byte	114
	.byte	116
	.byte	32
	.byte	105
	.byte	115
	.byte	32
	.byte	100
	.byte	101
	.byte	102
	.byte	97
	.byte	117
	.byte	108
	.byte	116
	.byte	101
	.byte	100
	.byte	46
	.byte	10
	.byte	0

	.text


	.data

.L15:
	.byte	109
	.byte	121
	.byte	95
	.byte	105
	.byte	110
	.byte	116
	.byte	32
	.byte	112
	.byte	97
	.byte	116
	.byte	99
	.byte	104
	.byte	101
	.byte	100
	.byte	32
	.byte	116
	.byte	111
	.byte	58
	.byte	32
	.byte	32
	.byte	37
	.byte	120
	.byte	10
	.byte	0

	.text


	.data

.L17:
	.byte	109
	.byte	121
	.byte	95
	.byte	105
	.byte	110
	.byte	116
	.byte	32
	.byte	105
	.byte	115
	.byte	32
	.byte	100
	.byte	101
	.byte	102
	.byte	97
	.byte	117
	.byte	108
	.byte	116
	.byte	101
	.byte	100
	.byte	46
	.byte	10
	.byte	0

	.text


	.data

.L19:
	.byte	109
	.byte	121
	.byte	95
	.byte	108
	.byte	111
	.byte	110
	.byte	103
	.byte	32
	.byte	112
	.byte	97
	.byte	116
	.byte	99
	.byte	104
	.byte	101
	.byte	100
	.byte	32
	.byte	116
	.byte	111
	.byte	58
	.byte	32
	.byte	32
	.byte	37
	.byte	108
	.byte	120
	.byte	10
	.byte	0

	.text


	.data

.L21:
	.byte	109
	.byte	121
	.byte	95
	.byte	108
	.byte	111
	.byte	110
	.byte	103
	.byte	32
	.byte	105
	.byte	115
	.byte	32
	.byte	100
	.byte	101
	.byte	102
	.byte	97
	.byte	117
	.byte	108
	.byte	116
	.byte	101
	.byte	100
	.byte	46
	.byte	10
	.byte	0

	.text


	.data

.L22:
	.byte	67
	.byte	97
	.byte	108
	.byte	108
	.byte	105
	.byte	110
	.byte	103
	.byte	32
	.byte	101
	.byte	116
	.byte	119
	.byte	97
	.byte	115
	.byte	40
	.byte	41
	.byte	58
	.byte	10
	.byte	0

	.text


	.data

.L23:
	.byte	67
	.byte	97
	.byte	108
	.byte	108
	.byte	105
	.byte	110
	.byte	103
	.byte	32
	.byte	110
	.byte	105
	.byte	120
	.byte	40
	.byte	41
	.byte	58
	.byte	10
	.byte	0

	.text


	.data

.L24:
	.byte	82
	.byte	101
	.byte	116
	.byte	117
	.byte	114
	.byte	110
	.byte	105
	.byte	110
	.byte	103
	.byte	32
	.byte	102
	.byte	114
	.byte	111
	.byte	109
	.byte	32
	.byte	110
	.byte	105
	.byte	120
	.byte	40
	.byte	41
	.byte	46
	.byte	10
	.byte	0

	.text

	push	%ebp
	movl	%ebp, %esp
	push	$my_char
	push	$.L2
	call	printf
	addl	%esp, $8
	push	$my_short
	push	$.L3
	call	printf
	addl	%esp, $8
	push	$my_int
	push	$.L4
	call	printf
	addl	%esp, $8
	push	$my_long
	push	$.L5
	call	printf
	addl	%esp, $8
	movsxb	%eax, my_char
	cmpl	%eax, $17
	je	.L6
	movsxb	%eax, my_char
	push	%eax
	push	$.L7
	call	printf
	addl	%esp, $8
	jmp	.L8
.L6:
	push	$.L9
	call	printf
	pop	%ecx
.L8:
	movsx	%eax, my_short
	cmpl	%eax, $17
	je	.L10
	movsx	%eax, my_short
	push	%eax
	push	$.L11
	call	printf
	addl	%esp, $8
	jmp	.L12
.L10:
	push	$.L13
	call	printf
	pop	%ecx
.L12:
	movl	%eax, my_int
	cmpl	%eax, $17
	je	.L14
	push	%eax
	push	$.L15
	call	printf
	addl	%esp, $8
	jmp	.L16
.L14:
	push	$.L17
	call	printf
	pop	%ecx
.L16:
	movl	%eax, my_long
	cmpl	%eax, $17
	je	.L18
	push	%eax
	push	$.L19
	call	printf
	addl	%esp, $8
	jmp	.L20
.L18:
	push	$.L21
	call	printf
	pop	%ecx
.L20:
	push	$.L22
	call	printf
	pop	%ecx
	call	etwas
	push	$.L23
	call	printf
	pop	%ecx
	call	nix
	push	$.L24
	call	printf
	pop	%ecx
	leave
	ret
	.align	4
	.globl fun1
fun1:

	.data

.L26:
	.byte	84
	.byte	104
	.byte	105
	.byte	115
	.byte	32
	.byte	105
	.byte	115
	.byte	32
	.byte	102
	.byte	117
	.byte	110
	.byte	49
	.byte	40
	.byte	41
	.byte	46
	.byte	10
	.byte	0

	.text

	push	%ebp
	movl	%ebp, %esp
	push	$.L26
	call	printf
	pop	%ecx
	leave
	ret
	.align	4
	.globl fun2
fun2:

	.data

.L28:
	.byte	84
	.byte	104
	.byte	105
	.byte	115
	.byte	32
	.byte	105
	.byte	115
	.byte	32
	.byte	102
	.byte	117
	.byte	110
	.byte	50
	.byte	40
	.byte	41
	.byte	46
	.byte	10
	.byte	0

	.text

	push	%ebp
	movl	%ebp, %esp
	push	$.L28
	call	printf
	pop	%ecx
	leave
	ret
	.align	4
