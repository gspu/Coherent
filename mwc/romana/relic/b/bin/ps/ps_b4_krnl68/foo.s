/	module name foo

	.text

	.globl main
main:

	.data

.L3:
	.byte	69
	.byte	118
	.byte	101
	.byte	114
	.byte	121
	.byte	116
	.byte	104
	.byte	105
	.byte	110
	.byte	103
	.byte	32
	.byte	105
	.byte	115
	.byte	32
	.byte	79
	.byte	75
	.byte	46
	.byte	10
	.byte	0

	.text


	.data

.L5:
	.byte	67
	.byte	111
	.byte	109
	.byte	112
	.byte	105
	.byte	108
	.byte	101
	.byte	114
	.byte	32
	.byte	98
	.byte	117
	.byte	103
	.byte	46
	.byte	10
	.byte	0

	.text

	enter	$8, $0
	movw	-8(%ebp), $-1
	movw	%ax, -8(%ebp)
	movzx	%eax, %ax
	movw	-4(%ebp), %ax
	cmpw	%ax, $-1
	jne	.L2
	push	$.L3
	jmp	.L20002
.L2:
	push	$.L5
.L20002:
	call	printf
	pop	%ecx
	leave
	ret
	.align	4
