/*
 * h/i386/opcode.h
 * Opcode index definitions for the i386.
 * Used in CODE items.
 * These are indicies into tables in n2/i386/optab.c and n3/i386/icode.c,
 * modifications must be made in parallel.
 * The list must stop below 200, as this is the coder macro base.
 * Be very careful if you change any of these.
 * i386.
 */

/*
 * Common Intel i386 instructions.
 * Dwords, words and bytes;
 * also flow of control and segmentation.
 * This is by no means a complete list of i386 opcodes.
 * Some are unused and could be deleted.
 * n1/i386/outmch.c assumes a fixed order [dword, byte, word] for variants.
 */
#define	ZADC	0
#define	ZADD	1
#define	ZADDB	2
#define	ZADDW	3
#define	ZAND	4
#define	ZANDB	5
#define	ZANDW	6
#define	ZCALL	7
#define	ZCBW	8
#define	ZCDQ	9
#define	ZCLC	10
#define	ZCLD	11
#define	ZCLI	12
#define	ZCMC	13
#define	ZCMP	14
#define	ZCMPB	15
#define	ZCMPW	16
#define	ZCMPS	17
#define	ZCMPSB	18
#define	ZCMPSW	19
#define	ZCWD	20
#define	ZCWDE	21
#define	ZDEC	22
#define	ZDECB	23
#define	ZDECW	24
#define	ZDIV	25
#define	ZDIVB	26
#define	ZDIVW	27
#define ZENTER	28
#define	ZHLT	29
#define	ZICALL	30
#define	ZIDIV	31
#define	ZIDIVB	32
#define	ZIDIVW	33
#define	ZIJMP	34
#define	ZIMUL	35
#define	ZIMULB	36
#define	ZIMULW	37
#define ZIMULI	38	/* Multiply immediate */
#define	ZINC	39
#define	ZINCB	40
#define	ZINCW	41
#define	ZIRET	42
#define	ZIXCALL	43
#define	ZIXJMP	44
#define	ZJA	45
#define	ZJAE	46
#define	ZJB	47
#define	ZJBE	48
#define	ZJCXZ	49
#define	ZJE	50
#define	ZJG	51
#define	ZJGE	52
#define	ZJL	53
#define	ZJLE	54
#define	ZJMP	55
#define	ZJNE	56
#define	ZJNO	57
#define	ZJNS	58
#define	ZJO	59
#define	ZJPE	60
#define	ZJPO	61
#define	ZJS	62
#define	ZLEA	63
#define ZLEAVE	64
#define	ZLOCK	65
#define	ZLODS	66
#define	ZLODSB	67
#define	ZLOOP	68
#define	ZLOOPE	69
#define	ZLOOPNE	70
#define	ZMOV	71
#define	ZMOVB	72
#define	ZMOVW	73
#define	ZMOVS	74
#define	ZMOVSB	75
#define	ZMOVSX	76
#define	ZMOVSXB	77
#define	ZMOVZX	78
#define	ZMOVZXB	79
#define	ZMUL	80
#define	ZMULB	81
#define	ZMULW	82
#define	ZNEG	83
#define	ZNEGB	84
#define	ZNEGW	85
#define	ZNOT	86
#define	ZNOTB	87
#define	ZNOTW	88
#define	ZNRET	89
#define	ZNXRET	90
#define	ZOR	91
#define	ZORB	92
#define	ZORW	93
#define	ZPOP	94
#define	ZPOPF	95
#define	ZPUSH	96
#define	ZPUSHF	97
#define	ZRCL	98
#define	ZRCLB	99
#define	ZRCR	100
#define	ZRCRB	101
#define	ZREPE	102
#define	ZREPNE	103
#define	ZRET	104
#define	ZROL	105
#define	ZROLB	106
#define	ZROR	107
#define	ZRORB	108
#define	ZSAL	109
#define	ZSALB	110
#define	ZSALW	111
#define	ZSAR	112
#define	ZSARB	113
#define	ZSARW	114
#define	ZSBB	115
#define	ZSCAS	116
#define	ZSCASB	117
#define	ZSHL	118
#define	ZSHLB	119
#define	ZSHLW	120
#define	ZSHR	121
#define	ZSHRB	122
#define	ZSHRW	123
#define	ZSTC	124
#define	ZSTD	125
#define	ZSTI	126
#define	ZSTOS	127
#define	ZSTOSB	128
#define	ZSUB	129
#define	ZSUBB	130
#define	ZSUBW	131
#define	ZTEST	132
#define	ZTESTB	133
#define	ZTESTW	134
#define	ZWAIT	135
#define	ZXCALL	136
#define	ZXCHG	137
#define	ZXCHGB	138
#define	ZXJMP	139
#define	ZXOR	140
#define	ZXORB	141
#define	ZXORW	142
#define	ZXRET	143

/*
 * Specials.
 */
#define	ZBYTE	144
#define	ZWORD	145
#define	ZLONG	146
#define	ZLPTR	147
#define	ZGPTR	148

/* Unused opcodes, retained for no good reason. */
#if	0
#define	ZAAA	xx
#define	ZAAD	xx
#define	ZAAM	xx
#define	ZAAS	xx
#define	ZADCB	xx
#define	ZDAA	xx
#define	ZDAS	xx
#define	ZIN	xx
#define	ZINB	xx
#define	ZINT	xx
#define	ZINTO	xx
#define	ZLAHF	xx
#define	ZLDS	xx
#define	ZLES	xx
#define	ZOUT	xx
#define	ZOUTB	xx
#define	ZSAHF	xx
#define	ZSBBB	xx
#define	ZXLAT	xx
#endif

#if	1
/*
 * Opcodes for the Intel 80x87 numeric data processor (NDP).
 * Not an exhaustive list, mostly just the ones we use.
 * n1/i386/outmch.c assumes a fixed order [double, float, dword, word] for variants.
 */
#define	ZFADDD	149
#define	ZFADDF	150
#define	ZFADDL	151
#define	ZFADDI	152
#define	ZFSUBD	153
#define	ZFSUBF	154
#define	ZFSUBL	155
#define	ZFSUBI	156
#define	ZFRSUBD	157
#define	ZFRSUBF	158
#define	ZFRSUBL	159
#define	ZFRSUBI	160
#define	ZFCOMPP	161
#define	ZFCOMPD	162
#define	ZFCOMPF	163
#define	ZFCOMPL	164
#define	ZFCOMPI	165
#define	ZFMULD	166
#define	ZFMULF	167
#define	ZFMULL	168
#define	ZFMULI	169
#define	ZFDIVD	170
#define	ZFDIVF	171
#define	ZFDIVL	172
#define	ZFDIVI	173
#define	ZFRDIVD	174
#define	ZFRDIVF	175
#define	ZFRDIVL	176
#define	ZFRDIVI	177
#define	ZFCHS	178
#define	ZFTST	179
#define	ZFLDZ	180
#define	ZFLD1	181
#define	ZFSTSW	182
#define	ZFLDD	183
#define	ZFLDF	184
#define	ZFLDL	185
#define	ZFLDI	186
#define	ZFLDT	187
#define	ZFSTD	188
#define	ZFSTF	189
#define	ZFSTL	190
#define	ZFSTI	191
#define	ZFSTPD	192
#define	ZFSTPF	193
#define	ZFSTPL	194
#define	ZFSTPI	195
#define	ZFSTPT	196
#define	ZFWAIT	197
#define	ZFDROP	198
#define	is_ndp_op(op)	(ZFADDD <= (op) && (op) <= ZFDROP)

/* Unused... */
#if	0
#define	ZFADD	xx
#define	ZFSUB	xx
#define	ZFRSUB	xx
#define	ZFMUL	xx
#define	ZFDIV	xx
#define	ZFRDIV	xx
#endif

#endif

/* h/i386/opcode.h */
