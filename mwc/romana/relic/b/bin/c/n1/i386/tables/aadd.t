//////////
/ n1/i386/tables/aadd.t
//////////

/////////
/
/ Assigned add (+=).
/ [OP0] is ZADD, [OP1] is ZINC.
/ Identical to asub.t except for the _dadd and _fadd calls,
/ I would like them to be one file again but don't see how to do it.
/
/////////

AADD:

/ Integer or pointer op= 1.
%	PEFFECT|PVALUE|PREL
	DWORD		ANYR	*	*	TEMP
		ADR|LV		DWORD|SHORT
		1|MMX		*
			[TL OP1]	[AL]
		[IFV]	[TL ZMOVSX]	[R],[AL]
		[IFR]	[REL0]		[LAB]

/ Integer or pointer op= immediate long.
%	PEFFECT|PVALUE|PREL
	DWORD		ANYR	*	*	TEMP
		ADR|LV		DWORD|SHORT
		IMM|MMX		LONG
/ Dword op= register long.
%	PEFFECT|PVALUE|PREL
	DWORD		ANYR	*	*	TEMP
		ADR|LV		DWORD
		REG|MMX		DWORD
			[TL OP0]	[AL],[AR]
		[IFV]	[TL ZMOVSX]	[R],[AL]
		[IFR]	[REL0]		[LAB]

/ Integer or pointer op= long.
%	PEFFECT|PVALUE|PREL|P_SRT|PBYTE
	DWORD		ANYR	*	ANYR	TEMP
		ADR|LV		DWORD|SHORT
		TREG		DWORD
			[TL OP0]	[AL],[TL R]
		[IFV]	[TL ZMOVSX]	[R],[AL]
		[IFR]	[REL0]		[LAB]

/////////
/ Fields.
/ The left hand operand is an lvalue bit field.
/ The right operand has been preshifted to the appropriate place.
/ If the value is required, there is a shift on top of the 'op=' node.
/ Grab the machine object that holds the field,
/ add the right operand to it,
/ extract the result and replace the value in the machine object.
/////////

%	PVALUE|PBYTE
	LONG		ANYR	*	*	TEMP
		ADR|LV		FLD
		ADR|IMM		LONG
			[TL ZMOVZX]	[R],[AL]		/ fetch
			[OP0]		[R],[AR]		/ op
			[ZAND]		[R],[EMASK]		/ extract
			[TL ZAND]	[AL],[TL CMASK]		/ clear
			[TL ZOR]	[AL],[TL R]		/ store

//////////
/ Floating point.
//////////

/ IEEE or DECVAX software floating point.
/ Double op= double.
%	PEFFECT|PVALUE|P_SRT|PDECVAX|PIEEE
	FF64		EDXEAX	*	EDXEAX	TEMP
		ADR|LV		FF64
		TREG		FF64
			[ZPUSH]	[HI AL]
			[ZPUSH]	[LO AL]
			[ZCALL]	[GID _dadd]
			[ZADD]	[REGNO ESP],[CONST 8]
			[ZMOV]	[HI AL],[HI R]
			[ZMOV]	[LO AL],[LO R]

/ Float op= double.
%	PEFFECT|PVALUE|P_SRT|PDECVAX|PIEEE
	FF64		EDXEAX	*	EDXEAX	TEMP
		ADR|LV		FF32
		TREG		FF64
			[ZPUSH]	[AL]
			[ZCALL]	[GID _fadd]
			[ZADD]	[REGNO ESP],[CONST 4]
			[ZCALL]	[GID _fdcvt]
			[ZMOV]	[AL], [LO R]
		[IFV]	[ZCALL]	[GID _dfcvt]

/ Hardware coprocessor (NDP) floating point.
%	PEFFECT|PVALUE|PNDP
	FLOAT		FPAC	*	FPAC	FPAC
		ADR|LV		FLOAT
		TREG		FF64
			[TL ZFADDD]	[AL]
		[IFV]	[TL ZFSTD]	[AL]
		[IFE]	[TL ZFSTPD]	[AL]

//////////
/ end of n1/i386/tables/aadd.t
//////////
