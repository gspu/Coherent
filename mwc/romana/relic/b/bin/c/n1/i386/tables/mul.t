//////////
/ n1/i386/tables/mul.t
//////////

/////////
/
/ Multiplication (*).
/ Immediate byte operands use the three-operand multiply immediate form.
/ Otherwise, the multiply is done in EDX:EAX.
/ The multiply instruction does not set any flags.
/ Signed multiply should use the new i386 two-operand MUL
/ but currently does not.
/
/////////

MUL:

/ Long * byte constant.
%	PEFFECT|PVALUE
	DWORD		ANYR	*	*	TEMP
		ADR		LONG
		SBYTE|MMX	LONG
			[ZIMULI]	[R],[AL],[AR]

/ Long * long.
%	PEFFECT|PVALUE
	DWORD		EDXEAX	EAX	*	EAX
		TREG		LONG
		ADR		LONG
			[TL ZIMUL]	[AR]

//////////
/ Floating point.
//////////

/ DECVAX or IEEE software floating point.
/ Double * double.
%	PEFFECT|PVALUE|P_SLT|PDECVAX|PIEEE
	FF64		EDXEAX	EDXEAX	*	TEMP
		TREG		FF64
		ADR|IMM		FF64
			[ZPUSH]	[HI AR]
			[ZPUSH]	[LO AR]
			[ZCALL]	[GID _dmul]
			[ZADD]	[REGNO ESP],[CONST 8]

/ Hardware coprocessor (NDP) floating point.
%	PVALUE|P_SLT|PNDP
	FLOAT		FPAC	FPAC	*	FPAC
		TREG		FF64
		ADR		NDPARG
			[TR ZFMULD] [AR]

//////////
/ end of n1/i386/tables/mul.t
//////////
