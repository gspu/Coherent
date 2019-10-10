//////////
/ n1/i386/tables/and.t
//////////

/////////
/
/ Bitwise AND (&).
/
/////////

AND:

/ Any & immediate, relational context.
%	PSREL
	*		NONE	*	*	NONE
		ADR		DWORD|WORD|BYTE
		IMM|MMX		DWORD
			[TL ZTEST]	[AL],[TL AR]
			[REL0]		[LAB]

/ Dword & dword, effect or value context.
%	PEFFECT|PVALUE|PEREL|P_SLT
	*		ANYR	ANYR	*	TEMP
		TREG		DWORD
		ADR|IMM		DWORD
			[ZAND]	[R],[AR]
		[IFR]	[REL0]	[LAB]

//////////
/ end of n1/i386/tables/and.t
//////////
