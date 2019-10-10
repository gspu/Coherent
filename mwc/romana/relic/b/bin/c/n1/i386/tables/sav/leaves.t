//////////
/ n1/i386/tables/leaves.t
//////////

/////////
/
/ Leaf nodes, fixups, type conversions and casts,
/ all performed by a single table.
/ Table of contents:
/	1. Loads.
/	2. Widens.
/	3. Shrinks.
/	4. Tests.
/	5. Function arguments.
/	6. Cleanups
/ These tables MUST be correct and complete, since they get used
/ to fix up any missing entries in the other tables.
/ Work on them carefully!
/ The compiler is not very good at detecting errors or omissions here.
/ A missing fixup entry usually makes the compiler run out of stack
/ as the code generator calls itself again and again.
/
/////////

CONVERT:
CAST:
FIXUP:
LEAF:

/////////
/
/ 1. Loads.
/
/////////

/ Load a constant 0 (long, word or byte) or NULL (pointer).
/ The subtract instruction is the shortest and fastest way to zero a register.
%	PVALUE
	NFLT		ANYR	*	*	TEMP
		0|MMX		*
		*		*
			[ZSUB]	[R],[R]

/ Load an effective address.
%	PVALUE
	DWORD		ANYR	*	*	TEMP
		LEA|MMX		DWORD
		*		*
			[ZLEA]	[R],[NSE AL]

/ Load a dword.
%	PVALUE
	DWORD		ANYR	*	*	TEMP
		ADR|IMM		DWORD
		*		*
/ Load a word.
%	PVALUE
	WORD		ANYR	*	*	TEMP
		ADR|IMM		WORD
		*		*
/ Load a byte.
%	PVALUE|PBYTE
	BYTE		ANYR	*	*	TEMP
		ADR|IMM		BYTE
		*		*
			[TN ZMOV]	[TN R],[AL]

#ifndef	NDPDEF

/ DECVAX or IEEE software floating point loads.
/ Double immediate load.
%	PVALUE|PIEEE|PDECVAX
	FF64		EDXEAX	*	*	TEMP
		IMM|MMX		FF64
		*		*
			[ZMOV]	[HI R],[HI AL]
			[ZMOV]	[LO R],[LO AL]

/ Double load.
/ The bogus left arg spec EDX prevents EDX from being used
/ for addressing the left arg, which would make the second fetch invalid.
%	PVALUE|PIEEE|PDECVAX
	FF64		EDXEAX	EDX	*	TEMP
		ADR		FF64
		*		*
			[ZMOV]	[HI R],[HI AL]
			[ZMOV]	[LO R],[LO AL]

/ The float load actually loads 64 bits.
/ It exists for much the same reasons as the byte and word loads.
/ DECVAX format: 0-extend the float to double.
%	PVALUE|PDECVAX
	FLOAT		EDXEAX	*	*	TEMP
		ADR|IMM		FF32
		*		*
			[ZMOV]	[HI R],[AL]
			[ZMOV]	[LO R],[CONST 0]

/ IEEE format: convert float to double.
%	PVALUE|PIEEE
	FLOAT		EDXEAX	*	*	TEMP
		ADR|IMM		FF32
		*		*
			[ZMOV]	[LO R],[AL]
			[ZCALL]	[GID _dfcvt]

#endif

#ifdef	NDPDEF 

/ Floating point loads using the numeric data coprocessor (80x87).
/ Load floating 0.
%	PVALUE|PNDP
	FF64		FPAC	*	*	FPAC
		0|MMX		*
		*		*
			[ZFLDZ]

/ Load floating 1.
%	PVALUE|PNDP
	FF64		FPAC	*	*	FPAC
		1|MMX		*
		*		*
			[ZFLD1]

/ General floating point load.
%	PVALUE|PNDP
	FF64		FPAC	*	*	FPAC
		ADR		FF64
		*		*
			[ZFLDD]	[AL]
#endif

/////////
/
/ 2. Widens.
/
/////////

/ Integer to integer widen conversions.
/ If [AL] is register, e.g. EAX (meaning word AX or byte AL),
/ the ZMOV[SZ]X generated below looks wrong (using EAX rather than AX or AL),
/ but it gets cleaned up ex post facto by n2/i386/asm.c/asm().
/ This is a kludge but at the moment I see no way around it.

/ Widen signed to signed using sign-extension.
/ Widen signed short to signed dword.
%	PVALUE|PEFFECT
	FS32		ANYR	*	*	TEMP
		ADR		FS8|FS16
		*		*
/ Widen signed byte to signed word.
%	PVALUE|PEFFECT
	FS16		ANYR	*	*	TEMP
		ADR		FS8
		*		*
			[TL ZMOVSX]	[R],[AL]

/ Widen when unsigned on either side using zero-extension.
/ Widen short to dword.
%	PVALUE|PEFFECT
	DWORD		ANYR	*	*	TEMP
		ADR		SHORT
		*		*
/ Widen byte to word.
%	PVALUE|PEFFECT
	WORD		ANYR	*	*	TEMP
		ADR		BYTE
		*		*
			[TL ZMOVZX]	[R],[AL]

#ifndef	NDPDEF

/ DECVAX or IEEE software floating point.
/ Widen signed immediate dword to double.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FF64		EDXEAX	*	*	TEMP
		IMM|MMX		FS32
		*		*
			[ZMOV]		[LO R],[AL]
			[ZCALL]		[GID _dicvt]

/ Widen signed integer to double.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FF64		EDXEAX	*	*	TEMP
		ADR		FS8|FS16|FS32
		*		*
			[TL ZMOVSX]	[LO R],[AL]
			[ZCALL]		[GID _dicvt]

/ Widen unsigned immediate dword to double.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FF64		EDXEAX	*	*	TEMP
		IMM|MMX		FU32|PTX
		*		*
			[ZMOV]		[LO R],[AL]
			[ZCALL]		[GID _ducvt]

/ Widen unsigned integer to double.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FF64		EDXEAX	*	*	TEMP
		ADR		FU8|FU16|FU32|PTX
		*		*
			[TL ZMOVZX]	[LO R],[AL]
			[ZCALL]		[GID _ducvt]

/ Widen float to double.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FF64		EDXEAX	*	*	TEMP
		ADR|IMM		FF32
		*		*
			[ZMOV]		[LO R],[AL]
			[ZCALL]		[GID _dfcvt]

#endif

#ifdef	NDPDEF

/ Widen to NDP float or double.
%	PVALUE|PNDP
	FLOAT		FPAC	*	*	FPAC
		ADR		FS16|FS32|FF32
		*		*
			[TL ZFLDD] [AL]

#endif

/////////
/
/ 3. Shrinks.
/ The word or byte value is simply the LO or LO LO part of the register.
/
/////////

/ To word.
/ Immediate to word.
%	PVALUE|PEFFECT
	WORD		ANYR	*	*	TEMP
		IMM|MMX		DWORD
		*		*
			[ZMOVW]	[LO R],[LO AL]

/ Dword to word.
%	PVALUE|PEFFECT|P_SLT
	WORD		ANYR	ANYR	*	TEMP
		TREG		DWORD
		*		*
			;

/ Direct dword to word.
%	PVALUE|PEFFECT
	WORD		ANYR	*	*	TEMP
		DIR|MMX		DWORD
		*		*
			[ZMOVW]	[LO R],[AL]

/ Addressible dword to word.
/ Since directs are handled above, the arg must be register.
%	PVALUE|PEFFECT
	WORD		ANYR	*	*	TEMP
		ADR		DWORD
		*		*
			[ZMOV]	[R],[AL]

/ To byte.
/ Immediate to byte.
%	PVALUE|PEFFECT
	BYTE		ANYR	*	*	TEMP
		IMM|MMX		DWORD
		*		*
			[ZMOVB]	[LO LO R],[LO LO AL]

/ Dword or word to byte.
%	PVALUE|PEFFECT|P_SLT
	BYTE		ANYR	ANYR	*	TEMP
		TREG		WORD|DWORD
		*		*
			;

/ Direct word or dword to byte.
%	PVALUE|PEFFECT
	BYTE		ANYR	*	*	TEMP
		DIR|MMX		WORD|DWORD
		*		*
			[ZMOVB]	[LO LO R],[AL]

/ Addressible word or dword to byte.
/ Since directs are handled above, the arg must be register.
%	PVALUE|PEFFECT
	BYTE		ANYR	*	*	TEMP
		ADR		WORD|DWORD
		*		*
			[ZMOV]	[R],[AL]

#ifndef	NDPDEF

/ IEEE or DECVAX software floating point.
/ The bogus left arg spec EDX prevents EDX from being used
/ for addressing the left arg, which would make the second fetch invalid.
/ Shrink double to signed long integer.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FS32		EDXEAX	EDX	*	EAX
		ADR|IMM		FF64
		*		*
			[ZMOV]	[REGNO EDX], [HI AL]
			[ZMOV]	[REGNO EAX], [LO AL]
			[ZCALL]	[GID _idcvt]

/ Shrink double to signed word or byte.
/ Cf. comment above re EDX.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FS16|FS8	EDXEAX	EDX	*	EAX
		ADR|IMM		FF64
		*		*
			[ZMOV]		[REGNO EDX], [HI AL]
			[ZMOV]		[REGNO EAX], [LO AL]
			[ZCALL]		[GID _idcvt]
			[TN ZMOVSX]	[REGNO EAX], [TN REGNO EAX]

/ Shrink double to unsigned long integer.
/ Cf. comment above re EDX.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FU32		EDXEAX	EDX	*	EAX
		ADR|IMM		FF64
		*		*
			[ZMOV]	[REGNO EDX], [HI AL]
			[ZMOV]	[REGNO EAX], [LO AL]
			[ZCALL]	[GID _udcvt]

/ Shrink double to unsigned long word or byte.
/ Cf. comment above re EDX.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FU16|FU8	EDXEAX	EDX	*	EAX
		ADR|IMM		FF64
		*		*
			[ZMOV]		[REGNO EDX], [HI AL]
			[ZMOV]		[REGNO EAX], [LO AL]
			[ZCALL]		[GID _udcvt]
			[TN ZMOVZX]	[REGNO EAX], [TN REGNO EAX]

/ Shrink float to signed long integer.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FS32		EAX	*	*	TEMP
		ADR|IMM		FF32
		*		*
			[ZMOV]	[R], [AL]
			[ZCALL]	[GID _ifcvt]

/ Shrink float to signed word or byte.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FS16|FS8	EAX	*	*	TEMP
		ADR|IMM		FF32
		*		*
			[ZMOV]		[R], [AL]
			[ZCALL]		[GID _ifcvt]
			[TN ZMOVSX]	[R], [TN R]

/ Shrink float to unsigned long integer.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FU32		EAX	*	*	TEMP
		ADR|IMM		FF32
		*		*
			[ZMOV]	[R], [AL]
			[ZCALL]	[GID _ufcvt]

/ Shrink float to unsigned word or byte.
%	PVALUE|PEFFECT|PDECVAX|PIEEE
	FU16|FU8	EAX	*	*	TEMP
		ADR|IMM		FF32
		*		*
			[ZMOV]		[R], [AL]
			[ZCALL]		[GID _ufcvt]
			[TN ZMOVZX]	[R], [TN R]

#endif

/////////
/
/ 4. Test contexts.
/ These handle checking of leaf nodes and transform the result
/ of a PVALUE match into a match that generates a jump.
/
/////////

/ Register.
/ Long.
%	PSREL
	LONG		NONE	*	*	NONE
		REG|MMX		LONG
		*		*
/ Pointer (equality only).
%	PEREL
	PTX		*	*	*	NONE
		REG|MMX		PTX
		*		*
			[ZOR]	[AL],[AL]
			[REL0]	[LAB]

/ Addressible.
/ Long.
%	PSREL
	LONG		NONE	*	*	NONE
		ADR		LONG
		*		*
/ Pointer.
%	PREL
	PTX		NONE	*	*	NONE
		ADR		PTX
		*		*
			[ZCMP]	[AL],[CONST 0]
			[REL0]	[LAB]

/ Short.
%	PSREL
	*		NONE	*	*	NONE
		ADR		SHORT
		*		*
			[TL ZCMP]	[AL],[CONST 0]
			[REL0]		[LAB]

/ Unsigned >= is always true.
%	PUGE
	*		NONE	*	*	NONE
		*		*
		*		*
			[ZJMP]	[LAB]

/ Unsigned < is always false.
%	PULT
	*		NONE	*	*	NONE
		*		*
		*		*
			;

/ Long.
%	PSREL
	LONG		NONE	ANYR	*	NONE
		TREG		LONG
		*		*
/ Pointer.
%	PREL
	PTX		NONE	ANYR	*	NONE
		TREG		PTX
		*		*
			[ZOR]	[RL],[RL]
			[REL0]	[LAB]

#ifndef	NDPDEF

/ Double, IEEE software floating point.
%	PREL|PIEEE
	FF64		NONE	*	*	NONE
		ADR		FF64
		*		*
			[ZTEST]	[HI AL],[CONST 0x7FF00000]
			[REL0]	[LAB]

/ Double, DECVAX software floating point.
%	PREL|PDECVAX
	FF64		NONE	*	*	NONE
		ADR		FF64
		*		*
			[ZTEST]	[HI AL],[CONST 0x7F800000]
			[REL0]	[LAB]

/ Float, IEEE or DECVAX software floating point.
%	PREL|PIEEE|PDECVAX
	FLOAT		NONE	*	*	NONE
		ADR		FF32
		*		*
			[ZTEST]	[AL],[CONST 0x7F800000]
			[REL0]	[LAB]

#endif

#ifdef	NDPDEF

/ NDP floating point.
%	PREL|PNDP
	FF64		*	*	*	NONE
		REG|MMX		FF64
		*		*
			[ZCALL]	[GID tstccp]
			[REL0]	[LAB]
#endif

/////////
/ 
/ 5. Push function arguments onto the stack.
/
/////////

/ Effective address.
%	PFNARG
	DWORD		ANYR	*	*	NONE
		LEA|MMX		DWORD
		*		*
			[ZLEA]	[R],[NSE AL]
			[ZPUSH]	[R]

/ Immediate.
%	PFNARG
	DWORD		NONE	*	*	NONE
		IMM|MMX		DWORD
		*		*
/ Addressible.
%	PFNARG
	DWORD	NONE	*	*	NONE
		ADR		DWORD
		*		*
			[ZPUSH]	[AL]

//////////
/ Floating point.
//////////

#ifndef	NDPDEF

/ IEEE or DECVAX software fp.
/ Push FPAC.
%	PFNARG|PIEEE|PDECVAX
	FF64		EDXEAX	EDXEAX	*	NONE
		REG|MMX		FF64
		*		*
			[ZPUSH]	[HI RL]
			[ZPUSH]	[LO RL]

/ Push addressible double.
%	PFNARG|PIEEE|PDECVAX
	FF64		NONE	*	*	NONE
		ADR		FF64
		*		*
			[ZPUSH]	[HI AL]
			[ZPUSH]	[LO AL]

#endif

#ifdef	NDPDEF

/ Hardware coprocessor (NDP) floating point.
/ Push double from NDP stacktop.
%	PFNARG|PNDP
	FF64		NONE	*	*	NONE
		REG|MMX		FF64
		*		*
			[ZCALL]	[GID dp87]

#endif

////////
/
/ 6. Miscellaneous cleanups.
/ Transform value contexts into effect contexts.
/ The empty entries are needed because a CALL node always produces a value.
/
////////

/ Ignore structs used for effect.
%	PEFFECT
	FBLK		NONE	*	*	NONE
		ADR|IMM		FBLK
		*		*
			;

/ Ignore dword, word, byte for effect.
%	PEFFECT
	NFLT		NONE	*	*	NONE
		REG|MMX		NFLT
		*		*
			;

#ifndef NDPDEF

/ Ignore software fp double for effect.
%	PEFFECT|PIEEE|PDECVAX
	FF64		NONE	*	*	NONE
		REG|MMX		FF64
		*		*
			;
#endif

#ifdef	NDPDEF

/ Clean the 80x87 stack.
%	PEFFECT|PNDP
	FF64		NONE	*	*	NONE
		REG|MMX		FF64
		*		*
			[ZFDROP]

#endif

//////////
/ end of n1/i386/tables/leaves.t
//////////
