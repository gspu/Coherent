/////////
/
/ Plain and simple assignment.
/ Handles doubles, longs, fields, bytes and words.
/ This table does not handle doubles.
/ It is not clear if some kinds of double assignments should be done
/ with move instructions to save the overhead of loading the 8087 emulator,
/ since the emulator is big and fat.
/ More complex double assignments, like the ones that must
/ return a value, use the 8087.
/ The '=' operator is a non conversion operator for pointers, words and longs.
/ There must be some special tables here to handle the cross type cases.
/ This approach makes everything appear to work in a rational fashion.
/
/////////

ASSIGN:
%	PEFFECT
	WORD		NONE	*	*	NONE
		REG|MMX		WORD
		0|MMX		*
			[ZSUB]	[AL],[AL]

%	PEFFECT
	WORD		NONE	*	*	NONE
		REG|MMX		WORD
		EASY|MMX	WORD
%	PEFFECT
	WORD		NONE	*	*	NONE
		ADR|LV		WORD
		REG|MMX		WORD
%	PEFFECT
	WORD		NONE	*	*	NONE
		ADR|LV		WORD
		IMM|MMX		WORD
			[ZMOV]	[AL],[AR]

%	PEFFECT
	WORD		NONE	*	*	NONE
		REG|MMX		WORD
		EASY|MMX	LONG
			[ZMOV]	[AL],[LO AR]

%	PEFFECT
	WORD		NONE	*	*	NONE
		RREG|MMX	WORD
		LEA|MMX		WORD
			[ZLEA]	[AL],[NSE AR]

%	PEFFECT
	WORD		NONE	*	*	NONE
		ADR|LV		BYTE
		IMM|MMX		WORD
			[ZMOVB]	[AL],[AR]

%	PEFFECT
	LONG		NONE	*	*	NONE
		ADR|LV		LONG
		IMM|MMX		LONG
			[ZMOV]	[LO AL],[LO AR]
			[ZMOV]	[HI AL],[HI AR]

%	PEFFECT|PEREL|P_SRT
	WORD		ANYR	*	ANYR	TEMP
		ADR|LV		FS8
		TREG		BYTE
			[ZMOVB]	[AL],[LO R]
		[IFR]	[ZORB]	[LO R],[LO R]
		[IFR]	[REL0]	[LAB]

%	PRVALUE|P_SRT
	WORD		AX	*	AX	TEMP
		ADR|LV		FS8
		TREG		BYTE
			[ZMOVB]	[AL],[LO R]
			[ZCBW]

%	PEFFECT|PRVALUE|PEREL|P_SRT
	WORD		ANYR	*	ANYR	TEMP
		ADR|LV		FU8
		TREG		BYTE
			[ZMOVB]	[AL],[LO R]
		[IFV]	[ZSUBB]	[HI R],[HI R]
		[IFR]	[ZORB]	[LO R],[LO R]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PEREL
	WORD		AX	*	*	AX
		ADR|LV		FS8
		ADR		BYTE
			[ZMOVB]	[LO R],[AR]
			[ZMOVB]	[AL],[LO R]
		[IFV]	[ZCBW]
		[IFR]	[ZORB]	[LO R],[LO R]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PEREL
	WORD		AX	*	*	AX
		ADR|LV		FU8
		ADR		BYTE
			[ZMOVB]	[LO R],[AR]
			[ZMOVB]	[AL],[LO R]
		[IFV]	[ZSUBB]	[HI R],[HI R]
		[IFR]	[ZORB]	[LO R],[LO R]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|P_SRT
	WORD		ANYR	*	ANYR	TEMP
		ADR|LV		WORD
		TREG		WORD
%	PLVALUE|P_SRT
	WORD		ANYL	*	ANYL	TEMP
		ADR|LV		WORD
		TREG		WORD
			[ZMOV]	[AL],[R]

%	PEFFECT
	WORD		AX	*	*	NONE
		ADR|LV		WORD
		EASY|MMX	WORD
			[ZMOV]	[R],[AR]
			[ZMOV]	[AL],[R]

%	PEFFECT|PRVALUE|P_SRT
	WORD		AX	*	AX	AX
		ADR|LV		FS8
		TREG		WORD
			[ZMOVB]	[AL],[LO R]
		[IFV]	[ZCBW]

%	PEFFECT|PRVALUE|P_SRT
	WORD		AX	*	AX	AX
		ADR|LV		FU8
		TREG		WORD
			[ZMOVB]	[AL],[LO R]
		[IFV]	[ZSUBB]	[HI R],[HI R]

%	PRVALUE|P_SRT
	LONG		ANYR	*	ANYR	TEMP
		ADR|LV		LONG
		TREG		LONG
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]

%	PEFFECT|PRVALUE
	WORD		AX	*	DXAX	AX
		ADR|LV		WORD
		TREG		LONG
			[ZMOV]	[AL],[R]

%	PEFFECT|PRVALUE|P_SRT
	WORD		DXAX	*	DXAX	AX
		ADR|LV		FS8
		TREG		LONG
			[ZMOVB]	[AL],[LO LO R]
		[IFV]	[ZCBW]

%	PEFFECT|PRVALUE|P_SRT
	WORD		DXAX	*	DXAX	AX
		ADR|LV		FU8
		TREG		LONG
			[ZMOVB]	[AL],[LO LO R]
		[IFV]	[ZSUBB]	[HI LO R],[HI LO R]

/////////
/
/ Assignment between pointers and integer types.
/ These are all set up as no conversion operations in the parser.
/ We attempt to do a reasonably good job on the common cases,
/ like setting a pointer to NULL.
/
/////////

#ifndef ONLYSMALL
%	PEFFECT
	LPTX		NONE	*	*	NONE
		ADR|LV		LPTX
		0|MMX		*
			[ZMOV]	[LO AL],[CONST 0]
			[ZMOV]	[HI AL],[CONST 0]

%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		0|MMX		*
			[ZSUB]	[LO R],[LO R]
			[ZMOV]	[HI R],[LO R]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[LO R]

%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		ICN|MMX		LPTX
			[ZMOV]	[LO R],[AR]
			[ZSUB]	[HI R],[HI R]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		ICN|MMX		LPTX
			[ZMOV]	[LO AL],[AR]
			[ZMOV]	[HI AL],[CONST 0]
		[IFV]	[ZLDES]	[LO R],[AL]

%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		LCN|MMX		LPTX
			[ZMOV]	[LO R],[LO AR]
			[ZMOV]	[HI R],[HI AR]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		LCN|MMX		LPTX
			[ZMOV]	[LO AL],[LO AR]
			[ZMOV]	[HI AL],[HI AR]
		[IFV]	[ZLDES]	[LO R],[AL]

%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		ACS|MMX		LPTX
			[ZMOV]	[LO R],[AR]
			[ZMOV]	[HI R],[REGNO CS]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]

%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		ADS|MMX		LPTX
			[ZMOV]	[LO R],[AR]
			[ZMOV]	[HI R],[REGNO DS]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		ACS|MMX		LPTX
			[ZMOV]	[LO AL],[AR]
			[ZMOV]	[HI AL],[REGNO CS]
		[IFV]	[ZLDES]	[LO R],[AL]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		ADS|MMX		LPTX
			[ZMOV]	[LO AL],[AR]
			[ZMOV]	[HI AL],[REGNO DS]
		[IFV]	[ZLDES]	[LO R],[AL]

%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		DIR|MMX		WORD
			[ZMOV]	[LO R],[AR]
			[ZSUB]	[HI R],[HI R]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		DIR|MMX		WORD
			[ZMOV]	[LO R],[AR]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[CONST 0]
		[IFV]	[ZLDES]	[LO R],[AL]

%	PRVALUE
	LPTX		ANYR	*	ANYR	TEMP
		ADR|LV		LPTX
		TREG		WORD
			[ZMOV]	[LO AL],[RR]
			[ZMOV]	[HI AL],[CONST 0]
			[ZMOV]	[LO R],[LO AL]
			[ZMOV]	[HI R],[HI AL]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	ANYR	TEMP
		ADR|LV		LPTX
		TREG		WORD
			[ZMOV]	[LO AL],[RR]
			[ZMOV]	[HI AL],[CONST 0]
		[IFV]	[ZLDES]	[LO R],[AL]

%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		DIR|MMX		LONG
%	PRVALUE
	LONG		ANYR	*	*	TEMP
		ADR|LV		LONG
		DIR|MMX		LPTX
			[ZMOV]	[LO R],[LO AR]
			[ZMOV]	[HI R],[HI AR]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]

%	PRVALUE
	LPTX		ANYR	*	ANYR	TEMP
		ADR|LV		LPTX
		TREG		LONG
			[ZMOV]	[LO AL],[LO RR]
			[ZMOV]	[HI AL],[HI RR]
			[ZMOV]	[LO R],[LO AL]
			[ZMOV]	[HI R],[HI AL]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	ANYR	TEMP
		ADR|LV		LPTX
		TREG		LONG
			[ZMOV]	[LO AL],[LO RR]
			[ZMOV]	[HI AL],[HI RR]
		[IFV]	[ZLDES]	[LO R],[AL]

%	PRVALUE|P_SRT
	LPTX		ANYR	*	ANYR	TEMP
		ADR|LV		LPTX
		TREG		LPTX
%	PLVALUE|P_SRT
	LPTX		ANYL	*	ANYL	TEMP
		ADR|LV		LPTX
		TREG		LPTX
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]

%	PRVALUE|PEFFECT|P_SRT
	LONG		ANYR	*	ANYR	TEMP
		ADR|LV		LONG
		TREG		LPTX
			[ZMOV]	[LO AL],[LO RR]
			[ZMOV]	[HI AL],[HI RR]

%	PRVALUE|PEFFECT
	WORD		ESAX	*	ESAX	AX
		ADR|LV		WORD
		TREG		LPTX
			[ZMOV]	[AL],[LO R]

%	PRVALUE|PEFFECT
	WORD		DXAX	*	DXAX	AX
		ADR|LV		FS8
		TREG		LPTX
			[ZMOVB]	[AL],[LO LO R]
		[IFV]	[ZCBW]

%	PRVALUE|PEFFECT
	WORD		DXAX	*	DXAX	AX
		ADR|LV		FU8
		TREG		LPTX
			[ZMOVB]	[AL],[LO LO R]
		[IFV]	[ZSUBB]	[HI LO R],[HI LO R]
#endif

/////////
/
/ Do a reasonably sleazy job on field assignment.
/ The right operand has been preshifted and masked,
/ just like the right side of an '|=' or '^=' operation.
/ All you do is clear the junk and OR it in.
/
/////////

%	PEFFECT|PRVALUE
	FS16		ANYR	*	*	TEMP
		ADR|LV		FFLD16
		IMM|MMX		WORD
			[ZAND]	[AL],[LO CMASK]
			[ZOR]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]

%	PEFFECT|PRVALUE
	UWORD		ANYR	*	*	TEMP
		ADR|LV		FFLD16
		IMM|MMX		WORD
			[ZAND]	[AL],[LO CMASK]
			[ZOR]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]
		[IFV]	[ZAND]	[R],[LO EMASK]

%	PEFFECT|PRVALUE|P_SRT
	FS16		ANYR	*	ANYR	TEMP
		ADR|LV		FFLD16
		TREG		WORD
			[ZAND]	[AL],[LO CMASK]
			[ZOR]	[AL],[R]
		[IFV]	[ZMOV]	[R],[AL]

%	PEFFECT|PRVALUE|P_SRT
	UWORD		ANYR	*	ANYR	TEMP
		ADR|LV		FFLD16
		TREG		WORD
			[ZAND]	[AL],[LO CMASK]
			[ZOR]	[AL],[R]
		[IFV]	[ZMOV]	[R],[AL]
		[IFV]	[ZAND]	[R],[LO EMASK]

%	PEFFECT|PRVALUE|P_SRT
	FS16		AX	*	AX	TEMP
		ADR|LV		FFLD8
		TREG		WORD
			[ZANDB]	[AL],[LO CMASK]
			[ZORB]	[AL],[LO R]
		[IFV]	[ZMOVB]	[LO R],[AL]

%	PEFFECT|PRVALUE|P_SRT
	UWORD		AX	*	AX	TEMP
		ADR|LV		FFLD8
		TREG		WORD
			[ZANDB]	[AL],[LO CMASK]
			[ZORB]	[AL],[LO R]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZAND]	[R],[LO EMASK]

/////////
/
/ Floating point.
/ Immediate moves and direct/direct moves are special.
/ Otherwise, load the "fpac" and move from it.
/ We know that the "fpac" is actually memory!!
/
/ If IEEE, a runtime routine handles 32 bit float assignment.
/ If DECVAX, we compile it inline because it is easy.
/
/////////

#ifndef NDPDEF
#ifndef IEEE
%	PEFFECT
	FF64		AX	*	*	NONE
		ADR|LV		FF32
		0|MMX		*
			[ZSUB]	[R],[R]
			[ZMOV]	[HI AL],[R]
			[ZMOV]	[LO AL],[R]

%	PEFFECT
	FF64		NONE	*	*	NONE
		ADR|LV		FF32
		IMM|MMX		FF64
			[ZMOV]	[HI AL],[LO LO AR]
			[ZMOV]	[LO AL],[HI LO AR]

%	PEFFECT|PRVALUE|P_SRT
	FF64		AX	*	FPAC	FPAC
		ADR|LV		FF32
		TREG		FF64
%	PEFFECT|PRVALUE|P_SRT
	FF64		AX	*	FPAC	FPAC
		ADR|LV		FF32
		TREG		FF32
			[ZMOV]	[R],[HI LO RR]
			[ZMOV]	[HI AL],[R]
			[ZMOV]	[R],[LO LO RR]
			[ZMOV]	[LO AL],[R]

#endif

%	PEFFECT
	FF64		AX	*	*	NONE
		ADR|LV		FF64
		0|MMX		*
			[ZSUB]	[R],[R]
			[ZMOV]	[HI LO AL],[R]
			[ZMOV]	[LO LO AL],[R]
			[ZMOV]	[HI HI AL],[R]
			[ZMOV]	[LO HI AL],[R]

%	PEFFECT
	FF64		NONE	*	*	NONE
		ADR|LV		FF64
		IMM|MMX		FF64
			[ZMOV]	[HI LO AL],[LO LO AR]
			[ZMOV]	[LO LO AL],[HI LO AR]
			[ZMOV]	[HI HI AL],[LO HI AR]
			[ZMOV]	[LO HI AL],[HI HI AR]

%	PEFFECT|PRVALUE|P_SRT
	FF64		AX	*	FPAC	FPAC
		ADR|LV		FF64
		TREG		FF64
			[ZMOV]	[R],[HI LO RR]
			[ZMOV]	[HI LO AL],[R]
			[ZMOV]	[R],[LO LO RR]
			[ZMOV]	[LO LO AL],[R]
			[ZMOV]	[R],[HI HI RR]
			[ZMOV]	[HI HI AL],[R]
			[ZMOV]	[R],[LO HI RR]
			[ZMOV]	[LO HI AL],[R]
#endif
#ifdef  NDPDEF
%	PEFFECT|P_SRT
	FF64		FPAC	*	FPAC	NONE
		ADR|LV		FF32
		TREG		FF64
%	PEFFECT|P_SRT
	FF64		FPAC	*	FPAC	NONE
		ADR|LV		FF32
		TREG		FF32
			[ZFSTPF] [AL]

%	PRVALUE|P_SRT
	FF64		FPAC	*	FPAC	FPAC
		ADR|LV		FF32
		TREG		FF64
%	PRVALUE|P_SRT
	FF64		FPAC	*	FPAC	FPAC
		ADR|LV		FF32
		TREG		FF32
			[ZFSTF]	[AL]

%	PEFFECT|P_SRT
	FF64		FPAC	*	FPAC	NONE
		ADR|LV		FF64
		TREG		FF64
			[ZFSTPD] [AL]

%	PRVALUE|P_SRT
	FF64		FPAC	*	FPAC	FPAC
		ADR|LV		FF64
		TREG		FF64
			[ZFSTD]	[AL]
#endif
