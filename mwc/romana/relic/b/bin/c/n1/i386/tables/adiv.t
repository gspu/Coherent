//////////
/ n1/i386/tables/adiv.t
//////////

/////////
/
/ Assigned divide (/=).
/ Use unsigned divide if either left or right operand is unsigned.
/
/////////

ADIV:

/ Unsigned integer by unsigned.
/ The result word or byte does not have to be sign- or zero-extended
/ in value context, the result is already correct.
%	PEFFECT|PVALUE
	LONG		EDXEAX	*	*	EAX
		ADR|LV		UINT
		ADR		LONG
/ Signed long by unsigned.
%	PEFFECT|PVALUE
	LONG		EDXEAX	*	*	EAX
		ADR|LV		FS32
		ADR		FU32
			[TL ZMOVSX]	[REGNO EAX], [AL]
			[ZSUB]		[REGNO EDX], [REGNO EDX]
			[ZDIV]		[AR]
			[TL ZMOV]	[AL], [TL REGNO EAX]

/ Signed word or byte by unsigned.
/ The result must be sign- or zero-extended in value context.
%	PEFFECT|PVALUE
	LONG		EDXEAX	*	*	EAX
		ADR|LV		FS16|FS8
		ADR		FU32
			[TL ZMOVSX]	[REGNO EAX], [AL]
			[ZSUB]		[REGNO EDX], [REGNO EDX]
			[ZDIV]		[AR]
			[TL ZMOV]	[AL], [TL REGNO EAX]
		[IFV]	[TL ZMOVSX]	[REGNO EAX], [TL REGNO EAX]

/ Signed integer by signed.
/ The result word or byte does not have to be sign- or zero-extended
/ in value context, the result is already correct.
%	PEFFECT|PVALUE
	LONG		EDXEAX	*	*	EAX
		ADR|LV		SINT
		ADR		FS32
			[TL ZMOVSX]	[REGNO EAX], [AL]
			[ZCDQ]
			[ZIDIV]		[AR]
			[TL ZMOV]	[AL], [TL REGNO EAX]

//////////
/ Fields.
/ The right hand operand of the field is not preshifted
/ like it is for add, subtract and the increment/decrement ops. 
//////////

/ Field.
%	PEFFECT|PVALUE
	LONG		EDXEAX	*	*	EAX
		ADR|LV		FLD
		ADR		LONG
			[TL ZMOVZX]	[REGNO EAX],[AL]	/ fetch
			[ZAND]		[REGNO EAX],[EMASK]	/ extract
			[ZSUB]		[REGNO EDX],[REGNO EDX]	/ widen
			[ZDIV]		[AR]			/ divide
			[ZAND]		[REGNO EAX],[EMASK]	/ mask
			[TL ZAND]	[AL],[TL CMASK]		/ clear
			[TL ZOR]	[AL],[TL REGNO EAX]	/ store

//////////
/ Floating point.
//////////

/ IEEE or DECVAX software floating point.
/ Double /= double.
%	PEFFECT|PVALUE|P_SRT|PDECVAX|PIEEE
	FF64		EDXEAX	*	EDXEAX	TEMP
		ADR|LV		FF64
		TREG		FF64
			[ZPUSH]	[HI AL]
			[ZPUSH]	[LO AL]
			[ZCALL]	[GID _ddiv]
			[ZADD]	[REGNO ESP],[CONST 8]
			[ZMOV]	[HI AL],[HI R]
			[ZMOV]	[LO AL],[LO R]

/ Float /= double.
%	PEFFECT|PVALUE|P_SRT|PDECVAX|PIEEE
	FF64		EDXEAX	*	EDXEAX	TEMP
		ADR|LV		FF32
		TREG		FF64
			[ZPUSH]	[AL]
			[ZCALL]	[GID _fdiv]
			[ZADD]	[REGNO ESP],[CONST 4]
			[ZCALL]	[GID _fdcvt]
			[ZMOV]	[AL], [LO R]
		[IFV]	[ZCALL]	[GID _dfcvt]

/ Hardware coprocessor (NDP) floating point.
%	PEFFECT|PVALUE|PNDP
	FLOAT		FPAC	*	FPAC	FPAC
		ADR|LV		FLOAT
		TREG		FF64
			[TL ZFRDIVD]	[AL]
		[IFV]	[TL ZFSTD]	[AL]
		[IFE]	[TL ZFSTPD]	[AL]

//////////
/ end of n1/i386/tables/adiv.t
//////////
