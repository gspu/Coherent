/////////
/
/ Twos complement. Integers are easy. Longs are negated in halves. There
/ may be a better sequence for long negation. This table handles routine
/ call style floating point.
/
/////////

NEG:
%	PEFFECT|PRVALUE|PREL|P_SLT
	WORD		ANYR	ANYR	*	TEMP
		TREG		WORD
		*		*
			[ZNEG]	[R]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PGE|PLT|P_SLT
	LONG		ANYR	ANYR	*	TEMP
		TREG		LONG
		*		*
			[ZNEG]	[HI R]
			[ZNEG]	[LO R]
			[ZSBB]	[HI R],[CONST 0]
		[IFR]	[REL1]	[LAB]

#ifndef NDPDEF
%	PEFFECT|PRVALUE|P_SLT
	FF64		FPAC	FPAC	*	FPAC
		TREG		FF64
		*		*
			[ZXORB]	[LO HI LO R],[CONST 0200]
#endif
#ifdef NDPDEF
%	PRVALUE|P_SLT
	FF64		FPAC	FPAC	*	FPAC
		TREG		FF64
		*		*
			[ZFCHS]
#endif
