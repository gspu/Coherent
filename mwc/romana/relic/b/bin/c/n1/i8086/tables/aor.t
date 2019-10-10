/////////
/
/ Assigned or.
/ The modifier has inserted a mask off of the field into the right subtree,
/ as well as the shift up. 
/ The right therefore may be ored into the left.
/
/////////

AOR:
%	PEFFECT|PRVALUE|PREL
	WORD		ANYR	*	*	TEMP
		RREG|MMX	WORD
		EASY|MMX	WORD
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		LREG|MMX	WORD
		EASY|MMX	WORD
%	PEFFECT|PRVALUE|PREL
	WORD		ANYR	*	*	TEMP
		ADR|LV		WORD
		IMM|MMX		WORD
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		ADR|LV		WORD
		IMM|MMX		WORD
			[ZOR]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL
	WORD		AX	*	*	AX
		ADR|LV		FS8
		IMM|MMX		WORD
			[ZORB]	[AL],[AR]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZCBW]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL
	WORD		AX	*	*	AX
		ADR|LV		FU8
		IMM|MMX		WORD
			[ZORB]	[AL],[AR]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZSUBB]	[HI R],[HI R]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL|P_SRT
	WORD		ANYR	*	ANYR	TEMP
		ADR|LV		WORD
		TREG		WORD
%	PLVALUE|P_SRT
	WORD		ANYL	*	ANYL	TEMP
		ADR|LV		WORD
		TREG		WORD
			[ZOR]	[AL],[R]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL|P_SRT
	WORD		AX	*	AX	AX
		ADR|LV		FS8
		TREG		WORD
			[ZORB]	[AL],[LO R]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZCBW]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL|P_SRT
	WORD		AX	*	AX	AX
		ADR|LV		FU8
		TREG		WORD
			[ZORB]	[AL],[LO R]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZSUBB]	[HI R],[HI R]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PLT|PGE
	LONG		DXAX	*	*	DXAX
		ADR|LV		LONG
		LHC|MMX		LONG
			[ZOR]	[HI AL],[HI AR]
		[IFV]	[ZMOV]	[LO R],[LO AL]
		[IFV]	[ZMOV]	[HI R],[HI AL]
		[IFR]	[REL1]	[LAB]

%	PEFFECT|PRVALUE
	LONG		DXAX	*	*	DXAX
		ADR|LV		LONG
		UHC|MMX		LONG
			[ZOR]	[LO AL],[LO AR]
		[IFV]	[ZMOV]	[LO R],[LO AL]
		[IFV]	[ZMOV]	[HI R],[HI AL]

%	PEFFECT|PRVALUE|PLT|PGE
	LONG		DXAX	*	*	DXAX
		ADR|LV		LONG
		IMM|MMX		LONG
			[ZOR]	[LO AL],[LO AR]
			[ZOR]	[HI AL],[HI AR]
		[IFV]	[ZMOV]	[LO R],[LO AL]
		[IFV]	[ZMOV]	[HI R],[HI AL]
		[IFR]	[REL1]	[LAB]

%	PEFFECT|PRVALUE
	LONG		DXAX	*	DXAX	DXAX
		ADR|LV		LONG
		TREG		LONG
			[ZOR]	[LO AL],[LO R]
			[ZOR]	[HI AL],[HI R]
		[IFV]	[ZMOV]	[LO R],[LO AL]
		[IFV]	[ZMOV]	[HI R],[HI AL]

/////////
/
/ Fields.
/ Distinguish between signed and unsigned fields.
/ If the value is needed, the mask must be done for unsigned fields.
/ The sign extend shifts don't need the mask, so it gets optimized out.
/
/////////

%	PEFFECT|PRVALUE
	FS16		ANYR	*	*	TEMP
		ADR|LV		FFLD16
		DIR|IMM|MMX	WORD
			[ZOR]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]

%	PEFFECT|PRVALUE
	UWORD		ANYR	*	*	TEMP
		ADR|LV		FFLD16
		DIR|IMM|MMX	WORD
			[ZOR]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]
		[IFV]	[ZAND]	[R],[LO EMASK]

%	PEFFECT|PRVALUE
	FS16		AX	*	*	TEMP
		ADR|LV		FFLD8
		DIR|IMM|MMX	WORD
			[ZORB]	[AL],[AR]
		[IFV]	[ZMOVB]	[LO R],[AL]

%	PEFFECT|PRVALUE
	UWORD		ANYR	*	*	TEMP
		ADR|LV		FFLD8
		DIR|IMM|MMX	WORD
			[ZORB]	[AL],[AR]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZAND]	[R],[LO EMASK]

%	PEFFECT|PRVALUE|P_SRT
	FS16		ANYR	*	ANYR	TEMP
		ADR|LV		FFLD16
		TREG		WORD
			[ZOR]	[AL],[R]
		[IFV]	[ZMOV]	[R],[AL]

%	PEFFECT|PRVALUE|P_SRT
	UWORD		ANYR	*	ANYR	TEMP
		ADR|LV		FFLD16
		TREG		WORD
			[ZOR]	[AL],[R]
		[IFV]	[ZMOV]	[R],[AL]
		[IFV]	[ZAND]	[R],[LO EMASK]

%	PEFFECT|PRVALUE|P_SRT
	FS16		AX	*	AX	TEMP
		ADR|LV		FFLD8
		TREG		WORD
			[ZORB]	[AL],[LO R]
		[IFV]	[ZMOVB]	[LO R],[AL]

%	PEFFECT|PRVALUE|P_SRT
	UWORD		AX	*	AX	TEMP
		ADR|LV		FFLD8
		TREG		WORD
			[ZORB]	[AL],[LO R]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZAND]	[R],[LO EMASK]
