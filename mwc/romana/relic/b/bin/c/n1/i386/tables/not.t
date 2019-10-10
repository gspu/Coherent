//////////
/ n1/i386/tables/not.t
//////////

/////////
/
/ Bitwise negation, i.e. one's complement (~).
/ Only integers are legal.
/ The NOT instruction sets no flags.
/
/////////

COM:

%	PEFFECT|PVALUE|P_SLT
	LONG		ANYR	ANYR	*	TEMP
		TREG		LONG
		*		*
			[ZNOT]	[R]

//////////
/ end of n1/i386/tables/not.t
//////////
