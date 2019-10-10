//////////
/ n1/i386/tables/orxor.t
//////////

/////////
/ 
/ Bitwise OR (|), bitwise XOR (^).
/ [OP0] is ZOR or ZXOR.
/ The OR and XOR operations set the flags.
/
/////////

OR:
XOR:

%	PEFFECT|PVALUE|PREL|P_SLT
	DWORD		ANYR	ANYR	*	TEMP
		TREG		DWORD
		ADR|IMM		DWORD
			[OP0]	[R],[AR]
		[IFR]	[REL0]	[LAB]

//////////
/ end of n1/i386/tables/orxor.t
//////////
