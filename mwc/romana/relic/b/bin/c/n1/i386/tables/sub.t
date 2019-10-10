//////////
/ n1/i386/tables/sub.t
//////////

/////////
/
/ Subtraction (-).
/ OP0 is ZSUB, OP1 is ZDEC.
/ Identical to add.t except for the _drsub call and pointer-pointer,
/ I would like them to be one file again but don't see how to do it.
/
/////////

SUB:

/ Dword op 1.
%	PEFFECT|PVALUE|PSREL|P_SLT
	DWORD		ANYR	ANYR	*	TEMP
		TREG		DWORD
		1|MMX		*
			[OP1]	[R]
		[IFR]	[REL0]	[LAB]

/ Pointer - pointer.
/ This should not get executed for addition.
/ Only RVALUE contexts need to be done.
%	PVALUE
	LONG		ANYR	*	*	TEMP
		DIR|IMM|MMX	PTX
		DIR|IMM|MMX	PTX
			[ZMOV]	[R],[AL]
			[OP0]	[R],[AR]

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
/ Double - double.
%	PEFFECT|PVALUE|P_SLT|PDECVAX|PIEEE
	FF64		EDXEAX	EDXEAX	*	TEMP
		TREG		FF64
		ADR|IMM		FF64
			[ZPUSH]	[HI AR]
			[ZPUSH]	[LO AR]
			[ZCALL]	[GID _drsub]
			[ZADD]	[REGNO ESP],[CONST 8]

/ Hardware coprocessor (NDP) floating point.
%	PVALUE|P_SLT|PNDP
	FLOAT		FPAC	FPAC	*	FPAC
		TREG		FF64
		ADR		NDPARG
			[TR ZFSUBD] [AR]

//////////
/ end of n1/i386/tables/sub.t
//////////
