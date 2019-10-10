/////////
/
/ Prefix increment and decrement. The LPTX case, which is used to do
/ these operations on pointers, must handle anything on the right hand
/ side. The others must only handle a 1.
/
/////////

INCBEF:
DECBEF:
%	PEFFECT|PRVALUE|PSREL
	WORD		ANYR	*	*	TEMP
		ADR|LV		WORD
		1|MMX		*
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		ADR|LV		WORD
		1|MMX		*
			[OP1]	[AL]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

#ifndef ONLYSMALL
%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		1|MMX		*
			[OP1]	[AL]
			[ZMOV]	[LO R],[LO AL]
			[ZMOV]	[HI R],[HI AL]

%	PEFFECT|PLVALUE
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		1|MMX		*
			[OP1]	[AL]
		[IFV]	[ZLDES]	[LO R],[AL]
#endif

%	PEFFECT|PRVALUE|PSREL
	WORD		ANYR	*	*	TEMP
		ADR|LV		WORD
		IMM|MMX		WORD
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		ADR|LV		WORD
		IMM|MMX		WORD
			[OP0]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

#ifndef ONLYSMALL
%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		IMM|MMX		WORD
			[OP0]	[AL],[AR]
			[ZMOV]	[LO R],[LO AL]
			[ZMOV]	[HI R],[HI AL]

%	PEFFECT|PLVALUE
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		IMM|MMX		WORD
			[OP0]	[AL],[AR]
		[IFV]	[ZLDES]	[LO R],[AL]
#endif

%	PEFFECT|PRVALUE|PSREL
	WORD		AX	*	*	AX
		ADR|LV		FS8
		1|MMX		*
			[TL OP1]	[AL]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZCBW]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PSREL
	WORD		AX	*	*	AX
		ADR|LV		FU8
		1|MMX		*
			[TL OP1]	[AL]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZSUBB]	[HI R],[HI R]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PLT|PGE
	LONG		ANYR	*	*	TEMP
		ADR|LV		LONG
		1|MMX		*
			[OP0]	[LO AL],[CONST 1]
			[OP2]	[HI AL],[CONST 0]
		[IFV]	[ZMOV]	[LO R],[LO AL]
		[IFV]	[ZMOV]	[HI R],[HI AL]
		[IFR]	[REL1]	[LAB]

%	PEFFECT|PRVALUE
	WORD		ANYR	*	*	TEMP
		ADR|LV		FFLD16
		IMM|MMX		WORD
			[ZMOV]	[R],[AL]
			[OP0]	[R],[AR]
			[ZAND]	[R],[LO EMASK]
			[ZAND]	[AL],[LO CMASK]
			[ZOR]	[AL],[R]

%	PEFFECT|PRVALUE
	WORD		AX	*	*	AX
		ADR|LV		FFLD8
		IMM|MMX		WORD
			[ZMOVB]	[LO R],[AL]
			[OP0]	[R],[AR]
			[ZAND]	[R],[LO EMASK]
			[ZANDB]	[AL],[LO CMASK]
			[ZORB]	[AL],[LO R]
