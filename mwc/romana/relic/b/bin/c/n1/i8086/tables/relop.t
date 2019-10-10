/////////
/
/ Relationals.
/ Handles only test contexts, since other parts of the compiler transform
/ all other contexts (like value) into the required '?' and ':' operations.
/ Some special things handle bytes in a slightly better way than the default
/ (saving the 'cbw') and other special things handle pointers.
/ Pointers are compared for equality by comparing both segment and offset.
/ Pointers are compared for inequality by comparing only the offset;
/ the C code always assumes OPTIMIZE(3).
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
/ Words and bytes.
/ An immediate can be compared to any addressable thing.
/ Special things for the byte compare to avoid the CBW.
/
/////////

%	PREL
	*		NONE	*	*	NONE
		ADR		BYTE
		BYTE|MMX	WORD
			[ZCMPB]	[AL],[AR]
			[REL0]	[LAB]

%	PREL
	*		NONE	*	*	NONE
		ADR|MMX		WORD
		IMM|MMX		WORD
%	PREL
	*		NONE	*	*	NONE
		RREG|LREG|MMX	WORD
		ADR|IMM		WORD
			[ZCMP]	[AL],[AR]
			[REL0]	[LAB]

%	PREL|P_SLT
	*		ANYR	ANYR	*	NONE
		TREG		BYTE
		ADR|IMM		BYTE
			[ZCMPB]	[LO R],[AR]
			[REL0]	[LAB]

%	PREL
	*		AX	*	*	NONE
		ADR		BYTE
		ADR|IMM		BYTE
			[ZMOVB]	[LO R],[AL]
			[ZCMPB]	[LO R],[AR]
			[REL0]	[LAB]

%	PREL|P_SLT
	*		ANYR	ANYR	*	NONE
		TREG		WORD
		ADR|IMM		WORD
			[ZCMP]	[R],[AR]
			[REL0]	[LAB]

/////////
/
/ Longs and pointers.
/ Equality.
/
/////////

%	PEQ
	WORD		NONE	*	*	NONE
		ADR	LONG
		IMM|MMX	LONG
			[ZCMP]	[HI AL],[HI AR]
			[ZJNE]	[LAB1]
			[ZCMP]	[LO AL],[LO AR]
			[ZJE]	[LAB]
		[DLAB1]:

%	PEQ
	*		NONE	ANYR	*	NONE
		TREG		LONG
		UHC|MMX		LONG
			[ZOR]	[HI RL],[HI RL]
			[ZJNE]	[LAB1]
			[ZCMP]	[LO RL],[LO AR]
			[ZJE]	[LAB]
		[DLAB1]:

%	PEQ
	*		NONE	ANYR	*	NONE
		TREG		LONG
		LHC|MMX		LONG
			[ZCMP]	[HI RL],[HI AR]
			[ZJNE]	[LAB1]
			[ZOR]	[LO RL],[LO RL]
			[ZJE]	[LAB]
		[DLAB1]:

%	PEQ
	*		NONE	ANYR	*	NONE
		TREG		LONG
		ADR|IMM		LONG
#ifndef ONLYSMALL
%	PEQ
	*		NONE	ANYR	*	NONE
		TREG		LPTX
		ADR		LPTX
#endif
			[ZCMP]	[HI RL],[HI AR]
			[ZJNE]	[LAB1]
			[ZCMP]	[LO RL],[LO AR]
			[ZJE]	[LAB]
		[DLAB1]:

#ifndef ONLYSMALL
%	PEQ
	*		NONE	ANYR	*	NONE
		TREG		LPTX
		ADR|IMM		WORD
			[ZOR]	[HI RL],[HI RL]
			[ZJNE]	[LAB1]
			[ZCMP]	[LO RL],[AR]
			[ZJE]	[LAB]
		[DLAB1]:

%	PEQ
	WORD		ANYR	ANYR	*	TEMP
		TREG		WORD
		ADR|IMM		LPTX
			[ZMOV]	[R],[HI AR]
			[ZOR]	[R],[R]
			[ZJNE]	[LAB1]
			[ZCMP]	[RL],[LO AR]
			[ZJE]	[LAB]
		[DLAB1]:
#endif

/////////
/
/ Inequality. Slightly different code for longs.
/
/////////

%	PNE
	*		NONE	*	*	NONE
		ADR		LONG
		IMM|MMX		LONG
			[ZCMP]	[HI AL],[HI AR]
			[ZJNE]	[LAB]
			[ZCMP]	[LO AL],[LO AR]
			[ZJNE]	[LAB]

%	PNE
	*		NONE	ANYR	*	NONE
		TREG		LONG
		UHC|MMX		LONG
			[ZOR]	[HI RL],[HI RL]
			[ZJNE]	[LAB]
			[ZCMP]	[LO RL],[LO AR]
			[ZJNE]	[LAB]

%	PNE
	*		NONE	ANYR	*	NONE
		TREG		LONG
		LHC|MMX		LONG
			[ZCMP]	[HI RL],[HI AR]
			[ZJNE]	[LAB]
			[ZOR]	[LO RL],[LO RL]
			[ZJNE]	[LAB]

%	PNE
	*		NONE	ANYR	*	NONE
		TREG		LONG
		ADR|IMM		LONG
#ifndef ONLYSMALL
%	PNE
	*		NONE	ANYR	*	NONE
		TREG		LPTX
		ADR		LPTX
#endif
			[ZCMP]	[HI RL],[HI AR]
			[ZJNE]	[LAB]
			[ZCMP]	[LO RL],[LO AR]
			[ZJNE]	[LAB]

#ifndef ONLYSMALL
%	PNE
	*		NONE	ANYR	*	NONE
		TREG		LPTX
		ADR|IMM		WORD
			[ZOR]	[HI RL],[HI RL]
			[ZJNE]	[LAB]
			[ZCMP]	[LO RL],[AR]
			[ZJNE]	[LAB]

%	PNE
	WORD		ANYR	ANYR	*	TEMP
		TREG		WORD
		ADR|IMM		LPTX
			[ZMOV]	[R],[HI AR]
			[ZOR]	[R],[R]
			[ZJNE]	[LAB]
			[ZCMP]	[RL],[LO AR]
			[ZJNE]	[LAB]
#endif

/////////
/
/ All the rest.
/
/////////

#ifndef ONLYSMALL
%	PNEREL
	*		NONE	*	*	NONE
		ADR		LPTX
		IMM|MMX		LPTX
			[ZCMP]	[LO AL],[LO AR]
			[REL0]	[LAB]

%	PNEREL
	*		NONE	ANYR	*	NONE
		TREG		LPTX
		ADR		LPTX
			[ZCMP]	[LO RL],[LO AR]
			[REL0]	[LAB]

%	PNEREL
	*		NONE	*	*	NONE
		ADR		LPTX
		IMM|MMX		WORD
			[ZCMP]	[LO AL],[AR]
			[REL0]	[LAB]

%	PNEREL
	*		NONE	ANYR	*	NONE
		TREG		LPTX
		ADR|IMM		WORD
			[ZCMP]	[LO RL],[AR]
			[REL0]	[LAB]

%	PNEREL
	*		NONE	*	*	NONE
		ADR		WORD
		IMM|MMX		LPTX
			[ZCMP]	[AL],[LO AR]
			[REL0]	[LAB]

%	PNEREL
	*		NONE	ANYR	*	NONE
		TREG		WORD
		ADR|IMM		LPTX
			[ZCMP]	[RL],[LO AR]
			[REL0]	[LAB]
#endif

%	PNEREL
	*		NONE	*	*	NONE
		ADR		LONG
		IMM|MMX		LONG
			[ZCMP]	[HI AL],[HI AR]
			[LREL1]	[LAB1]
			[LREL0]	[LAB]
			[ZCMP]	[LO AL],[LO AR]
			[LREL2]	[LAB]
		[DLAB1]:

%	PNEREL
	*		NONE	ANYR	*	NONE
		TREG		LONG
		UHC|MMX		LONG
			[ZOR]	[HI RL],[HI RL]
			[LREL1]	[LAB1]
			[LREL0]	[LAB]
			[ZCMP]	[LO RL],[LO AR]
			[LREL2]	[LAB]
		[DLAB1]:

%	PNEREL
	*		NONE	ANYR	*	NONE
		TREG		LONG
		ADR|IMM		LONG
			[ZCMP]	[HI RL],[HI AR]
			[LREL1]	[LAB1]
			[LREL0]	[LAB]
			[ZCMP]	[LO RL],[LO AR]
			[LREL2]	[LAB]
		[DLAB1]:

////////
/
/ Floating point, using the numeric data coprocessor (8087).
/
////////

#ifdef NDPDEF
%	PREL
	*		*	FPAC	*	NONE
		TREG		FF64
		ADR		FS16
			[ZFCOMPI] [AR]
			[CALL] [GID cfcc]
			[REL0] [LAB]

%	PREL|P_SLT
	*		*	FPAC	*	NONE
		TREG		FF64
		ADR		FS32
			[ZFCOMPL] [AR]
			[CALL] [GID cfcc]
			[REL0] [LAB]

%	PREL|P_SLT
	*		*	FPAC	*	NONE
		TREG		FF64
		ADR		FF32
			[ZFCOMPF] [AR]
			[CALL] [GID cfcc]
			[REL0] [LAB]

%	PREL|P_SLT
	*		*	FPAC	*	NONE
		TREG		FF64
		ADR		FF64
			[ZFCOMPD] [AR]
			[CALL] [GID cfcc]
			[REL0] [LAB]
#endif
