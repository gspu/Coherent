//////////
/ n1/i386/tables/add.t
//////////

/////////
/
/ Addition (+).
/ OP0 is ZADD, OP1 is ZINC.
/ Identical to sub.t except for the _dadd call,
/ I would like them to be one file again but don't see how to do it.
/
/////////

ADD:

/ Dword op 1.
%	PEFFECT|PVALUE|PSREL|P_SLT
	DWORD		ANYR	ANYR	*	TEMP
		TREG		DWORD
		1|MMX		*
			[OP1]	[R]
		[IFR]	[REL0]	[LAB]

/ Dword op dword.
%	PEFFECT|PVALUE|PSREL|P_SLT
	DWORD		ANYR	ANYR	*	TEMP
		TREG		DWORD
		ADR|IMM		DWORD
			[OP0]	[R],[AR]
		[IFR]	[REL0]	[LAB]

//////////
/ Floating point.
//////////

/ DECVAX or IEEE software floating point.
/ Double + double.
%	PEFFECT|PVALUE|P_SLT|PDECVAX|PIEEE
	FF64		EDXEAX	EDXEAX	*	TEMP
		TREG		FF64
		ADR|IMM		FF64
			[ZPUSH]	[HI AR]
			[ZPUSH]	[LO AR]
			[ZCALL]	[GID _dadd]
			[ZADD]	[REGNO ESP],[CONST 8]

/ Hardware coprocessor (NDP) floating point.
%	PVALUE|P_SLT|PNDP
	FLOAT		FPAC	FPAC	*	FPAC
		TREG		FF64
		ADR		NDPARG
			[TR ZFADDD] [AR]

//////////
/ end of n1/i386/tables/add.t
//////////
