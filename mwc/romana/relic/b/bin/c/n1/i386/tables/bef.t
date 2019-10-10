//////////
/ n1/i386/tables/bef.t
//////////

/////////
/
/ Prefix increment and decrement (++@, --@).
/ The PTX case, which does these operations on pointers,
/ must handle anything on the right hand side.
/ The other cases must only handle a 1.
/ OP0 is ADD or SUB, OP1 is INC or DEC; cf. n1/i386/table1.c/optab[].
/ For the TL OP1 implementation, cf. n1/i386/outmch.c/maptype().
/
/////////

INCBEF:
DECBEF:

/ Increment integer or pointer by 1.
%	PEFFECT|PVALUE|PSREL
	DWORD		ANYR	*	*	TEMP
		ADR|LV		DWORD|SHORT
		1|MMX		*
			[TL OP1]	[AL]
		[IFV]	[TL ZMOVSX]	[R],[AL]
		[IFR]	[REL0]		[LAB]

/ Increment pointer by constant.
%	PEFFECT|PVALUE|PSREL
	DWORD		ANYR	*	*	TEMP
		ADR|LV		PTX
		IMM|MMX		LONG
			[OP0]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

//////////
/
/ Fields.
/ The increment is preshifted.
/
//////////

/ Field.
%	PEFFECT|PVALUE|PBYTE
	LONG		ANYR	*	*	TEMP
		ADR|LV		FLD
		IMM|MMX		LONG
			[TL ZMOVZX]	[R],[AL]	/ fetch
			[OP0]		[R],[AR]	/ bump
			[ZAND]		[R],[EMASK]	/ mask
			[TL ZAND]	[AL],[TL CMASK]	/ clear
			[TL ZOR]	[AL],[TL R]	/ store

//////////
/ end of n1/i386/tables/bef.t
//////////
