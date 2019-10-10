/////////
/ 
/ Bit or. There is no long or instruction so all long or operations are
/ done in halves. The or sets the flags. Try to eliminate operations with
/ useless parts (0 halves) of constants.
/
/////////

OR:
%	PEFFECT|PRVALUE|PREL|P_SLT
	WORD		ANYR	ANYR	*	TEMP
		TREG		WORD
		ADR|IMM		WORD
			[ZOR]	[R],[AR]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|P_SLT
	LONG		ANYR	ANYR	*	TEMP
		TREG		LONG
		UHC|MMX		LONG
			[ZOR]	[LO R],[LO AR]

%	PEFFECT|PRVALUE|PGE|PLT|P_SLT
	LONG		ANYR	ANYR	*	TEMP
		TREG		LONG
		LHC|MMX		LONG
			[ZOR]	[HI R],[HI AR]
		[IFR]	[REL1]	[LAB]

%	PEFFECT|PRVALUE|PGE|PLT|P_SLT
	LONG		ANYR	ANYR	*	TEMP
		TREG		LONG
		ADR|IMM		LONG
			[ZOR]	[LO R],[LO AR]
			[ZOR]	[HI R],[HI AR]
		[IFR]	[REL1]	[LAB]
