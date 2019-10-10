#ifdef vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif
char macros[] = {
/* /newbits/bin/cc/c/n1/i8086/tables/prefac.f */
/* /newbits/bin/cc/c/n1/i8086/tables/aadd.t */
/*    0 */
	ZINC, M_AL,
	M_IFV, ZMOV, M_R, M_AL, M_ENDIF,
	M_IFR, M_REL0, M_LAB, M_ENDIF,
	M_END,
/*   12 */
	ZINCB, M_AL,
	M_IFV, ZMOVB, M_LO, M_R, M_AL, M_ENDIF,
	M_IFV, ZCBW,
	M_JMP1, 0x06,		/* 6 */
/*   24 */
	ZINCB, M_AL,
	M_IFV, ZMOVB, M_LO, M_R, M_AL, M_ENDIF,
	M_IFV, ZSUBB, M_HI, M_R, M_HI, M_R,
	M_JMP1, 0x06,		/* 6 */
/*   40 */
	ZADD, M_AL, M_AR,
	M_JMP1, 0x02,		/* 2 */
/*   45 */
	M_JMP1, 0x28,		/* 40 */
/*   47 */
	ZADDB, M_AL, M_AR,
	M_JMP1, 0x0E,		/* 14 */
/*   52 */
	ZADDB, M_AL, M_AR,
	M_JMP1, 0x1A,		/* 26 */
/*   57 */
	ZADD, M_HI, M_AL, M_HI, M_AR,
	M_IFV, ZMOV, M_LO, M_R, M_LO, M_AL, M_ENDIF,
	M_IFV, ZMOV, M_HI, M_R, M_HI, M_AL, M_ENDIF,
	M_IFR, M_REL1,
	M_JMP1, 0x09,		/* 9 */
/*   80 */
	ZADD, M_LO, M_AL, M_LO, M_AR,
	ZADC,
	M_JMP1, 0x3A,		/* 58 */
/*   88 */
	ZADD, M_AL, M_R,
	M_JMP1, 0x02,		/* 2 */
/*   93 */
	ZADDB, M_AL, M_LO, M_R,
	M_JMP1, 0x0E,		/* 14 */
/*   99 */
	ZADDB, M_AL, M_LO, M_R,
	M_JMP1, 0x1A,		/* 26 */
/*  105 */
	ZMOV, M_R, M_AL,
	ZADD, M_R, M_AR,
	ZMOV, M_AL, M_R,
	M_JMP1, 0x07,		/* 7 */
/*  116 */
	ZMOVB, M_LO, M_R, M_AL,
	ZCBW,
	ZADD, M_R, M_AR,
	ZMOVB, M_AL, M_LO,
	M_JMP1, 0x71,		/* 113 */
/*  129 */
	ZMOVB, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_R, M_HI, M_R,
	M_JMP1, 0x79,		/* 121 */
/*  140 */
	ZMOV, M_LO, M_R, M_AR,
	ZCWD,
	ZADD, M_LO, M_AL, M_LO, M_R,
	ZADC, M_HI, M_AL, M_HI, M_R,
	M_END,
/*  156 */
	ZMOV, M_LO, M_R, M_AR,
	ZSUB, M_HI, M_R, M_HI, M_R,
	M_JMP1, 0x91,		/* 145 */
/*  167 */
	M_JMP1, 0x91,		/* 145 */
/*  169 */
	ZADD, M_LO, M_R, M_LO, M_AL,
	ZADC, M_HI, M_R, M_HI, M_AL,
	ZMOV, M_LO, M_AL, M_LO, M_R,
	ZMOV, M_HI, M_AL, M_HI, M_R,
	M_JMP1, 0x4C,		/* 76 */
/*  191 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZADD, M_LO, M_R, M_LO, M_AR,
	ZADC, M_HI, M_R, M_HI, M_AR,
	M_JMP1, 0xB3,		/* 179 */
/*  213 */
	ZMOVB, M_LO, M_R, M_AL,
	ZCBW,
	ZADD, M_R, M_AR,
	ZAND, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB, M_AL, M_R,
	M_END,
/*  233 */
	ZMOV, M_R, M_AL,
	ZADD, M_R, M_AR,
	ZAND, M_R, M_LO, M_EMASK,
	ZAND, M_AL, M_LO, M_CMASK,
	ZOR,
	M_JMP1, 0xE6,		/* 230 */
/* /newbits/bin/cc/c/n1/i8086/tables/aand.t */
/*  250 */
	ZAND,
	M_JMP1, 0x29,		/* 41 */
/*  253 */
	ZANDB,
	M_JMP1, 0x30,		/* 48 */
/*  256 */
	ZANDB,
	M_JMP1, 0x35,		/* 53 */
/*  259 */
	ZAND,
	M_JMP1, 0x59,		/* 89 */
/*  262 */
	ZANDB,
	M_JMP1, 0x5E,		/* 94 */
/*  265 */
	ZANDB,
	M_JMP1, 0x64,		/* 100 */
/*  268 */
	ZMOV, M_LO, M_AL, M_LO, M_AR,
	ZAND,
	M_JMP1, 0x3A,		/* 58 */
/*  276 */
	ZAND, M_HI, M_AL, M_HI, M_AR,
	M_IFV, ZMOV, M_LO, M_R, M_LO, M_AL, M_ENDIF,
	M_IFV, ZMOV, M_HI, M_R, M_HI, M_AL, M_ENDIF,
	M_END,
/*  296 */
	ZAND, M_LO, M_AL, M_LO, M_AR,
	ZMOV,
	M_JMPB, 0x19,		/* 277 */
/*  304 */
	ZAND, M_LO, M_AL, M_LO,
	M_JMPB, 0x1C,		/* 280 */
/*  310 */
	ZAND,
	M_JMPB, 0x2A,		/* 269 */
/*  313 */
	ZAND, M_LO, M_AL, M_LO, M_R,
	ZAND, M_HI, M_AL, M_HI, M_R,
	M_JMPB, 0x2A,		/* 281 */
/*  325 */
	ZAND, M_AL, M_AR,
	M_IFV, ZMOV, M_R,
	M_JMPB, 0x26,		/* 293 */
/*  333 */
	ZAND, M_AL, M_AR,
	M_IFV, ZMOV, M_R, M_AL, M_ENDIF,
	M_IFV, ZAND, M_R, M_LO, M_EMASK, M_ENDIF,
	M_END,
/*  348 */
	ZANDB, M_AL, M_AR,
	M_IFV, ZMOVB, M_LO,
	M_JMPB, 0x18,		/* 330 */
/*  356 */
	ZANDB, M_AL, M_AR,
	M_IFV, ZMOVB, M_LO,
	M_JMPB, 0x18,		/* 338 */
/*  364 */
	ZAND, M_AL, M_R,
	M_JMPB, 0x27,		/* 328 */
/*  369 */
	ZAND, M_AL, M_R,
	M_JMPB, 0x24,		/* 336 */
/*  374 */
	ZANDB, M_AL, M_LO, M_R,
	M_JMPB, 0x1B,		/* 351 */
/*  380 */
	ZANDB, M_AL, M_LO, M_R,
	M_JMPB, 0x19,		/* 359 */
/* /newbits/bin/cc/c/n1/i8086/tables/add.t */
/*  386 */
	ZINC,
	M_JMP1, 0x71,		/* 113 */
/*  389 */
	ZINC, M_R,
	M_JMPB, 0x05,		/* 386 */
/*  393 */
	ZADD, M_R, M_AR,
	M_JMP1, 0x07,		/* 7 */
/*  398 */
	ZADD, M_LO, M_R, M_LO, M_AR,
	ZADC, M_HI, M_R, M_HI, M_AR,
	M_JMP1, 0x4C,		/* 76 */
/* /newbits/bin/cc/c/n1/i8086/tables/adiv.t */
/*  410 */
	ZMOV, M_LO, M_R, M_AL,
	ZCWD,
	ZIDIV, M_AR,
	ZMOV, M_AL, M_LO, M_R,
	M_END,
/*  422 */
	ZMOV, M_LO, M_R, M_AL,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV,
	M_JMPB, 0x10,		/* 416 */
/*  434 */
	ZMOV, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZAND, M_AL, M_LO, M_CMASK,
	ZOR,
	M_JMPB, 0x2A,		/* 418 */
/*  462 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZCBW,
	ZCWD,
	ZIDIV, M_AR,
	ZMOVB, M_AL, M_LO, M_LO, M_R,
	M_IFV, ZCBW, M_ENDIF,
	M_END,
/*  480 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	ZCWD,
	ZDIV, M_AR,
	ZMOVB, M_AL, M_LO, M_LO, M_R,
	M_IFV, ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R, M_ENDIF,
	M_END,
/*  510 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB, M_AL, M_LO,
	M_JMPB, 0x78,		/* 419 */
/*  541 */
	ZPUSH, M_HI, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x01,
	ZADD, M_REGNO, SP, M_ICON, 0x01,
	ZMOV, M_LO, M_AL, M_LO, M_R,
	ZMOV,
	M_JMP1, 0x97,		/* 151 */
/*  569 */
	ZPUSH, M_HI, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x02,
	M_JMPB, 0x1C,		/* 556 */
/*  586 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	M_JMPB, 0x2D,		/* 547 */
/*  594 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	M_JMPB, 0x19,		/* 575 */
/* /newbits/bin/cc/c/n1/i8086/tables/aft.t */
/*  602 */
	M_IFV, ZMOV, M_R, M_AL, M_ENDIF,
	M_OP1, M_AL,
	M_END,
/*  610 */
	M_IFV, ZMOV, M_R, M_AL, M_ENDIF,
	M_OP0, M_AL, M_AR,
	M_END,
/*  619 */
	M_IFV, ZMOVB, M_LO, M_R, M_AL, M_ENDIF,
	M_IFV, ZCBW, M_ENDIF,
	M_TL,
	M_JMPB, 0x16,		/* 607 */
/*  631 */
	M_IFV, ZMOVB, M_LO, M_R, M_AL, M_ENDIF,
	M_IFV, ZSUBB, M_HI, M_R, M_HI, M_R,
	M_JMPB, 0x10,		/* 627 */
/*  645 */
	M_IFV, ZMOV, M_LO, M_R, M_LO, M_AL, M_ENDIF,
	M_IFV, ZMOV, M_HI, M_R, M_HI, M_AL, M_ENDIF,
	M_OP0, M_LO, M_AL, M_ICON, 0x02,
	M_OP2, M_HI, M_AL, M_ICON, 0x03,
	M_END,
/*  670 */
	ZMOV, M_LO, M_R, M_AL,
	ZMOV, M_HI, M_R, M_LO, M_R,
	M_OP0, M_HI, M_R, M_AR,
	ZAND, M_HI, M_R, M_LO, M_EMASK,
	ZAND, M_AL, M_LO, M_CMASK,
	ZOR, M_AL, M_HI, M_R,
	M_IFV, ZAND, M_LO,
	M_JMP2, 0x57, 0x01,	/* 343 */
/*  702 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZMOV, M_HI, M_R, M_LO, M_R,
	M_OP0, M_HI, M_R, M_AR,
	ZAND, M_HI, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB, M_AL, M_LO,
	M_JMPB, 0x22,		/* 694 */
/* /newbits/bin/cc/c/n1/i8086/tables/amul.t */
/*  730 */
	ZIMULI, M_R, M_AL, M_AR,
	ZMOV,
	M_JMP1, 0xE6,		/* 230 */
/*  737 */
	ZMOV, M_LO, M_R, M_AL,
	ZIMUL,
	M_JMP2, 0xA0, 0x01,	/* 416 */
/*  745 */
	ZMOV, M_LO, M_R, M_AL,
	ZMUL,
	M_JMP2, 0xA0, 0x01,	/* 416 */
/*  753 */
	ZMOV, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZMUL,
	M_JMP2, 0xC1, 0x01,	/* 449 */
/*  766 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZCBW,
	ZIMUL,
	M_JMP2, 0xD6, 0x01,	/* 470 */
/*  776 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	ZMUL,
	M_JMP2, 0xEE, 0x01,	/* 494 */
/*  792 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZMUL,
	M_JMP2, 0x0E, 0x02,	/* 526 */
/*  806 */
	ZPUSH, M_HI, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x03,
	M_JMP2, 0x2C, 0x02,	/* 556 */
/*  824 */
	ZPUSH, M_HI, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x04,
	M_JMP2, 0x2C, 0x02,	/* 556 */
/*  842 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	M_JMPB, 0x24,		/* 812 */
/*  850 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	M_JMPB, 0x1A,		/* 830 */
/* /newbits/bin/cc/c/n1/i8086/tables/and.t */
/*  858 */
	ZTEST, M_AL, M_AR,
	M_REL0, M_LAB,
	M_END,
/*  864 */
	ZTESTB,
	M_JMPB, 0x06,		/* 859 */
/*  867 */
	ZAND,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/*  871 */
	ZTEST, M_HI, M_AL, M_HI, M_AR,
	M_REL1, M_LAB,
	M_END,
/*  879 */
	ZMOV, M_R, M_HI, M_AL,
	ZTEST, M_R,
	M_JMPB, 0x0B,		/* 874 */
/*  887 */
	ZTEST, M_HI, M_RL,
	M_JMPB, 0x10,		/* 874 */
/*  892 */
	ZTEST, M_LO, M_AL, M_LO,
	M_JMPB, 0x24,		/* 860 */
/*  898 */
	ZTEST, M_HI, M_AL, M_HI,
	M_JMPB, 0x2A,		/* 860 */
/*  904 */
	ZTEST, M_LO, M_RL,
	M_JMPB, 0x0C,		/* 895 */
/*  909 */
	ZTEST, M_HI, M_RL,
	M_JMPB, 0x0B,		/* 901 */
/*  914 */
	ZTEST, M_LO, M_AL, M_LO, M_AR,
	ZJNE, M_LAB,
	ZTEST, M_HI, M_AL, M_HI, M_AR,
	ZJNE, M_LAB,
	M_END,
/*  929 */
	ZTEST, M_LO, M_RL, M_LO, M_AR,
	ZJNE, M_LAB,
	ZTEST, M_HI, M_RL,
	M_JMPB, 0x0F,		/* 924 */
/*  941 */
	ZTEST, M_LO, M_AL, M_LO, M_AR,
	ZJNE, M_LAB0,
	ZTEST, M_HI, M_AL, M_HI, M_AR,
	ZJE, M_LAB,
	M_DLAB0,
	M_END,
/*  957 */
	ZTEST, M_LO, M_RL, M_LO, M_AR,
	ZJNE, M_LAB0,
	ZTEST, M_HI, M_RL,
	M_JMPB, 0x10,		/* 951 */
/*  969 */
	ZAND, M_LO, M_R, M_LO, M_AR,
	M_END,
/*  975 */
	ZAND, M_LO, M_R, M_LO, M_AR,
	ZSUB, M_HI, M_R,
	M_JMP1, 0x99,		/* 153 */
/*  985 */
	ZAND, M_HI, M_R, M_HI, M_AR,
	M_END,
/*  991 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	M_JMPB, 0x0B,		/* 985 */
/*  998 */
	ZAND, M_LO, M_R, M_LO, M_AR,
	M_JMPB, 0x07,		/* 996 */
/* /newbits/bin/cc/c/n1/i8086/tables/aor.t */
/* 1005 */
	ZOR,
	M_JMP1, 0x29,		/* 41 */
/* 1008 */
	ZORB,
	M_JMP1, 0x30,		/* 48 */
/* 1011 */
	ZORB,
	M_JMP1, 0x35,		/* 53 */
/* 1014 */
	ZOR,
	M_JMP1, 0x59,		/* 89 */
/* 1017 */
	ZORB,
	M_JMP1, 0x5E,		/* 94 */
/* 1020 */
	ZORB,
	M_JMP1, 0x64,		/* 100 */
/* 1023 */
	ZOR,
	M_JMP1, 0x3A,		/* 58 */
/* 1026 */
	ZOR,
	M_JMP2, 0x31, 0x01,	/* 305 */
/* 1030 */
	ZOR, M_LO, M_AL, M_LO, M_AR,
	M_JMPB, 0x0C,		/* 1023 */
/* 1037 */
	ZOR, M_LO, M_AL, M_LO, M_R,
	ZOR,
	M_JMP2, 0x3F, 0x01,	/* 319 */
/* 1046 */
	ZOR,
	M_JMP2, 0x46, 0x01,	/* 326 */
/* 1050 */
	ZOR,
	M_JMP2, 0x4E, 0x01,	/* 334 */
/* 1054 */
	ZORB,
	M_JMP2, 0x5D, 0x01,	/* 349 */
/* 1058 */
	ZORB,
	M_JMP2, 0x65, 0x01,	/* 357 */
/* 1062 */
	ZOR,
	M_JMP2, 0x6D, 0x01,	/* 365 */
/* 1066 */
	ZOR,
	M_JMP2, 0x72, 0x01,	/* 370 */
/* 1070 */
	ZORB,
	M_JMP2, 0x77, 0x01,	/* 375 */
/* 1074 */
	ZORB,
	M_JMP2, 0x7D, 0x01,	/* 381 */
/* /newbits/bin/cc/c/n1/i8086/tables/arem.t */
/* 1078 */
	ZMOV, M_LO, M_R, M_AL,
	ZCWD,
	ZIDIV, M_AR,
	ZMOV,
	M_JMP1, 0x98,		/* 152 */
/* 1088 */
	ZMOV, M_LO, M_R, M_AL,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV,
	M_JMPB, 0x0E,		/* 1084 */
/* 1100 */
	ZMOV, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	ZAND, M_HI, M_R, M_LO, M_EMASK,
	ZAND, M_AL, M_LO, M_CMASK,
	ZOR,
	M_JMP1, 0x98,		/* 152 */
/* 1128 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZCBW,
	ZCWD,
	ZIDIV, M_AR,
	ZMOVB, M_AL, M_LO, M_HI, M_R,
	M_IFV, ZMOVB, M_LO, M_LO, M_R, M_LO, M_HI, M_R, M_ENDIF,
	M_JMP2, 0xDC, 0x01,	/* 476 */
/* 1154 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	ZCWD,
	ZDIV, M_AR,
	ZMOVB, M_AL, M_LO, M_HI, M_R,
	M_IFV, ZSUBB, M_HI, M_HI, M_R, M_HI, M_HI, M_R, M_ENDIF,
	M_END,
/* 1184 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	ZAND, M_HI, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB, M_AL, M_LO,
	M_JMP1, 0x99,		/* 153 */
/* 1215 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x05,
	M_JMP2, 0x2C, 0x02,	/* 556 */
/* 1233 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x06,
	M_JMP2, 0x2C, 0x02,	/* 556 */
/* /newbits/bin/cc/c/n1/i8086/tables/ashl.t */
/* 1251 */
	ZSAL,
	M_JMP1, 0x29,		/* 41 */
/* 1254 */
	ZSALB,
	M_JMP1, 0x30,		/* 48 */
/* 1257 */
	ZSALB,
	M_JMP1, 0x35,		/* 53 */
/* 1260 */
	ZSAL, M_AL, M_ICON, 0x02,
	M_JMP1, 0x02,		/* 2 */
/* 1266 */
	ZSALB, M_AL, M_ICON, 0x02,
	M_JMP1, 0x0E,		/* 14 */
/* 1272 */
	ZSALB, M_AL, M_ICON, 0x02,
	M_JMP1, 0x1A,		/* 26 */
/* 1278 */
	ZSAL, M_AL, M_ICON, 0x02,
	M_JMPB, 0x16,		/* 1260 */
/* 1284 */
	ZSALB, M_AL, M_ICON, 0x02,
	M_JMPB, 0x16,		/* 1266 */
/* 1290 */
	ZSALB, M_AL, M_ICON, 0x02,
	M_JMPB, 0x16,		/* 1272 */
/* 1296 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAL, M_AL, M_REGNO, CL,
	M_JMP1, 0x02,		/* 2 */
/* 1306 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSALB, M_AL, M_REGNO, CL,
	M_JMP1, 0x0E,		/* 14 */
/* 1316 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSALB, M_AL, M_REGNO, CL,
	M_JMP1, 0x1A,		/* 26 */
/* 1326 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_DLAB0, ZSAL, M_LO, M_R, M_ICON, 0x02,
	ZRCL, M_HI, M_R, M_ICON, 0x02,
	ZLOOP, M_LAB0,
	M_JMP1, 0xB3,		/* 179 */
/* 1355 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSAL, M_AL, M_REGNO, CL,
	M_JMP2, 0x48, 0x01,	/* 328 */
/* 1366 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSALB, M_AL, M_REGNO, CL,
	M_IFV, ZMOVB, M_LO, M_R, M_AL,
	M_JMPB, 0xE5,		/* 1150 */
/* 1381 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSALB, M_AL, M_REGNO, CL,
	M_IFV, ZMOVB, M_LO, M_R, M_AL, M_ENDIF,
	M_IFV, ZSUBB, M_HI, M_R,
	M_JMPB, 0xDB,		/* 1180 */
/* 1401 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSAL, M_LO, M_R, M_ICON, 0x02,
	ZRCL, M_HI, M_R, M_ICON, 0x02,
	ZLOOP, M_LAB1,
	ZMOV, M_LO, M_AL, M_LO, M_R,
	ZMOV, M_HI, M_AL, M_HI, M_R,
	M_DLAB0,
	M_END,
/* 1442 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZSHL,
	M_JMP1, 0xED,		/* 237 */
/* 1452 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZSHLB, M_LO, M_R, M_AR,
	ZAND, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB,
	M_JMP2, 0xA2, 0x01,	/* 418 */
/* 1477 */
	M_JMPB, 0x23,		/* 1442 */
/* 1479 */
	M_JMPB, 0x1B,		/* 1452 */
/* 1481 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZSHL, M_R, M_ICON, 0x02,
	ZSHL, M_R, M_ICON, 0x02,
	M_JMP1, 0xEF,		/* 239 */
/* 1498 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZSHLB, M_LO, M_R, M_ICON, 0x02,
	ZSHLB, M_LO, M_R, M_ICON, 0x02,
	M_JMPB, 0x34,		/* 1465 */
/* 1519 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHL, M_R, M_REGNO, CL,
	M_JMP1, 0xEF,		/* 239 */
/* 1536 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHLB, M_LO, M_R, M_REGNO, CL,
	M_JMPB, 0x59,		/* 1465 */
/* 1556 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x25,		/* 1529 */
/* 1568 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x20,		/* 1548 */
/* /newbits/bin/cc/c/n1/i8086/tables/ashr.t */
/* 1582 */
	ZSAR,
	M_JMP1, 0x29,		/* 41 */
/* 1585 */
	ZSHR,
	M_JMP1, 0x29,		/* 41 */
/* 1588 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZSHR,
	M_JMP1, 0xED,		/* 237 */
/* 1598 */
	ZSARB,
	M_JMP1, 0x30,		/* 48 */
/* 1601 */
	ZSHRB,
	M_JMP1, 0x35,		/* 53 */
/* 1604 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZSHRB,
	M_JMPB, 0x98,		/* 1462 */
/* 1616 */
	M_JMPB, 0x22,		/* 1582 */
/* 1618 */
	M_JMPB, 0x21,		/* 1585 */
/* 1620 */
	M_JMPB, 0x20,		/* 1588 */
/* 1622 */
	M_JMPB, 0x18,		/* 1598 */
/* 1624 */
	M_JMPB, 0x17,		/* 1601 */
/* 1626 */
	M_JMPB, 0x16,		/* 1604 */
/* 1628 */
	ZSAR, M_HI, M_AL, M_AR,
	ZRCR, M_LO, M_AL,
	M_JMP2, 0x18, 0x01,	/* 280 */
/* 1638 */
	ZSHR,
	M_JMPB, 0x0A,		/* 1629 */
/* 1641 */
	ZSAR, M_AL, M_ICON, 0x02,
	ZSAR,
	M_JMP2, 0xED, 0x04,	/* 1261 */
/* 1649 */
	ZSHR, M_AL, M_ICON, 0x02,
	ZSHR,
	M_JMP2, 0xED, 0x04,	/* 1261 */
/* 1657 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZSHR, M_R, M_ICON, 0x02,
	ZSHR,
	M_JMPB, 0xB0,		/* 1493 */
/* 1671 */
	ZSARB, M_AL, M_ICON, 0x02,
	ZSARB,
	M_JMP2, 0xF3, 0x04,	/* 1267 */
/* 1679 */
	ZSHRB, M_AL, M_ICON, 0x02,
	ZSHRB,
	M_JMP2, 0xF9, 0x04,	/* 1273 */
/* 1687 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZSHRB, M_LO, M_R, M_ICON, 0x02,
	ZSHRB,
	M_JMPB, 0xBD,		/* 1513 */
/* 1704 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAR,
	M_JMP2, 0x15, 0x05,	/* 1301 */
/* 1712 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHR,
	M_JMP2, 0x15, 0x05,	/* 1301 */
/* 1720 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHR,
	M_JMPB, 0xC9,		/* 1531 */
/* 1734 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSARB,
	M_JMP2, 0x1F, 0x05,	/* 1311 */
/* 1742 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHRB,
	M_JMP2, 0x29, 0x05,	/* 1321 */
/* 1750 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHRB,
	M_JMPB, 0xD6,		/* 1550 */
/* 1766 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_DLAB0, ZSAR, M_HI, M_R, M_ICON, 0x02,
	ZRCR, M_LO,
	M_JMP2, 0x44, 0x05,	/* 1348 */
/* 1791 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_DLAB0, ZSHR,
	M_JMPB, 0x19,		/* 1782 */
/* 1809 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSAR,
	M_JMP2, 0x50, 0x05,	/* 1360 */
/* 1817 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSHR,
	M_JMP2, 0x50, 0x05,	/* 1360 */
/* 1825 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x69,		/* 1730 */
/* 1837 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSARB,
	M_JMP2, 0x5B, 0x05,	/* 1371 */
/* 1845 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSHRB,
	M_JMP2, 0x6A, 0x05,	/* 1386 */
/* 1853 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x67,		/* 1762 */
/* 1867 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSAR, M_HI, M_R, M_ICON, 0x02,
	ZRCR, M_LO,
	M_JMP2, 0x91, 0x05,	/* 1425 */
/* 1894 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSHR,
	M_JMPB, 0x1B,		/* 1885 */
/* /newbits/bin/cc/c/n1/i8086/tables/assign.t */
/* 1914 */
	ZSUB, M_AL, M_AL,
	M_END,
/* 1918 */
	ZMOV, M_AL, M_AR,
	M_END,
/* 1922 */
	ZMOV, M_AL, M_LO, M_AR,
	M_END,
/* 1927 */
	ZLEA, M_AL, M_NSE, M_AR,
	M_END,
/* 1932 */
	ZMOVB, M_AL, M_AR,
	M_END,
/* 1936 */
	ZMOV, M_LO, M_AL, M_LO, M_AR,
	ZMOV, M_HI, M_AL, M_HI, M_AR,
	M_END,
/* 1947 */
	ZMOVB, M_AL, M_LO, M_R,
	M_IFR, ZORB, M_LO, M_R, M_LO,
	M_JMP1, 0x25,		/* 37 */
/* 1958 */
	ZMOVB, M_AL, M_LO, M_R,
	ZCBW,
	M_END,
/* 1964 */
	ZMOVB, M_AL, M_LO, M_R,
	M_IFV, ZSUBB, M_HI, M_R, M_HI, M_R, M_ENDIF,
	M_JMPB, 0x18,		/* 1951 */
/* 1977 */
	ZMOVB, M_LO, M_R, M_AR,
	ZMOVB, M_AL, M_LO, M_R,
	M_IFV, ZCBW,
	M_JMPB, 0x0D,		/* 1974 */
/* 1989 */
	ZMOVB, M_LO, M_R, M_AR,
	M_JMPB, 0x1D,		/* 1964 */
/* 1995 */
	M_JMP2, 0xDE, 0x02,	/* 734 */
/* 1998 */
	ZMOV, M_R,
	M_JMP2, 0xDD, 0x02,	/* 733 */
/* 2003 */
	ZMOVB, M_AL,
	M_JMP2, 0xDA, 0x01,	/* 474 */
/* 2008 */
	ZMOVB, M_AL, M_LO, M_R,
	M_JMP2, 0x73, 0x05,	/* 1395 */
/* 2015 */
	M_JMP2, 0x31, 0x02,	/* 561 */
/* 2018 */
	M_JMP2, 0xDE, 0x02,	/* 734 */
/* 2021 */
	M_JMP2, 0xD7, 0x01,	/* 471 */
/* 2024 */
	M_JMP2, 0xEF, 0x01,	/* 495 */
/* 2027 */
	ZAND, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x16, 0x04,	/* 1046 */
/* 2034 */
	ZAND, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x1A, 0x04,	/* 1050 */
/* 2041 */
	ZAND, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x26, 0x04,	/* 1062 */
/* 2048 */
	ZAND, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x2A, 0x04,	/* 1066 */
/* 2055 */
	ZANDB, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x2E, 0x04,	/* 1070 */
/* 2062 */
	ZANDB, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x32, 0x04,	/* 1074 */
/* 2069 */
	ZSUB, M_R, M_R,
	ZMOV, M_HI, M_AL, M_R,
	ZMOV, M_LO,
	M_JMP1, 0xE6,		/* 230 */
/* 2080 */
	ZMOV, M_HI, M_AL, M_LO, M_LO, M_AR,
	ZMOV, M_LO, M_AL, M_HI, M_LO, M_AR,
	M_END,
/* 2093 */
	ZMOV, M_R, M_HI, M_LO, M_RR,
	ZMOV, M_HI, M_AL, M_R,
	ZMOV, M_R, M_LO, M_LO, M_RR,
	M_JMPB, 0x1F,		/* 2076 */
/* 2109 */
	ZSUB, M_R, M_R,
	ZMOV, M_HI, M_LO, M_AL, M_R,
	ZMOV, M_LO, M_LO, M_AL, M_R,
	ZMOV, M_HI, M_HI, M_AL, M_R,
	ZMOV, M_LO, M_HI,
	M_JMP1, 0xE6,		/* 230 */
/* 2132 */
	ZMOV, M_HI, M_LO, M_AL, M_LO, M_LO, M_AR,
	ZMOV, M_LO, M_LO, M_AL, M_HI, M_LO, M_AR,
	ZMOV, M_HI, M_HI, M_AL, M_LO, M_HI, M_AR,
	ZMOV, M_LO, M_HI, M_AL, M_HI, M_HI, M_AR,
	M_END,
/* 2161 */
	ZMOV, M_R, M_HI, M_LO, M_RR,
	ZMOV, M_HI, M_LO, M_AL, M_R,
	ZMOV, M_R, M_LO, M_LO, M_RR,
	ZMOV, M_LO, M_LO, M_AL, M_R,
	ZMOV, M_R, M_HI, M_HI, M_RR,
	ZMOV, M_HI, M_HI, M_AL, M_R,
	ZMOV, M_R, M_LO, M_HI, M_RR,
	M_JMPB, 0x45,		/* 2127 */
/* /newbits/bin/cc/c/n1/i8086/tables/asub.t */
/* 2198 */
	ZDEC,
	M_JMP1, 0x01,		/* 1 */
/* 2201 */
	ZDECB,
	M_JMP1, 0x0D,		/* 13 */
/* 2204 */
	ZDECB,
	M_JMP1, 0x19,		/* 25 */
/* 2207 */
	ZSUB,
	M_JMP1, 0x29,		/* 41 */
/* 2210 */
	M_JMPB, 0x03,		/* 2207 */
/* 2212 */
	ZSUBB,
	M_JMP1, 0x30,		/* 48 */
/* 2215 */
	ZSUBB,
	M_JMP1, 0x35,		/* 53 */
/* 2218 */
	ZSUB,
	M_JMP1, 0x3A,		/* 58 */
/* 2221 */
	ZSUB, M_LO, M_AL, M_LO, M_AR,
	ZSBB,
	M_JMP1, 0x3A,		/* 58 */
/* 2229 */
	ZSUB,
	M_JMP1, 0x59,		/* 89 */
/* 2232 */
	ZSUBB,
	M_JMP1, 0x5E,		/* 94 */
/* 2235 */
	ZSUBB,
	M_JMP1, 0x64,		/* 100 */
/* 2238 */
	ZMOV, M_R, M_AL,
	ZSUB,
	M_JMP1, 0x6D,		/* 109 */
/* 2244 */
	ZMOVB, M_LO, M_R, M_AL,
	ZCBW,
	ZSUB,
	M_JMP1, 0x7A,		/* 122 */
/* 2252 */
	ZMOVB, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_R, M_HI, M_R,
	M_JMPB, 0x0C,		/* 2249 */
/* 2263 */
	ZMOV, M_LO, M_R, M_AR,
	ZCWD,
	ZSUB, M_LO, M_AL, M_LO, M_R,
	ZSBB,
	M_JMP1, 0x97,		/* 151 */
/* 2276 */
	ZMOV, M_LO, M_R, M_AR,
	ZSUB, M_HI, M_R, M_HI, M_R,
	M_JMPB, 0x11,		/* 2268 */
/* 2287 */
	M_JMPB, 0x13,		/* 2268 */
/* 2289 */
	ZSUB, M_LO, M_AL, M_LO, M_R,
	ZSBB, M_HI, M_AL, M_HI, M_R,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_JMP1, 0x4C,		/* 76 */
/* 2311 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZSUB, M_LO, M_R, M_LO, M_AR,
	ZSBB,
	M_JMP1, 0xCF,		/* 207 */
/* 2329 */
	ZMOVB, M_LO, M_R, M_AL,
	ZCBW,
	ZSUB,
	M_JMP1, 0xDB,		/* 219 */
/* 2337 */
	ZMOV, M_R, M_AL,
	ZSUB,
	M_JMP1, 0xED,		/* 237 */
/* /newbits/bin/cc/c/n1/i8086/tables/axor.t */
/* 2343 */
	ZXOR,
	M_JMP1, 0x29,		/* 41 */
/* 2346 */
	ZXORB,
	M_JMP1, 0x30,		/* 48 */
/* 2349 */
	ZXORB,
	M_JMP1, 0x35,		/* 53 */
/* 2352 */
	ZXOR,
	M_JMP1, 0x59,		/* 89 */
/* 2355 */
	ZXORB,
	M_JMP1, 0x5E,		/* 94 */
/* 2358 */
	ZXORB,
	M_JMP1, 0x64,		/* 100 */
/* 2361 */
	ZXOR,
	M_JMP1, 0x3A,		/* 58 */
/* 2364 */
	ZNOT, M_LO, M_AL,
	M_JMPB, 0x06,		/* 2361 */
/* 2369 */
	ZXOR,
	M_JMP2, 0x31, 0x01,	/* 305 */
/* 2373 */
	ZXOR, M_LO, M_AL, M_LO, M_AR,
	ZNOT, M_HI, M_AL,
	M_JMP1, 0x3E,		/* 62 */
/* 2383 */
	ZXOR, M_LO, M_AL, M_LO, M_AR,
	M_JMPB, 0x15,		/* 2367 */
/* 2390 */
	ZXOR, M_LO, M_AL, M_LO, M_R,
	ZXOR,
	M_JMP2, 0x3F, 0x01,	/* 319 */
/* 2399 */
	ZXOR,
	M_JMP2, 0x46, 0x01,	/* 326 */
/* 2403 */
	ZXOR,
	M_JMP2, 0x4E, 0x01,	/* 334 */
/* 2407 */
	ZXORB,
	M_JMP2, 0x5D, 0x01,	/* 349 */
/* 2411 */
	ZXORB,
	M_JMP2, 0x65, 0x01,	/* 357 */
/* 2415 */
	ZXOR,
	M_JMP2, 0x6D, 0x01,	/* 365 */
/* 2419 */
	ZXOR,
	M_JMP2, 0x72, 0x01,	/* 370 */
/* 2423 */
	ZXORB,
	M_JMP2, 0x77, 0x01,	/* 375 */
/* 2427 */
	ZXORB,
	M_JMP2, 0x7D, 0x01,	/* 381 */
/* /newbits/bin/cc/c/n1/i8086/tables/bef.t */
/* 2431 */
	M_OP1,
	M_JMP1, 0x01,		/* 1 */
/* 2434 */
	M_OP0,
	M_JMP1, 0x29,		/* 41 */
/* 2437 */
	M_TL, M_OP1,
	M_JMP1, 0x0D,		/* 13 */
/* 2441 */
	M_TL, M_OP1,
	M_JMP1, 0x19,		/* 25 */
/* 2445 */
	M_OP0, M_LO, M_AL, M_ICON, 0x02,
	M_OP2, M_HI, M_AL, M_ICON, 0x03,
	M_JMP1, 0x3E,		/* 62 */
/* 2457 */
	ZMOV, M_R, M_AL,
	M_OP0,
	M_JMP1, 0xED,		/* 237 */
/* 2463 */
	ZMOVB, M_LO, M_R, M_AL,
	M_OP0,
	M_JMP2, 0xB7, 0x05,	/* 1463 */
/* /newbits/bin/cc/c/n1/i8086/tables/div.t */
/* 2471 */
	ZCWD,
	ZIDIV, M_AR,
	M_END,
/* 2475 */
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	M_END,
/* /newbits/bin/cc/c/n1/i8086/tables/leaves.t */
/* 2483 */
	ZSUB, M_R, M_R,
	M_END,
/* 2487 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZMOV, M_HI, M_R, M_LO, M_R,
	M_END,
/* 2498 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	M_JMP2, 0xD4, 0x03,	/* 980 */
/* 2506 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_END,
/* 2517 */
	ZLEA, M_R, M_NSE, M_AL,
	M_END,
/* 2522 */
	ZMOV, M_R, M_AL,
	M_END,
/* 2526 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	M_JMPB, 0x14,		/* 2511 */
/* 2533 */
	ZMOV, M_REGNO, AX, M_HI, M_AL,
	ZMOV, M_HI, M_LO, M_R, M_REGNO, AX,
	ZMOV, M_REGNO, AX, M_LO, M_AL,
	ZMOV, M_LO, M_LO, M_R, M_REGNO, AX,
	ZMOV, M_HI, M_HI, M_R, M_ICON, 0x03,
	ZMOV, M_LO, M_HI, M_R, M_ICON, 0x03,
	M_END,
/* 2568 */
	ZMOV, M_HI, M_LO, M_R, M_LO, M_LO, M_AL,
	ZMOV, M_LO, M_LO, M_R, M_HI, M_LO, M_AL,
	ZMOV, M_HI, M_HI, M_R, M_LO, M_HI, M_AL,
	ZMOV, M_LO, M_HI, M_R, M_HI,
	M_JMPB, 0x50,		/* 2514 */
/* 2596 */
	ZMOV, M_REGNO, AX, M_HI, M_LO, M_AL,
	ZMOV, M_HI, M_LO, M_R, M_REGNO, AX,
	ZMOV, M_REGNO, AX, M_LO, M_LO, M_AL,
	ZMOV, M_LO, M_LO, M_R, M_REGNO, AX,
	ZMOV, M_REGNO, AX, M_HI, M_HI, M_AL,
	ZMOV, M_HI, M_HI, M_R, M_REGNO, AX,
	ZMOV, M_REGNO, AX, M_LO, M_HI, M_AL,
	ZMOV, M_LO, M_HI, M_R, M_REGNO, AX,
	M_END,
/* 2645 */
	ZMOV, M_R, M_AL,
	ZCBW,
	M_END,
/* 2650 */
	ZMOVB, M_LO, M_R, M_AL,
	M_JMP2, 0xDC, 0x01,	/* 476 */
/* 2657 */
	ZMOVB, M_LO, M_R, M_AL,
	M_JMP2, 0x73, 0x05,	/* 1395 */
/* 2664 */
	M_END,
/* 2665 */
	ZMOV, M_LO, M_R, M_AL,
	ZCWD,
	M_END,
/* 2671 */
	ZCWD,
	M_END,
/* 2673 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZCBW,
	ZCWD,
	M_END,
/* 2681 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	ZCWD,
	M_END,
/* 2695 */
	M_JMPB, 0x1E,		/* 2665 */
/* 2697 */
	ZMOV, M_LO, M_R,
	M_JMPB, 0xC6,		/* 2502 */
/* 2702 */
	ZMOV, M_R, M_LO, M_AL,
	M_END,
/* 2707 */
	M_END,
/* 2708 */
	M_JMPB, 0x06,		/* 2702 */
/* 2710 */
	ZMOV, M_R, M_LO,
	M_JMPB, 0x42,		/* 2647 */
/* 2715 */
	ZMOV, M_R, M_LO, M_AL,
	ZSUBB,
	M_JMP2, 0xD5, 0x03,	/* 981 */
/* 2723 */
	M_JMPB, 0x0D,		/* 2710 */
/* 2725 */
	M_JMPB, 0x0A,		/* 2715 */
/* 2727 */
	ZCBW,
	M_END,
/* 2729 */
	M_END,
/* 2730 */
	M_JMPB, 0x55,		/* 2645 */
/* 2732 */
	ZMOV, M_R,
	M_JMPB, 0x10,		/* 2718 */
/* 2736 */
	ZMOVB, M_LO, M_R, M_HI,
	M_JMPB, 0x5D,		/* 2647 */
/* 2742 */
	ZMOVB, M_LO, M_R, M_HI,
	M_JMPB, 0x1C,		/* 2718 */
/* 2748 */
	ZCBW,
	M_END,
/* 2750 */
	M_JMPB, 0x1F,		/* 2719 */
/* 2752 */
	ZOR, M_AL, M_AL,
	M_REL0, M_LAB,
	M_END,
/* 2758 */
	ZCMP, M_AL, M_ICON, 0x03,
	M_REL0, M_LAB,
	M_END,
/* 2765 */
	ZCMPB,
	M_JMPB, 0x07,		/* 2759 */
/* 2768 */
	ZOR, M_LO, M_AL, M_HI,
	M_JMPB, 0x12,		/* 2754 */
/* 2774 */
	ZMOV, M_R, M_LO, M_AL,
	ZOR, M_R,
	M_JMPB, 0x09,		/* 2771 */
/* 2782 */
	ZOR, M_HI, M_AL, M_HI, M_AL,
	M_REL1, M_LAB,
	M_END,
/* 2790 */
	ZCMP, M_HI, M_AL, M_ICON, 0x03,
	M_REL1, M_LAB,
	M_END,
/* 2798 */
	ZOR, M_HI, M_AL, M_HI, M_AL,
	ZJS, M_LAB,
	ZJNE, M_LAB0,
	ZOR, M_LO, M_AL, M_LO, M_AL,
	M_JMP2, 0xB9, 0x03,	/* 953 */
/* 2815 */
	ZCMP, M_HI, M_AL, M_ICON, 0x03,
	ZJS, M_LAB,
	ZJNE, M_LAB0,
	ZCMP, M_LO, M_AL, M_ICON, 0x03,
	M_JMP2, 0xB9, 0x03,	/* 953 */
/* 2832 */
	ZOR, M_HI, M_AL, M_HI, M_AL,
	ZJS, M_LAB0,
	ZJNE, M_LAB,
	ZOR, M_LO, M_AL, M_LO, M_AL,
	ZJNE, M_LAB,
	M_DLAB0,
	M_END,
/* 2850 */
	ZCMP, M_HI, M_AL, M_ICON, 0x03,
	ZJS, M_LAB0,
	ZJNE, M_LAB,
	ZCMP, M_LO, M_AL, M_ICON, 0x03,
	M_JMPB, 0x12,		/* 2846 */
/* 2866 */
	ZJMP, M_LAB,
	M_END,
/* 2869 */
	M_END,
/* 2870 */
	ZOR, M_RL, M_RL,
	M_REL0, M_LAB,
	M_END,
/* 2876 */
	M_CALL, M_GID, 0x07,
	M_REL0, M_LAB,
	M_END,
/* 2882 */
	ZTEST, M_HI, M_LO, M_AL, M_ICON, 0x04,
	M_REL0, M_LAB,
	M_END,
/* 2891 */
	ZTEST, M_HI,
	M_JMPB, 0x08,		/* 2885 */
/* 2895 */
	ZPUSH, M_AL,
	M_END,
/* 2898 */
	ZSUB, M_R, M_R,
	ZPUSH, M_R,
	M_END,
/* 2904 */
	ZMOV, M_R, M_AL,
	M_JMPB, 0x06,		/* 2901 */
/* 2909 */
	ZLEA, M_R, M_NSE,
	M_JMPB, 0x06,		/* 2906 */
/* 2914 */
	M_JMPB, 0x13,		/* 2895 */
/* 2916 */
	ZPUSH, M_HI, M_AL,
	ZPUSH,
	M_JMPB, 0xD8,		/* 2704 */
/* 2922 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_LO, M_R,
	M_END,
/* 2934 */
	ZMOV, M_LO, M_R, M_HI, M_AL,
	ZPUSH, M_LO, M_R,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	M_JMPB, 0x11,		/* 2930 */
/* 2949 */
	M_JMPB, 0x21,		/* 2916 */
/* 2951 */
	M_CALL, M_GID, 0x08,
	M_END,
/* 2955 */
	ZPUSH, M_HI, M_LO, M_AL,
	ZPUSH, M_LO, M_LO, M_AL,
	ZPUSH, M_HI, M_HI, M_AL,
	ZPUSH, M_LO, M_HI, M_AL,
	M_END,
/* 2972 */
	M_END,
/* 2973 */
	M_END,
/* 2974 */
	M_END,
/* /newbits/bin/cc/c/n1/i8086/tables/mul.t */
/* 2975 */
	ZIMULI, M_R, M_AL, M_AR,
	M_END,
/* 2980 */
	ZIMULI, M_R, M_AR, M_AL,
	M_END,
/* 2985 */
	ZIMUL, M_AR,
	M_END,
/* 2988 */
	ZMUL, M_AR,
	M_END,
/* /newbits/bin/cc/c/n1/i8086/tables/neg.t */
/* 2991 */
	ZNEG,
	M_JMP1, 0x71,		/* 113 */
/* 2994 */
	ZNEG, M_HI, M_R,
	ZNEG, M_LO, M_R,
	ZSBB, M_HI, M_R, M_ICON, 0x03,
	M_JMP1, 0x4C,		/* 76 */
/* 3007 */
	ZXORB, M_LO, M_HI, M_LO, M_R, M_ICON, 0x05,
	M_END,
/* /newbits/bin/cc/c/n1/i8086/tables/not.t */
/* 3015 */
	ZNOT, M_R,
	M_END,
/* 3018 */
	ZNOT, M_LO, M_R,
	ZNOT,
	M_JMP1, 0x99,		/* 153 */
/* /newbits/bin/cc/c/n1/i8086/tables/or.t */
/* 3024 */
	ZOR,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/* 3028 */
	ZOR,
	M_JMP2, 0xCA, 0x03,	/* 970 */
/* 3032 */
	ZOR,
	M_JMP2, 0x94, 0x01,	/* 404 */
/* 3036 */
	ZOR, M_LO, M_R, M_LO, M_AR,
	M_JMPB, 0x09,		/* 3032 */
/* /newbits/bin/cc/c/n1/i8086/tables/relop.t */
/* 3043 */
	ZCMPB,
	M_JMP2, 0x5B, 0x03,	/* 859 */
/* 3047 */
	ZCMP,
	M_JMP2, 0x5B, 0x03,	/* 859 */
/* 3051 */
	ZCMPB, M_LO, M_R,
	M_JMP2, 0x5C, 0x03,	/* 860 */
/* 3057 */
	ZMOVB, M_LO, M_R, M_AL,
	M_JMPB, 0x0A,		/* 3051 */
/* 3063 */
	ZCMP,
	M_JMPB, 0x0B,		/* 3053 */
/* 3066 */
	ZCMP, M_HI, M_AL, M_HI, M_AR,
	ZJNE, M_LAB1,
	ZCMP, M_LO, M_AL, M_LO, M_AR,
	ZJE, M_LAB,
	M_DLAB1,
	M_END,
/* 3082 */
	ZOR, M_HI, M_RL, M_HI, M_RL,
	ZJNE, M_LAB1,
	ZCMP, M_LO, M_RL,
	M_JMPB, 0x10,		/* 3076 */
/* 3094 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	ZJNE, M_LAB1,
	ZOR, M_LO, M_RL, M_LO, M_RL,
	M_JMPB, 0x1C,		/* 3078 */
/* 3108 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	M_JMPB, 0x1A,		/* 3087 */
/* 3115 */
	ZCMP, M_HI, M_AL, M_HI, M_AR,
	ZJNE, M_LAB,
	ZCMP, M_LO, M_AL, M_LO,
	M_JMP2, 0x9D, 0x03,	/* 925 */
/* 3129 */
	ZOR, M_HI, M_RL, M_HI, M_RL,
	ZJNE, M_LAB,
	ZCMP, M_LO, M_RL,
	M_JMPB, 0x0E,		/* 3125 */
/* 3141 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	ZJNE, M_LAB,
	ZOR, M_LO, M_RL, M_LO, M_RL,
	ZJNE, M_LAB,
	M_END,
/* 3156 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	M_JMPB, 0x1B,		/* 3134 */
/* 3163 */
	ZCMP, M_HI, M_AL, M_HI, M_AR,
	M_LREL1, M_LAB1,
	M_LREL0, M_LAB,
	ZCMP, M_LO, M_AL, M_LO, M_AR,
	M_LREL2,
	M_JMPB, 0x63,		/* 3079 */
/* 3180 */
	ZOR, M_HI, M_RL, M_HI, M_RL,
	M_LREL1, M_LAB1,
	M_LREL0, M_LAB,
	ZCMP, M_LO, M_RL,
	M_JMPB, 0x11,		/* 3175 */
/* 3194 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	M_JMPB, 0x0E,		/* 3185 */
/* /newbits/bin/cc/c/n1/i8086/tables/rem.t */
/* 3201 */
	M_JMP2, 0xA7, 0x09,	/* 2471 */
/* 3204 */
	M_JMP2, 0xAB, 0x09,	/* 2475 */
/* /newbits/bin/cc/c/n1/i8086/tables/shl.t */
/* 3207 */
	ZSAL,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/* 3211 */
	M_JMPB, 0x04,		/* 3207 */
/* 3213 */
	ZSAL, M_R, M_ICON, 0x02,
	ZSAL, M_R, M_ICON, 0x02,
	M_JMP1, 0x07,		/* 7 */
/* 3223 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAL, M_R, M_REGNO, CL,
	M_JMP1, 0x07,		/* 7 */
/* 3233 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSAL, M_R, M_REGNO, CL,
	M_END,
/* 3242 */
	ZMOV, M_REGNO, CX, M_AR,
	M_DLAB0, ZSAL, M_LO, M_R, M_ICON, 0x02,
	ZRCL, M_HI, M_R, M_ICON, 0x02,
	ZLOOP, M_LAB0,
	M_END,
/* 3260 */
	ZMOV, M_REGNO, CX, M_AR,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSAL, M_LO, M_R, M_ICON, 0x02,
	ZRCL, M_HI, M_R, M_ICON, 0x02,
	ZLOOP, M_LAB1,
	M_DLAB0,
	M_END,
/* /newbits/bin/cc/c/n1/i8086/tables/shr.t */
/* 3281 */
	ZSAR,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/* 3285 */
	ZSHR,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/* 3289 */
	ZSAR,
	M_JMPB, 0x48,		/* 3218 */
/* 3292 */
	ZSAR, M_R, M_ICON, 0x02,
	M_JMPB, 0x07,		/* 3289 */
/* 3298 */
	ZSHR,
	M_JMPB, 0x51,		/* 3218 */
/* 3301 */
	ZSHR, M_R, M_ICON, 0x02,
	M_JMPB, 0x07,		/* 3298 */
/* 3307 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAR,
	M_JMPB, 0x54,		/* 3228 */
/* 3314 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHR,
	M_JMPB, 0x5B,		/* 3228 */
/* 3321 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSAR,
	M_JMPB, 0x58,		/* 3238 */
/* 3328 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSHR,
	M_JMPB, 0x5F,		/* 3238 */
/* 3335 */
	ZMOV, M_REGNO, CX, M_AR,
	M_DLAB0, ZSAR, M_HI, M_R, M_ICON, 0x02,
	ZRCR, M_LO,
	M_JMPB, 0x5D,		/* 3254 */
/* 3349 */
	ZMOV, M_REGNO, CX, M_AR,
	M_DLAB0, ZSHR,
	M_JMPB, 0x0E,		/* 3341 */
/* 3357 */
	ZMOV, M_REGNO, CX, M_AR,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSAR, M_HI, M_R, M_ICON, 0x02,
	ZRCR, M_LO,
	M_JMPB, 0x61,		/* 3274 */
/* 3373 */
	ZMOV, M_REGNO, CX, M_AR,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSHR,
	M_JMPB, 0x10,		/* 3365 */
/* /newbits/bin/cc/c/n1/i8086/tables/sub.t */
/* 3383 */
	ZDEC,
	M_JMP1, 0x71,		/* 113 */
/* 3386 */
	ZDEC, M_R,
	M_JMPB, 0x05,		/* 3383 */
/* 3390 */
	ZSUB,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/* 3394 */
	ZSUB, M_LO, M_R, M_LO, M_AR,
	ZSBB,
	M_JMP2, 0x94, 0x01,	/* 404 */
/* /newbits/bin/cc/c/n1/i8086/tables/xor.t */
/* 3403 */
	ZXOR,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/* 3407 */
	ZXOR,
	M_JMP2, 0xCA, 0x03,	/* 970 */
/* 3411 */
	ZXOR, M_LO, M_R, M_LO, M_AR,
	M_JMP2, 0xCD, 0x0B,	/* 3021 */
/* 3419 */
	ZXOR,
	M_JMP2, 0x94, 0x01,	/* 404 */
/* 3423 */
	ZNOT, M_LO, M_R,
	M_JMPB, 0x07,		/* 3419 */
/* 3428 */
	ZXOR, M_LO, M_R, M_LO, M_AR,
	M_JMPB, 0x07,		/* 3426 */
 0
};
