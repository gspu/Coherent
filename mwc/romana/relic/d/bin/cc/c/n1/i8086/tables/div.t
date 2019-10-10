/////////
/
/ Division. Can only be performed in the DXAX register.
/ The dividend is zero or sign extended. No flags are set.
/
/////////

DIV:
%	PEFFECT|PRVALUE|PLVALUE
	WORD		DXAX	AX	*	AX
		TREG		FS16
		ADR		WORD
			[ZCWD]
			[ZIDIV]	[AR]

%	PEFFECT|PRVALUE|PLVALUE
	WORD		DXAX	AX	*	AX
		TREG		UWORD
		ADR		WORD
			[ZSUB]	[HI R],[HI R]
			[ZDIV]	[AR]

////////
/
/ Floating point, using the numeric data coprocessor (8087).
/
////////

#ifdef NDPDEF
%	PRVALUE|P_SLT
	FF32|FF64	FPAC	FPAC	*	FPAC
		TREG		FF64
		ADR		FS16
			[ZFDIVI] [AR]

%	PRVALUE|P_SLT
	FF32|FF64	FPAC	FPAC	*	FPAC
		TREG		FF64
		ADR		FS32
			[ZFDIVL] [AR]

%	PRVALUE|P_SLT
	FF32|FF64	FPAC	FPAC	*	FPAC
		TREG		FF64
		ADR		FF32
			[ZFDIVF] [AR]

%	PRVALUE|P_SLT
	FF32|FF64	FPAC	FPAC	*	FPAC
		TREG		FF64
		ADR		FF64
			[ZFDIVD] [AR]
#endif
