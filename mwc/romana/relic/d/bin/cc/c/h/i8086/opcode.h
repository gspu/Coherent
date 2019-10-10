/*
 * Opcode index definitions for the iAPX-86.
 * These are used in CODE file items and are indicies into opcode tables
 * in n2/i8086/optab.c and n3/i8086/icode.c.
 * Modifications must be made in parallel.
 * Be very careful if you change any of these.
 */

/*
 * Common Intel 8086 instructions.
 * Words and bytes. Also flow of control and
 * segmentation.
 */
#define	ZAAA	0
#define	ZAAD	1
#define	ZAAM	2
#define	ZAAS	3
#define	ZADC	4
#define	ZADCB	5
#define	ZADD	6
#define	ZADDB	7
#define	ZAND	8
#define	ZANDB	9
#define	ZCALL	10
#define	ZCBW	11
#define	ZCLC	12
#define	ZCLD	13
#define	ZCLI	14
#define	ZCMC	15
#define	ZCMP	16
#define	ZCMPB	17
#define	ZCMPS	18
#define	ZCMPSB	19
#define	ZCWD	20
#define	ZDAA	21
#define	ZDAS	22
#define	ZDEC	23
#define	ZDECB	24
#define	ZDIV	25
#define	ZDIVB	26
#define	ZHLT	27
#define	ZICALL	28
#define	ZIDIV	29
#define	ZIDIVB	30
#define	ZIJMP	31
#define	ZIMUL	32
#define	ZIMULB	33
#define	ZIN	34
#define	ZINB	35
#define	ZINC	36
#define	ZINCB	37
#define	ZINT	38
#define	ZINTO	39
#define	ZIRET	40
#define	ZIXCALL	41
#define	ZIXJMP	42
#define	ZJA	43
#define	ZJAE	44
#define	ZJB	45
#define	ZJBE	46
#define	ZJCXZ	47
#define	ZJE	48
#define	ZJG	49
#define	ZJGE	50
#define	ZJL	51
#define	ZJLE	52
#define	ZJMP	53
#define	ZJNE	54
#define	ZJNO	55
#define	ZJNS	56
#define	ZJO	57
#define	ZJPE	58
#define	ZJPO	59
#define	ZJS	60
#define	ZLAHF	61
#define	ZLDS	62
#define	ZLEA	63
#define	ZLES	64
#define	ZLOCK	65
#define	ZLODS	66
#define	ZLODSB	67
#define	ZLOOP	68
#define	ZLOOPE	69
#define	ZLOOPNE	70
#define	ZMOV	71
#define	ZMOVB	72
#define	ZMOVS	73
#define	ZMOVSB	74
#define	ZMUL	75
#define	ZMULB	76
#define	ZNEG	77
#define	ZNEGB	78
#define	ZNOT	79
#define	ZNOTB	80
#define	ZNRET	81
#define	ZNXRET	82
#define	ZOR	83
#define	ZORB	84
#define	ZOUT	85
#define	ZOUTB	86
#define	ZPOP	87
#define	ZPOPF	88
#define	ZPUSH	89
#define	ZPUSHF	90
#define	ZRCL	91
#define	ZRCLB	92
#define	ZRCR	93
#define	ZRCRB	94
#define	ZREPE	95
#define	ZREPNE	96
#define	ZRET	97
#define	ZROL	98
#define	ZROLB	99
#define	ZROR	100
#define	ZRORB	101
#define	ZSAHF	102
#define	ZSAL	103
#define	ZSALB	104
#define	ZSAR	105
#define	ZSARB	106
#define	ZSBB	107
#define	ZSBBB	108
#define	ZSCAS	109
#define	ZSCASB	110
#define	ZSHL	111
#define	ZSHLB	112
#define	ZSHR	113
#define	ZSHRB	114
#define	ZSTC	115
#define	ZSTD	116
#define	ZSTI	117
#define	ZSTOS	118
#define	ZSTOSB	119
#define	ZSUB	120
#define	ZSUBB	121
#define	ZTEST	122
#define	ZTESTB	123
#define	ZWAIT	124
#define	ZXCALL	125
#define	ZXCHG	126
#define	ZXCHGB	127
#define	ZXJMP	128
#define	ZXLAT	129
#define	ZXOR	130
#define	ZXORB	131
#define	ZXRET	132
#define	ZBYTE	133
#define	ZWORD	134
#define	ZLPTR	135
#define	ZGPTR	136

/*
 * Opcodes for the Intel 8087 numeric
 * data processor. Not an exhaustive list.
 * Only the ones we use. The list must 
 * stop below 200, as this is the coder macro
 * base.
 */
#define	ZFADD	137
#define	ZFADDF	138
#define	ZFADDD	139
#define	ZFADDI	140
#define	ZFADDL	141
#define	ZFSUB	142
#define	ZFSUBF	143
#define	ZFSUBD	144
#define	ZFSUBI	145
#define	ZFSUBL	146
#define	ZFRSUB	147
#define	ZFRSUBF	148
#define	ZFRSUBD	149
#define	ZFRSUBI	150
#define	ZFRSUBL	151
#define	ZFCOMPP	152
#define	ZFCOMPF	153
#define	ZFCOMPD	154
#define	ZFCOMPI	155
#define	ZFCOMPL	156
#define	ZFMUL	157
#define	ZFMULF	158
#define	ZFMULD	159
#define	ZFMULI	160
#define	ZFMULL	161
#define	ZFDIV	162
#define	ZFDIVF	163
#define	ZFDIVD	164
#define	ZFDIVI	165
#define	ZFDIVL	166
#define	ZFRDIV	167
#define	ZFRDIVF	168
#define	ZFRDIVD	169
#define	ZFRDIVI	170
#define	ZFRDIVL	171
#define	ZFCHS	172
#define	ZFTST	173
#define	ZFLDZ	174
#define	ZFLD1	175
#define	ZFSTSW	176
#define	ZFLDF	177
#define	ZFLDD	178
#define	ZFLDI	179
#define	ZFLDL	180
#define	ZFLDT	181
#define	ZFSTF	182
#define	ZFSTD	183
#define	ZFSTI	184
#define	ZFSTL	185
#define	ZFSTPF	186
#define	ZFSTPD	187
#define	ZFSTPI	188
#define	ZFSTPL	189
#define	ZFSTPT	190
#define	ZFWAIT	191
#define	ZFDROP	192
#define	ZFSQRT	193
#define	ZFABS	194
#define	ZFPREM	195
#define	ZFRND	196

/*
 * Specials.
 */
#define	ZLDES	197
#define ZIMULI	198
#define ZENTER	199
#define ZLEAVE	200	/* Not in code tables, so excess of 199 ok */
