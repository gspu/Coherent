	.ttl	Full test of asm 386
	.llen	100
abc:
	aaa		/ 0000 0037
	aad		/ 0000 ffffd50a
	aam		/ 0000 ffffd40a
	aas		/ 0000 003f
	arpl	%bx, 0x3e8	/ 0000 0063
	call	abc	/ 0000 00e8
	cbw		/ 0001 0098
	cbtw		/ 0001 0098
	cwde		/ 0002 0098
	cwtl		/ 0002 0098
	clc		/ 0000 00f8
	cld		/ 0000 00fc
	cli		/ 0000 00fa
	clts		/ 0008 0006
	cmc		/ 0000 00f5
	cmpsb		/ 0400 00a6
	cmpsw		/ 0401 00a7
	cmpsl		/ 0402 00a7
	cwd		/ 0001 0099
	cwtd		/ 0001 0099
	cdq		/ 0002 0099
	cltd		/ 0002 0099
	daa		/ 0000 0027
	das		/ 0000 002f
	enter	$0x64, $0x3e8	/ 0000 00c8
	f2xm1		/ 2000 ffffd9f0
	fabs		/ 2000 ffffd9e1
	fbld	0x64(%di)	/ 2020 ffffdf04
	fbstp	(%bp, %si)	/ 2020 ffffdf06
	fchs		/ 2000 ffffd9e0
	fcompp		/ 2000 ffffded9
	fcos		/ 2000 ffffd9ff
	fdecstp		/ 2000 ffffd9f6
	ffree	%st	/ 2010 ffffddc0
	fincstp		/ 2000 ffffd9f7
	fldcw	0x64(%bp, %si)	/ 2020 ffffd905
	fldenv	0x3e8	/ 2020 ffffd904
	fld1		/ 2000 ffffd9e8
	fldl2t		/ 2000 ffffd9e9
	fldl2e		/ 2000 ffffd9ea
	fldpi		/ 2000 ffffd9eb
	fldlg2		/ 2000 ffffd9ec
	fldln2		/ 2000 ffffd9ed
	fldz		/ 2000 ffffd9ee
	fnclex		/ 2000 ffffdbe2
	fclex		/ 2004 ffffdbe2
	fninit		/ 2000 ffffdbe3
	finit		/ 2004 ffffdbe3
	fnop		/ 2000 ffffd9d0
	fpatan		/ 2000 ffffd9f3
	fprem		/ 2000 ffffd9f8
	fprem1		/ 2000 ffffd9f5
	fptan		/ 2000 ffffd9f2
	frstpm		/ 2000 ffffdbf4
	frndint		/ 2000 ffffd9fc
	frstor	0x3e8	/ 2020 ffffdd04
	fnsave	0x3e8	/ 2020 ffffdd06
	fsave	(%bx, %di)	/ 2024 ffffdd06
	fscale		/ 2000 ffffd9fd
	fsetpm		/ 2000 ffffdbe4
	fsin		/ 2000 ffffd9fe
	fsincos		/ 2000 ffffd9fb
	fsqrt		/ 2000 ffffd9fa
	fnstcw	0x3e8(%bp)	/ 2020 ffffd907
	fstcw	0x3e8	/ 2024 ffffd907
	fnstenv	(%edi)	/ 2020 ffffd906
	fstenv	(, %edx, 0)	/ 2024 ffffd906
	ftst		/ 2000 ffffd9e4
	fucompp		/ 2000 ffffdae9
	fwait		/ 2000 009b
	fxam		/ 2000 ffffd9e5
	fxtract		/ 2000 ffffd9f4
	fyl2x		/ 2000 ffffd9f1
	fyl2xp1		/ 2000 ffffd9f9
	hlt		/ 0000 00f4
	icall	%edx	/ 0022 ffffff02
	ijmp	0x3e8(%di)	/ 0022 ffffff04
	ilcall	0x3e8	/ 0022 ffffff03
	iljmp	0x64(%bp, %di)	/ 0022 ffffff05
	int	$3	/ 0000 00cc
	int	$0x64	/ 0000 00cd
	into		/ 0001 00ce
	insb		/ 0400 006c
	insb	(%dx)	/ 0400 006c
	insw		/ 0401 006d
	insw	(%dx)	/ 0401 006d
	insl		/ 0402 006d
	insl	(%dx)	/ 0402 006d
	iret		/ 0000 00cf
	iretd		/ 0000 00cf
	ja	abc	/ 0000 0007
	jae	abc	/ 0000 0003
	jb	.	/ 0000 0002
	jbe	.	/ 0000 0006
	jc	.	/ 0000 0002
	je	abc	/ 0000 0004
	jz	abc	/ 0000 0004
	jg	.	/ 0000 000f
	jge	abc	/ 0000 000d
	jl	abc	/ 0000 000c
	jle	.	/ 0000 000e
	jna	.	/ 0000 0006
	jnae	.	/ 0000 0002
	jnb	.	/ 0000 0003
	jnbe	abc	/ 0000 0007
	jnc	abc	/ 0000 0003
	jne	abc	/ 0000 0005
	jng	.	/ 0000 000e
	jnge	abc	/ 0000 000c
	jnl	abc	/ 0000 000d
	jnle	abc	/ 0000 000f
	jno	abc	/ 0000 0001
	jnp	abc	/ 0000 000b
	jns	.	/ 0000 0009
	jnz	abc	/ 0000 0005
	jo	.	/ 0000 0000
	jp	.	/ 0000 000a
	jpe	abc	/ 0000 000a
	jpo	abc	/ 0000 000b
	js	.	/ 0000 0008
	jz	abc	/ 0000 0004
	jmp	abc	/ 0000 00e9
	jcxz	.	/ 0000 00e3
	jecxz	.	/ 0000 00e3
	lahf		/ 0000 009f
	lcall	$0, $0x186a0	/ 0002 009a
	ljmp	$0, $0x186a0	/ 0002 00ea
	leave		/ 0000 00c9
	lldt	0x3e8	/ 0028 0002
	lmsw	0x3e8	/ 0028 0106
	lock		/ 0080 00f0
	lodsb		/ 0400 00ac
	lodsw		/ 0401 00ad
	lodsl		/ 0402 00ad
	loop	.	/ 0000 00e2
	loope	.	/ 0000 00e1
	loopz	.	/ 0000 00e1
	loopne	.	/ 0000 00e0
	loopnz	.	/ 0000 00e0
	ltr	%sp	/ 0028 0003
	movsb		/ 0400 00a4
	movsw		/ 0401 00a5
	movsl		/ 0402 00a5
	nop		/ 0000 0090
	outsb		/ 0400 006e
	outsw		/ 0401 006f
	outsl		/ 0402 006f
	rep		/ 0200 00f3
	repe		/ 0200 00f3
	repz		/ 0200 00f3
	repne		/ 0200 00f2
	repnz		/ 0200 00f2
	ret		/ 0000 00c3
	ret	$0x3e8	/ 0000 00c2
	lret		/ 0000 00cb
	lret	$0x3e8	/ 0000 00ca
	sahf		/ 0000 009e
	scasb		/ 0400 00ae
	scasl		/ 0402 00af
	scasw		/ 0401 00af
	seta	0x3e8	/ 0008 0097
	setae	%al	/ 0008 0093
	setb	0x3e8(%bp)	/ 0008 0092
	setbe	(%eax, %ecx, 0)	/ 0008 0096
	setc	0x3e8(%bp, %di)	/ 0008 0092
	sete	0x64(%ebp)	/ 0008 0094
	setz	0x64(%ecx, 4)	/ 0008 0094
	setg	0x3e8	/ 0008 009f
	setge	0x186a0(%eax)	/ 0008 009d
	setl	0x3e8(%esi, %eax, 2)	/ 0008 009c
	setle	(, %edx, 2)	/ 0008 009e
	setna	%bh	/ 0008 0096
	setnae	0x64(%bx, %di)	/ 0008 0092
	setnb	0x3e8	/ 0008 0093
	setnbe	0x3e8	/ 0008 0097
	setnc	0x3e8	/ 0008 0093
	setne	%dh	/ 0008 0095
	setng	0x3e8(%si)	/ 0008 009e
	setnge	0x3e8	/ 0008 009c
	setnl	%bl	/ 0008 009d
	setnle	%bl	/ 0008 009f
	setno	0x64(%bp)	/ 0008 0091
	setnp	0x186a0(%eax)	/ 0008 009b
	setns	(, %ebp, 8)	/ 0008 0099
	setnz	(%eax, 2)	/ 0008 0095
	seto	(%ebx, %esi)	/ 0008 0090
	setp	0x3e8(%bx, %si)	/ 0008 009a
	setpe	(%bp, %si)	/ 0008 009a
	setpo	0x3e8	/ 0008 009b
	sets	0x64(%bx, %di)	/ 0008 0098
	setz	0x3e8	/ 0008 0094
	sgdt	abc	/ 0028 0100
	sidt	abc	/ 0028 0101
	sldt	%si	/ 0028 0000
	smsw	%di	/ 0028 0104
	stc		/ 0000 00f9
	std		/ 0000 00fd
	sti		/ 0000 00fb
	stosb		/ 0400 00aa
	stosw		/ 0401 00ab
	stosl		/ 0402 00ab
	str		/ 0028 0001
	verr	%si	/ 0028 0004
	verw	%ax	/ 0028 0005
	wait		/ 0000 009b
	xlat		/ 0000 00d7
	xlatb		/ 0000 00d7
	adcl	$-5, 0x3e8	/ 0122 ffff8302
	adcw	$-5, 0x3e8	/ 0121 ffff8302
	adcb	$0x64, %al	/ 0000 0014
	adcw	$0x3e8, %ax	/ 0001 0015
	adcl	$0x186a0, %eax	/ 0002 0015
	adcl	$0x186a0	/ 0002 0015
	adcb	$0x64, 0x3e8	/ 0120 ffff8002
	adcw	$0x3e8, 0x64(%bx, %si)	/ 0121 ffff8102
	adcl	$0x186a0, 0x3e8	/ 0122 ffff8102
	adcb	0x3e8, %bh	/ 0100 0012
	adcw	0x3e8, %sp	/ 0101 0013
	adcl	0x64(%bp, %di), %edx	/ 0102 0013
	adcb	%cl, 0x3e8	/ 0100 0010
	adcw	%si, (%edx, %ebp, 0)	/ 0101 0011
	adcl	%esi, %ecx	/ 0102 0011
	addl	$064, 0x64(%di)	/ 0122 ffff8300
	addw	$-5, 0x64(%ebp)	/ 0121 ffff8300
	addb	$0x64, %al	/ 0100 0004
	addw	$0x3e8, %ax	/ 0101 0005
	addl	$0x186a0, %eax	/ 0102 0005
	addl	$0x186a0	/ 0102 0005
	addb	$0x64, 0x64(%bx, %di)	/ 0120 ffff8000
	addw	$0x3e8, 0x3e8(%di)	/ 0121 ffff8100
	addl	$0x186a0, 0x64(%edx)	/ 0122 ffff8100
	addb	%ah, %ah	/ 0100 0002
	addw	(%bp, %si), %cx	/ 0101 0003
	addl	(%edi, %ebp), %ebp	/ 0102 0003
	addb	%dh, 0x3e8	/ 0100 0000
	addw	%di, (%ebp, %ebp)	/ 0101 0001
	addl	%ecx, 0x64(%bp, %di)	/ 0102 0001
	andl	$-5, 0x3e8	/ 0122 ffff8304
	andw	$-5, %si	/ 0121 ffff8304
	andb	$0x64, %al	/ 0100 0024
	andw	$0x3e8, %ax	/ 0101 0025
	andl	$0x186a0, %eax	/ 0102 0025
	andl	$0x186a0	/ 0102 0025
	andb	$0x64, %ah	/ 0120 ffff8004
	andw	$0x3e8, 0x3e8	/ 0121 ffff8104
	andl	$0x186a0, 0x3e8	/ 0122 ffff8104
	andb	0x64(%bx), %ah	/ 0100 0022
	andw	0x3e8, %bp	/ 0101 0023
	andl	(%bx, %si), %esp	/ 0102 0023
	andb	%bh, 0x3e8	/ 0100 0020
	andw	%ax, (%bp, %di)	/ 0101 0021
	andl	%esi, 0x64(%esi, 8)	/ 0102 0021
	boundw	0x3e8, %di	/ 0001 0062
	boundl	0x3e8, %ebp	/ 0002 0062
	bsfw	0x3e8, %bx	/ 0009 00bc
	bsfl	%ebp, %eax	/ 000a 00bc
	bsrw	0x3e8(%bp, %si), %ax	/ 0009 00bd
	bsrl	(%ebp, %edi), %esi	/ 000a 00bd
	btw	%cx, 0x3e8	/ 0009 00a3
	btl	%esi, 0x3e8(%bx)	/ 000a 00a3
	btw	$0x64, 0x3e8	/ 0029 ffffba04
	btl	$0x64, (, %edx, 8)	/ 002a ffffba04
	btcw	%cx, %dx	/ 0109 00bb
	btcl	%eax, %edx	/ 010a 00bb
	btcw	$0x64, (%ecx, 0)	/ 0129 ffffba07
	btcl	$0x64, %edx	/ 012a ffffba07
	btrw	%ax, 0x3e8(%edx, %ecx, 1)	/ 0109 00b3
	btrl	%eax, 0x3e8	/ 010a 00b3
	btrw	$0x64, (%bp, %si)	/ 0129 ffffba06
	btrl	$0x64, 0x3e8	/ 012a ffffba06
	btsw	%cx, 0x3e8(%bx, %si)	/ 0109 00ab
	btsl	%ecx, 0x64(%di)	/ 010a 00ab
	btsw	$0x64, 0x3e8	/ 0129 ffffba05
	btsl	$0x64, 0x64(%ecx, 0)	/ 012a ffffba05
	cmpl	$064, (%bp, %si)	/ 0022 ffff8307
	cmpw	$-5, 0x3e8	/ 0021 ffff8307
	cmpb	$0x64, %al	/ 0000 003c
	cmpw	$0x3e8, %ax	/ 0001 003d
	cmpl	$0x186a0, %eax	/ 0002 003d
	cmpl	$0x186a0	/ 0002 003d
	cmpb	$0x64, (, %eax, 2)	/ 0020 ffff8007
	cmpw	$0x3e8, 0x3e8(%bp)	/ 0021 ffff8107
	cmpl	$0x186a0, 0x3e8	/ 0022 ffff8107
	cmpb	0x3e8(%bx), %bl	/ 0000 003a
	cmpw	0x64(%bx), %bp	/ 0001 003b
	cmpl	0x64(%eax), %ebp	/ 0002 003b
	cmpb	%ah, 0x3e8	/ 0000 0038
	cmpw	%dx, 0x3e8	/ 0001 0039
	cmpl	%ebp, 0x3e8	/ 0002 0039
	decw	%sp	/ 0011 0048
	decl	%edi	/ 0012 0048
	decb	0x3e8(%bx, %di)	/ 0120 fffffe01
	decw	0x64(%bp, %di)	/ 0121 ffffff01
	decl	0x3e8	/ 0122 ffffff01
	divb	%dl, %al	/ 0020 fffff606
	divb	0x3e8	/ 0020 fffff606
	divw	(%ecx, %ecx, 0), %ax	/ 0021 fffff706
	divw	0x3e8	/ 0021 fffff706
	divl	(%esi, %ecx, 1), %eax	/ 0022 fffff706
	divl	0x3e8	/ 0022 fffff706
	fadds	0x3e8	/ 2020 ffffd800
	faddl	(%si)	/ 2020 ffffdc00
	fadd	%st(7), %st	/ 2010 ffffd8c0
	fadd	%st(6)	/ 2010 ffffd8c0
	fadd		/ 2000 ffffdec1
	fadd	%st, %st(6)	/ 2010 ffffdcc0
	faddp	%st, %st(0)	/ 2010 ffffdec0
	faddp	%st(5)	/ 2010 ffffdec0
	faddp		/ 2000 ffffdec1
	fcoms	(%eax)	/ 2020 ffffd802
	fcoml	0x3e8(%bp, %si)	/ 2020 ffffdc02
	fcom	%st(7), %st	/ 2010 ffffd8d0
	fcom	%st(6)	/ 2010 ffffd8d0
	fcom		/ 2000 ffffd8d1
	fcomps	0x3e8(%bx, %di)	/ 2020 ffffd803
	fcompl	0x3e8	/ 2020 ffffdc03
	fcomp	%st(7)	/ 2010 ffffd8d8
	fcomp		/ 2000 ffffd8d9
	fdivs	(%di)	/ 2020 ffffd806
	fdivl	(%si)	/ 2020 ffffdc06
	fdiv	%st, %st	/ 2010 ffffd8f0
	fdiv	%st(3)	/ 2010 ffffd8f0
	fdiv		/ 2000 ffffdef1
	fdiv	%st, %st(3)	/ 2010 ffffdcf0
	fdivp	%st, %st	/ 2010 ffffdef0
	fdivp	%st(0)	/ 2010 ffffdef0
	fdivp		/ 2000 ffffdef1
	fdivrs	0x3e8(%edx, %edx, 0)	/ 2020 ffffd807
	fdivrl	0x64(%bp)	/ 2020 ffffdc07
	fdivr	%st(1), %st	/ 2010 ffffd8f8
	fdivr	%st(4)	/ 2010 ffffd8f8
	fdivr		/ 2000 ffffdef9
	fdivr	%st, %st(4)	/ 2010 ffffdcf8
	fdivrp	%st, %st(5)	/ 2010 ffffdef8
	fdivrp	%st(7)	/ 2010 ffffdef8
	fdivrp		/ 2000 ffffdef9
	fiaddl	0x3e8	/ 2020 ffffda00
	fiadds	0x3e8(%bp, %di)	/ 2020 ffffde00
	ficoml	0x3e8(%bp, %si)	/ 2020 ffffda02
	ficoms	0x64(%ebp, 2)	/ 2020 ffffde02
	ficompl	0x3e8	/ 2020 ffffda03
	ficomps	0x3e8	/ 2020 ffffde03
	fidivl	(%bx, %di)	/ 2020 ffffda06
	fidivs	0x3e8(%bx, %di)	/ 2020 ffffde06
	fidivrl	(%di)	/ 2020 ffffda07
	fidivrs	0x64(%ecx)	/ 2020 ffffde07
	fildl	0x3e8	/ 2020 ffffdb00
	filds	0x186a0(%eax)	/ 2020 ffffdf00
	fildll	0x3e8(%bp)	/ 2020 ffffdf05
	fimull	(%bp, %si)	/ 2020 ffffda01
	fimuls	(%edx)	/ 2020 ffffde01
	fistl	0x3e8	/ 2020 ffffdb02
	fists	0x3e8	/ 2020 ffffdf02
	fistpl	0x3e8	/ 2020 ffffdb03
	fistps	0x64(%bx)	/ 2020 ffffdf03
	fistpll	(%esi, %ecx)	/ 2020 ffffdf07
	fisubl	0x3e8(%bx, %si)	/ 2020 ffffda04
	fisubs	(%esi)	/ 2020 ffffde04
	fisubrl	0x3e8	/ 2020 ffffda05
	fisubrs	0x3e8(%ebp, %ecx, 4)	/ 2020 ffffde05
	fld	%st(5)	/ 2010 ffffd9c0
	flds	0x3e8	/ 2020 ffffd900
	fldl	0x4(%eax, %edx)	/ 2020 ffffdd00
	fldt	0x64(%di)	/ 2020 ffffdb05
	fmuls	(%edx)	/ 2020 ffffd801
	fmull	0x3e8(%bx, %di)	/ 2020 ffffdc01
	fmul	%st(3), %st	/ 2010 ffffd8c8
	fmul	%st(6)	/ 2010 ffffd8c8
	fmul		/ 2000 ffffdec9
	fmul	%st, %st(4)	/ 2010 ffffdcc8
	fmulp	%st, %st(5)	/ 2010 ffffdec8
	fmulp	%st(6)	/ 2010 ffffdec8
	fmulp		/ 2000 ffffdec9
	fst	%st(4)	/ 2010 ffffddd0
	fsts	(%bx, %di)	/ 2020 ffffd902
	fstl	0x4(%eax, %edi)	/ 2020 ffffdd02
	fstp	%st(5)	/ 2010 ffffddd8
	fstps	(%bp, %si)	/ 2020 ffffd903
	fstpl	0x186a0(%ebp)	/ 2020 ffffdd03
	fstpt	0x3e8(%bp, %si)	/ 2020 ffffdb07
	fnstsw	0x64(%bx, %di)	/ 2020 ffffdd07
	fnstsw	%ax	/ 2000 ffffdfe0
	fstsw	(%edx, %esi, 4)	/ 2024 ffffdd07
	fstsw	%ax	/ 2004 ffffdfe0
	fsubs	0x64(%ebx)	/ 2020 ffffd804
	fsubl	0x186a0(%ebx)	/ 2020 ffffdc04
	fsub	%st(3), %st	/ 2010 ffffd8e0
	fsub	%st(1)	/ 2010 ffffd8e0
	fsub		/ 2000 ffffdee1
	fsub	%st, %st(6)	/ 2010 ffffdce0
	fsubp	%st, %st(7)	/ 2010 ffffdee0
	fsubp	%st	/ 2010 ffffdee0
	fsubp		/ 2000 ffffdee1
	fsubrs	(%ebx, 1)	/ 2020 ffffd805
	fsubrl	(%ebp, %ebx)	/ 2020 ffffdc05
	fsubr	%st(5), %st	/ 2010 ffffd8e8
	fsubr	%st	/ 2010 ffffd8e8
	fsubr		/ 2000 ffffdee9
	fsubr	%st, %st(5)	/ 2010 ffffdce8
	fsubrp	%st, %st(2)	/ 2010 ffffdee8
	fsubrp	%st(0)	/ 2010 ffffdee8
	fsubrp		/ 2000 ffffdee9
	fucom	%st, %st	/ 2010 ffffdde0
	fucom	%st(7)	/ 2010 ffffdde0
	fucom		/ 2000 ffffdde1
	fucomp	%st, %st(7)	/ 2010 ffffdde8
	fucomp	%st(7)	/ 2010 ffffdde8
	fucomp		/ 2000 ffffdde9
	fxch	%st, %st(1)	/ 2010 ffffd9c8
	fxch	%st(4), %st	/ 2010 ffffd9c8
	fxch	%st(0)	/ 2010 ffffd9c8
	fxch		/ 2000 ffffd9c9
	idivb	0x3e8, %al	/ 0020 fffff607
	idivb	0x3e8	/ 0020 fffff607
	idivw	0x3e8, %ax	/ 0021 fffff707
	idivw	%si	/ 0021 fffff707
	idivl	%edx, %eax	/ 0022 fffff707
	idivl	0x4(%ebx, %esi)	/ 0022 fffff707
	imulb	0x3e8(%bx, %si)	/ 0020 fffff605
	imulw	0x3e8	/ 0021 fffff705
	imull	0x3e8(%ebp, %edi, 4)	/ 0022 fffff705
	imulw	0x3e8, %cx	/ 0009 00af
	imull	(%bx), %eax	/ 000a 00af
	imulw	$-5, (%bx), %di	/ 0001 006b
	imull	$-5, 0x64(%esi), %edx	/ 0002 006b
	imulw	$-5, %bp	/ 0041 006b
	imull	$064, %esi	/ 0042 006b
	imulw	$0x3e8, %bx, %ax	/ 0001 0069
	imull	$0x186a0, %esi, %ebx	/ 0002 0069
	imulw	$0x3e8, %sp	/ 0041 0069
	imull	$0x186a0, %ebp	/ 0042 0069
	inb	$0x64	/ 0000 00e4
	inw	$0x64	/ 0001 00e5
	inl	$0x64	/ 0002 00e5
	inb	(%dx)	/ 0000 00ec
	inw	(%dx)	/ 0001 00ed
	inl	(%dx)	/ 0002 00ed
	incw	%si	/ 0011 0040
	incl	%esp	/ 0012 0040
	incb	0x3e8	/ 0120 fffffe00
	incw	0x64(%edx, 1)	/ 0121 ffffff00
	incl	(%ecx, 8)	/ 0122 ffffff00
	larw	0x3e8, %sp	/ 0009 0002
	larl	(%ecx), %edi	/ 000a 0002
	leaw	0x3e8(%bx, %si), %sp	/ 0001 008d
	leal	0x4(%ebx, %ebx), %edi	/ 0002 008d
	lgdtw	(%esi, %edx, 2)	/ 0029 0102
	lgdtl	0x4(%ecx, %edx)	/ 002a 0102
	lidtw	0x3e8(%eax, %eax, 0)	/ 0029 0103
	lidtl	(%ebp, %ebx)	/ 002a 0103
	ldsw	(%edx, %eax, 4), %cx	/ 0001 00c5
	ldsl	0x3e8, %ebp	/ 0002 00c5
	lssw	0x3e8, %bx	/ 0009 00b2
	lssl	(%bx, %si), %esi	/ 000a 00b2
	lesw	0x64(%edx, 1), %ax	/ 0001 00c4
	lesl	(%di), %ebx	/ 0002 00c4
	lfsw	0x186a0(%ebp), %di	/ 0009 00b4
	lfsl	0x64(%bp, %di), %ebx	/ 000a 00b4
	lgsw	0x64(%ebp, 4), %di	/ 0009 00b5
	lgsl	0x3e8, %esi	/ 000a 00b5
	lslw	0x186a0(%eax), %sp	/ 0009 0003
	lsll	%edi, %edx	/ 000a 0003
	movb	abc, %al	/ 0000 00a0
	movw	abc, %ax	/ 0001 00a1
	movl	abc, %eax	/ 0002 00a1
	movb	%al, abc	/ 0000 00a2
	movw	%ax, abc	/ 0001 00a3
	movl	%eax, abc	/ 0002 00a3
	movb	0x64(%bx, %si), %cl	/ 0000 008a
	movw	0x64(%ecx, 0), %cx	/ 0001 008b
	movl	(%eax), %edx	/ 0002 008b
	movb	%bl, (%si)	/ 0000 0088
	movw	%bp, 0x3e8(%bp, %di)	/ 0001 0089
	movl	%edx, 0x186a0(%esi)	/ 0002 0089
	movw	%gs, %dx	/ 0000 008c
	movw	0x4(%edi, %ebp), %fs	/ 0000 008e
	movb	$0x64, %bl	/ 0010 00b0
	movw	$0x3e8, %bx	/ 0011 00b8
	movl	$0x186a0, %ecx	/ 0012 00b8
	movb	$0x64, 0x3e8	/ 0000 00c6
	movw	$0x3e8, 0x64(%bx)	/ 0001 00c7
	movl	$0x186a0, 0x3e8	/ 0002 00c7
	movl	%cr0, %ecx	/ 000a 0020
	movl	%edi, %cr2	/ 000a 0022
	movl	%dr3, %ecx	/ 000a 0021
	movl	%ecx, %dr3	/ 000a 0023
	movl	%tr6, %ebp	/ 000a 0024
	movl	%ecx, %tr6	/ 000a 0026
	movsxb	(%edi, %ebp), %si	/ 0009 00be
	movsxb	0x186a0(%edx), %ebx	/ 000a 00be
	movsxw	0x64(%bp, %si), %esi	/ 080a 00bf
	movsbw	%al, %cx	/ 0009 00be
	movsbl	0x3e8, %ebx	/ 000a 00be
	movswl	0x64(%di), %edx	/ 080a 00bf
	movzxb	0x4(%ebx, %ebp), %sp	/ 0009 00b6
	movzxb	%dh, %eax	/ 000a 00b6
	movzxw	0x64(%bp, %si), %eax	/ 080a 00b7
	movzbw	0x64(%bp, %di), %bp	/ 0009 00b6
	movzbl	(, %eax, 2), %edx	/ 000a 00b6
	movzwl	0x3e8, %edi	/ 080a 00b7
	mulb	0x3e8, %al	/ 0020 fffff604
	mulb	%dh	/ 0020 fffff604
	mulw	0x3e8, %ax	/ 0021 fffff704
	mulw	%ax	/ 0021 fffff704
	mull	0x64(%bx, %di), %eax	/ 0022 fffff704
	mull	0x3e8	/ 0022 fffff704
	negb	(%bp, %si)	/ 0120 fffff603
	negw	(%ebx, %edi, 1)	/ 0121 fffff703
	negl	(%edi)	/ 0122 fffff703
	notb	%ch	/ 0120 fffff602
	notw	0x64(%eax)	/ 0121 fffff702
	notl	%ebx	/ 0122 fffff702
	orl	$-5, 0x3e8	/ 0122 ffff8301
	orw	$-5, 0x3e8(%di)	/ 0121 ffff8301
	orb	$0x64, %al	/ 0100 000c
	orw	$0x3e8, %ax	/ 0101 000d
	orl	$0x186a0, %eax	/ 0102 000d
	orl	$0x186a0	/ 0102 000d
	orb	$0x64, 0x3e8(%edx, %ebp, 4)	/ 0120 ffff8001
	orw	$0x3e8, (%ebp, %eax)	/ 0121 ffff8101
	orl	$0x186a0, 0x3e8	/ 0122 ffff8101
	orb	0x3e8, %bl	/ 0100 000a
	orw	%si, %di	/ 0101 000b
	orl	0x64(%di), %ebx	/ 0102 000b
	orb	%bh, (%bx, %si)	/ 0100 0008
	orw	%di, 0x3e8	/ 0101 0009
	orl	%edx, 0x3e8(%di)	/ 0102 0009
	outb	$0x64	/ 0000 00e6
	outw	$0x64	/ 0001 00e7
	outl	$0x64	/ 0002 00e7
	outb	(%dx)	/ 0000 00ee
	outw	(%dx)	/ 0001 00ef
	outl	(%dx)	/ 0002 00ef
	popw	%bp	/ 0011 0058
	popl	%edx	/ 0012 0058
	popw	%ds	/ 0000 001f
	popw	%es	/ 0000 0007
	popw	%ss	/ 0000 0017
	popw	%fs	/ 0008 00a1
	popw	%gs	/ 0008 00a9
	popw	0x3e8	/ 0021 ffff8f00
	popl	(, %eax, 8)	/ 0022 ffff8f00
	popaw		/ 0001 0061
	popal		/ 0002 0061
	popfw		/ 0001 009d
	popfl		/ 0002 009d
	pushw	%bp	/ 0011 0050
	pushl	%eax	/ 0012 0050
	pushb	$-5	/ 0000 006a
	pushw	$0x3e8	/ 0001 0068
	pushl	$0x186a0	/ 0002 0068
	pushw	%cs	/ 0000 000e
	pushw	%ds	/ 0000 001e
	pushw	%es	/ 0000 0006
	pushw	%ss	/ 0000 0016
	pushw	%fs	/ 0008 00a0
	pushw	%gs	/ 0008 00a8
	pushw	(, %edx, 8)	/ 0021 ffffff06
	pushl	0x64(%esi)	/ 0022 ffffff06
	pushaw		/ 0001 0060
	pushal		/ 0002 0060
	pushfw		/ 0001 009c
	pushfl		/ 0002 009c
	rclb	$1, 0x64(%si)	/ 0020 ffffd002
	rclb	%dl	/ 0020 ffffd002
	rclb	%cl, (%edx)	/ 0020 ffffd202
	rclb	$0x64, 0x64(%bp)	/ 0020 ffffc002
	rclw	$1, %ax	/ 0021 ffffd102
	rclw	(%ebp, %ebx, 0)	/ 0021 ffffd102
	rclw	%cl, 0x64(%si)	/ 0021 ffffd302
	rclw	$0x64, 0x3e8(%bx, %di)	/ 0021 ffffc102
	rcll	$1, 0x3e8	/ 0022 ffffd102
	rcll	%esi	/ 0022 ffffd102
	rcll	%cl, %esp	/ 0022 ffffd302
	rcll	$0x64, (, %ebp, 8)	/ 0022 ffffc102
	rcrb	$1, (%ebp, 0)	/ 0020 ffffd003
	rcrb	%cl	/ 0020 ffffd003
	rcrb	%cl, (%bx, %si)	/ 0020 ffffd203
	rcrb	$0x64, (%bx)	/ 0020 ffffc003
	rcrw	$1, (%bx)	/ 0021 ffffd103
	rcrw	(%bp, %di)	/ 0021 ffffd103
	rcrw	%cl, 0x64(%si)	/ 0021 ffffd303
	rcrw	$0x64, (%edi, 2)	/ 0021 ffffc103
	rcrl	$1, %ebp	/ 0022 ffffd103
	rcrl	(%si)	/ 0022 ffffd103
	rcrl	%cl, 0x64(%bp, %si)	/ 0022 ffffd303
	rcrl	$0x64, %esp	/ 0022 ffffc103
	rolb	$1, 0x3e8(%bx)	/ 0020 ffffd000
	rolb	%dh	/ 0020 ffffd000
	rolb	%cl, %ah	/ 0020 ffffd200
	rolb	$0x64, (%ebp, 8)	/ 0020 ffffc000
	rolw	$1, 0x64(%di)	/ 0021 ffffd100
	rolw	0x64(%bx, %si)	/ 0021 ffffd100
	rolw	%cl, 0x3e8	/ 0021 ffffd300
	rolw	$0x64, (%bx, %si)	/ 0021 ffffc100
	roll	$1, (, %eax, 1)	/ 0022 ffffd100
	roll	0x3e8	/ 0022 ffffd100
	roll	%cl, 0x3e8	/ 0022 ffffd300
	roll	$0x64, (%bp, %di)	/ 0022 ffffc100
	rorb	$1, (%ecx)	/ 0020 ffffd001
	rorb	%dh	/ 0020 ffffd001
	rorb	%cl, 0x3e8	/ 0020 ffffd201
	rorb	$0x64, 0x3e8	/ 0020 ffffc001
	rorw	$1, 0x3e8(%bx, %si)	/ 0021 ffffd101
	rorw	0x64(%ecx)	/ 0021 ffffd101
	rorw	%cl, 0x3e8	/ 0021 ffffd301
	rorw	$0x64, %di	/ 0021 ffffc101
	rorl	$1, 0x64(%si)	/ 0022 ffffd101
	rorl	0x3e8	/ 0022 ffffd101
	rorl	%cl, 0x3e8	/ 0022 ffffd301
	rorl	$0x64, (%bp, %si)	/ 0022 ffffc101
	salb	$1, 0x64(%bx, %di)	/ 0020 ffffd004
	salb	0x3e8	/ 0020 ffffd004
	salb	%cl, %cl	/ 0020 ffffd204
	salb	$0x64, (%eax, %ecx)	/ 0020 ffffc004
	salw	$1, 0x3e8	/ 0021 ffffd104
	salw	0x3e8(%bx)	/ 0021 ffffd104
	salw	%cl, 0x64(%di)	/ 0021 ffffd304
	salw	$0x64, %cx	/ 0021 ffffc104
	sall	$1, 0x3e8	/ 0022 ffffd104
	sall	%ebx	/ 0022 ffffd104
	sall	%cl, %esi	/ 0022 ffffd304
	sall	$0x64, %ecx	/ 0022 ffffc104
	shlb	$1, 0x64(%bp, %di)	/ 0020 ffffd004
	shlb	%ch	/ 0020 ffffd004
	shlb	%cl, 0x186a0(%ebp)	/ 0020 ffffd204
	shlb	$0x64, 0x3e8(%bx, %si)	/ 0020 ffffc004
	shlw	$1, %sp	/ 0021 ffffd104
	shlw	%di	/ 0021 ffffd104
	shlw	%cl, 0x3e8(%bx)	/ 0021 ffffd304
	shlw	$0x64, 0x3e8	/ 0021 ffffc104
	shll	$1, %edx	/ 0022 ffffd104
	shll	0x64(%bp, %si)	/ 0022 ffffd104
	shll	%cl, %ebx	/ 0022 ffffd304
	shll	$0x64, %ecx	/ 0022 ffffc104
	sarb	$1, %bh	/ 0020 ffffd007
	sarb	0x3e8(%ebx, %esi, 0)	/ 0020 ffffd007
	sarb	%cl, (%edx)	/ 0020 ffffd207
	sarb	$0x64, 0x3e8	/ 0020 ffffc007
	sarw	$1, 0x4(%edi, %eax)	/ 0021 ffffd107
	sarw	0x3e8	/ 0021 ffffd107
	sarw	%cl, (%edx, 2)	/ 0021 ffffd307
	sarw	$0x64, %bx	/ 0021 ffffc107
	sarl	$1, (, %edi, 1)	/ 0022 ffffd107
	sarl	0x3e8(%bx, %di)	/ 0022 ffffd107
	sarl	%cl, (%ebx, 4)	/ 0022 ffffd307
	sarl	$0x64, %ebp	/ 0022 ffffc107
	shrb	$1, 0x3e8	/ 0020 ffffd005
	shrb	%ch	/ 0020 ffffd005
	shrb	%cl, %cl	/ 0020 ffffd205
	shrb	$0x64, 0x3e8	/ 0020 ffffc005
	shrw	$1, 0x64(%edi)	/ 0021 ffffd105
	shrw	0x3e8	/ 0021 ffffd105
	shrw	%cl, %di	/ 0021 ffffd305
	shrw	$0x64, %bp	/ 0021 ffffc105
	shrl	$1, %edx	/ 0022 ffffd105
	shrl	0x64(%bp, %di)	/ 0022 ffffd105
	shrl	%cl, 0x3e8	/ 0022 ffffd305
	shrl	$0x64, 0x3e8	/ 0022 ffffc105
	sbbl	$-5, %esp	/ 0122 ffff8303
	sbbw	$064, 0x3e8	/ 0121 ffff8303
	sbbb	$0x64, %al	/ 0100 001c
	sbbw	$0x3e8, %ax	/ 0101 001d
	sbbl	$0x186a0, %eax	/ 0102 001d
	sbbl	$0x186a0	/ 0102 001d
	sbbb	$0x64, %ch	/ 0120 ffff8003
	sbbw	$0x3e8, %cx	/ 0121 ffff8103
	sbbl	$0x186a0, 0x3e8	/ 0122 ffff8103
	sbbb	0x3e8(%bx, %si), %dl	/ 0100 001a
	sbbw	%ax, %bp	/ 0101 001b
	sbbl	0x3e8(%bx), %esp	/ 0102 001b
	sbbb	%ah, 0x64(%ecx)	/ 0100 0018
	sbbw	%si, 0x186a0(%ebp)	/ 0101 0019
	sbbl	%edx, 0x3e8(%edi, %esi, 0)	/ 0102 0019
	shldw	$0x64, %dx, 0x64(%bx, %si)	/ 0009 00a4
	shldl	$0x64, %eax, 0x3e8	/ 000a 00a4
	shldw	%cl, %cx, %cx	/ 0009 00a5
	shldl	%cl, %esp, %ecx	/ 000a 00a5
	shrdw	$0x64, %bx, 0x3e8	/ 0009 00ac
	shrdl	$0x64, %edx, 0x3e8	/ 000a 00ac
	shrdw	%cl, %sp, 0x3e8	/ 0009 00ad
	shrdl	%cl, %ebx, %edi	/ 000a 00ad
	shrdw	%ax, 0x3e8(%ebx, %edi, 2)	/ 0009 00ad
	shrdl	%ebp, 0x64(%esi)	/ 000a 00ad
	subl	$064, %ecx	/ 0122 ffff8305
	subw	$064, %di	/ 0121 ffff8305
	subb	$0x64, %al	/ 0100 002c
	subw	$0x3e8, %ax	/ 0101 002d
	subl	$0x186a0, %eax	/ 0102 002d
	subl	$0x186a0	/ 0102 002d
	subb	$0x64, %al	/ 0120 ffff8005
	subw	$0x3e8, (%ebp, %ebx)	/ 0121 ffff8105
	subl	$0x186a0, 0x3e8	/ 0122 ffff8105
	subb	0x64(%esi), %dh	/ 0100 002a
	subw	0x3e8, %cx	/ 0101 002b
	subl	0x3e8, %esp	/ 0102 002b
	subb	%bl, %cl	/ 0100 0028
	subw	%dx, (%edx, %esi)	/ 0101 0029
	subl	%ebx, (%bp, %di)	/ 0102 0029
	testb	$0x64, %al	/ 0000 00a8
	testw	$0x3e8, %ax	/ 0001 00a9
	testl	$0x186a0, %eax	/ 0002 00a9
	testl	$0x186a0	/ 0002 00a9
	testb	$0x64, (%si)	/ 0020 fffff600
	testw	$0x3e8, 0x64(%edx)	/ 0021 fffff700
	testl	$0x186a0, 0x3e8	/ 0022 fffff700
	testb	%dl, 0x3e8	/ 0000 0084
	testw	%si, 0x64(%bp, %si)	/ 0001 0085
	testl	%edx, %edx	/ 0002 0085
	xchgw	%bp, %ax	/ 0011 0090
	xchgw	%ax, %ax	/ 0011 0090
	xchgl	%eax, %eax	/ 0012 0090
	xchgl	%esp	/ 0012 0090
	xchgl	%eax, %esp	/ 0012 0090
	xchgb	0x64(%esi), %dl	/ 0100 0086
	xchgw	0x3e8, %bp	/ 0101 0087
	xchgl	0x64(%esi), %ebp	/ 0102 0087
	xchgb	%dl, (, %ecx, 0)	/ 0100 0086
	xchgw	%bp, 0x3e8	/ 0101 0087
	xchgl	%ebx, 0x3e8(%bp)	/ 0102 0087
	xorl	$-5, (%di)	/ 0122 ffff8306
	xorw	$-5, %sp	/ 0121 ffff8306
	xorb	$0x64, %al	/ 0100 0034
	xorw	$0x3e8, %ax	/ 0101 0035
	xorl	$0x186a0, %eax	/ 0102 0035
	xorl	$0x186a0	/ 0102 0035
	xorb	$0x64, 0x3e8	/ 0120 ffff8006
	xorw	$0x3e8, 0x3e8	/ 0121 ffff8106
	xorl	$0x186a0, 0x64(%di)	/ 0122 ffff8106
	xorb	0x186a0(%ebp), %bl	/ 0100 0032
	xorw	0x3e8(%di), %bx	/ 0101 0033
	xorl	(%esi), %esi	/ 0102 0033
	xorb	%ah, (%bp, %di)	/ 0100 0030
	xorw	%ax, 0x64(%di)	/ 0101 0031
	xorl	%ebx, 0x3e8	/ 0102 0031
