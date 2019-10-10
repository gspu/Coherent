/////////
/
/ Ones complement. Longs get done in halves. 
/ Only integers are legal.
/ The NOT instruction sets no flags.
/
/////////

COM:
%	PEFFECT|PRVALUE|P_SLT
	WORD		ANYR	ANYR	*	TEMP
		TREG		WORD
		*		*
			[ZNOT]	[R]

%	PEFFECT|PRVALUE|P_SLT
	LONG		ANYR	ANYR	*	TEMP
		TREG		LONG
		*		*
			[ZNOT]	[LO R]
			[ZNOT]	[HI R]
