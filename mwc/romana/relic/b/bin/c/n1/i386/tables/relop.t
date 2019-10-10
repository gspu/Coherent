//////////
/ n1/i386/tables/relop.t
//////////

/////////
/
/ Relations (== != > >= < <=).
/ Handles only test contexts.
/ The compiler transforms all other contexts (like value) into "?:" operations.
/
/////////

EQ:
NE:
GT:
GE:
LT:
LE:
ULT:
ULE:
UGT:
UGE:

/////////
/
/ Simple stuff.
/ Longs, words and bytes.
/ An immediate can be compared to any addressible thing.
/
/////////

/ Dword register :: immediate.
%	PREL
	*		NONE	*	*	NONE
		REG|MMX		DWORD
		ADR|IMM		DWORD
			[ZCMP]	[AL],[AR]
			[REL0]	[LAB]

/ Dword :: immediate.
%	PREL|P_SLT
	*		ANYR	ANYR	*	NONE
		TREG		DWORD
		IMM|MMX		DWORD
			[ZCMP]	[RL],[AR]
			[REL0]	[LAB]

/ Signed word :: word constant.
%	PREL
	*		NONE	*	*	NONE
		ADR		FS16
		SWORD|MMX	LONG
/ Signed byte :: byte constant.
%	PREL
	*		NONE	*	*	NONE
		ADR		FS8
		SBYTE|MMX	LONG
/ Unsigned word :: word constant.
%	PREL
	*		NONE	*	*	NONE
		ADR		FU16
		UWORD|MMX	LONG
/ Unsigned byte :: byte constant.
%	PREL
	*		NONE	*	*	NONE
		ADR		FU8
		UBYTE|MMX	LONG
			[TL ZCMP]	[AL],[AR]
			[REL0]		[LAB]

/ Dword :: dword.
%	PREL|P_SLT
	*		ANYR	ANYR	*	NONE
		TREG		DWORD
		ADR|IMM		DWORD
/ Word :: word.
%	PREL|P_SLT
	*		ANYR	ANYR	*	NONE
		TREG		WORD
		ADR|IMM		WORD
/ Byte :: byte.
%	PREL|P_SLT|PBYTE
	*		ANYR	ANYR	*	NONE
		TREG		BYTE
		ADR|IMM		BYTE
			[TL ZCMP]	[TL R],[AR]
			[REL0]		[LAB]

//////////
/ Floating point.
//////////

/ DECVAX or IEEE software floating point.
/ Double :: double.
/ _dcmp(d) pops its argument before returning, so it need not be done here.
%	PREL|PDECVAX|PIEEE
	*		EDXEAX	EDXEAX	*	NONE
		TREG		FF64
		ADR|IMM		FF64
			[ZPUSH]	[HI AR]
			[ZPUSH]	[LO AR]
			[ZCALL]	[GID _dcmp]
			[REL0]	[LAB]

/ Hardware coprocessor (NDP) floating point.
%	PREL|P_SLT|PNDP
	*		*	FPAC	*	NONE
		TREG		FF64
		ADR		NDPARG
			[TR ZFCOMPD]	[AR]
			[ZCALL]		[GID _cfcc]
			[REL0]		[LAB]

//////////
/ end of n1/i386/tables/relop.t
//////////
