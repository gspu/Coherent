//////////
/ n1/i386/tables/neg.t
//////////

/////////
/
/ Negation, a.k.a. unary minus, a.k.a. two's complement (-@).
/
/////////

NEG:

/ Longs.
%	PEFFECT|PVALUE|PGE|PLT|P_SLT
	LONG		ANYR	ANYR	*	TEMP
		TREG		LONG
		*		*
			[ZNEG]	[R]
		[IFR]	[REL1]	[LAB]

//////////
/ Floating point.
//////////

/ DECVAX or IEEE software floating point.
/ Twiddle the sign bit directly.
/ N.B. this changes 0.0 to -0.0.
%	PEFFECT|PVALUE|P_SLT|PIEEE|PDECVAX
	FF64		EDXEAX	EDXEAX	*	EDXEAX
		TREG		FF64
		*		*
			[ZXOR]	[REGNO EDX],[CONST 0x80000000]

/ Hardware coprocessor (NDP) floating point.
%	PVALUE|P_SLT|PNDP
	FF64		FPAC	FPAC	*	FPAC
		TREG		FF64
		*		*
			[ZFCHS]

//////////
/ end of n1/i386/tables/neg.t
//////////
