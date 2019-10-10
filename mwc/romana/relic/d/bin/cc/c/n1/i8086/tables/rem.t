/////////
/
/ Remainder. Almost the same as divide. The result comes back in a new
/ place (the dx). No flags are set. There is no need to handle doubles in
/ this table as the operation is not defined on doubles in C.
/
/////////

REM:
%	PEFFECT|PRVALUE|PLVALUE
	WORD		DXAX	AX	*	DX
		TREG		FS16
		ADR		WORD
			[ZCWD]
			[ZIDIV]	[AR]

%	PEFFECT|PRVALUE|PLVALUE
	WORD		DXAX	AX	*	DX
		TREG		UWORD
		ADR		WORD
			[ZSUB]	[HI R],[HI R]
			[ZDIV]	[AR]
