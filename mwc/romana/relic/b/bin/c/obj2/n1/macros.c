#ifdef vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif
char macros[] = {
/* /src386/bin/c/n1/i386/tables/prefac.f */
/* /src386/bin/c/n1/i386/tables/aadd.t */
/*    0 */
	M_TL, M_OP1, M_AL,
	M_IFV, M_TL, ZMOVSX, M_R, M_AL, M_ENDIF,
	M_IFR, M_REL0, M_LAB, M_ENDIF,
	M_END,
/*   14 */
	M_TL, M_OP0, M_AL, M_AR,
	M_JMP1, 0x03,		/* 3 */
/*   20 */
	M_TL, M_OP0, M_AL, M_TL, M_R,
	M_JMP1, 0x03,		/* 3 */
/*   27 */
	M_TL, ZMOVZX, M_R, M_AL,
	M_OP0, M_R, M_AR,
	ZAND, M_R, M_EMASK,
	M_TL, ZAND, M_AL, M_TL, M_CMASK,
	M_TL, ZOR, M_AL, M_TL, M_R,
	M_END,
/*   48 */
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	ZCALL, M_GID, 0x01,
	ZADD, M_REGNO, ESP, M_ICON, 0x01,
	ZMOV, M_HI, M_AL, M_HI, M_R,
	ZMOV, M_LO, M_AL, M_LO, M_R,
	M_END,
/*   73 */
	ZPUSH, M_AL,
	ZCALL, M_GID, 0x02,
	ZADD, M_REGNO, ESP, M_ICON, 0x02,
	ZCALL, M_GID, 0x03,
	ZMOV, M_AL, M_LO, M_R,
	M_IFV, ZCALL, M_GID, 0x04, M_ENDIF,
	M_END,
/*   96 */
	M_TL, ZFADDD, M_AL,
	M_IFV, M_TL, ZFSTD, M_AL, M_ENDIF,
	M_IFE, M_TL, ZFSTPD, M_AL, M_ENDIF,
	M_END,
/* /src386/bin/c/n1/i386/tables/add.t */
/*  110 */
	M_OP1, M_R,
	M_JMP1, 0x09,		/* 9 */
/*  114 */
	M_OP0, M_R, M_AR,
	M_JMP1, 0x09,		/* 9 */
/*  119 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZCALL, M_GID, 0x01,
	ZADD, M_REGNO, ESP, M_ICON, 0x01,
	M_END,
/*  134 */
	M_TR, ZFADDD, M_AR,
	M_END,
/* /src386/bin/c/n1/i386/tables/adiv.t */
/*  138 */
	M_TL, ZMOVSX, M_REGNO, EAX, M_AL,
	ZSUB, M_REGNO, EDX, M_REGNO, EDX,
	ZDIV, M_AR,
	M_TL, ZMOV, M_AL, M_TL, M_REGNO, EAX,
	M_END,
/*  157 */
	M_TL, ZMOVSX, M_REGNO, EAX, M_AL,
	ZSUB, M_REGNO, EDX, M_REGNO, EDX,
	ZDIV, M_AR,
	M_TL, ZMOV, M_AL, M_TL, M_REGNO, EAX,
	M_IFV, M_TL, ZMOVSX, M_REGNO, EAX, M_TL, M_REGNO, EAX, M_ENDIF,
	M_END,
/*  185 */
	M_TL, ZMOVSX, M_REGNO, EAX, M_AL,
	ZCDQ,
	ZIDIV,
	M_JMP1, 0x95,		/* 149 */
/*  194 */
	M_TL, ZMOVZX, M_REGNO, EAX, M_AL,
	ZAND, M_REGNO, EAX, M_EMASK,
	ZSUB, M_REGNO, EDX, M_REGNO, EDX,
	ZDIV, M_AR,
	ZAND, M_REGNO, EAX, M_EMASK,
	M_TL, ZAND, M_AL, M_TL, M_CMASK,
	M_TL, ZOR,
	M_JMP1, 0x98,		/* 152 */
/*  223 */
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	ZCALL, M_GID, 0x05,
	M_JMP1, 0x39,		/* 57 */
/*  234 */
	ZPUSH, M_AL,
	ZCALL, M_GID, 0x06,
	M_JMP1, 0x4E,		/* 78 */
/*  241 */
	M_TL, ZFRDIVD,
	M_JMP1, 0x62,		/* 98 */
/* /src386/bin/c/n1/i386/tables/aft.t */
/*  245 */
	M_IFV, M_TL, ZMOVSX, M_R, M_AL, M_ENDIF,
	M_TL, M_OP1, M_AL,
	M_END,
/*  255 */
	M_IFV, ZMOV, M_R, M_AL, M_ENDIF,
	M_OP0, M_AL, M_AR,
	M_END,
/*  264 */
	M_TL, ZMOVZX, M_R, M_AL,
	M_IFV, ZAND, M_R, M_EMASK, M_ENDIF,
	M_IFV, ZPUSH, M_R, M_ENDIF,
	M_OP0, M_R, M_AR,
	ZAND, M_R, M_EMASK,
	M_TL, ZAND, M_AL, M_TL, M_CMASK,
	M_TL, ZOR, M_AL, M_TL, M_R,
	M_IFV, ZPOP, M_R, M_ENDIF,
	M_END,
/* /src386/bin/c/n1/i386/tables/alogic.t */
/*  298 */
	M_TL, M_OP0, M_AL, M_TL,
	M_JMP1, 0x11,		/* 17 */
/*  304 */
	M_JMP1, 0x14,		/* 20 */
/*  306 */
	M_TL, M_OP0, M_AL, M_TL, M_AR,
	M_IFV, M_TL, ZMOVZX, M_R,
	M_JMP1, 0x6B,		/* 107 */
/*  317 */
	M_TL, M_OP0, M_AL, M_TL, M_R,
	M_JMPB, 0x0B,		/* 311 */
/*  324 */
	M_TL, M_OP0, M_AL, M_TL, M_AR,
	M_IFV, M_TL, ZMOVZX, M_R, M_AL, M_ENDIF,
	M_IFV, ZAND, M_R, M_EMASK, M_ENDIF,
	M_END,
/*  341 */
	M_TL, M_OP0, M_AL, M_TL, M_R,
	M_JMPB, 0x11,		/* 329 */
/* /src386/bin/c/n1/i386/tables/amul.t */
/*  348 */
	ZIMULI, M_R, M_AL, M_AR,
	ZMOV, M_AL, M_R,
	M_END,
/*  356 */
	ZMOV, M_REGNO, EAX, M_AL,
	M_TL, ZIMUL, M_AR,
	ZMOV, M_AL,
	M_JMP1, 0x9A,		/* 154 */
/*  367 */
	M_TL, ZMOVSX, M_REGNO, EAX, M_AL,
	M_TL, ZIMUL,
	M_JMP1, 0xA8,		/* 168 */
/*  376 */
	M_TL, ZMOVZX, M_REGNO, EAX, M_AL,
	ZAND, M_REGNO, EAX, M_EMASK,
	ZMUL,
	M_JMP1, 0xD1,		/* 209 */
/*  388 */
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	ZCALL, M_GID, 0x07,
	M_JMP1, 0x39,		/* 57 */
/*  399 */
	ZPUSH, M_AL,
	ZCALL, M_GID, 0x08,
	M_JMP1, 0x4E,		/* 78 */
/*  406 */
	M_TL, ZFMULD,
	M_JMP1, 0x62,		/* 98 */
/* /src386/bin/c/n1/i386/tables/and.t */
/*  410 */
	M_TL, ZTEST, M_AL, M_TL, M_AR,
	M_REL0, M_LAB,
	M_END,
/*  418 */
	ZAND,
	M_JMP1, 0x73,		/* 115 */
/* /src386/bin/c/n1/i386/tables/arem.t */
/*  421 */
	M_TL, ZMOVSX, M_REGNO, EAX, M_AL,
	ZSUB, M_REGNO, EDX, M_REGNO, EDX,
	ZDIV, M_AR,
	M_TL, ZMOV, M_AL, M_TL, M_REGNO, EDX,
	M_END,
/*  440 */
	M_TL, ZMOVSX, M_REGNO, EAX, M_AL,
	ZSUB, M_REGNO, EDX, M_REGNO, EDX,
	ZDIV, M_AR,
	M_TL, ZMOV, M_AL, M_TL, M_REGNO, EDX,
	M_IFV, M_TL, ZMOVSX, M_REGNO, EDX, M_TL, M_REGNO, EDX, M_ENDIF,
	M_END,
/*  468 */
	M_TL, ZMOVSX, M_REGNO, EAX, M_AL,
	ZCDQ,
	ZIDIV,
	M_JMPB, 0x2B,		/* 432 */
/*  477 */
	M_TL, ZMOVZX, M_REGNO, EAX, M_AL,
	ZAND, M_REGNO, EAX, M_EMASK,
	ZSUB, M_REGNO, EDX, M_REGNO, EDX,
	ZDIV, M_AR,
	ZAND, M_REGNO, EAX, M_EMASK,
	M_TL, ZAND, M_AL, M_TL, M_CMASK,
	M_TL, ZOR,
	M_JMPB, 0x45,		/* 435 */
/* /src386/bin/c/n1/i386/tables/ashift.t */
/*  506 */
	M_JMP1, 0x0E,		/* 14 */
/*  508 */
	ZMOV, M_R, M_AL,
	ZPUSH, M_REGNO, ECX,
	ZMOV, M_REGNO, ECX, M_AR,
	M_TL, M_OP0, M_R, M_REGNO, CL,
	ZPOP, M_REGNO, ECX,
	M_JMPB, 0xAE,		/* 352 */
/*  528 */
	M_TL, ZMOVSX, M_R, M_AL,
	ZPUSH, M_REGNO, ECX,
	ZMOV, M_REGNO, ECX, M_AR,
	M_TL, M_OP0, M_TL, M_R, M_REGNO, CL,
	ZPOP, M_REGNO, ECX,
	M_TL, ZMOV, M_AL, M_TL, M_R,
	M_IFV, M_TL, ZMOVSX, M_R, M_TL, M_R, M_ENDIF,
	M_END,
/*  561 */
	M_TL, ZMOVZX, M_R, M_AL,
	ZAND, M_R, M_EMASK,
	M_OP1,
	M_JMP1, 0x20,		/* 32 */
/*  571 */
	M_TL, ZMOVZX, M_R, M_AL,
	ZAND, M_R, M_EMASK,
	ZPUSH, M_REGNO, ECX,
	ZMOV, M_REGNO, ECX, M_AR,
	M_OP1, M_R, M_REGNO, CL,
	ZPOP, M_REGNO, ECX,
	M_JMP1, 0x22,		/* 34 */
/* /src386/bin/c/n1/i386/tables/assign.t */
/*  594 */
	ZSUB, M_AL, M_AL,
	M_END,
/*  598 */
	ZMOV, M_AL, M_AR,
	M_END,
/*  602 */
	ZLEA, M_AL, M_NSE, M_AR,
	M_END,
/*  607 */
	M_TL, ZMOV, M_AL, M_TL, M_AR,
	M_END,
/*  613 */
	M_JMPB, 0x0F,		/* 598 */
/*  615 */
	ZMOVW, M_AL, M_AR,
	M_END,
/*  619 */
	M_IFV, M_TL, ZMOVSX, M_R, M_LO, M_RR, M_ENDIF,
	ZMOVW, M_AL, M_LO, M_R,
	M_IFR, ZCMPW, M_LO, M_R, M_ICON, 0x03,
	M_JMP1, 0x08,		/* 8 */
/*  638 */
	ZMOVB, M_AL, M_AR,
	M_END,
/*  642 */
	M_IFV, M_TL, ZMOVSX, M_R, M_LO, M_LO, M_RR, M_ENDIF,
	ZMOVB, M_AL, M_LO, M_LO, M_R,
	M_IFR, ZCMPB, M_LO,
	M_JMPB, 0x1A,		/* 632 */
/*  660 */
	M_TR, ZMOVSX, M_R, M_TR, M_RR,
	M_JMP2, 0x60, 0x01,	/* 352 */
/*  668 */
	M_TL, ZMOV, M_AL, M_TL, M_AR,
	M_IFV, M_TL, ZMOVSX, M_R, M_TL, M_AR, M_ENDIF,
	M_END,
/*  681 */
	M_TL, ZAND, M_AL, M_TL, M_CMASK,
	M_TL, ZOR,
	M_JMP2, 0x34, 0x01,	/* 308 */
/*  691 */
	M_TL, ZAND, M_AL, M_TL, M_CMASK,
	M_TL, ZOR,
	M_JMP2, 0x46, 0x01,	/* 326 */
/*  701 */
	M_TL, ZAND, M_AL, M_TL, M_CMASK,
	M_TL, ZOR, M_AL, M_TL, M_R,
	M_IFV, M_TL, ZMOVSX,
	M_JMP2, 0x3A, 0x01,	/* 314 */
/*  717 */
	M_TL, ZAND, M_AL, M_TL, M_CMASK,
	M_TL, ZOR, M_AL, M_TL, M_R,
	M_IFV, M_TL, ZMOVSX,
	M_JMP2, 0x4C, 0x01,	/* 332 */
/*  733 */
	ZSUB, M_R, M_R,
	ZMOV, M_HI, M_AL, M_R,
	ZMOV, M_LO, M_AL, M_R,
	M_END,
/*  745 */
	ZMOV, M_HI, M_AL, M_HI, M_AR,
	ZMOV, M_LO, M_AL, M_LO, M_AR,
	M_END,
/*  756 */
	M_JMP1, 0x3E,		/* 62 */
/*  758 */
	ZSUB, M_R, M_R,
	M_JMP2, 0x60, 0x01,	/* 352 */
/*  764 */
	ZMOV,
	M_JMPB, 0x0D,		/* 752 */
/*  767 */
	ZMOV, M_HI, M_R, M_HI, M_AR,
	ZMOV, M_LO, M_R, M_LO, M_AR,
	M_JMP1, 0x53,		/* 83 */
/*  779 */
	ZMOV, M_AL, M_HI, M_R,
	M_END,
/*  784 */
	M_JMP1, 0x53,		/* 83 */
/*  786 */
	M_TL, ZFSTPD, M_AL,
	M_END,
/*  790 */
	M_TL, ZFSTD, M_AL,
	M_END,
/* /src386/bin/c/n1/i386/tables/asub.t */
/*  794 */
	M_JMP1, 0x00,		/* 0 */
/*  796 */
	M_JMP1, 0x0E,		/* 14 */
/*  798 */
	M_JMP1, 0x14,		/* 20 */
/*  800 */
	M_JMP1, 0x1B,		/* 27 */
/*  802 */
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	ZCALL, M_GID, 0x09,
	M_JMP1, 0x39,		/* 57 */
/*  813 */
	ZPUSH, M_AL,
	ZCALL, M_GID, 0x0A,
	M_JMP1, 0x4E,		/* 78 */
/*  820 */
	M_TL, ZFRSUBD,
	M_JMP1, 0x62,		/* 98 */
/* /src386/bin/c/n1/i386/tables/bef.t */
/*  824 */
	M_JMP1, 0x00,		/* 0 */
/*  826 */
	M_OP0, M_AL, M_AR,
	M_IFV, ZMOV,
	M_JMP1, 0x06,		/* 6 */
/*  833 */
	M_JMP1, 0x1B,		/* 27 */
/* /src386/bin/c/n1/i386/tables/div.t */
/*  835 */
	ZCDQ,
	ZIDIV, M_AR,
	M_END,
/*  839 */
	ZSUB, M_REGNO, EDX, M_REGNO, EDX,
	ZDIV, M_AR,
	M_END,
/*  847 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZCALL, M_GID, 0x0B,
	M_JMP1, 0x80,		/* 128 */
/*  858 */
	M_TR, ZFDIVD, M_AR,
	M_END,
/* /src386/bin/c/n1/i386/tables/leaves.t */
/*  862 */
	ZSUB, M_R, M_R,
	M_END,
/*  866 */
	ZLEA, M_R, M_NSE, M_AL,
	M_END,
/*  871 */
	M_TN, ZMOV, M_TN, M_R, M_AL,
	M_END,
/*  877 */
	ZMOV, M_HI, M_R, M_HI, M_AL,
	ZMOV, M_LO, M_R, M_LO, M_AL,
	M_END,
/*  888 */
	M_JMPB, 0x0B,		/* 877 */
/*  890 */
	ZMOV, M_HI, M_R, M_AL,
	ZMOV, M_LO, M_R, M_ICON, 0x03,
	M_END,
/*  900 */
	ZMOV, M_LO, M_R, M_AL,
	ZCALL, M_GID, 0x04,
	M_END,
/*  908 */
	ZFLDZ,
	M_END,
/*  910 */
	ZFLD1,
	M_END,
/*  912 */
	ZFLDD, M_AL,
	M_END,
/*  915 */
	M_TL, ZMOVSX,
	M_JMPB, 0x2B,		/* 874 */
/*  919 */
	ZMOV, M_LO, M_R, M_AL,
	ZCALL, M_GID, 0x0C,
	M_END,
/*  927 */
	M_TL, ZMOVSX,
	M_JMPB, 0x09,		/* 920 */
/*  931 */
	ZMOV, M_LO, M_R, M_AL,
	ZCALL, M_GID, 0x0D,
	M_END,
/*  939 */
	M_TL, ZMOVZX,
	M_JMPB, 0x09,		/* 932 */
/*  943 */
	M_JMPB, 0x2B,		/* 900 */
/*  945 */
	M_TL,
	M_JMPB, 0x22,		/* 912 */
/*  948 */
	ZMOVW,
	M_JMPB, 0x42,		/* 883 */
/*  951 */
	M_END,
/*  952 */
	ZMOVW, M_LO,
	M_JMPB, 0x50,		/* 874 */
/*  956 */
	ZMOV,
	M_JMPB, 0x53,		/* 874 */
/*  959 */
	ZMOVB, M_LO, M_LO, M_R, M_LO,
	M_JMPB, 0x4F,		/* 885 */
/*  966 */
	M_END,
/*  967 */
	ZMOVB, M_LO,
	M_JMPB, 0x10,		/* 953 */
/*  971 */
	M_JMPB, 0x0F,		/* 956 */
/*  973 */
	ZMOV, M_REGNO, EDX, M_HI, M_AL,
	ZMOV, M_REGNO, EAX, M_LO, M_AL,
	ZCALL, M_GID, 0x0E,
	M_END,
/*  987 */
	ZMOV, M_REGNO, EDX, M_HI, M_AL,
	ZMOV, M_REGNO, EAX, M_LO, M_AL,
	ZCALL, M_GID, 0x0E,
	M_TN, ZMOVSX, M_REGNO, EAX, M_TN,
	M_JMP1, 0x9A,		/* 154 */
/* 1007 */
	ZMOV, M_REGNO, EDX, M_HI, M_AL,
	ZMOV, M_REGNO, EAX, M_LO, M_AL,
	ZCALL, M_GID, 0x0F,
	M_END,
/* 1021 */
	ZMOV, M_REGNO, EDX, M_HI, M_AL,
	ZMOV, M_REGNO, EAX, M_LO, M_AL,
	ZCALL, M_GID, 0x0F,
	M_TN, ZMOVZX,
	M_JMPB, 0x22,		/* 1002 */
/* 1038 */
	ZMOV, M_R, M_AL,
	ZCALL, M_GID, 0x10,
	M_END,
/* 1045 */
	ZMOV, M_R, M_AL,
	ZCALL, M_GID, 0x10,
	M_TN, ZMOVSX, M_R, M_TN, M_R,
	M_END,
/* 1057 */
	ZMOV, M_R, M_AL,
	ZCALL, M_GID, 0x11,
	M_END,
/* 1064 */
	ZMOV, M_R, M_AL,
	ZCALL, M_GID, 0x11,
	M_TN, ZMOVZX,
	M_JMPB, 0x13,		/* 1053 */
/* 1074 */
	ZOR, M_AL, M_AL,
	M_REL0, M_LAB,
	M_END,
/* 1080 */
	ZCMP, M_AL, M_ICON, 0x03,
	M_REL0, M_LAB,
	M_END,
/* 1087 */
	M_TL,
	M_JMPB, 0x08,		/* 1080 */
/* 1090 */
	ZJMP, M_LAB,
	M_END,
/* 1093 */
	M_END,
/* 1094 */
	ZOR, M_RL, M_RL,
	M_REL0, M_LAB,
	M_END,
/* 1100 */
	ZTEST, M_HI, M_AL, M_ICON, 0x04,
	M_REL0, M_LAB,
	M_END,
/* 1108 */
	ZTEST, M_HI, M_AL, M_ICON, 0x05,
	M_REL0, M_LAB,
	M_END,
/* 1116 */
	ZTEST,
	M_JMPB, 0x07,		/* 1110 */
/* 1119 */
	ZCALL, M_GID, 0x12,
	M_REL0, M_LAB,
	M_END,
/* 1125 */
	ZLEA, M_R, M_NSE, M_AL,
	ZPUSH, M_R,
	M_END,
/* 1132 */
	ZPUSH, M_AL,
	M_END,
/* 1135 */
	ZPUSH, M_HI, M_RL,
	ZPUSH, M_LO, M_RL,
	M_END,
/* 1142 */
	ZPUSH, M_HI, M_AL,
	ZPUSH, M_LO, M_AL,
	M_END,
/* 1149 */
	ZFWAIT,
	M_JMPB, 0x08,		/* 1142 */
/* 1152 */
	ZCALL, M_GID, 0x13,
	M_END,
/* 1156 */
	M_END,
/* 1157 */
	M_END,
/* 1158 */
	M_END,
/* 1159 */
	ZFDROP,
	M_END,
/* /src386/bin/c/n1/i386/tables/mul.t */
/* 1161 */
	ZIMULI, M_R, M_AL, M_AR,
	M_END,
/* 1166 */
	M_TL, ZIMUL, M_AR,
	M_END,
/* 1170 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZCALL, M_GID, 0x07,
	M_JMP1, 0x80,		/* 128 */
/* 1181 */
	M_TR, ZFMULD, M_AR,
	M_END,
/* /src386/bin/c/n1/i386/tables/neg.t */
/* 1185 */
	ZNEG, M_R,
	M_IFR, M_REL1,
	M_JMP1, 0x0B,		/* 11 */
/* 1191 */
	ZXOR, M_REGNO, EDX, M_ICON, 0x06,
	M_END,
/* 1197 */
	ZFCHS,
	M_END,
/* /src386/bin/c/n1/i386/tables/not.t */
/* 1199 */
	ZNOT, M_R,
	M_END,
/* /src386/bin/c/n1/i386/tables/orxor.t */
/* 1202 */
	M_JMP1, 0x72,		/* 114 */
/* /src386/bin/c/n1/i386/tables/relop.t */
/* 1204 */
	ZCMP, M_AL,
	M_JMP2, 0x9E, 0x01,	/* 414 */
/* 1209 */
	ZCMP, M_RL,
	M_JMP2, 0x9E, 0x01,	/* 414 */
/* 1214 */
	M_TL,
	M_JMPB, 0x0B,		/* 1204 */
/* 1217 */
	M_TL, ZCMP, M_TL, M_R,
	M_JMP2, 0x9E, 0x01,	/* 414 */
/* 1224 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZCALL, M_GID, 0x14,
	M_REL0, M_LAB,
	M_END,
/* 1236 */
	M_TR, ZFCOMPD, M_AR,
	ZCALL, M_GID, 0x15,
	M_REL0, M_LAB,
	M_END,
/* /src386/bin/c/n1/i386/tables/rem.t */
/* 1245 */
	M_JMP2, 0x43, 0x03,	/* 835 */
/* 1248 */
	M_JMP2, 0x47, 0x03,	/* 839 */
/* /src386/bin/c/n1/i386/tables/shift.t */
/* 1251 */
	M_TL,
	M_JMP1, 0x72,		/* 114 */
/* 1254 */
	ZMOV, M_REGNO, ECX, M_AR,
	M_TL, M_OP0, M_R, M_REGNO, CL,
	M_END,
/* /src386/bin/c/n1/i386/tables/sub.t */
/* 1264 */
	M_JMP1, 0x6E,		/* 110 */
/* 1266 */
	ZMOV, M_R, M_AL,
	M_OP0, M_R, M_AR,
	M_END,
/* 1273 */
	M_JMP1, 0x72,		/* 114 */
/* 1275 */
	ZPUSH, M_HI, M_AR,
	ZPUSH, M_LO, M_AR,
	ZCALL, M_GID, 0x16,
	M_JMP1, 0x80,		/* 128 */
/* 1286 */
	M_TR, ZFSUBD, M_AR,
	M_END,
 0
};
