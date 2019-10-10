//////////
/ n1/i386/tables/assign.t
//////////

/////////
/
/ Assignment (=).
/ Longs, pointers, words, bytes, fields, doubles.
/ The '=' operator is a non-conversion operator for integer types and pointers.
/ There must be some special entries here to handle the cross type cases.
/
/////////

ASSIGN:

//////////
/
/ Simple integer assignment.
/
//////////

//////////
/ Longs and pointers.
//////////

/ Register dword = 0 or NULL.
%	PEFFECT
	DWORD		NONE	*	*	NONE
		REG|MMX		DWORD
		0|MMX		*
			[ZSUB]	[AL],[AL]

/ Register dword = dword.
%	PEFFECT
	DWORD		NONE	*	*	NONE
		REG|MMX		DWORD
		EASY|MMX	DWORD
/ Dword = register dword.
%	PEFFECT
	DWORD		NONE	*	*	NONE
		ADR|LV		DWORD
		REG|MMX		DWORD
			[ZMOV]	[AL],[AR]

/ Register dword = effective address.
%	PEFFECT
	DWORD		NONE	*	*	NONE
		REG|MMX		DWORD
		LEA|MMX		DWORD
			[ZLEA]	[AL],[NSE AR]

/ Integer or pointer = immediate.
%	PEFFECT
	DWORD		NONE	*	*	NONE
		ADR|LV		DWORD|SHORT
		IMM|MMX		DWORD
			[TL ZMOV]	[AL],[TL AR]

/ Dword = dword.
%	PEFFECT|PVALUE|P_SRT
	DWORD		ANYR	*	ANYR	TEMP
		ADR|LV		DWORD
		TREG		DWORD
			[ZMOV]	[AL],[AR]

//////////
/ Words.
//////////

/ Word = immediate word.
%	PEFFECT
	DWORD		NONE	*	*	NONE
		ADR|LV		WORD
		IMM|MMX		WORD
			[ZMOVW]	[AL],[AR]

/ Word = word.
%	PEFFECT|PVALUE|PREL|P_SRT
	LONG		ANYR	*	ANYR	TEMP
		ADR|LV		WORD
		TREG		WORD
		[IFV]	[TL ZMOVSX]	[R],[LO RR]
			[ZMOVW]		[AL],[LO R]
		[IFR]	[ZCMPW]		[LO R],[CONST 0]
		[IFR]	[REL0]		[LAB]

//////////
/ Bytes.
//////////

/ Byte = immediate byte.
%	PEFFECT
	LONG		NONE	*	*	NONE
		ADR|LV		BYTE
		IMM|MMX		BYTE
			[ZMOVB]	[AL], [AR]

/ Byte = byte.
%	PEFFECT|PVALUE|PREL|P_SRT|PBYTE
	LONG		ANYR	*	ANYR	TEMP
		ADR|LV		BYTE
		TREG		BYTE
		[IFV]	[TL ZMOVSX]	[R],[LO LO RR]
			[ZMOVB]		[AL],[LO LO R]
		[IFR]	[ZCMPB]		[LO LO R],[CONST 0]
		[IFR]	[REL0]		[LAB]

//////////
/
/ Mixed type assignment.
/
//////////

/ Signed dword = signed short.
%	PEFFECT|PVALUE|P_SRT|PBYTE
	DWORD		ANYR	*	ANYR	TEMP
		ADR|LV		FS32
		TREG		FS16|FS8
			[TR ZMOVSX]	[R],[TR RR]
			[ZMOV]		[AL],[R]

/ Dword = short.
%	PEFFECT|PVALUE|P_SRT|PBYTE
	DWORD		ANYR	*	ANYR	TEMP
		ADR|LV		DWORD
		TREG		SHORT
			[TR ZMOVZX]	[R],[TR RR]
			[ZMOV]		[AL],[R]

/ Short = long or pointer.
%	PEFFECT|PVALUE|P_SRT|PBYTE
	LONG		ANYR	*	ANYR	TEMP
		ADR|LV		SHORT
		TREG		DWORD
			[TL ZMOV]	[AL],[TL AR]
		[IFV]	[TL ZMOVSX]	[R], [TL AR]

/////////
/
/ Field assignment.
/ The right operand has been preshifted and masked,
/ just like the right side of an '|=' or '^=' operation.
/ All you do is clear the junk and OR it in.
/ In value context, unsigned result must be masked;
/ signed result mask is unnecessary because of sign-extend shifts.
/
/////////

/ Immediate to signed field.
%	PEFFECT|PVALUE
	FS32		ANYR	*	*	TEMP
		ADR|LV		FLD
		IMM|MMX		LONG
			[TL ZAND]	[AL],[TL CMASK]		/ clear
			[TL ZOR]	[AL],[TL AR]		/ store
		[IFV]	[TL ZMOVZX]	[R],[AL]		/ fetch

/ Immediate to unsigned field.
%	PEFFECT|PVALUE
	FU32		ANYR	*	*	TEMP
		ADR|LV		FLD
		IMM|MMX		LONG
			[TL ZAND]	[AL],[TL CMASK]		/ clear
			[TL ZOR]	[AL],[TL AR]		/ store
		[IFV]	[TL ZMOVZX]	[R],[AL]		/ fetch
		[IFV]	[ZAND]		[R],[EMASK]		/ mask

/ Signed field.
%	PEFFECT|PVALUE|P_SRT|PBYTE
	FS32		ANYR	*	ANYR	TEMP
		ADR|LV		FLD
		TREG		LONG
			[TL ZAND]	[AL],[TL CMASK]
			[TL ZOR]	[AL],[TL R]
		[IFV]	[TL ZMOVSX]	[R],[AL]

/ Unsigned field.
%	PEFFECT|PVALUE|P_SRT|PBYTE
	FU32		ANYR	*	ANYR	TEMP
		ADR|LV		FLD
		TREG		LONG
			[TL ZAND]	[AL],[TL CMASK]
			[TL ZOR]	[AL],[TL R]
		[IFV]	[TL ZMOVSX]	[R],[AL]
		[IFV]	[ZAND]		[R],[EMASK]

//////////
/ Floating point.
//////////

#ifndef	NDPDEF

/ DECVAX and IEEE software floating point assignment.
/ Zero and immediates are special.
/ Floats are different for DECVAX and IEEE. 

/ Double = 0.
%	PEFFECT|PIEEE|PDECVAX
	FF64		EAX	*	*	NONE
		ADR|LV		FF64
		0|MMX		*
			[ZSUB]	[R],[R]
			[ZMOV]	[HI AL],[R]
			[ZMOV]	[LO AL],[R]

/ Double = constant.
%	PEFFECT|PIEEE|PDECVAX
	FF64		NONE	*	*	NONE
		ADR|LV		FF64
		IMM|MMX		FF64
			[ZMOV]	[HI AL],[HI AR]
			[ZMOV]	[LO AL],[LO AR]

/ Double = double.
%	PEFFECT|PVALUE|P_SRT|PIEEE|PDECVAX
	FF64		EDXEAX	*	EDXEAX	EDXEAX
		ADR|LV		FF64
		TREG		FF64
			[ZMOV]	[HI AL],[HI R]
			[ZMOV]	[LO AL],[LO R]

/ Assignment to 32-bit floats.
/ Float = 0.
%	PEFFECT|PDECVAX|PIEEE
	FF64		EAX	*	*	NONE
		ADR|LV		FF32
		0|MMX		*
			[ZSUB]	[R],[R]
			[ZMOV]	[AL],[R]

/ Float = constant, DECVAX.
%	PEFFECT|PDECVAX
	FF64		NONE	*	*	NONE
		ADR|LV		FF32
		IMM|MMX		FF64
			[ZMOV]	[AL],[LO AR]

/ Float = constant, IEEE.
%	PEFFECT|PVALUE|PDECVAX
	FF64		EDXEAX	*	*	TEMP
		ADR|LV		FF32
		IMM|MMX		FF64
			[ZMOV]	[HI R], [HI AR]
			[ZMOV]	[LO R], [LO AR]
			[ZCALL]	[GID _fdcvt]
			[ZMOV]	[AL],[LO R]
		[IFV]	[ZCALL]	[GID _dfcvt]

/ Float = double or float, DECVAX.
%	PEFFECT|PVALUE|P_SRT|PDECVAX
	FF64		EDXEAX	*	EDXEAX	EDXEAX
		ADR|LV		FF32
		TREG		FLOAT
			[ZMOV]	[AL],[HI R]

/ Float = double or float, IEEE.
%	PEFFECT|PVALUE|P_SRT|PIEEE
	FF64		EDXEAX	*	EDXEAX	EDXEAX
		ADR|LV		FF32
		TREG		FLOAT
			[ZCALL]	[GID _fdcvt]
			[ZMOV]	[AL],[LO R]
		[IFV]	[ZCALL]	[GID _dfcvt]

#endif

#ifdef	NDPDEF

/ Hardware coprocessor (NDP) floating point.
/ Pop the NDP stack on assignment for effect.
%	PEFFECT|P_SRT|PNDP
	FF64		FPAC	*	FPAC	NONE
		ADR|LV		FLOAT
		TREG		FLOAT
			[TL ZFSTPD]	[AL]

/ Leave the value on the NDP stack on assignment for value.
%	PVALUE|P_SRT|PNDP
	FF64		FPAC	*	FPAC	FPAC
		ADR|LV		FLOAT
		TREG		FLOAT
			[TL ZFSTD]	[AL]

#endif

//////////
/ end of n1/i386/tables/assign.t
//////////
