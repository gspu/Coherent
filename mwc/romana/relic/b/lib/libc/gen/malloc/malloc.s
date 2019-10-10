/	module name malloc
	.alignoff

	.text
newarena:
	enter	$4, $0
	push	%esi
	push	%edi
	push	%ebx
	cmpb	.L3, $0
	je	.L4
	subl	%eax, %eax
	jmp	.L2
.L4:
	push	$0
	call	sbrk
	pop	%ecx
	andl	%eax, $3
	movl	-4(%ebp), %eax
	orl	%eax, %eax
	je	.L5
	movl	%eax, $4
	subl	%eax, -4(%ebp)
	push	%eax
	call	sbrk
	pop	%ecx
	cmpl	%eax, $-1
	jne	.L5
	movb	.L3, $1
	jmp	.L20002
.L5:
	movl	%eax, 8(%ebp)
	addl	%eax, $4104
	decl	%eax
	andl	%eax, $-4096
	movl	-4(%ebp), %eax
	cmpl	%eax, 8(%ebp)
	jae	.L6
	movl	%eax, 8(%ebp)
	movl	-4(%ebp), %eax
.L6:
	movl	%eax, __a_first
	movl	__a_scanp, %eax
.L8:
	push	-4(%ebp)
	call	sbrk
	pop	%ecx
	movl	%ebx, %eax
	cmpl	%eax, $-1
	jne	.L7
	movb	.L3, $1
	movl	%eax, -4(%ebp)
	cmpl	%eax, $256
	jbe	.L20002
	subl	-4(%ebp), $256
	movl	%eax, -4(%ebp)
	cmpl	%eax, $8
	jae	.L8
	movl	-4(%ebp), $8
	jmp	.L8
.L7:
	cmpl	__a_top, $0
	jne	.L11
	movl	__a_count, $2
	movl	%eax, %ebx
	movl	%edi, %eax
	movl	__a_scanp, %eax
	movl	__a_first, %eax
	jmp	.L12
.L11:
	cmpl	%ebx, __a_top
	jne	.L13
	subl	%ebx, $8
	addl	-4(%ebp), $8
	movl	%edi, 4(%ebx)
	incl	__a_count
	jmp	.L12
.L13:
	movl	%eax, __a_top
	subl	%eax, $8
	movl	%esi, %eax
	movl	%edi, 4(%esi)
	movl	4(%esi), %ebx
	addl	__a_count, $2
.L12:
	movl	%eax, -4(%ebp)
	subl	%eax, $8
	orl	%eax, $1
	movl	(%ebx), %eax
	movl	%eax, %ebx
	addl	%eax, -4(%ebp)
	movl	__a_top, %eax
	subl	%eax, $8
	movl	%esi, %eax
	movl	(%esi), $0
	movl	4(%esi), %edi
.L20002:
	movl	%eax, $1
.L2:
	pop	%ebx
	pop	%edi
	pop	%esi
	leave
	ret
	.align	4
	.globl malloc
malloc:
	enter	$8, $0
	push	%esi
	push	%edi
	push	%ebx
	cmpl	8(%ebp), $0
	jne	.L17
.L20004:
	subl	%eax, %eax
	jmp	.L15
.L17:
	movl	%eax, 8(%ebp)
	addl	%eax, $8
	decl	%eax
	andl	%eax, $-4
	movl	-4(%ebp), %eax
	cmpl	%eax, 8(%ebp)
	jb	.L20004
.L21:
	subl	%esi, %esi
	movl	%ebx, __a_scanp
	movl	%eax, __a_count
	movl	-8(%ebp), %eax
.L23:
	movl	%eax, -8(%ebp)
	decl	-8(%ebp)
	orl	%eax, %eax
	je	.L22
	movl	%ecx, (%ebx)
	movl	%edi, %ecx
	testl	%ecx, $1
	jne	.L24
	subl	%esi, %esi
.L25:
	orl	%edi, %edi
	je	.L10001
	movl	%eax, %edi
	andl	%eax, $-2
	addl	%eax, %ebx
	jmp	.L10002
.L10001:
	movl	%eax, 4(%ebx)
.L10002:
	movl	%ebx, %eax
	jmp	.L23
.L22:
	cmpl	%ebx, __a_scanp
	je	.L20
	push	$24
	push	$.L16
	push	$2
	call	write
	addl	%esp, $12
	call	abort
.L20:
	push	-4(%ebp)
	call	newarena
	pop	%ecx
	orl	%eax, %eax
	jne	.L21
	jmp	.L20004
.L24:
	orl	%esi, %esi
	je	.L26
	movl	%ebx, %esi
	movl	%eax, %edi
	andl	%eax, $-2
	addl	(%ebx), %eax
	movl	%eax, (%ebx)
	movl	%edi, %eax
	decl	__a_count
.L26:
	cmpl	%edi, -4(%ebp)
	jae	.L27
	movl	%esi, %ebx
	jmp	.L25
.L27:
	movl	%eax, -4(%ebp)
	subl	%edi, %eax
	movl	%eax, %edi
	cmpl	%eax, $8
	jae	.L29
	movl	%eax, (%ebx)
	andl	%eax, $-2
	movl	-4(%ebp), %eax
	movl	(%ebx), %eax
	movl	%eax, %ebx
	addl	%eax, -4(%ebp)
	movl	__a_scanp, %eax
	jmp	.L30
.L29:
	movl	%eax, -4(%ebp)
	movl	(%ebx), %eax
	movl	%eax, %ebx
	addl	%eax, -4(%ebp)
	movl	__a_scanp, %eax
	movl	%ecx, __a_scanp
	movl	(%ecx), %edi
	incl	__a_count
.L30:
	lea	%eax, 4(%ebx)
.L15:
	pop	%ebx
	pop	%edi
	pop	%esi
	leave
	ret
	.align	4
	.globl free
free:
	push	%ebp
	movl	%ebp, %esp
	push	%esi
	push	%edi
	push	%ebx
	cmpl	8(%ebp), $0
	je	.L32
	movl	%eax, 8(%ebp)
	subl	%eax, $4
	movl	%ebx, %eax
	movl	%esi, (%ebx)
	cmpl	%esi, $2
	jae	.L34
	push	$22
	push	$.L35
	push	$2
	call	write
	addl	%esp, $12
	call	abort
.L34:
	orl	(%ebx), $1
	movl	%edi, %esi
	andl	%edi, $-2
	movl	%eax, %ebx
	addl	%eax, %edi
	cmpl	%eax, __a_scanp
	je	.L10003
	movl	%edi, __a_scanp
	testl	(%edi), $1
	jne	.L32
.L10003:
	movl	__a_scanp, %ebx
.L32:
	pop	%ebx
	pop	%edi
	pop	%esi
	leave
	ret
	.align	4
	.align	4

	.data
	.align	4
	.globl __a_scanp
__a_scanp:
	.long	0
	.align	4
	.globl __a_first
__a_first:
	.long	0
	.align	4
	.globl __a_top
__a_top:
	.long	0
	.align	4
	.globl __a_count
__a_count:
	.long	0
	.align	1
.L3:
	.byte	0
	.align	1
.L16:
	.byte	66
	.byte	97
	.byte	100
	.byte	32
	.byte	112
	.byte	111
	.byte	105
	.byte	110
	.byte	116
	.byte	101
	.byte	114
	.byte	32
	.byte	105
	.byte	110
	.byte	32
	.byte	109
	.byte	97
	.byte	108
	.byte	108
	.byte	111
	.byte	99
	.byte	46
	.byte	13
	.byte	10
	.byte	0
	.align	1
.L35:
	.byte	66
	.byte	97
	.byte	100
	.byte	32
	.byte	112
	.byte	111
	.byte	105
	.byte	110
	.byte	116
	.byte	101
	.byte	114
	.byte	32
	.byte	105
	.byte	110
	.byte	32
	.byte	102
	.byte	114
	.byte	101
	.byte	101
	.byte	46
	.byte	13
	.byte	10
	.byte	0
