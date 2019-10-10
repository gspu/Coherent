//////////
/ n1/i386/tables/rem.t
//////////

/////////
/
/ Remainder (%).
/ Should use unsigned divide when left operand is signed and right is unsigned,
/ but the parser generates the appropriate conversions to make it unnecessary.
/ Almost the same as divide, but the result comes back in EDX.
/ No flags are set.
/ There is no need to handle doubles in this table,
/ as the operation is not defined on doubles in C.
/
/////////

REM:

/ Signed integer.
%	PEFFECT|PVALUE
	DWORD		EDXEAX	EAX	*	EDX
		TREG		FS32
		ADR		LONG
			[ZCDQ]
			[ZIDIV]	[AR]

/ Unsigned integer.
%	PEFFECT|PVALUE
	DWORD		EDXEAX	EAX	*	EDX
		TREG		FU32
		ADR		LONG
			[ZSUB]	[REGNO EDX],[REGNO EDX]
			[ZDIV]	[AR]

//////////
/ end of n1/i386/tables/rem.t
//////////
