#ifdef vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif
char macros[] = {
/* /usr/src/c/n1/i8086/tables/prefac.f */
/* /usr/src/c/n1/i8086/tables/aadd.t */
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
	ZINC, M_AL,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_END,
/*   53 */
	ZINC, M_AL,
	M_IFV, ZLDES, M_LO, M_R, M_AL, M_ENDIF,
	M_END,
/*   62 */
	ZADD, M_AL, M_AR,
	M_JMP1, 0x02,		/* 2 */
/*   67 */
	M_JMP1, 0x3E,		/* 62 */
/*   69 */
	ZADDB, M_AL, M_AR,
	M_JMP1, 0x0E,		/* 14 */
/*   74 */
	ZADDB, M_AL, M_AR,
	M_JMP1, 0x1A,		/* 26 */
/*   79 */
	ZADD, M_AL, M_AR,
	M_JMP1, 0x2A,		/* 42 */
/*   84 */
	ZADD, M_AL, M_AR,
	M_JMP1, 0x37,		/* 55 */
/*   89 */
	ZADD, M_HI, M_AL, M_HI, M_AR,
	M_IFV, ZMOV, M_LO, M_R, M_LO, M_AL, M_ENDIF,
	M_IFV, ZMOV, M_HI, M_R, M_HI, M_AL, M_ENDIF,
	M_IFR, M_REL1,
	M_JMP1, 0x09,		/* 9 */
/*  112 */
	ZADD, M_LO, M_AL, M_LO, M_AR,
	ZADC,
	M_JMP1, 0x5A,		/* 90 */
/*  120 */
	ZADD, M_AL, M_R,
	M_JMP1, 0x02,		/* 2 */
/*  125 */
	ZADDB, M_AL, M_LO, M_R,
	M_JMP1, 0x0E,		/* 14 */
/*  131 */
	ZADDB, M_AL, M_LO, M_R,
	M_JMP1, 0x1A,		/* 26 */
/*  137 */
	ZADD, M_AL, M_RR,
	M_JMP1, 0x2A,		/* 42 */
/*  142 */
	ZADD, M_AL, M_RR,
	M_JMP1, 0x37,		/* 55 */
/*  147 */
	ZMOV, M_R, M_AL,
	ZADD, M_R, M_AR,
	ZMOV, M_AL, M_R,
	M_JMP1, 0x07,		/* 7 */
/*  158 */
	ZMOVB, M_LO, M_R, M_AL,
	ZCBW,
	ZADD, M_R, M_AR,
	ZMOVB, M_AL, M_LO,
	M_JMP1, 0x9B,		/* 155 */
/*  171 */
	ZMOVB, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_R, M_HI, M_R,
	M_JMP1, 0xA3,		/* 163 */
/*  182 */
	ZMOV, M_LO, M_R, M_AR,
	ZCWD,
	ZADD, M_LO, M_AL, M_LO, M_R,
	ZADC, M_HI, M_AL, M_HI, M_R,
	M_END,
/*  198 */
	ZMOV, M_LO, M_R, M_AR,
	ZSUB, M_HI, M_R, M_HI, M_R,
	M_JMP1, 0xBB,		/* 187 */
/*  209 */
	M_JMP1, 0xBB,		/* 187 */
/*  211 */
	ZADD, M_LO, M_R, M_LO, M_AL,
	ZADC, M_HI, M_R, M_HI, M_AL,
	ZMOV, M_LO, M_AL, M_LO, M_R,
	ZMOV, M_HI, M_AL, M_HI, M_R,
	M_JMP1, 0x6C,		/* 108 */
/*  233 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZADD, M_LO, M_R, M_LO, M_AR,
	ZADC, M_HI, M_R, M_HI, M_AR,
	M_JMP1, 0xDD,		/* 221 */
/*  255 */
	ZMOVB, M_LO, M_R, M_AL,
	ZCBW,
	ZADD, M_R, M_AR,
	ZAND, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB, M_AL, M_R,
	M_END,
/*  275 */
	ZMOV, M_R, M_AL,
	ZADD, M_R, M_AR,
	ZAND, M_R, M_LO, M_EMASK,
	ZAND, M_AL, M_LO, M_CMASK,
	ZOR,
	M_JMPB, 0x12,		/* 272 */
/*  292 */
	ZFADDF, M_AL,
	M_IFV, ZFSTF, M_AL, M_ENDIF,
	M_IFE, ZFSTPF,
	M_JMP1, 0x3B,		/* 59 */
/*  302 */
	ZFADDD, M_AL,
	M_IFV, ZFSTD, M_AL, M_ENDIF,
	M_IFE, ZFSTPD,
	M_JMP1, 0x3B,		/* 59 */
/* /usr/src/c/n1/i8086/tables/aand.t */
/*  312 */
	ZAND,
	M_JMP1, 0x3F,		/* 63 */
/*  315 */
	ZANDB,
	M_JMP1, 0x46,		/* 70 */
/*  318 */
	ZANDB,
	M_JMP1, 0x4B,		/* 75 */
/*  321 */
	ZAND,
	M_JMP1, 0x79,		/* 121 */
/*  324 */
	ZANDB,
	M_JMP1, 0x7E,		/* 126 */
/*  327 */
	ZANDB,
	M_JMP1, 0x84,		/* 132 */
/*  330 */
	ZMOV, M_LO, M_AL, M_LO, M_AR,
	ZAND,
	M_JMP1, 0x5A,		/* 90 */
/*  338 */
	ZAND, M_HI, M_AL, M_HI, M_AR,
	M_IFV, ZMOV, M_LO, M_R, M_LO, M_AL, M_ENDIF,
	M_IFV, ZMOV, M_HI, M_R, M_HI,
	M_JMP1, 0x3B,		/* 59 */
/*  357 */
	ZAND, M_LO, M_AL, M_LO, M_AR,
	ZMOV,
	M_JMPB, 0x18,		/* 339 */
/*  365 */
	ZAND, M_LO, M_AL, M_LO,
	M_JMPB, 0x1B,		/* 342 */
/*  371 */
	ZAND,
	M_JMPB, 0x29,		/* 331 */
/*  374 */
	ZAND, M_LO, M_AL, M_LO, M_R,
	ZAND, M_HI, M_AL, M_HI, M_R,
	M_JMPB, 0x29,		/* 343 */
/*  386 */
	ZAND, M_AL, M_AR,
	M_IFV, ZMOV,
	M_JMP1, 0x3A,		/* 58 */
/*  393 */
	ZAND, M_AL, M_AR,
	M_IFV, ZMOV, M_R, M_AL, M_ENDIF,
	M_IFV, ZAND, M_R, M_LO, M_EMASK, M_ENDIF,
	M_END,
/*  408 */
	ZANDB, M_AL, M_AR,
	M_IFV, ZMOVB,
	M_JMP1, 0x39,		/* 57 */
/*  415 */
	ZANDB, M_AL, M_AR,
	M_IFV, ZMOVB, M_LO,
	M_JMPB, 0x17,		/* 398 */
/*  423 */
	ZAND, M_AL, M_R,
	M_JMPB, 0x25,		/* 389 */
/*  428 */
	ZAND, M_AL, M_R,
	M_JMPB, 0x23,		/* 396 */
/*  433 */
	ZANDB, M_AL, M_LO, M_R,
	M_JMPB, 0x1A,		/* 411 */
/*  439 */
	ZANDB, M_AL, M_LO, M_R,
	M_JMPB, 0x19,		/* 418 */
/* /usr/src/c/n1/i8086/tables/add.t */
/*  445 */
	ZINC,
	M_JMP1, 0x9B,		/* 155 */
/*  448 */
	ZINC, M_LO, M_R,
	M_END,
/*  452 */
	ZINC, M_R,
	M_JMPB, 0x09,		/* 445 */
/*  456 */
	ZINC, M_LO, M_R,
	M_JMPB, 0x0B,		/* 448 */
/*  461 */
	ZADD, M_R, M_AR,
	M_JMP1, 0x07,		/* 7 */
/*  466 */
	ZADD, M_LO, M_R, M_AR,
	M_END,
/*  471 */
	ZADD, M_LO, M_R, M_AR,
	ZSUB, M_HI, M_R,
	M_JMP1, 0xC3,		/* 195 */
/*  480 */
	ZADD, M_LO, M_R, M_AR,
	ZSUB, M_REGNO, AX, M_REGNO, AX,
	ZMOV, M_HI, M_R, M_REGNO, AX,
	M_END,
/*  495 */
	ZADD, M_LO, M_R, M_LO,
	M_JMPB, 0x19,		/* 474 */
/*  501 */
	ZADD, M_LO, M_R, M_LO, M_AR,
	ZMOV, M_REGNO, AX, M_HI, M_AR,
	M_JMPB, 0x16,		/* 489 */
/*  513 */
	ZADD, M_LO, M_R, M_AR,
	ZMOV, M_HI, M_R, M_REGNO, CS,
	M_END,
/*  523 */
	ZADD, M_LO, M_R, M_AR,
	ZMOV, M_HI, M_R, M_REGNO, DS,
	M_END,
/*  533 */
	ZADD, M_LO, M_R, M_AR,
	ZPUSH, M_REGNO, CS,
	ZPOP,
	M_JMP1, 0xC3,		/* 195 */
/*  543 */
	ZADD, M_LO, M_R, M_AR,
	ZPUSH, M_REGNO, DS,
	M_JMPB, 0x0A,		/* 540 */
/*  552 */
	ZADD, M_LO, M_R, M_LO, M_AR,
	ZMOV, M_HI, M_R, M_HI, M_AR,
	M_END,
/*  563 */
	ZADD, M_LO, M_R, M_LO, M_AR,
	ZPUSH, M_HI, M_AR,
	M_JMPB, 0x1F,		/* 540 */
/*  573 */
	M_JMPB, 0x15,		/* 552 */
/*  575 */
	ZADD, M_LO, M_R, M_LO, M_AR,
	ZADC, M_HI, M_R, M_HI, M_AR,
	M_JMP1, 0x6C,		/* 108 */
/*  587 */
	ZADD, M_LO, M_R, M_LO, M_AR,
	M_END,
/*  593 */
	M_JMPB, 0x29,		/* 552 */
/*  595 */
	ZPUSH, M_HI, M_AL,
	ZMOV, M_R, M_LO, M_AL,
	ZADD, M_R, M_AR,
	ZPUSH, M_R,
	M_END,
/*  608 */
	ZPUSH, M_HI, M_AR,
	ZMOV, M_R, M_LO, M_AR,
	ZADD, M_R, M_AL,
	M_JMPB, 0x0D,		/* 605 */
/*  620 */
	ZFADDI, M_AR,
	M_END,
/*  623 */
	ZFADDL, M_AR,
	M_END,
/*  626 */
	ZFADDF, M_AR,
	M_END,
/*  629 */
	ZFADDD, M_AR,
	M_END,
/* /usr/src/c/n1/i8086/tables/adiv.t */
/*  632 */
	ZMOV, M_LO, M_R, M_AL,
	ZCWD,
	ZIDIV, M_AR,
	ZMOV, M_AL,
	M_JMPB, 0xC0,		/* 449 */
/*  643 */
	ZMOV, M_LO, M_R, M_AL,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV,
	M_JMPB, 0x0F,		/* 638 */
/*  655 */
	ZMOV, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZAND, M_AL, M_LO, M_CMASK,
	ZOR,
	M_JMPB, 0x29,		/* 640 */
/*  683 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZCBW,
	ZCWD,
	ZIDIV, M_AR,
	ZMOVB, M_AL, M_LO, M_LO, M_R,
	M_IFV, ZCBW, M_ENDIF,
	M_END,
/*  701 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	ZCWD,
	ZDIV, M_AR,
	ZMOVB, M_AL, M_LO, M_LO, M_R,
	M_IFV, ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R, M_ENDIF,
	M_END,
/*  731 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB, M_AL, M_LO, M_LO, M_R,
	M_END,
/*  763 */
	ZPUSH, M_HI, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x01,
	ZADD, M_REGNO, SP, M_ICON, 0x01,
	ZMOV, M_LO, M_AL, M_LO, M_R,
	ZMOV,
	M_JMP1, 0xC1,		/* 193 */
/*  791 */
	ZPUSH, M_HI, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x02,
	M_JMPB, 0x1C,		/* 778 */
/*  808 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	M_JMPB, 0x2D,		/* 769 */
/*  816 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	M_JMPB, 0x19,		/* 797 */
/*  824 */
	ZFRDIVF,
	M_JMP2, 0x25, 0x01,	/* 293 */
/*  828 */
	ZFRDIVD,
	M_JMP2, 0x2F, 0x01,	/* 303 */
/* /usr/src/c/n1/i8086/tables/aft.t */
/*  832 */
	M_IFV, ZMOV, M_R, M_AL, M_ENDIF,
	M_OP1, M_AL,
	M_END,
/*  840 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_JMPB, 0x0D,		/* 837 */
/*  852 */
	M_IFV, ZLDES, M_LO,
	M_JMPB, 0x15,		/* 834 */
/*  857 */
	M_IFV, ZMOV, M_R, M_AL, M_ENDIF,
	M_OP0, M_AL, M_AR,
	M_END,
/*  866 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_JMPB, 0x0E,		/* 862 */
/*  878 */
	M_IFV, ZLDES, M_LO,
	M_JMPB, 0x16,		/* 859 */
/*  883 */
	M_IFV, ZMOVB, M_LO, M_R, M_AL, M_ENDIF,
	M_IFV, ZCBW, M_ENDIF,
	M_TL,
	M_JMPB, 0x38,		/* 837 */
/*  895 */
	M_IFV, ZMOVB, M_LO, M_R, M_AL, M_ENDIF,
	M_IFV, ZSUBB, M_HI, M_R, M_HI, M_R,
	M_JMPB, 0x10,		/* 891 */
/*  909 */
	M_IFV, ZMOV, M_LO, M_R, M_LO, M_AL, M_ENDIF,
	M_IFV, ZMOV, M_HI, M_R, M_HI, M_AL, M_ENDIF,
	M_OP0, M_LO, M_AL, M_ICON, 0x02,
	M_OP2, M_HI, M_AL, M_ICON, 0x03,
	M_END,
/*  934 */
	ZMOV, M_LO, M_R, M_AL,
	ZMOV, M_HI, M_R, M_LO, M_R,
	M_OP0, M_HI, M_R, M_AR,
	ZAND, M_HI, M_R, M_LO, M_EMASK,
	ZAND, M_AL, M_LO, M_CMASK,
	ZOR, M_AL, M_HI, M_R,
	M_IFV, ZAND, M_LO,
	M_JMP2, 0x93, 0x01,	/* 403 */
/*  966 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZMOV, M_HI, M_R, M_LO, M_R,
	M_OP0, M_HI, M_R, M_AR,
	ZAND, M_HI, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB, M_AL, M_LO,
	M_JMPB, 0x22,		/* 958 */
/* /usr/src/c/n1/i8086/tables/amul.t */
/*  994 */
	ZIMULI, M_R, M_AL, M_AR,
	ZMOV, M_AL, M_R,
	M_END,
/* 1002 */
	ZMOV, M_LO, M_R, M_AL,
	ZIMUL,
	M_JMP2, 0x7E, 0x02,	/* 638 */
/* 1010 */
	ZMOV, M_LO, M_R, M_AL,
	ZMUL,
	M_JMP2, 0x7E, 0x02,	/* 638 */
/* 1018 */
	ZMOV, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZMUL,
	M_JMP2, 0x9E, 0x02,	/* 670 */
/* 1031 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZCBW,
	ZIMUL,
	M_JMP2, 0xB3, 0x02,	/* 691 */
/* 1041 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	ZMUL,
	M_JMP2, 0xCB, 0x02,	/* 715 */
/* 1057 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZMUL,
	M_JMP2, 0xEB, 0x02,	/* 747 */
/* 1071 */
	ZPUSH, M_HI, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x03,
	M_JMP2, 0x0A, 0x03,	/* 778 */
/* 1089 */
	ZPUSH, M_HI, M_R,
	ZPUSH, M_LO, M_R,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x04,
	M_JMP2, 0x0A, 0x03,	/* 778 */
/* 1107 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	M_JMPB, 0x24,		/* 1077 */
/* 1115 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	M_JMPB, 0x1A,		/* 1095 */
/* 1123 */
	ZFMULF,
	M_JMP2, 0x25, 0x01,	/* 293 */
/* 1127 */
	ZFMULD,
	M_JMP2, 0x2F, 0x01,	/* 303 */
/* /usr/src/c/n1/i8086/tables/and.t */
/* 1131 */
	ZTEST, M_AL, M_AR,
	M_REL0, M_LAB,
	M_END,
/* 1137 */
	ZTESTB,
	M_JMPB, 0x06,		/* 1132 */
/* 1140 */
	ZAND,
	M_JMP2, 0xCE, 0x01,	/* 462 */
/* 1144 */
	ZTEST, M_HI, M_AL, M_HI, M_AR,
	M_REL1, M_LAB,
	M_END,
/* 1152 */
	ZMOV, M_R, M_HI, M_AL,
	ZTEST, M_R,
	M_JMPB, 0x0B,		/* 1147 */
/* 1160 */
	ZTEST, M_HI, M_RL,
	M_JMPB, 0x10,		/* 1147 */
/* 1165 */
	ZTEST, M_LO, M_AL, M_LO,
	M_JMPB, 0x24,		/* 1133 */
/* 1171 */
	ZTEST, M_HI, M_AL, M_HI,
	M_JMPB, 0x2A,		/* 1133 */
/* 1177 */
	ZTEST, M_LO, M_RL,
	M_JMPB, 0x0C,		/* 1168 */
/* 1182 */
	ZTEST, M_HI, M_RL,
	M_JMPB, 0x0B,		/* 1174 */
/* 1187 */
	ZTEST, M_LO, M_AL, M_LO, M_AR,
	ZJNE, M_LAB,
	ZTEST, M_HI, M_AL, M_HI, M_AR,
	ZJNE, M_LAB,
	M_END,
/* 1202 */
	ZTEST, M_LO, M_RL, M_LO, M_AR,
	ZJNE, M_LAB,
	ZTEST, M_HI, M_RL,
	M_JMPB, 0x0F,		/* 1197 */
/* 1214 */
	ZTEST, M_LO, M_AL, M_LO, M_AR,
	ZJNE, M_LAB0,
	ZTEST, M_HI, M_AL, M_HI, M_AR,
	ZJE, M_LAB,
	M_DLAB0,
	M_END,
/* 1230 */
	ZTEST, M_LO, M_RL, M_LO, M_AR,
	ZJNE, M_LAB0,
	ZTEST, M_HI, M_RL,
	M_JMPB, 0x10,		/* 1224 */
/* 1242 */
	ZAND,
	M_JMP2, 0x4C, 0x02,	/* 588 */
/* 1246 */
	ZAND,
	M_JMP2, 0xF0, 0x01,	/* 496 */
/* 1250 */
	ZAND,
	M_JMP2, 0x2E, 0x02,	/* 558 */
/* 1254 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	M_JMPB, 0x09,		/* 1250 */
/* 1261 */
	ZAND, M_LO, M_R, M_LO, M_AR,
	M_JMPB, 0x07,		/* 1259 */
/* /usr/src/c/n1/i8086/tables/aor.t */
/* 1268 */
	ZOR,
	M_JMP1, 0x3F,		/* 63 */
/* 1271 */
	ZORB,
	M_JMP1, 0x46,		/* 70 */
/* 1274 */
	ZORB,
	M_JMP1, 0x4B,		/* 75 */
/* 1277 */
	ZOR,
	M_JMP1, 0x79,		/* 121 */
/* 1280 */
	ZORB,
	M_JMP1, 0x7E,		/* 126 */
/* 1283 */
	ZORB,
	M_JMP1, 0x84,		/* 132 */
/* 1286 */
	ZOR,
	M_JMP1, 0x5A,		/* 90 */
/* 1289 */
	ZOR,
	M_JMP2, 0x6E, 0x01,	/* 366 */
/* 1293 */
	ZOR, M_LO, M_AL, M_LO, M_AR,
	M_JMPB, 0x0C,		/* 1286 */
/* 1300 */
	ZOR, M_LO, M_AL, M_LO, M_R,
	ZOR,
	M_JMP2, 0x7C, 0x01,	/* 380 */
/* 1309 */
	ZOR,
	M_JMP2, 0x83, 0x01,	/* 387 */
/* 1313 */
	ZOR,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/* 1317 */
	ZORB,
	M_JMP2, 0x99, 0x01,	/* 409 */
/* 1321 */
	ZORB,
	M_JMP2, 0xA0, 0x01,	/* 416 */
/* 1325 */
	ZOR,
	M_JMP2, 0xA8, 0x01,	/* 424 */
/* 1329 */
	ZOR,
	M_JMP2, 0xAD, 0x01,	/* 429 */
/* 1333 */
	ZORB,
	M_JMP2, 0xB2, 0x01,	/* 434 */
/* 1337 */
	ZORB,
	M_JMP2, 0xB8, 0x01,	/* 440 */
/* /usr/src/c/n1/i8086/tables/arem.t */
/* 1341 */
	ZMOV, M_LO, M_R, M_AL,
	ZCWD,
	ZIDIV, M_AR,
	ZMOV,
	M_JMP1, 0xC2,		/* 194 */
/* 1351 */
	ZMOV, M_LO, M_R, M_AL,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV,
	M_JMPB, 0x0E,		/* 1347 */
/* 1363 */
	ZMOV, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	ZAND, M_HI, M_R, M_LO, M_EMASK,
	ZAND, M_AL, M_LO, M_CMASK,
	ZOR,
	M_JMP1, 0xC2,		/* 194 */
/* 1391 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZCBW,
	ZCWD,
	ZIDIV, M_AR,
	ZMOVB, M_AL, M_LO, M_HI, M_R,
	M_IFV, ZMOVB, M_LO, M_LO, M_R, M_LO, M_HI, M_R, M_ENDIF,
	M_JMP2, 0xB9, 0x02,	/* 697 */
/* 1417 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	ZCWD,
	ZDIV, M_AR,
	ZMOVB, M_AL, M_LO, M_HI, M_R,
	M_IFV, ZSUBB, M_HI, M_HI, M_R, M_HI, M_HI, M_R, M_ENDIF,
	M_END,
/* 1447 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZAND, M_LO, M_R, M_LO, M_EMASK,
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	ZAND, M_HI, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB, M_AL, M_LO,
	M_JMP1, 0xC3,		/* 195 */
/* 1478 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x05,
	M_JMP2, 0x0A, 0x03,	/* 778 */
/* 1496 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_CALL, M_GID, 0x06,
	M_JMP2, 0x0A, 0x03,	/* 778 */
/* /usr/src/c/n1/i8086/tables/ashl.t */
/* 1514 */
	ZSAL,
	M_JMP1, 0x3F,		/* 63 */
/* 1517 */
	ZSALB,
	M_JMP1, 0x46,		/* 70 */
/* 1520 */
	ZSALB,
	M_JMP1, 0x4B,		/* 75 */
/* 1523 */
	ZSAL, M_AL, M_ICON, 0x02,
	M_JMP1, 0x02,		/* 2 */
/* 1529 */
	ZSALB, M_AL, M_ICON, 0x02,
	M_JMP1, 0x0E,		/* 14 */
/* 1535 */
	ZSALB, M_AL, M_ICON, 0x02,
	M_JMP1, 0x1A,		/* 26 */
/* 1541 */
	ZSAL, M_AL, M_ICON, 0x02,
	M_JMPB, 0x16,		/* 1523 */
/* 1547 */
	ZSALB, M_AL, M_ICON, 0x02,
	M_JMPB, 0x16,		/* 1529 */
/* 1553 */
	ZSALB, M_AL, M_ICON, 0x02,
	M_JMPB, 0x16,		/* 1535 */
/* 1559 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAL, M_AL, M_REGNO, CL,
	M_JMP1, 0x02,		/* 2 */
/* 1569 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSALB, M_AL, M_REGNO, CL,
	M_JMP1, 0x0E,		/* 14 */
/* 1579 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSALB, M_AL, M_REGNO, CL,
	M_JMP1, 0x1A,		/* 26 */
/* 1589 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_DLAB0, ZSAL, M_LO, M_R, M_ICON, 0x02,
	ZRCL, M_HI, M_R, M_ICON, 0x02,
	ZLOOP, M_LAB0,
	M_JMP1, 0xDD,		/* 221 */
/* 1618 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSAL, M_AL, M_REGNO, CL,
	M_JMP2, 0x85, 0x01,	/* 389 */
/* 1629 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSALB, M_AL, M_REGNO, CL,
	M_IFV, ZMOVB, M_LO, M_R, M_AL,
	M_JMPB, 0xE5,		/* 1413 */
/* 1644 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSALB, M_AL, M_REGNO, CL,
	M_IFV, ZMOVB, M_LO, M_R, M_AL, M_ENDIF,
	M_IFV, ZSUBB, M_HI, M_R,
	M_JMPB, 0xDB,		/* 1443 */
/* 1664 */
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
/* 1705 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZSHL,
	M_JMP2, 0x17, 0x01,	/* 279 */
/* 1716 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZSHLB, M_LO, M_R, M_AR,
	ZAND, M_R, M_LO, M_EMASK,
	ZANDB, M_AL, M_LO, M_CMASK,
	ZORB,
	M_JMP2, 0x80, 0x02,	/* 640 */
/* 1741 */
	M_JMPB, 0x24,		/* 1705 */
/* 1743 */
	M_JMPB, 0x1B,		/* 1716 */
/* 1745 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZSHL, M_R, M_ICON, 0x02,
	ZSHL, M_R, M_ICON, 0x02,
	M_JMP2, 0x19, 0x01,	/* 281 */
/* 1763 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZSHLB, M_LO, M_R, M_ICON, 0x02,
	ZSHLB, M_LO, M_R, M_ICON, 0x02,
	M_JMPB, 0x35,		/* 1729 */
/* 1784 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHL, M_R, M_REGNO, CL,
	M_JMP2, 0x19, 0x01,	/* 281 */
/* 1802 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHLB, M_LO, M_R, M_REGNO, CL,
	M_JMPB, 0x5B,		/* 1729 */
/* 1822 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x26,		/* 1794 */
/* 1834 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x20,		/* 1814 */
/* /usr/src/c/n1/i8086/tables/ashr.t */
/* 1848 */
	ZSAR,
	M_JMP1, 0x3F,		/* 63 */
/* 1851 */
	ZSHR,
	M_JMP1, 0x3F,		/* 63 */
/* 1854 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZSHR,
	M_JMP2, 0x17, 0x01,	/* 279 */
/* 1865 */
	ZSARB,
	M_JMP1, 0x46,		/* 70 */
/* 1868 */
	ZSHRB,
	M_JMP1, 0x4B,		/* 75 */
/* 1871 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZSHRB,
	M_JMPB, 0x9B,		/* 1726 */
/* 1883 */
	M_JMPB, 0x23,		/* 1848 */
/* 1885 */
	M_JMPB, 0x22,		/* 1851 */
/* 1887 */
	M_JMPB, 0x21,		/* 1854 */
/* 1889 */
	M_JMPB, 0x18,		/* 1865 */
/* 1891 */
	M_JMPB, 0x17,		/* 1868 */
/* 1893 */
	M_JMPB, 0x16,		/* 1871 */
/* 1895 */
	ZSAR, M_HI, M_AL, M_AR,
	ZRCR, M_LO, M_AL,
	M_JMP2, 0x56, 0x01,	/* 342 */
/* 1905 */
	ZSHR,
	M_JMPB, 0x0A,		/* 1896 */
/* 1908 */
	ZSAR, M_AL, M_ICON, 0x02,
	ZSAR,
	M_JMP2, 0xF4, 0x05,	/* 1524 */
/* 1916 */
	ZSHR, M_AL, M_ICON, 0x02,
	ZSHR,
	M_JMP2, 0xF4, 0x05,	/* 1524 */
/* 1924 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZSHR, M_R, M_ICON, 0x02,
	ZSHR,
	M_JMPB, 0xB3,		/* 1757 */
/* 1938 */
	ZSARB, M_AL, M_ICON, 0x02,
	ZSARB,
	M_JMP2, 0xFA, 0x05,	/* 1530 */
/* 1946 */
	ZSHRB, M_AL, M_ICON, 0x02,
	ZSHRB,
	M_JMP2, 0x00, 0x06,	/* 1536 */
/* 1954 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZSHRB, M_LO, M_R, M_ICON, 0x02,
	ZSHRB,
	M_JMPB, 0xBF,		/* 1778 */
/* 1971 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAR,
	M_JMP2, 0x1C, 0x06,	/* 1564 */
/* 1979 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHR,
	M_JMP2, 0x1C, 0x06,	/* 1564 */
/* 1987 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHR,
	M_JMPB, 0xCB,		/* 1796 */
/* 2001 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSARB,
	M_JMP2, 0x26, 0x06,	/* 1574 */
/* 2009 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHRB,
	M_JMP2, 0x30, 0x06,	/* 1584 */
/* 2017 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHRB,
	M_JMPB, 0xD7,		/* 1816 */
/* 2033 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_DLAB0, ZSAR, M_HI, M_R, M_ICON, 0x02,
	ZRCR, M_LO,
	M_JMP2, 0x4B, 0x06,	/* 1611 */
/* 2058 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_DLAB0, ZSHR,
	M_JMPB, 0x19,		/* 2049 */
/* 2076 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSAR,
	M_JMP2, 0x57, 0x06,	/* 1623 */
/* 2084 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSHR,
	M_JMP2, 0x57, 0x06,	/* 1623 */
/* 2092 */
	ZMOV, M_R, M_AL,
	ZAND, M_R, M_LO, M_EMASK,
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x69,		/* 1997 */
/* 2104 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSARB,
	M_JMP2, 0x62, 0x06,	/* 1634 */
/* 2112 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSHRB,
	M_JMP2, 0x71, 0x06,	/* 1649 */
/* 2120 */
	ZMOVB, M_LO, M_R, M_AL,
	ZANDB, M_LO, M_R, M_LO, M_EMASK,
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x67,		/* 2029 */
/* 2134 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSAR, M_HI, M_R, M_ICON, 0x02,
	ZRCR, M_LO,
	M_JMP2, 0x98, 0x06,	/* 1688 */
/* 2161 */
	ZMOV, M_REGNO, CX, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSHR,
	M_JMPB, 0x1B,		/* 2152 */
/* /usr/src/c/n1/i8086/tables/assign.t */
/* 2181 */
	ZSUB, M_AL, M_AL,
	M_END,
/* 2185 */
	ZMOV, M_AL, M_AR,
	M_END,
/* 2189 */
	ZMOV, M_AL, M_LO, M_AR,
	M_END,
/* 2194 */
	ZLEA, M_AL, M_NSE, M_AR,
	M_END,
/* 2199 */
	ZMOVB, M_AL, M_AR,
	M_END,
/* 2203 */
	ZMOV, M_LO, M_AL, M_LO, M_AR,
	ZMOV, M_HI, M_AL, M_HI, M_AR,
	M_END,
/* 2214 */
	ZMOVB, M_AL, M_LO, M_R,
	M_IFR, ZORB, M_LO, M_R, M_LO,
	M_JMP1, 0x25,		/* 37 */
/* 2225 */
	ZMOVB, M_AL, M_LO, M_R,
	ZCBW,
	M_END,
/* 2231 */
	ZMOVB, M_AL, M_LO, M_R,
	M_IFV, ZSUBB, M_HI, M_R, M_HI, M_R, M_ENDIF,
	M_JMPB, 0x18,		/* 2218 */
/* 2244 */
	ZMOVB, M_LO, M_R, M_AR,
	ZMOVB, M_AL, M_LO, M_R,
	M_IFV, ZCBW,
	M_JMPB, 0x0D,		/* 2241 */
/* 2256 */
	ZMOVB, M_LO, M_R, M_AR,
	M_JMPB, 0x1D,		/* 2231 */
/* 2262 */
	M_JMP2, 0xE6, 0x03,	/* 998 */
/* 2265 */
	ZMOV, M_R,
	M_JMP2, 0xE5, 0x03,	/* 997 */
/* 2270 */
	ZMOVB, M_AL,
	M_JMP2, 0xB7, 0x02,	/* 695 */
/* 2275 */
	ZMOVB, M_AL, M_LO, M_R,
	M_JMP2, 0x7A, 0x06,	/* 1658 */
/* 2282 */
	M_JMP2, 0x0F, 0x03,	/* 783 */
/* 2285 */
	M_JMP2, 0xE6, 0x03,	/* 998 */
/* 2288 */
	M_JMP2, 0xB4, 0x02,	/* 692 */
/* 2291 */
	M_JMP2, 0xCC, 0x02,	/* 716 */
/* 2294 */
	ZMOV, M_LO, M_AL, M_ICON, 0x03,
	ZMOV,
	M_JMP2, 0xA1, 0x03,	/* 929 */
/* 2303 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZMOV, M_HI, M_R, M_LO, M_R,
	ZMOV, M_LO, M_AL, M_LO, M_R,
	ZMOV, M_HI,
	M_JMP2, 0x80, 0x02,	/* 640 */
/* 2323 */
	ZMOV, M_LO, M_R, M_AR,
	ZSUB, M_HI, M_R, M_HI, M_R,
	M_JMP2, 0x0F, 0x03,	/* 783 */
/* 2335 */
	ZMOV, M_LO, M_AL, M_AR,
	ZMOV, M_HI, M_AL, M_ICON, 0x03,
	M_JMP1, 0x37,		/* 55 */
/* 2346 */
	ZMOV, M_LO, M_R, M_LO, M_AR,
	ZMOV, M_HI, M_R, M_HI, M_AR,
	M_JMP2, 0x0F, 0x03,	/* 783 */
/* 2359 */
	ZMOV, M_LO, M_AL, M_LO, M_AR,
	ZMOV, M_HI, M_AL, M_HI,
	M_JMP1, 0x56,		/* 86 */
/* 2370 */
	ZMOV, M_LO, M_R, M_AR,
	ZMOV, M_HI, M_R, M_REGNO, CS,
	M_JMP2, 0x0F, 0x03,	/* 783 */
/* 2382 */
	ZMOV, M_LO, M_R, M_AR,
	ZMOV, M_HI, M_R, M_REGNO, DS,
	M_JMP2, 0x0F, 0x03,	/* 783 */
/* 2394 */
	ZMOV, M_LO, M_AL, M_AR,
	ZMOV, M_HI, M_AL, M_REGNO, CS,
	M_JMP1, 0x37,		/* 55 */
/* 2405 */
	ZMOV, M_LO, M_AL, M_AR,
	ZMOV, M_HI, M_AL, M_REGNO, DS,
	M_JMP1, 0x37,		/* 55 */
/* 2416 */
	M_JMPB, 0x5D,		/* 2323 */
/* 2418 */
	ZMOV, M_LO, M_R, M_AR,
	ZMOV, M_LO, M_AL, M_LO, M_R,
	M_JMPB, 0x58,		/* 2339 */
/* 2429 */
	ZMOV, M_LO, M_AL, M_RR,
	ZMOV, M_HI, M_AL, M_ICON, 0x03,
	M_JMP1, 0x2A,		/* 42 */
/* 2440 */
	ZMOV, M_LO, M_AL, M_RR,
	M_JMPB, 0x69,		/* 2339 */
/* 2446 */
	M_JMPB, 0x64,		/* 2346 */
/* 2448 */
	ZMOV, M_LO, M_AL, M_LO, M_RR,
	ZMOV, M_HI, M_AL, M_HI,
	M_JMP1, 0x8B,		/* 139 */
/* 2459 */
	ZMOV, M_LO, M_AL, M_LO, M_RR,
	ZMOV, M_HI, M_AL, M_HI,
	M_JMP1, 0x90,		/* 144 */
/* 2470 */
	M_JMP2, 0x0F, 0x03,	/* 783 */
/* 2473 */
	ZMOV, M_LO, M_AL, M_LO, M_RR,
	ZMOV, M_HI, M_AL, M_HI, M_RR,
	M_END,
/* 2484 */
	M_JMP2, 0x7F, 0x02,	/* 639 */
/* 2487 */
	M_JMP2, 0xB4, 0x02,	/* 692 */
/* 2490 */
	M_JMP2, 0xCC, 0x02,	/* 716 */
/* 2493 */
	ZAND, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x1D, 0x05,	/* 1309 */
/* 2500 */
	ZAND, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x21, 0x05,	/* 1313 */
/* 2507 */
	ZAND, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x2D, 0x05,	/* 1325 */
/* 2514 */
	ZAND, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x31, 0x05,	/* 1329 */
/* 2521 */
	ZANDB, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x35, 0x05,	/* 1333 */
/* 2528 */
	ZANDB, M_AL, M_LO, M_CMASK,
	M_JMP2, 0x39, 0x05,	/* 1337 */
/* 2535 */
	ZFSTPF, M_AL,
	M_END,
/* 2538 */
	ZFSTF, M_AL,
	M_END,
/* 2541 */
	ZFSTPD, M_AL,
	M_END,
/* 2544 */
	ZFSTD, M_AL,
	M_END,
/* /usr/src/c/n1/i8086/tables/asub.t */
/* 2547 */
	ZDEC,
	M_JMP1, 0x01,		/* 1 */
/* 2550 */
	ZDECB,
	M_JMP1, 0x0D,		/* 13 */
/* 2553 */
	ZDECB,
	M_JMP1, 0x19,		/* 25 */
/* 2556 */
	ZDEC,
	M_JMP1, 0x29,		/* 41 */
/* 2559 */
	ZDEC,
	M_JMP1, 0x36,		/* 54 */
/* 2562 */
	ZSUB,
	M_JMP1, 0x3F,		/* 63 */
/* 2565 */
	M_JMPB, 0x03,		/* 2562 */
/* 2567 */
	ZSUBB,
	M_JMP1, 0x46,		/* 70 */
/* 2570 */
	ZSUBB,
	M_JMP1, 0x4B,		/* 75 */
/* 2573 */
	ZSUB,
	M_JMP1, 0x50,		/* 80 */
/* 2576 */
	ZSUB,
	M_JMP1, 0x55,		/* 85 */
/* 2579 */
	ZSUB,
	M_JMP1, 0x5A,		/* 90 */
/* 2582 */
	ZSUB, M_LO, M_AL, M_LO, M_AR,
	ZSBB,
	M_JMP1, 0x5A,		/* 90 */
/* 2590 */
	ZSUB,
	M_JMP1, 0x79,		/* 121 */
/* 2593 */
	ZSUBB,
	M_JMP1, 0x7E,		/* 126 */
/* 2596 */
	ZSUBB,
	M_JMP1, 0x84,		/* 132 */
/* 2599 */
	ZSUB,
	M_JMP1, 0x8A,		/* 138 */
/* 2602 */
	ZSUB,
	M_JMP1, 0x8F,		/* 143 */
/* 2605 */
	ZMOV, M_R, M_AL,
	ZSUB,
	M_JMP1, 0x97,		/* 151 */
/* 2611 */
	ZMOVB, M_LO, M_R, M_AL,
	ZCBW,
	ZSUB,
	M_JMP1, 0xA4,		/* 164 */
/* 2619 */
	ZMOVB, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_R, M_HI, M_R,
	M_JMPB, 0x0C,		/* 2616 */
/* 2630 */
	ZMOV, M_LO, M_R, M_AR,
	ZCWD,
	ZSUB, M_LO, M_AL, M_LO, M_R,
	ZSBB,
	M_JMP1, 0xC1,		/* 193 */
/* 2643 */
	ZMOV, M_LO, M_R, M_AR,
	ZSUB, M_HI, M_R, M_HI, M_R,
	M_JMPB, 0x11,		/* 2635 */
/* 2654 */
	M_JMPB, 0x13,		/* 2635 */
/* 2656 */
	ZSUB, M_LO, M_AL, M_LO, M_R,
	ZSBB, M_HI, M_AL, M_HI, M_R,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_JMP1, 0x6C,		/* 108 */
/* 2678 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZSUB, M_LO, M_R, M_LO, M_AR,
	ZSBB,
	M_JMP1, 0xF9,		/* 249 */
/* 2696 */
	ZMOVB, M_LO, M_R, M_AL,
	ZCBW,
	ZSUB,
	M_JMP2, 0x05, 0x01,	/* 261 */
/* 2705 */
	ZMOV, M_R, M_AL,
	ZSUB,
	M_JMP2, 0x17, 0x01,	/* 279 */
/* 2712 */
	ZFRSUBF,
	M_JMP2, 0x25, 0x01,	/* 293 */
/* 2716 */
	ZFRSUBD,
	M_JMP2, 0x2F, 0x01,	/* 303 */
/* /usr/src/c/n1/i8086/tables/axor.t */
/* 2720 */
	ZXOR,
	M_JMP1, 0x3F,		/* 63 */
/* 2723 */
	ZXORB,
	M_JMP1, 0x46,		/* 70 */
/* 2726 */
	ZXORB,
	M_JMP1, 0x4B,		/* 75 */
/* 2729 */
	ZXOR,
	M_JMP1, 0x79,		/* 121 */
/* 2732 */
	ZXORB,
	M_JMP1, 0x7E,		/* 126 */
/* 2735 */
	ZXORB,
	M_JMP1, 0x84,		/* 132 */
/* 2738 */
	ZXOR,
	M_JMP1, 0x5A,		/* 90 */
/* 2741 */
	ZNOT, M_LO, M_AL,
	M_JMPB, 0x06,		/* 2738 */
/* 2746 */
	ZXOR,
	M_JMP2, 0x6E, 0x01,	/* 366 */
/* 2750 */
	ZXOR, M_LO, M_AL, M_LO, M_AR,
	ZNOT, M_HI, M_AL,
	M_JMP1, 0x5E,		/* 94 */
/* 2760 */
	ZXOR, M_LO, M_AL, M_LO, M_AR,
	M_JMPB, 0x15,		/* 2744 */
/* 2767 */
	ZXOR, M_LO, M_AL, M_LO, M_R,
	ZXOR,
	M_JMP2, 0x7C, 0x01,	/* 380 */
/* 2776 */
	ZXOR,
	M_JMP2, 0x83, 0x01,	/* 387 */
/* 2780 */
	ZXOR,
	M_JMP2, 0x8A, 0x01,	/* 394 */
/* 2784 */
	ZXORB,
	M_JMP2, 0x99, 0x01,	/* 409 */
/* 2788 */
	ZXORB,
	M_JMP2, 0xA0, 0x01,	/* 416 */
/* 2792 */
	ZXOR,
	M_JMP2, 0xA8, 0x01,	/* 424 */
/* 2796 */
	ZXOR,
	M_JMP2, 0xAD, 0x01,	/* 429 */
/* 2800 */
	ZXORB,
	M_JMP2, 0xB2, 0x01,	/* 434 */
/* 2804 */
	ZXORB,
	M_JMP2, 0xB8, 0x01,	/* 440 */
/* /usr/src/c/n1/i8086/tables/bef.t */
/* 2808 */
	M_OP1,
	M_JMP1, 0x01,		/* 1 */
/* 2811 */
	M_OP1,
	M_JMP1, 0x29,		/* 41 */
/* 2814 */
	M_OP1,
	M_JMP1, 0x36,		/* 54 */
/* 2817 */
	M_OP0,
	M_JMP1, 0x3F,		/* 63 */
/* 2820 */
	M_OP0,
	M_JMP1, 0x50,		/* 80 */
/* 2823 */
	M_OP0,
	M_JMP1, 0x55,		/* 85 */
/* 2826 */
	M_TL, M_OP1,
	M_JMP1, 0x0D,		/* 13 */
/* 2830 */
	M_TL, M_OP1,
	M_JMP1, 0x19,		/* 25 */
/* 2834 */
	M_OP0, M_LO, M_AL, M_ICON, 0x02,
	M_OP2, M_HI, M_AL, M_ICON, 0x03,
	M_JMP1, 0x5E,		/* 94 */
/* 2846 */
	ZMOV, M_R, M_AL,
	M_OP0,
	M_JMP2, 0x17, 0x01,	/* 279 */
/* 2853 */
	ZMOVB, M_LO, M_R, M_AL,
	M_OP0,
	M_JMP2, 0xBF, 0x06,	/* 1727 */
/* /usr/src/c/n1/i8086/tables/div.t */
/* 2861 */
	ZCWD,
	ZIDIV, M_AR,
	M_END,
/* 2865 */
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZDIV, M_AR,
	M_END,
/* 2873 */
	ZFDIVI, M_AR,
	M_END,
/* 2876 */
	ZFDIVL, M_AR,
	M_END,
/* 2879 */
	ZFDIVF, M_AR,
	M_END,
/* 2882 */
	ZFDIVD, M_AR,
	M_END,
/* /usr/src/c/n1/i8086/tables/leaves.t */
/* 2885 */
	ZSUB, M_R, M_R,
	M_END,
/* 2889 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZMOV, M_HI, M_R, M_LO, M_R,
	M_END,
/* 2900 */
	ZFLDZ,
	M_END,
/* 2902 */
	ZFLD1,
	M_END,
/* 2904 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	M_JMP2, 0xDB, 0x01,	/* 475 */
/* 2912 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	M_JMP1, 0x2F,		/* 47 */
/* 2919 */
	ZLEA, M_R, M_NSE, M_AL,
	M_END,
/* 2924 */
	ZMOV, M_R, M_AL,
	M_END,
/* 2928 */
	M_JMP1, 0x2A,		/* 42 */
/* 2930 */
	ZFLDD, M_AL,
	M_END,
/* 2933 */
	ZLEA, M_LO, M_R, M_NSE, M_AL,
	ZPUSH, M_REGNO, SS,
	M_JMP2, 0x1C, 0x02,	/* 540 */
/* 2944 */
	M_JMP1, 0x2A,		/* 42 */
/* 2946 */
	ZMOV, M_LO, M_R, M_LO, M_AL,
	ZPUSH, M_HI, M_AL,
	M_JMP2, 0x1C, 0x02,	/* 540 */
/* 2957 */
	ZMOV, M_LO, M_R,
	M_JMPB, 0x34,		/* 2908 */
/* 2962 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZMOV, M_HI, M_R, M_LO, M_R,
	ZMOV, M_LO,
	M_JMPB, 0x31,		/* 2925 */
/* 2976 */
	M_JMP1, 0x2A,		/* 42 */
/* 2978 */
	ZMOV, M_LO, M_R, M_HI, M_AL,
	ZMOV, M_HI, M_R, M_LO, M_R,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	M_END,
/* 2994 */
	ZMOV, M_LO, M_R, M_AL,
	M_JMP2, 0x19, 0x02,	/* 537 */
/* 3001 */
	ZMOV, M_LO, M_R, M_AL,
	M_JMP2, 0x23, 0x02,	/* 547 */
/* 3008 */
	M_JMP1, 0x2A,		/* 42 */
/* 3010 */
	ZLDES,
	M_JMPB, 0x26,		/* 2973 */
/* 3013 */
	ZMOV, M_R, M_AL,
	ZCBW,
	M_END,
/* 3018 */
	ZMOVB, M_LO, M_R, M_AL,
	M_JMP2, 0xB9, 0x02,	/* 697 */
/* 3025 */
	ZMOVB, M_LO, M_R, M_AL,
	M_JMP2, 0x7A, 0x06,	/* 1658 */
/* 3032 */
	M_END,
/* 3033 */
	ZMOV, M_LO, M_R, M_AL,
	ZCWD,
	M_END,
/* 3039 */
	ZCWD,
	M_END,
/* 3041 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZCBW,
	ZCWD,
	M_END,
/* 3049 */
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	ZCWD,
	M_END,
/* 3063 */
	M_JMPB, 0x1E,		/* 3033 */
/* 3065 */
	M_JMPB, 0x6C,		/* 2957 */
/* 3067 */
	ZFLDI, M_AL,
	M_END,
/* 3070 */
	ZFLDL, M_AL,
	M_END,
/* 3073 */
	ZFLDF, M_AL,
	M_END,
/* 3076 */
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZMOVB, M_LO, M_LO,
	M_JMPB, 0x46,		/* 3014 */
/* 3086 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZMOV, M_HI, M_R, M_LO,
	M_JMPB, 0x0F,		/* 3080 */
/* 3097 */
	ZSUB, M_HI, M_R, M_HI, M_R,
	ZMOVB, M_LO, M_LO, M_R, M_AL,
	ZSUBB, M_HI, M_LO, M_R, M_HI, M_LO, M_R,
	M_END,
/* 3115 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZMOV, M_HI, M_R, M_LO, M_R,
	ZMOVB, M_LO,
	M_JMPB, 0x9A,		/* 2973 */
/* 3129 */
	ZSUB, M_HI, M_R, M_HI,
	M_JMPB, 0xA2,		/* 2971 */
/* 3135 */
	M_JMPB, 0xAD,		/* 2962 */
/* 3137 */
	ZMOV, M_HI, M_R, M_HI, M_AL,
	M_JMPB, 0x9A,		/* 2988 */
/* 3144 */
	ZMOV, M_HI, M_R, M_HI, M_RL,
	ZMOV, M_LO, M_R, M_LO, M_RL,
	M_END,
/* 3155 */
	M_JMPB, 0x12,		/* 3137 */
/* 3157 */
	ZMOV, M_HI, M_R, M_REGNO, SS,
	ZLEA, M_LO,
	M_JMPB, 0xF4,		/* 2920 */
/* 3166 */
	M_JMPB, 0x25,		/* 3129 */
/* 3168 */
	M_JMPB, 0x1F,		/* 3137 */
/* 3170 */
	ZMOV, M_HI, M_R, M_REGNO, DS,
	M_JMPB, 0xCB,		/* 2972 */
/* 3177 */
	M_JMPB, 0x28,		/* 3137 */
/* 3179 */
	ZMOV,
	M_JMPB, 0xBE,		/* 2990 */
/* 3182 */
	M_END,
/* 3183 */
	M_JMPB, 0x04,		/* 3179 */
/* 3185 */
	ZMOV, M_R, M_LO,
	M_JMPB, 0xAD,		/* 3015 */
/* 3190 */
	ZMOV, M_R, M_LO, M_AL,
	ZSUBB,
	M_JMP2, 0xDC, 0x01,	/* 476 */
/* 3198 */
	M_JMPB, 0x0D,		/* 3185 */
/* 3200 */
	M_JMPB, 0x0A,		/* 3190 */
/* 3202 */
	ZCBW,
	M_END,
/* 3204 */
	M_END,
/* 3205 */
	M_JMPB, 0xC0,		/* 3013 */
/* 3207 */
	ZMOV, M_R,
	M_JMPB, 0x10,		/* 3193 */
/* 3211 */
	ZMOVB, M_LO, M_R, M_HI,
	M_JMPB, 0xC8,		/* 3015 */
/* 3217 */
	ZMOVB, M_LO, M_R, M_HI,
	M_JMPB, 0x1C,		/* 3193 */
/* 3223 */
	ZCBW,
	M_END,
/* 3225 */
	M_JMPB, 0x1F,		/* 3194 */
/* 3227 */
	ZOR, M_AL, M_AL,
	M_REL0, M_LAB,
	M_END,
/* 3233 */
	ZCMP, M_AL, M_ICON, 0x03,
	M_REL0, M_LAB,
	M_END,
/* 3240 */
	ZCMPB,
	M_JMPB, 0x07,		/* 3234 */
/* 3243 */
	ZOR, M_LO, M_AL, M_HI,
	M_JMPB, 0x12,		/* 3229 */
/* 3249 */
	ZMOV, M_R, M_LO, M_AL,
	ZOR, M_R,
	M_JMPB, 0x09,		/* 3246 */
/* 3257 */
	ZOR, M_HI, M_AL, M_HI, M_AL,
	M_REL1, M_LAB,
	M_END,
/* 3265 */
	ZCMP, M_HI, M_AL, M_ICON, 0x03,
	M_REL1, M_LAB,
	M_END,
/* 3273 */
	ZOR, M_HI, M_AL, M_HI, M_AL,
	ZJS, M_LAB,
	ZJNE, M_LAB0,
	ZOR, M_LO, M_AL, M_LO, M_AL,
	M_JMP2, 0xCA, 0x04,	/* 1226 */
/* 3290 */
	ZCMP, M_HI, M_AL, M_ICON, 0x03,
	ZJS, M_LAB,
	ZJNE, M_LAB0,
	ZCMP, M_LO, M_AL, M_ICON, 0x03,
	M_JMP2, 0xCA, 0x04,	/* 1226 */
/* 3307 */
	ZOR, M_HI, M_AL, M_HI, M_AL,
	ZJS, M_LAB0,
	ZJNE, M_LAB,
	ZOR, M_LO, M_AL, M_LO, M_AL,
	ZJNE, M_LAB,
	M_DLAB0,
	M_END,
/* 3325 */
	ZCMP, M_HI, M_AL, M_ICON, 0x03,
	ZJS, M_LAB0,
	ZJNE, M_LAB,
	ZCMP, M_LO, M_AL, M_ICON, 0x03,
	M_JMPB, 0x12,		/* 3321 */
/* 3341 */
	M_CALL, M_GID, 0x07,
	M_REL0, M_LAB,
	M_END,
/* 3347 */
	M_JMPB, 0x68,		/* 3243 */
/* 3349 */
	ZMOV, M_R, M_HI, M_AL,
	ZOR, M_R, M_LO,
	M_JMPB, 0x7F,		/* 3229 */
/* 3358 */
	ZCMP, M_LO,
	M_JMPB, 0x7E,		/* 3234 */
/* 3362 */
	ZJMP, M_LAB,
	M_END,
/* 3365 */
	M_END,
/* 3366 */
	ZOR, M_RL, M_RL,
	M_REL0, M_LAB,
	M_END,
/* 3372 */
	ZOR, M_LO, M_RL, M_HI,
	M_JMPB, 0x08,		/* 3368 */
/* 3378 */
	ZOR, M_LO, M_RL, M_LO,
	M_JMPB, 0x0E,		/* 3368 */
/* 3384 */
	ZPUSH, M_AL,
	M_END,
/* 3387 */
	ZSUB, M_R, M_R,
	ZPUSH, M_R,
	M_END,
/* 3393 */
	ZMOV,
	M_JMP2, 0x68, 0x02,	/* 616 */
/* 3397 */
	ZLEA, M_R, M_NSE,
	M_JMP2, 0x69, 0x02,	/* 617 */
/* 3403 */
	ZPUSH, M_REGNO, SS,
	ZLEA, M_LO, M_R, M_NSE, M_AL,
	ZPUSH, M_LO, M_R,
	M_END,
/* 3415 */
	ZPUSH, M_ICON, 0x03,
	M_JMPB, 0x22,		/* 3384 */
/* 3420 */
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_END,
/* 3427 */
	ZSUB, M_R, M_R,
	ZPUSH,
	M_JMPB, 0x2A,		/* 3389 */
/* 3433 */
	ZSUB, M_R, M_R,
	ZPUSH, M_R,
	M_JMPB, 0x2D,		/* 3393 */
/* 3440 */
	ZMOV, M_R, M_HI, M_AL,
	ZPUSH, M_R,
	ZMOV, M_R, M_LO,
	M_JMP2, 0x69, 0x02,	/* 617 */
/* 3452 */
	ZPUSH, M_REGNO, CS,
	M_JMPB, 0x25,		/* 3418 */
/* 3457 */
	ZPUSH, M_REGNO, DS,
	M_JMPB, 0x2A,		/* 3418 */
/* 3462 */
	ZPUSH, M_REGNO, CS,
	ZMOV, M_LO, M_R,
	M_JMPB, 0x3A,		/* 3410 */
/* 3470 */
	ZPUSH, M_REGNO, DS,
	M_JMPB, 0x08,		/* 3465 */
/* 3475 */
	M_JMPB, 0x5B,		/* 3384 */
/* 3477 */
	M_JMPB, 0x39,		/* 3420 */
/* 3479 */
	ZSUB, M_LO, M_R, M_LO, M_R,
	ZPUSH, M_LO, M_R,
	M_JMPB, 0x4C,		/* 3411 */
/* 3489 */
	ZMOV, M_LO, M_R, M_HI, M_AL,
	ZPUSH, M_LO, M_R,
	ZMOV, M_LO, M_R, M_LO,
	M_JMPB, 0x5B,		/* 3410 */
/* 3503 */
	M_JMPB, 0x53,		/* 3420 */
/* 3505 */
	M_CALL, M_GID, 0x08,
	M_END,
/* 3509 */
	M_END,
/* 3510 */
	M_END,
/* 3511 */
	ZFDROP,
	M_END,
/* /usr/src/c/n1/i8086/tables/mul.t */
/* 3513 */
	ZIMULI, M_R, M_AL, M_AR,
	M_END,
/* 3518 */
	ZIMULI, M_R, M_AR, M_AL,
	M_END,
/* 3523 */
	ZIMUL, M_AR,
	M_END,
/* 3526 */
	ZMUL, M_AR,
	M_END,
/* 3529 */
	ZFMULI, M_AR,
	M_END,
/* 3532 */
	ZFMULL, M_AR,
	M_END,
/* 3535 */
	ZFMULF, M_AR,
	M_END,
/* 3538 */
	ZFMULD, M_AR,
	M_END,
/* /usr/src/c/n1/i8086/tables/neg.t */
/* 3541 */
	ZNEG,
	M_JMP1, 0x9B,		/* 155 */
/* 3544 */
	ZNEG, M_HI, M_R,
	ZNEG, M_LO, M_R,
	ZSBB, M_HI, M_R, M_ICON, 0x03,
	M_JMP1, 0x6C,		/* 108 */
/* 3557 */
	ZFCHS,
	M_END,
/* /usr/src/c/n1/i8086/tables/not.t */
/* 3559 */
	ZNOT, M_R,
	M_END,
/* 3562 */
	ZNOT, M_LO, M_R,
	ZNOT,
	M_JMP1, 0xC3,		/* 195 */
/* /usr/src/c/n1/i8086/tables/or.t */
/* 3568 */
	ZOR,
	M_JMP2, 0xCE, 0x01,	/* 462 */
/* 3572 */
	ZOR,
	M_JMP2, 0x4C, 0x02,	/* 588 */
/* 3576 */
	ZOR,
	M_JMP2, 0x45, 0x02,	/* 581 */
/* 3580 */
	ZOR, M_LO, M_R, M_LO, M_AR,
	M_JMPB, 0x09,		/* 3576 */
/* /usr/src/c/n1/i8086/tables/relop.t */
/* 3587 */
	ZCMPB,
	M_JMP2, 0x6C, 0x04,	/* 1132 */
/* 3591 */
	ZCMP,
	M_JMP2, 0x6C, 0x04,	/* 1132 */
/* 3595 */
	ZCMPB, M_LO, M_R,
	M_JMP2, 0x6D, 0x04,	/* 1133 */
/* 3601 */
	ZMOVB, M_LO, M_R, M_AL,
	M_JMPB, 0x0A,		/* 3595 */
/* 3607 */
	ZCMP,
	M_JMPB, 0x0B,		/* 3597 */
/* 3610 */
	ZCMP, M_HI, M_AL, M_HI, M_AR,
	ZJNE, M_LAB1,
	ZCMP, M_LO, M_AL, M_LO, M_AR,
	ZJE, M_LAB,
	M_DLAB1,
	M_END,
/* 3626 */
	ZOR, M_HI, M_RL, M_HI, M_RL,
	ZJNE, M_LAB1,
	ZCMP, M_LO, M_RL,
	M_JMPB, 0x10,		/* 3620 */
/* 3638 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	ZJNE, M_LAB1,
	ZOR, M_LO, M_RL, M_LO, M_RL,
	M_JMPB, 0x1C,		/* 3622 */
/* 3652 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	M_JMPB, 0x1A,		/* 3631 */
/* 3659 */
	ZOR, M_HI, M_RL, M_HI, M_RL,
	ZJNE, M_LAB1,
	ZCMP, M_LO, M_RL,
	M_JMPB, 0x30,		/* 3621 */
/* 3671 */
	ZMOV, M_R, M_HI, M_AR,
	ZOR, M_R, M_R,
	ZJNE, M_LAB1,
	ZCMP,
	M_JMPB, 0x2E,		/* 3635 */
/* 3683 */
	ZCMP, M_HI, M_AL, M_HI, M_AR,
	ZJNE, M_LAB,
	ZCMP, M_LO, M_AL, M_LO,
	M_JMP2, 0xAE, 0x04,	/* 1198 */
/* 3697 */
	ZOR, M_HI, M_RL, M_HI, M_RL,
	ZJNE, M_LAB,
	ZCMP, M_LO, M_RL,
	M_JMPB, 0x0E,		/* 3693 */
/* 3709 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	ZJNE, M_LAB,
	ZOR, M_LO, M_RL, M_LO, M_RL,
	ZJNE, M_LAB,
	M_END,
/* 3724 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	M_JMPB, 0x1B,		/* 3702 */
/* 3731 */
	ZOR, M_HI, M_RL, M_HI, M_RL,
	ZJNE, M_LAB,
	ZCMP, M_LO, M_RL,
	M_JMP2, 0xAE, 0x04,	/* 1198 */
/* 3744 */
	ZMOV, M_R, M_HI, M_AR,
	ZOR, M_R, M_R,
	ZJNE, M_LAB,
	ZCMP,
	M_JMPB, 0x30,		/* 3706 */
/* 3756 */
	ZCMP,
	M_JMP2, 0x8E, 0x04,	/* 1166 */
/* 3760 */
	ZCMP,
	M_JMP2, 0x9A, 0x04,	/* 1178 */
/* 3764 */
	ZCMP, M_LO,
	M_JMP2, 0x6C, 0x04,	/* 1132 */
/* 3769 */
	ZCMP, M_LO, M_RL,
	M_JMP2, 0x6D, 0x04,	/* 1133 */
/* 3775 */
	ZCMP,
	M_JMP2, 0x8F, 0x04,	/* 1167 */
/* 3779 */
	ZCMP,
	M_JMP2, 0x9B, 0x04,	/* 1179 */
/* 3783 */
	ZCMP, M_HI, M_AL, M_HI, M_AR,
	M_LREL1, M_LAB1,
	M_LREL0, M_LAB,
	ZCMP, M_LO, M_AL, M_LO, M_AR,
	M_LREL2,
	M_JMPB, 0xAF,		/* 3623 */
/* 3800 */
	ZOR, M_HI, M_RL, M_HI, M_RL,
	M_LREL1, M_LAB1,
	M_LREL0, M_LAB,
	ZCMP, M_LO, M_RL,
	M_JMPB, 0x11,		/* 3795 */
/* 3814 */
	ZCMP, M_HI, M_RL, M_HI, M_AR,
	M_JMPB, 0x0E,		/* 3805 */
/* 3821 */
	ZFCOMPI, M_AR,
	M_CALL, M_GID, 0x09,
	M_REL0, M_LAB,
	M_END,
/* 3829 */
	ZFCOMPL,
	M_JMPB, 0x08,		/* 3822 */
/* 3832 */
	ZFCOMPF,
	M_JMPB, 0x0B,		/* 3822 */
/* 3835 */
	ZFCOMPD,
	M_JMPB, 0x0E,		/* 3822 */
/* /usr/src/c/n1/i8086/tables/rem.t */
/* 3838 */
	M_JMP2, 0x2D, 0x0B,	/* 2861 */
/* 3841 */
	M_JMP2, 0x31, 0x0B,	/* 2865 */
/* /usr/src/c/n1/i8086/tables/shl.t */
/* 3844 */
	ZSAL,
	M_JMP2, 0xCE, 0x01,	/* 462 */
/* 3848 */
	M_JMPB, 0x04,		/* 3844 */
/* 3850 */
	ZSAL, M_R, M_ICON, 0x02,
	ZSAL, M_R, M_ICON, 0x02,
	M_JMP1, 0x07,		/* 7 */
/* 3860 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAL, M_R, M_REGNO, CL,
	M_JMP1, 0x07,		/* 7 */
/* 3870 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSAL, M_R, M_REGNO, CL,
	M_END,
/* 3879 */
	ZMOV, M_REGNO, CX, M_AR,
	M_DLAB0, ZSAL, M_LO, M_R, M_ICON, 0x02,
	ZRCL, M_HI, M_R, M_ICON, 0x02,
	ZLOOP, M_LAB0,
	M_END,
/* 3897 */
	ZMOV, M_REGNO, CX, M_AR,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSAL, M_LO, M_R, M_ICON, 0x02,
	ZRCL, M_HI, M_R, M_ICON, 0x02,
	ZLOOP, M_LAB1,
	M_DLAB0,
	M_END,
/* 3918 */
	ZSAL,
	M_JMP2, 0xD8, 0x01,	/* 472 */
/* 3922 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAL, M_LO, M_R, M_REGNO, CL,
	M_JMP2, 0xDB, 0x01,	/* 475 */
/* 3934 */
	ZMOV, M_REGNO, CX,
	M_JMPB, 0x0C,		/* 3925 */
/* /usr/src/c/n1/i8086/tables/shr.t */
/* 3939 */
	ZSAR,
	M_JMP2, 0xCE, 0x01,	/* 462 */
/* 3943 */
	ZSHR,
	M_JMP2, 0xCE, 0x01,	/* 462 */
/* 3947 */
	ZSAR,
	M_JMPB, 0x5D,		/* 3855 */
/* 3950 */
	ZSAR, M_R, M_ICON, 0x02,
	M_JMPB, 0x07,		/* 3947 */
/* 3956 */
	ZSHR,
	M_JMPB, 0x66,		/* 3855 */
/* 3959 */
	ZSHR, M_R, M_ICON, 0x02,
	M_JMPB, 0x07,		/* 3956 */
/* 3965 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSAR,
	M_JMPB, 0x69,		/* 3865 */
/* 3972 */
	ZMOVB, M_REGNO, CL, M_AR,
	ZSHR,
	M_JMPB, 0x70,		/* 3865 */
/* 3979 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSAR,
	M_JMPB, 0x6D,		/* 3875 */
/* 3986 */
	ZMOV, M_REGNO, CX, M_AR,
	ZSHR,
	M_JMPB, 0x74,		/* 3875 */
/* 3993 */
	ZMOV, M_REGNO, CX, M_AR,
	M_DLAB0, ZSAR, M_HI, M_R, M_ICON, 0x02,
	ZRCR, M_LO,
	M_JMPB, 0x72,		/* 3891 */
/* 4007 */
	ZMOV, M_REGNO, CX, M_AR,
	M_DLAB0, ZSHR,
	M_JMPB, 0x0E,		/* 3999 */
/* 4015 */
	ZMOV, M_REGNO, CX, M_AR,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSAR, M_HI, M_R, M_ICON, 0x02,
	ZRCR, M_LO,
	M_JMPB, 0x76,		/* 3911 */
/* 4031 */
	ZMOV, M_REGNO, CX, M_AR,
	ZJCXZ, M_LAB0,
	M_DLAB1, ZSHR,
	M_JMPB, 0x10,		/* 4023 */
/* /usr/src/c/n1/i8086/tables/sub.t */
/* 4041 */
	ZDEC,
	M_JMP1, 0x9B,		/* 155 */
/* 4044 */
	ZDEC, M_LO, M_R,
	M_END,
/* 4048 */
	ZDEC, M_R,
	M_JMPB, 0x09,		/* 4041 */
/* 4052 */
	ZDEC, M_LO, M_R,
	M_JMPB, 0x0B,		/* 4044 */
/* 4057 */
	ZSUB,
	M_JMP2, 0xCE, 0x01,	/* 462 */
/* 4061 */
	ZSUB,
	M_JMP2, 0xD3, 0x01,	/* 467 */
/* 4065 */
	ZSUB, M_LO, M_R, M_LO, M_AR,
	ZSBB,
	M_JMP2, 0x45, 0x02,	/* 581 */
/* 4074 */
	ZSUB,
	M_JMP2, 0x4C, 0x02,	/* 588 */
/* 4078 */
	ZMOV, M_R, M_LO, M_AL,
	ZSUB,
	M_JMP2, 0x4D, 0x02,	/* 589 */
/* 4086 */
	M_JMPB, 0x0C,		/* 4074 */
/* 4088 */
	ZFSUBI, M_AR,
	M_END,
/* 4091 */
	ZFSUBL, M_AR,
	M_END,
/* 4094 */
	ZFSUBF, M_AR,
	M_END,
/* 4097 */
	ZFSUBD, M_AR,
	M_END,
/* /usr/src/c/n1/i8086/tables/xor.t */
/* 4100 */
	ZXOR,
	M_JMP2, 0xCE, 0x01,	/* 462 */
/* 4104 */
	ZXOR,
	M_JMP2, 0x4C, 0x02,	/* 588 */
/* 4108 */
	ZXOR, M_LO, M_R, M_LO, M_AR,
	M_JMP2, 0xED, 0x0D,	/* 3565 */
/* 4116 */
	ZXOR,
	M_JMP2, 0x45, 0x02,	/* 581 */
/* 4120 */
	ZNOT, M_LO, M_R,
	M_JMPB, 0x07,		/* 4116 */
/* 4125 */
	ZXOR, M_LO, M_R, M_LO, M_AR,
	M_JMPB, 0x07,		/* 4123 */
 0
};
