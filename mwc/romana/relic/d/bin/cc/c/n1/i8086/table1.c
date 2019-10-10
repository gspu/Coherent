/*
 * This file contains all of the machine specific tables used
 * by the coder half of the Intel iAPX-86 code generator.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * Width table.
 */
char	wtype[]	= {
	0,			/* S8 */
	0,			/* U8 */
	1,			/* S16 */
	1,			/* U16 */
	2,			/* S32 */
	2,			/* U32 */
	3,			/* F32 */
	4,			/* F64 */
	5,			/* BLK */
	0,			/* FLD8 */
	1,			/* FLD16 */
	2,			/* LPTR */
	2,			/* LPTB */
	1,			/* SPTR */
	1			/* SPTB */
};

/*
 * Register table.
 * Indexed by 'real' register numbers (as defined in mch.h).
 */
REGDESC	reg[]	= {
/*		KIND	 KIND	  int	   REGNAME  REGNAME  REGNAME  PREGSET	*/
/*		r_lvalue r_rvalue r_goal   r_enpair r_hihalf r_lohalf r_phys	*/
/* ax   */	0,	 KWB|KSP, MRVALUE,    DXAX,   AH,	AL,	BAX,
/* dx   */	0,	 KWB|KSP, MRVALUE,    DXAX,   DH,	DL,	BDX,
/* bx   */	KWB|KSP, KWB|KSP, MRVALUE,    CXBX,   BH,	BL,	BBX,
/* cx   */	0, 	 KWB|KSP, MRVALUE,    CXBX,   CH,	CL,	BCX,
/* si   */	KW|KSP,	 KW|KSP,  MRVALUE,      -1,   -1,	-1,	BSI,
/* di   */	KW|KSP,	 KW|KSP,  MRVALUE,      -1,   -1,	-1,	BDI,
/* sp   */	0,	 0,	       -1,      -1,   -1,	-1,	BSP,
/* bp   */	KW|KSP,  0,	       -1,      -1,   -1,	-1,	BBP,
/* fpac */	0,	 KD,	  MRVALUE,      -1,   -1,	-1,	BFPAC,
/* es   */	0,	 0,	       -1,      -1,   -1,	-1,	BES,
/* cs   */	0,	 0,	       -1,      -1,   -1,	-1,	BCS,
/* ss   */	0,	 0,            -1,      -1,   -1,	-1,	BSS,
/* ds   */	0,	 0,            -1,      -1,   -1,	-1,	BDS,
/* dxax */	0,       KL|KLP,  MRVALUE,      -1,   DX,	AX,	BDX|BAX,
/* cxbx */	KL,      KL|KLP,  MRVALUE,      -1,   CX,	BX,	BCX|BBX,
/* sssp */	0,	 0,	       -1,      -1,   SS,	SP,	BSS|BSP,
/* ssbp */	0,	 0,	       -1,      -1,   SS,	BP,	BSS|BBP,
/* esax */	0,	 KLP,     MLVALUE,      -1,   ES,	AX,	BES|BAX,
/* esbx */	KLP,	 0,       MLVALUE,      -1,   ES,	BX,	BES|BBX,
/* essi */	KLP,	 0,       MLVALUE,      -1,   ES,	SI,	BES|BSI,
/* esdi */	KLP,     0,       MLVALUE,      -1,   ES,	DI,	BES|BDI,
/* dsax */	0,	 KLP,     MLVALUE,      -1,   DS,	AX,	BDS|BAX,
/* dsbx */	KLP,	 0,       MLVALUE,      -1,   DS,	BX,	BDS|BBX,
/* dssi */	KLP,	 0,       MLVALUE,      -1,   DS,	SI,	BDS|BSI,
/* dsdi */	KLP,     0,       MLVALUE,      -1,   DS,	DI,	BDS|BDI,
/* al   */	0,	 0,            -1,      AX,   -1,	-1,	BAX,
/* bl   */	0,       0,	       -1,      BX,   -1,	-1,	BBX,
/* cl   */	0,	 0,	       -1,      CX,   -1,	-1,	BCX,
/* dl   */	0,       0,	       -1,      DX,   -1,	-1,	BDX,
/* ah   */	0,	 0,            -1,      AX,   -1,	-1,	BAX,
/* bh   */	0,	 0,	       -1,      BX,   -1,	-1,	BBX,
/* ch   */	0,	 0,	       -1,      CX,   -1,	-1,	BCX,
/* dh   */	0,	 0, 	       -1,      DX,   -1,	-1,	BDX
/* None */
/* Anyr */
/* Anyl */
/* Pair */
/* Temp */
/* Lo */
/* Hi */
};

/*
 * The opcode table.
 * Indexed by operation.
 * The 0th and 1st entries of the relations are used by REL0 and by REL1.
 * The 2nd entry is the special supress EQ/NE entry used by LREL1 and LREL0.
 */
unsigned char	optab[][3] = {

	{ ZADD,	ZINC,	ZADC },
	{ ZSUB,	ZDEC,	ZSBB },
	{ ZMUL,	0,	0 },
	{ ZDIV,	0,	0 },
	{ ZDIV,	0,	0 },
	{ ZAND,	ZTEST,	ZSUB },
	{ ZOR,	0,	0 },
	{ ZXOR,	ZNOT,	0 },
	{ ZSAL,	ZSHL,	ZRCL },
	{ ZSAR,	ZSHR,	ZRCR },

	{ ZADD,	ZINC,	ZADC },
	{ ZSUB,	ZDEC,	ZSBB },
	{ ZMUL,	ZMOV,	ZMOVB },
	{ ZDIV,	ZMOV,	ZMOVB },
	{ ZDIV,	ZMOV,	ZMOVB },
	{ ZAND,	ZSUB,	0 },
	{ ZOR,	0,	0 },
	{ ZXOR,	ZNOT,	0 },
	{ ZSAL,	ZSHL,	ZRCL },
	{ ZSAR,	ZSHR,	ZRCR },

	{ ZJE,	ZJE,	0 },
	{ ZJNE,	ZJNE,	0 },
	{ ZJG,	0,	ZJG },
	{ ZJGE,	ZJNS,	ZJG },
	{ ZJLE,	0,	ZJL },
	{ ZJL,	ZJS,	ZJL },
	{ ZJA,	0,	ZJA },
	{ ZJAE,	0,	ZJA },
	{ ZJBE,	0,	ZJB },
	{ ZJB,	0,	ZJB },

	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ ZNEG,	ZSBB,	0 },
	{ ZNOT,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ ZADD,	ZINC,	ZADC },
	{ ZSUB,	ZDEC,	ZSBB },
	{ ZADD,	ZINC,	ZADC },
	{ ZSUB,	ZDEC,	ZSBB },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ ZMOV,	ZSUB,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 },
	{ 0,	0,	0 }
};

/*
 * Per type info.
 * Column 0 is function return register (-1 if never a return type).
 * Column 1 is function return context, for select.
 * Column 2 is size of a temp.
 * Column 3 is the actual size in memory, for autoinc/autodec.
 * Column 4 is bit set for searching pattern tables.
 * Column 5 is register kind.
 * Column 6 is register kind for a pair.
 */
PERTYPE	pertype[] = {
	/*	REGNAME	char		char	char	TYPESET	KIND	KIND	*/
	/*	p_frreg	p_frcat		p_size	p_incr	p_type	p_kind	p_pair	*/
	{	AX,	MRVALUE,	2,	1,	FS8,	KB,	KL },
	{	AX,	MRVALUE,	2,	1,	FU8,	KB,	KL },
	{	AX,	MRVALUE,	2,	2,	FS16,	KW,	KL },
	{	AX,	MRVALUE,	2,	2,	FU16,	KW,	KL },
	{	DXAX,	MRVALUE,	4,	4,	FS32,	KL,	0  },
	{	DXAX,	MRVALUE,	4,	4,	FU32,	KL,	0  },
	{	-1,	MRVALUE,	8,	8,	FF32,	0,	0  },
	{	FPAC,	MRVALUE,	8,	8,	FF64,	KD,	0  },
	{	-1,	MRVALUE,	0,	0,	FBLK,	0,	0  },
	{	-1,	MRVALUE,	0,	0,	FFLD8,	0,	0  },
	{	-1,	MRVALUE,	0,	0,	FFLD16,	0,	0  },
	{	DXAX,	MRVALUE,	4,	4,	FLPTR,	KLP,	0  },
	{	DXAX,	MRVALUE,	4,	4,	FLPTB,	KLP,	0  },
	{	AX,	MRVALUE,	2,	2,	FSPTR,	KSP,	0  },
	{	AX,	MRVALUE,	2,	2,	FSPTB,	KSP,	0  }
};

/*
 * These tables adjust relational ops when the
 * sense is reversed or when the subtrees are swapped.
 */
char	fliprel[] = {
	EQ, NE, LT, LE, GE, GT, ULT, ULE, UGE, UGT
};

char	otherel[] = {
	NE, EQ, LE, LT, GT, GE, ULE, ULT, UGT, UGE
};

/*
 * Type table.
 * Indexed by machine type.
 * Used by type testing macros defined in cc1mch.h.
 */
char	tinfo[]	= {
	060,	064,		/* S8,    U8,    */
	022,	026,		/* S16,   U16,   */
	021,	025,		/* S32,   U32,   */
	010,	010,		/* F32,   F64,   */
	0100,			/* BLK,          */
	060,			/* FLD8,         */
	020,			/* FLD16,        */
	0205,	0305,		/* LPTR,  LPTRB, */
	0226,	0326		/* SPTR,  SPTRB  */
};
