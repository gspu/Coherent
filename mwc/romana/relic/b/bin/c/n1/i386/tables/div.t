//////////
/ n1/i386/tables/div.t
//////////

/////////
/
/ Division (/).
/ Should use unsigned divide when left operand is signed and right is unsigned,
/ but the parser generates the appropriate conversions to make it unnecessary.
/ The dividend is zero or sign extended.
/ No flags are set.
/
/////////

DIV:

/ Signed integer.
%	PEFFECT|PVALUE
	DWORD		EDXEAX	EAX	*	EAX
		TREG		FS32
		ADR		LONG
			[ZCDQ]
			[ZIDIV]	[AR]

/ Unsigned integer.
%	PEFFECT|PVALUE
	DWORD		EDXEAX	EAX	*	EAX
		TREG		FU32
		ADR		LONG
			[ZSUB]	[REGNO EDX],[REGNO EDX]
			[ZDIV]	[AR]

//////////
/ Floating point.
//////////

/ DECVAX or IEEE software floating point.
/ Double / double.
%	PEFFECT|PVALUE|P_SLT|PDECVAX|PIEEE
	FF64		EDXEAX	EDXEAX	*	TEMP
		TREG		FF64
		ADR|IMM		FF64
			[ZPUSH]	[HI AR]
			[ZPUSH]	[LO AR]
			[ZCALL]	[GID _drdiv]
			[ZADD]	[REGNO ESP],[CONST 8]

/ Hardware coprocessor (NDP) floating point.
%	PVALUE|P_SLT|PNDP
	FLOAT		FPAC	FPAC	*	FPAC
		TREG		FF64
		ADR		NDPARG
			[TR ZFDIVD] [AR]

//////////
/ end of n1/i386/tables/div.t
//////////
