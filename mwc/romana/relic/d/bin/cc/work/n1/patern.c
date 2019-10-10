#ifdef vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif
#if !TINY
#define fl(f,l)	, f, l
#else
#define fl(f,l)	/* f, l */
#endif
extern char macros[];
/* /newbits/bin/cc/c/n1/i8086/tables/prefac.f */
#define	WORD	(FS16|FU16|FSPTR|FSPTB)
#define	UWORD	(FU16|FSPTR|FSPTB)
#define	LONG	(FS32|FU32)
#define	INT	(FS16|FU16|FS32|FU32)
#define	BYTE	(FS8|FU8)
#define	NFLT	(FS8|FU8|FS16|FU16|FSPTR|FSPTB|FLPTR|FLPTB|FS32|FU32)
#define	PEREL	(PEQ|PNE)
#define	PSREL	(PEQ|PNE|PGT|PGE|PLT|PLE)
#define	PUREL	(PEQ|PNE|PUGT|PULE|PUGE|PULT)
#define	PREL	(PEQ|PNE|PGT|PGE|PLT|PLE|PUGT|PUGE|PULT|PULE)
#define	PNEREL	(PGT|PGE|PLT|PLE|PUGT|PUGE|PULT|PULE)
#define	LPTX	(FLPTR|FLPTB)
/* /newbits/bin/cc/c/n1/i8086/tables/aadd.t */
PAT p1[] = {
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  2,  0, &macros[   0] fl(1,8) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  2,  0, &macros[   0] fl(1,12) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  2,  2,  0, &macros[  12] fl(1,20) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  3,  2,  0, &macros[  24] fl(1,29) }
/* #ifndef ONLYSMALL */
/* #endif */,
 {  1,  1, ANYR, NONE, NONE, TEMP,  3,  1,  4,  1, &macros[  40] fl(1,55) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  3,  1,  4,  1, &macros[  40] fl(1,59) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[  45] fl(1,67) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  3,  1, &macros[  45] fl(1,71) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[  45] fl(1,75) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  3,  1, &macros[  45] fl(1,79) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  2,  5,  1, &macros[  47] fl(1,87) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  3,  5,  1, &macros[  52] fl(1,96) }
/* #ifndef ONLYSMALL */
/* #endif */,
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  6,  4, &macros[  57] fl(1,122) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  5,  4, &macros[  80] fl(1,131) },
 {  4,  1, ANYR, NONE, ANYR, TEMP,  1,  1,  7,  1, &macros[  88] fl(1,141) },
 {  5,  1, ANYL, NONE, ANYL, TEMP,  1,  1,  7,  1, &macros[  88] fl(1,145) },
 {  4,  1,   AX, NONE,   AX,   AX,  1,  2,  7,  1, &macros[  93] fl(1,153) },
 {  4,  1,   AX,   AX, NONE,   AX,  1,  3,  7,  1, &macros[  99] fl(1,162) }
/* #ifndef ONLYSMALL */
/* #endif */,
 {  6,  1, ANYR, NONE, NONE, TEMP,  1,  1,  4,  1, &macros[ 105] fl(1,188) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  4,  1, &macros[ 105] fl(1,192) },
 {  6,  1,   AX, NONE, NONE,   AX,  1,  2,  4,  1, &macros[ 116] fl(1,201) },
 {  6,  1,   AX, NONE, NONE,   AX,  1,  3,  4,  1, &macros[ 129] fl(1,211) },
 {  7,  4, DXAX, NONE, NONE, NONE,  1,  4,  8,  5, &macros[ 140] fl(1,221) },
 {  7,  4, DXAX, NONE, NONE, NONE,  1,  4,  8,  6, &macros[ 156] fl(1,230) },
 {  7,  4, DXAX, NONE, DXAX, NONE,  1,  4,  7,  4, &macros[ 167] fl(1,239) },
 {  8,  4, DXAX, NONE, DXAX, DXAX,  1,  4,  7,  4, &macros[ 169] fl(1,246) },
 {  9,  4, DXAX, NONE, NONE, DXAX,  1,  4,  4,  4, &macros[ 191] fl(1,256) },
 { 10,  1,   AX, NONE, NONE,   AX,  1,  7,  4,  1, &macros[ 213] fl(1,279) },
 { 10,  1, ANYR, NONE, NONE, TEMP,  1,  8,  4,  1, &macros[ 233] fl(1,290) }
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/aand.t */
PAT p2[] = {
 {  1,  1, ANYR, NONE, NONE, TEMP,  9,  1, 10,  1, &macros[ 250] fl(2,11) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[ 250] fl(2,15) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  2,  5,  1, &macros[ 253] fl(2,23) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  3,  5,  1, &macros[ 256] fl(2,32) },
 {  4,  1, ANYR, NONE, ANYR, TEMP,  1,  1,  7,  1, &macros[ 259] fl(2,41) },
 {  4,  1,   AX, NONE,   AX,   AX,  1,  2,  7,  1, &macros[ 262] fl(2,49) },
 {  4,  1,   AX, NONE,   AX,   AX,  1,  3,  7,  1, &macros[ 265] fl(2,58) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  6,  4, &macros[ 268] fl(2,67) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1,  4, 11,  4, &macros[ 276] fl(2,77) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1,  4, 12,  4, &macros[ 296] fl(2,85) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1,  4, 13,  4, &macros[ 304] fl(2,94) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  5,  4, &macros[ 310] fl(2,102) },
 { 11,  4, DXAX, NONE, DXAX, DXAX,  1,  4,  7,  4, &macros[ 313] fl(2,112) },
 { 11,  5, ANYR, NONE, NONE, TEMP,  1,  8, 14,  1, &macros[ 325] fl(2,129) },
 { 11,  9, ANYR, NONE, NONE, TEMP,  1,  8, 14,  1, &macros[ 333] fl(2,136) },
 { 11,  5,   AX, NONE, NONE, TEMP,  1,  7, 14,  1, &macros[ 348] fl(2,144) },
 { 11,  9, ANYR, NONE, NONE, TEMP,  1,  7, 14,  1, &macros[ 356] fl(2,151) },
 { 12,  5, ANYR, NONE, ANYR, TEMP,  1,  8,  7,  1, &macros[ 364] fl(2,159) },
 { 12,  9, ANYR, NONE, ANYR, TEMP,  1,  8,  7,  1, &macros[ 369] fl(2,166) },
 { 12,  5,   AX, NONE,   AX, TEMP,  1,  7,  7,  1, &macros[ 374] fl(2,174) },
 { 12,  9,   AX, NONE,   AX, TEMP,  1,  7,  7,  1, &macros[ 380] fl(2,181) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/add.t */
PAT p3[] = {
 { 13,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  2,  0, &macros[ 386] fl(3,11) },
 { 14,  1, ANYL, ANYL, NONE, TEMP,  7,  1,  2,  0, &macros[ 386] fl(3,15) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 13,  1, ANYR, ANYR, NONE, TEMP,  7,  1, 15,  0, &macros[ 389] fl(3,34) },
 { 14,  1, ANYL, ANYL, NONE, TEMP,  7,  1, 15,  0, &macros[ 389] fl(3,38) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 13,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  4,  1, &macros[ 393] fl(3,59) },
 { 14,  1, ANYL, ANYL, NONE, TEMP,  7,  1,  4,  1, &macros[ 393] fl(3,63) }
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */,
 { 15,  4, DXAX, DXAX, NONE, DXAX,  7,  4,  4,  4, &macros[ 398] fl(3,181) }
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/adiv.t */
PAT p4[] = {
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  5, 16,  1, &macros[ 410] fl(4,10) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  6, 16,  1, &macros[ 422] fl(4,19) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  8, 16,  1, &macros[ 434] fl(4,28) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  2, 16,  1, &macros[ 462] fl(4,40) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  3, 16,  1, &macros[ 480] fl(4,51) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  7, 16,  1, &macros[ 510] fl(4,62) },
 { 11,  4, DXAX, NONE, DXAX, DXAX,  1, 10,  7,  4, &macros[ 541] fl(4,74) },
 { 11,  4, DXAX, NONE, DXAX, DXAX,  1, 11,  7,  4, &macros[ 569] fl(4,87) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1, 10, 16,  4, &macros[ 586] fl(4,100) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1, 11, 16,  4, &macros[ 594] fl(4,113) }
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/aft.t */
PAT p5[] = {
 { 11,  1, ANYR, NONE, NONE, TEMP, 16,  1,  2,  0, &macros[ 602] fl(5,13) },
 {  2,  1, ANYL, NONE, NONE, TEMP, 16,  1,  2,  0, &macros[ 602] fl(5,17) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 11,  1, ANYR, NONE, NONE, TEMP, 16,  1,  5,  1, &macros[ 610] fl(5,41) },
 {  2,  1, ANYL, NONE, NONE, TEMP, 16,  1,  5,  1, &macros[ 610] fl(5,45) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 11,  1,   AX, NONE, NONE,   AX, 16,  2,  2,  0, &macros[ 619] fl(5,69) },
 { 11,  1,   AX, NONE, NONE,   AX, 16,  3,  2,  0, &macros[ 631] fl(5,77) },
 { 11,  4, ANYR, NONE, NONE, TEMP, 16,  4,  2,  0, &macros[ 645] fl(5,85) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  8,  5,  1, &macros[ 670] fl(5,94) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  7,  5,  1, &macros[ 702] fl(5,106) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/amul.t */
PAT p6[] = {
 { 16,  1, ANYR, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[ 730] fl(6,11) },
 { 17,  1, DXAX, NONE, NONE,   AX,  1,  5, 16,  1, &macros[ 737] fl(6,18) },
 { 17,  1, DXAX, NONE, NONE,   AX,  1,  9, 16,  1, &macros[ 745] fl(6,26) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  8, 16,  1, &macros[ 753] fl(6,34) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  2, 16,  1, &macros[ 766] fl(6,45) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  3, 16,  1, &macros[ 776] fl(6,55) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  7, 16,  1, &macros[ 792] fl(6,65) },
 { 11,  4, DXAX, NONE, DXAX, DXAX,  1, 10,  7,  4, &macros[ 806] fl(6,76) },
 { 11,  4, DXAX, NONE, DXAX, DXAX,  1, 11,  7,  4, &macros[ 824] fl(6,89) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1, 10, 16,  4, &macros[ 842] fl(6,102) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1, 11, 16,  4, &macros[ 850] fl(6,115) }
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/and.t */
PAT p7[] = {
 { 18,  0, NONE, NONE, NONE, NONE, 16,  1,  5,  1, &macros[ 858] fl(7,13) },
 { 18,  0, NONE, NONE, NONE, NONE,  9,  1,  4,  1, &macros[ 858] fl(7,17) },
 { 18,  0, NONE, NONE, NONE, NONE, 16, 12,  5,  1, &macros[ 864] fl(7,24) },
 { 19,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  4,  1, &macros[ 867] fl(7,31) },
 { 20,  0, NONE, NONE, NONE, NONE, 16,  4,  5,  4, &macros[ 871] fl(7,38) },
 { 20,  0,   AX, NONE, NONE, NONE, 16,  4,  4,  4, &macros[ 879] fl(7,45) },
 { 20,  0, NONE, ANYR, NONE, NONE,  7,  4,  4,  4, &macros[ 887] fl(7,53) },
 { 21,  0, NONE, NONE, NONE, NONE,  4,  4, 12,  4, &macros[ 892] fl(7,60) },
 { 21,  0, NONE, NONE, NONE, NONE,  4,  4,  6,  4, &macros[ 898] fl(7,67) },
 { 21,  0, NONE, ANYR, NONE, NONE,  7,  4, 12,  4, &macros[ 904] fl(7,74) },
 { 21,  0, NONE, ANYR, NONE, NONE,  7,  4,  6,  4, &macros[ 909] fl(7,81) },
 { 22,  0, NONE, NONE, NONE, NONE,  4,  4,  5,  4, &macros[ 914] fl(7,88) },
 { 22,  0, NONE, ANYR, NONE, NONE,  7,  4,  4,  4, &macros[ 929] fl(7,97) },
 { 23,  0, NONE, NONE, NONE, NONE,  4,  4,  5,  4, &macros[ 941] fl(7,106) },
 { 23,  0, NONE, ANYR, NONE, NONE,  7,  4,  4,  4, &macros[ 957] fl(7,116) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4, 13,  4, &macros[ 969] fl(7,126) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4, 12,  4, &macros[ 975] fl(7,132) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4, 11,  4, &macros[ 985] fl(7,139) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4,  6,  4, &macros[ 991] fl(7,145) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4,  4,  4, &macros[ 998] fl(7,152) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/aor.t */
PAT p8[] = {
 {  1,  1, ANYR, NONE, NONE, TEMP,  9,  1, 10,  1, &macros[1005] fl(8,11) },
 {  2,  1, ANYL, NONE, NONE, TEMP, 17,  1, 10,  1, &macros[1005] fl(8,15) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[1005] fl(8,19) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[1005] fl(8,23) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  2,  5,  1, &macros[1008] fl(8,31) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  3,  5,  1, &macros[1011] fl(8,40) },
 {  4,  1, ANYR, NONE, ANYR, TEMP,  1,  1,  7,  1, &macros[1014] fl(8,49) },
 {  5,  1, ANYL, NONE, ANYL, TEMP,  1,  1,  7,  1, &macros[1014] fl(8,53) },
 {  4,  1,   AX, NONE,   AX,   AX,  1,  2,  7,  1, &macros[1017] fl(8,61) },
 {  4,  1,   AX, NONE,   AX,   AX,  1,  3,  7,  1, &macros[1020] fl(8,70) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  6,  4, &macros[1023] fl(8,79) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1,  4, 12,  4, &macros[1026] fl(8,88) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  5,  4, &macros[1030] fl(8,96) },
 { 11,  4, DXAX, NONE, DXAX, DXAX,  1,  4,  7,  4, &macros[1037] fl(8,106) },
 { 11,  5, ANYR, NONE, NONE, TEMP,  1,  8, 14,  1, &macros[1046] fl(8,124) },
 { 11,  9, ANYR, NONE, NONE, TEMP,  1,  8, 14,  1, &macros[1050] fl(8,131) },
 { 11,  5,   AX, NONE, NONE, TEMP,  1,  7, 14,  1, &macros[1054] fl(8,139) },
 { 11,  9, ANYR, NONE, NONE, TEMP,  1,  7, 14,  1, &macros[1058] fl(8,146) },
 { 12,  5, ANYR, NONE, ANYR, TEMP,  1,  8,  7,  1, &macros[1062] fl(8,154) },
 { 12,  9, ANYR, NONE, ANYR, TEMP,  1,  8,  7,  1, &macros[1066] fl(8,161) },
 { 12,  5,   AX, NONE,   AX, TEMP,  1,  7,  7,  1, &macros[1070] fl(8,169) },
 { 12,  9,   AX, NONE,   AX, TEMP,  1,  7,  7,  1, &macros[1074] fl(8,176) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/arem.t */
PAT p9[] = {
 { 17,  1, DXAX, NONE, NONE,   DX,  1,  5, 16,  1, &macros[1078] fl(9,11) },
 { 17,  1, DXAX, NONE, NONE,   DX,  1,  9, 16,  1, &macros[1088] fl(9,20) },
 { 11,  1, DXAX, NONE, NONE,   DX,  1,  8, 16,  1, &macros[1100] fl(9,29) },
 { 11,  1, DXAX, NONE, NONE,   AX,  1,  2, 16,  1, &macros[1128] fl(9,41) },
 { 11,  1, DXAX, NONE, NONE,   DX,  1,  3, 16,  1, &macros[1154] fl(9,53) },
 { 11,  1, DXAX, NONE, NONE,   DX,  1,  7, 16,  1, &macros[1184] fl(9,64) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1, 10, 16,  4, &macros[1215] fl(9,76) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1, 11, 16,  4, &macros[1233] fl(9,89) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/ashl.t */
PAT p10[] = {
 { 25,  1, ANYR, NONE, NONE, TEMP,  1,  1, 18,  1, &macros[1251] fl(10,12) },
 { 26,  1, ANYL, NONE, NONE, TEMP,  1,  1, 18,  1, &macros[1251] fl(10,16) },
 { 25,  1,   AX, NONE, NONE, TEMP,  1,  2, 18,  1, &macros[1254] fl(10,24) },
 { 25,  1,   AX, NONE, NONE, TEMP,  1,  3, 18,  1, &macros[1257] fl(10,33) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  2,  0, &macros[1260] fl(10,42) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  2,  0, &macros[1260] fl(10,46) },
 {  1,  1,   AX, NONE, NONE, TEMP,  1,  2,  2,  0, &macros[1266] fl(10,54) },
 {  1,  1,   AX, NONE, NONE, TEMP,  1,  3,  2,  0, &macros[1272] fl(10,63) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1, 15,  0, &macros[1278] fl(10,72) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1, 15,  0, &macros[1278] fl(10,76) },
 {  1,  1,   AX, NONE, NONE, TEMP,  1,  2, 15,  0, &macros[1284] fl(10,85) },
 {  1,  1,   AX, NONE, NONE, TEMP,  1,  3, 15,  0, &macros[1290] fl(10,95) },
 {  1,  1, ANYR, NONE,   CX, TEMP,  1,  1,  5,  1, &macros[1296] fl(10,105) },
 {  2,  1, ANYL, NONE,   CX, TEMP,  1,  1,  5,  1, &macros[1296] fl(10,109) },
 {  1,  1,   AX, NONE,   CX, TEMP,  1,  2,  5,  1, &macros[1306] fl(10,118) },
 {  1,  1,   AX, NONE,   CX, TEMP,  1,  3,  5,  1, &macros[1316] fl(10,128) },
 {  9,  4, DXAX, NONE,   CX, TEMP,  1,  4,  5,  1, &macros[1326] fl(10,138) },
 { 11,  1, ANYR, NONE,   CX, TEMP,  1,  1, 16,  1, &macros[1355] fl(10,152) },
 { 11,  1,   AX, NONE,   CX, TEMP,  1,  2, 16,  1, &macros[1366] fl(10,160) },
 { 11,  1,   AX, NONE,   CX, TEMP,  1,  3, 16,  1, &macros[1381] fl(10,169) },
 { 10,  4, DXAX, NONE,   CX, TEMP,  1,  4, 16,  1, &macros[1401] fl(10,178) },
 { 27,  1, ANYR, NONE, NONE, TEMP,  1,  8, 18,  1, &macros[1442] fl(10,193) },
 { 27,  1,   AX, NONE, NONE, TEMP,  1,  7, 18,  1, &macros[1452] fl(10,204) },
 { 11,  1, ANYR, NONE, NONE, TEMP,  1,  8,  2,  0, &macros[1477] fl(10,215) },
 { 11,  1,   AX, NONE, NONE, TEMP,  1,  7,  2,  0, &macros[1479] fl(10,226) },
 { 11,  1, ANYR, NONE, NONE, TEMP,  1,  8, 15,  0, &macros[1481] fl(10,237) },
 { 11,  1,   AX, NONE, NONE, TEMP,  1,  7, 15,  0, &macros[1498] fl(10,249) },
 { 11,  1, ANYR, NONE,   CX, TEMP,  1,  8,  5,  1, &macros[1519] fl(10,261) },
 { 11,  1,   AX, NONE,   CX, TEMP,  1,  7,  5,  1, &macros[1536] fl(10,273) },
 { 11,  1, ANYR, NONE,   CX, TEMP,  1,  8, 16,  1, &macros[1556] fl(10,285) },
 { 11,  1,   AX, NONE,   CX, TEMP,  1,  7, 16,  1, &macros[1568] fl(10,297) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/ashr.t */
PAT p11[] = {
 { 25,  1, ANYR, NONE, NONE, TEMP,  1,  5, 18,  1, &macros[1582] fl(11,12) },
 { 25,  1, ANYR, NONE, NONE, TEMP,  1,  9, 18,  1, &macros[1585] fl(11,20) },
 { 27,  1, ANYR, NONE, NONE, TEMP,  1,  8, 18,  1, &macros[1588] fl(11,28) },
 { 25,  1,   AX, NONE, NONE, TEMP,  1,  2, 18,  1, &macros[1598] fl(11,39) },
 { 25,  1,   AX, NONE, NONE, TEMP,  1,  3, 18,  1, &macros[1601] fl(11,48) },
 { 27,  1,   AX, NONE, NONE, TEMP,  1,  7, 18,  1, &macros[1604] fl(11,57) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  5,  2,  0, &macros[1616] fl(11,68) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  9,  2,  0, &macros[1618] fl(11,76) },
 { 11,  1, ANYR, NONE, NONE, TEMP,  1,  8,  2,  0, &macros[1620] fl(11,84) },
 {  1,  1,   AX, NONE, NONE, TEMP,  1,  2,  2,  0, &macros[1622] fl(11,95) },
 {  1,  1,   AX, NONE, NONE, TEMP,  1,  3,  2,  0, &macros[1624] fl(11,104) },
 { 11,  1,   AX, NONE, NONE, TEMP,  1,  7,  2,  0, &macros[1626] fl(11,113) },
 { 11,  4, DXAX, NONE, NONE, TEMP,  1, 10,  2,  0, &macros[1628] fl(11,124) },
 { 11,  4, DXAX, NONE, NONE, TEMP,  1, 11,  2,  0, &macros[1638] fl(11,133) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  5, 15,  0, &macros[1641] fl(11,142) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  9, 15,  0, &macros[1649] fl(11,151) },
 { 11,  1, ANYR, NONE, NONE, TEMP,  1,  8, 15,  0, &macros[1657] fl(11,160) },
 {  1,  1,   AX, NONE, NONE, TEMP,  1,  2, 15,  0, &macros[1671] fl(11,172) },
 {  1,  1,   AX, NONE, NONE, TEMP,  1,  3, 15,  0, &macros[1679] fl(11,182) },
 { 11,  1,   AX, NONE, NONE, TEMP,  1,  7, 15,  0, &macros[1687] fl(11,192) },
 {  1,  1, ANYR, NONE,   CX, TEMP,  1,  5,  5,  1, &macros[1704] fl(11,204) },
 {  1,  1, ANYR, NONE,   CX, TEMP,  1,  9,  5,  1, &macros[1712] fl(11,213) },
 { 11,  1, ANYR, NONE,   CX, TEMP,  1,  8,  5,  1, &macros[1720] fl(11,222) },
 {  1,  1,   AX, NONE,   CX, TEMP,  1,  2,  5,  1, &macros[1734] fl(11,234) },
 {  1,  1,   AX, NONE,   CX, TEMP,  1,  3,  5,  1, &macros[1742] fl(11,244) },
 { 11,  1,   AX, NONE,   CX, TEMP,  1,  7,  5,  1, &macros[1750] fl(11,254) },
 {  9,  4, DXAX, NONE,   CX, TEMP,  1, 10,  5,  1, &macros[1766] fl(11,266) },
 {  9,  4, DXAX, NONE,   CX, DXAX,  1, 11,  5,  1, &macros[1791] fl(11,280) },
 { 11,  1, ANYR, NONE,   CX, TEMP,  1,  5, 16,  1, &macros[1809] fl(11,294) },
 { 11,  1, ANYR, NONE,   CX, TEMP,  1,  9, 16,  1, &macros[1817] fl(11,302) },
 { 11,  1, ANYR, NONE,   CX, TEMP,  1,  8, 16,  1, &macros[1825] fl(11,310) },
 { 11,  1,   AX, NONE,   CX, TEMP,  1,  2, 16,  1, &macros[1837] fl(11,322) },
 { 11,  1,   AX, NONE,   CX, TEMP,  1,  3, 16,  1, &macros[1845] fl(11,331) },
 { 11,  1,   AX, NONE,   CX, TEMP,  1,  7, 16,  1, &macros[1853] fl(11,340) },
 { 10,  4, DXAX, NONE,   CX, TEMP,  1, 10, 16,  1, &macros[1867] fl(11,352) },
 { 10,  4, DXAX, NONE,   CX, TEMP,  1, 11, 16,  1, &macros[1894] fl(11,367) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/assign.t */
PAT p12[] = {
 {  7,  1, NONE, NONE, NONE, NONE,  3,  1, 19,  0, &macros[1914] fl(12,18) },
 {  7,  1, NONE, NONE, NONE, NONE,  3,  1, 10,  1, &macros[1918] fl(12,24) },
 {  7,  1, NONE, NONE, NONE, NONE,  1,  1,  3,  1, &macros[1918] fl(12,28) },
 {  7,  1, NONE, NONE, NONE, NONE,  1,  1,  5,  1, &macros[1918] fl(12,32) },
 {  7,  1, NONE, NONE, NONE, NONE,  3,  1, 10,  4, &macros[1922] fl(12,38) },
 {  7,  1, NONE, NONE, NONE, NONE,  9,  1, 20,  1, &macros[1927] fl(12,44) },
 {  7,  1, NONE, NONE, NONE, NONE,  1, 12,  5,  1, &macros[1932] fl(12,50) },
 {  7,  4, NONE, NONE, NONE, NONE,  1,  4,  5,  4, &macros[1936] fl(12,56) },
 { 28,  1, ANYR, NONE, ANYR, TEMP,  1,  2,  7, 12, &macros[1947] fl(12,63) },
 { 29,  1,   AX, NONE,   AX, TEMP,  1,  2,  7, 12, &macros[1958] fl(12,71) },
 { 30,  1, ANYR, NONE, ANYR, TEMP,  1,  3,  7, 12, &macros[1964] fl(12,78) },
 { 31,  1,   AX, NONE, NONE,   AX,  1,  2, 16, 12, &macros[1977] fl(12,87) },
 { 31,  1,   AX, NONE, NONE,   AX,  1,  3, 16, 12, &macros[1989] fl(12,97) },
 { 12,  1, ANYR, NONE, ANYR, TEMP,  1,  1,  7,  1, &macros[1995] fl(12,107) },
 {  5,  1, ANYL, NONE, ANYL, TEMP,  1,  1,  7,  1, &macros[1995] fl(12,111) },
 {  7,  1,   AX, NONE, NONE, NONE,  1,  1, 10,  1, &macros[1998] fl(12,117) },
 { 12,  1,   AX, NONE,   AX,   AX,  1,  2,  7,  1, &macros[2003] fl(12,124) },
 { 12,  1,   AX, NONE,   AX,   AX,  1,  3,  7,  1, &macros[2008] fl(12,131) },
 { 29,  4, ANYR, NONE, ANYR, TEMP,  1,  4,  7,  4, &macros[2015] fl(12,138) },
 { 11,  1,   AX, NONE, DXAX,   AX,  1,  1,  7,  4, &macros[2018] fl(12,145) },
 { 12,  1, DXAX, NONE, DXAX,   AX,  1,  2,  7,  4, &macros[2021] fl(12,151) },
 { 12,  1, DXAX, NONE, DXAX,   AX,  1,  3,  7,  4, &macros[2024] fl(12,158) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 11,  5, ANYR, NONE, NONE, TEMP,  1,  8,  5,  1, &macros[2027] fl(12,372) },
 { 11,  9, ANYR, NONE, NONE, TEMP,  1,  8,  5,  1, &macros[2034] fl(12,380) },
 { 12,  5, ANYR, NONE, ANYR, TEMP,  1,  8,  7,  1, &macros[2041] fl(12,389) },
 { 12,  9, ANYR, NONE, ANYR, TEMP,  1,  8,  7,  1, &macros[2048] fl(12,397) },
 { 12,  5,   AX, NONE,   AX, TEMP,  1,  7,  7,  1, &macros[2055] fl(12,406) },
 { 12,  9,   AX, NONE,   AX, TEMP,  1,  7,  7,  1, &macros[2062] fl(12,414) }
/* #ifndef NDPDEF */
/* #ifndef IEEE */,
 {  7, 13,   AX, NONE, NONE, NONE,  1, 14, 19,  0, &macros[2069] fl(12,437) },
 {  7, 13, NONE, NONE, NONE, NONE,  1, 14,  5, 13, &macros[2080] fl(12,445) },
 { 12, 13,   AX, NONE, FPAC, FPAC,  1, 14,  7, 13, &macros[2093] fl(12,452) },
 { 12, 13,   AX, NONE, FPAC, FPAC,  1, 14,  7, 14, &macros[2093] fl(12,456) },
 {  7, 13,   AX, NONE, NONE, NONE,  1, 13, 19,  0, &macros[2109] fl(12,467) },
 {  7, 13, NONE, NONE, NONE, NONE,  1, 13,  5, 13, &macros[2132] fl(12,477) },
 { 12, 13,   AX, NONE, FPAC, FPAC,  1, 13,  7, 13, &macros[2161] fl(12,486) }
/* #endif */
/* #ifdef  NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/asub.t */
PAT p13[] = {
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  2,  0, &macros[2198] fl(13,8) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  2,  0, &macros[2198] fl(13,12) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  2,  2,  0, &macros[2201] fl(13,20) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  3,  2,  0, &macros[2204] fl(13,29) }
/* #ifndef ONLYSMALL */
/* #endif */,
 {  1,  1, ANYR, NONE, NONE, TEMP,  3,  1,  4,  1, &macros[2207] fl(13,55) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  3,  1,  4,  1, &macros[2207] fl(13,59) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[2210] fl(13,67) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  3,  1, &macros[2210] fl(13,71) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[2210] fl(13,75) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  3,  1, &macros[2210] fl(13,79) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  2,  5,  1, &macros[2212] fl(13,87) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  3,  5,  1, &macros[2215] fl(13,96) }
/* #ifndef ONLYSMALL */
/* #endif */,
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  6,  4, &macros[2218] fl(13,122) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  5,  4, &macros[2221] fl(13,131) },
 {  4,  1, ANYR, NONE, ANYR, TEMP,  1,  1,  7,  1, &macros[2229] fl(13,141) },
 {  5,  1, ANYL, NONE, ANYL, TEMP,  1,  1,  7,  1, &macros[2229] fl(13,145) },
 {  4,  1,   AX, NONE,   AX,   AX,  1,  2,  7,  1, &macros[2232] fl(13,153) },
 {  4,  1,   AX,   AX, NONE,   AX,  1,  3,  7,  1, &macros[2235] fl(13,162) }
/* #ifndef ONLYSMALL */
/* #endif */,
 {  6,  1, ANYR, NONE, NONE, TEMP,  1,  1,  4,  1, &macros[2238] fl(13,188) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  4,  1, &macros[2238] fl(13,192) },
 {  6,  1,   AX, NONE, NONE,   AX,  1,  2,  4,  1, &macros[2244] fl(13,201) },
 {  6,  1,   AX, NONE, NONE,   AX,  1,  3,  4,  1, &macros[2252] fl(13,211) },
 {  7,  4, DXAX, NONE, NONE, NONE,  1,  4,  8,  5, &macros[2263] fl(13,221) },
 {  7,  4, DXAX, NONE, NONE, NONE,  1,  4,  8,  6, &macros[2276] fl(13,230) },
 {  7,  4, DXAX, NONE, DXAX, NONE,  1,  4,  7,  4, &macros[2287] fl(13,239) },
 {  8,  4, DXAX, NONE, DXAX, DXAX,  1,  4,  7,  4, &macros[2289] fl(13,246) },
 {  9,  4, DXAX, NONE, NONE, DXAX,  1,  4,  4,  4, &macros[2311] fl(13,256) },
 { 10,  1,   AX, NONE, NONE,   AX,  1,  7,  4,  1, &macros[2329] fl(13,280) },
 { 10,  1, ANYR, NONE, NONE, TEMP,  1,  8,  4,  1, &macros[2337] fl(13,291) }
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/axor.t */
PAT p14[] = {
 {  1,  1, ANYR, NONE, NONE, TEMP,  9,  1, 10,  1, &macros[2343] fl(14,11) },
 {  2,  1, ANYL, NONE, NONE, TEMP, 17,  1, 10,  1, &macros[2343] fl(14,15) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[2343] fl(14,19) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[2343] fl(14,23) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  2,  5,  1, &macros[2346] fl(14,31) },
 {  1,  1,   AX, NONE, NONE,   AX,  1,  3,  5,  1, &macros[2349] fl(14,40) },
 {  4,  1, ANYR, NONE, ANYR, TEMP,  1,  1,  7,  1, &macros[2352] fl(14,49) },
 {  5,  1, ANYL, NONE, ANYL, TEMP,  1,  1,  7,  1, &macros[2352] fl(14,53) },
 {  4,  1,   AX, NONE,   AX,   AX,  1,  2,  7,  1, &macros[2355] fl(14,61) },
 {  4,  1,   AX, NONE,   AX,   AX,  1,  3,  7,  1, &macros[2358] fl(14,70) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  6,  4, &macros[2361] fl(14,79) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4, 11,  4, &macros[2364] fl(14,88) },
 { 11,  4, DXAX, NONE, NONE, DXAX,  1,  4, 12,  4, &macros[2369] fl(14,98) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4, 13,  4, &macros[2373] fl(14,106) },
 {  3,  4, DXAX, NONE, NONE, DXAX,  1,  4,  5,  4, &macros[2383] fl(14,116) },
 { 11,  4, DXAX, NONE, DXAX, DXAX,  1,  4,  7,  4, &macros[2390] fl(14,126) },
 { 11,  5, ANYR, NONE, NONE, TEMP,  1,  8, 14,  1, &macros[2399] fl(14,144) },
 { 11,  9, ANYR, NONE, NONE, TEMP,  1,  8, 14,  1, &macros[2403] fl(14,151) },
 { 11,  5,   AX, NONE, NONE, TEMP,  1,  7, 14,  1, &macros[2407] fl(14,159) },
 { 11,  9, ANYR, NONE, NONE, TEMP,  1,  7, 14,  1, &macros[2411] fl(14,166) },
 { 12,  5, ANYR, NONE, ANYR, TEMP,  1,  8,  7,  1, &macros[2415] fl(14,174) },
 { 12,  9, ANYR, NONE, ANYR, TEMP,  1,  8,  7,  1, &macros[2419] fl(14,181) },
 { 12,  5,   AX, NONE,   AX, TEMP,  1,  7,  7,  1, &macros[2423] fl(14,189) },
 { 12,  9,   AX, NONE,   AX, TEMP,  1,  7,  7,  1, &macros[2427] fl(14,196) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/bef.t */
PAT p15[] = {
 { 32,  1, ANYR, NONE, NONE, TEMP,  1,  1,  2,  0, &macros[2431] fl(15,11) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  2,  0, &macros[2431] fl(15,15) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 32,  1, ANYR, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[2434] fl(15,40) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  1,  1,  5,  1, &macros[2434] fl(15,44) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 32,  1,   AX, NONE, NONE,   AX,  1,  2,  2,  0, &macros[2437] fl(15,69) },
 { 32,  1,   AX, NONE, NONE,   AX,  1,  3,  2,  0, &macros[2441] fl(15,78) },
 {  3,  4, ANYR, NONE, NONE, TEMP,  1,  4,  2,  0, &macros[2445] fl(15,87) },
 { 11,  1, ANYR, NONE, NONE, TEMP,  1,  8,  5,  1, &macros[2457] fl(15,97) },
 { 11,  1,   AX, NONE, NONE,   AX,  1,  7,  5,  1, &macros[2463] fl(15,107) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/div.t */
PAT p16[] = {
 { 17,  1, DXAX,   AX, NONE,   AX,  7,  5, 16,  1, &macros[2471] fl(16,9) },
 { 17,  1, DXAX,   AX, NONE,   AX,  7,  9, 16,  1, &macros[2475] fl(16,16) }
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/leaves.t */
PAT p17[] = {
 { 10,  1, ANYR, NONE, NONE, TEMP, 19,  0,  0,  0, &macros[2483] fl(17,29) },
 {  2,  1, ANYL, NONE, NONE, TEMP, 19,  0,  0,  0, &macros[2483] fl(17,33) },
 { 10, 15, ANYR, NONE, NONE, TEMP, 19,  0,  0,  0, &macros[2487] fl(17,39) }
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifdef NDPDEF */
/* #endif */,
 { 10,  4, ANYR, NONE, NONE, TEMP, 12,  4,  0,  0, &macros[2498] fl(17,82) },
 { 10,  4, ANYR, NONE, NONE, TEMP,  6,  4,  0,  0, &macros[2506] fl(17,89) },
 { 10,  1, ANYR, NONE, NONE, TEMP, 20,  1,  0,  0, &macros[2517] fl(17,104) },
 {  2,  1, ANYL, NONE, NONE, TEMP, 20,  1,  0,  0, &macros[2517] fl(17,108) },
 { 10, 16, ANYR, NONE, NONE, TEMP,  5, 12,  0,  0, &macros[2522] fl(17,114) },
 { 10,  1, ANYR, NONE, NONE, TEMP,  4,  1,  0,  0, &macros[2522] fl(17,118) },
 {  2,  1, ANYL, NONE, NONE, TEMP,  4,  1,  0,  0, &macros[2522] fl(17,122) },
 { 10,  4, ANYR, NONE, NONE, TEMP,  4,  4,  0,  0, &macros[2526] fl(17,128) },
 {  2,  4, ANYL, NONE, NONE, TEMP,  4,  4,  0,  0, &macros[2526] fl(17,132) }
/* #ifdef NDPDEF */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifndef NDPDEF */,
 { 10, 17, FPAC,   AX, NONE, FPAC, 16, 14,  0,  0, &macros[2533] fl(17,267) },
 { 10, 13, FPAC, NONE, NONE, FPAC,  5, 13,  0,  0, &macros[2568] fl(17,278) },
 { 10, 13, FPAC,   AX, NONE, FPAC, 16, 13,  0,  0, &macros[2596] fl(17,287) }
/* #endif */,
 { 10,  1,   AX, NONE, NONE, TEMP,  9, 12,  0,  0, &macros[2645] fl(17,308) },
 { 33, 18,   AX, NONE, NONE, TEMP,  4,  2,  0,  0, &macros[2650] fl(17,315) },
 { 33, 19,   AX, NONE, NONE, TEMP,  4,  3,  0,  0, &macros[2657] fl(17,322) },
 { 34,  1, ANYR, ANYR, NONE, TEMP,  7, 12,  0,  0, &macros[2664] fl(17,329) },
 { 10,  4, DXAX, NONE, NONE, TEMP,  9, 12,  0,  0, &macros[2665] fl(17,336) },
 { 34,  4, DXAX,   AX, NONE, TEMP,  7, 12,  0,  0, &macros[2671] fl(17,343) },
 { 10,  4, DXAX, NONE, NONE, TEMP, 16,  2,  0,  0, &macros[2673] fl(17,349) },
 { 10,  4, DXAX, NONE, NONE, TEMP, 16,  3,  0,  0, &macros[2681] fl(17,357) },
 { 10,  4, DXAX, NONE, NONE, TEMP,  4,  5,  0,  0, &macros[2695] fl(17,365) },
 { 10,  4, DXAX, NONE, NONE, TEMP,  4,  9,  0,  0, &macros[2697] fl(17,372) }
/* #ifdef NDPDEF */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */,
 { 10,  1, ANYR, NONE, NONE, TEMP,  5, 15,  0,  0, &macros[2702] fl(17,535) },
 { 34,  1, DXAX, DXAX, NONE,   AX,  7, 15,  0,  0, &macros[2707] fl(17,541) },
 { 10,  1, ANYR, NONE, NONE, TEMP, 16, 15,  0,  0, &macros[2708] fl(17,547) },
 { 10,  2,   AX, NONE, NONE, TEMP,  9, 15,  0,  0, &macros[2710] fl(17,554) },
 { 10,  3,   AX, NONE, NONE, TEMP,  9, 15,  0,  0, &macros[2715] fl(17,561) },
 { 10,  2,   AX, NONE, NONE, TEMP,  4, 15,  0,  0, &macros[2723] fl(17,568) },
 { 10,  3,   AX, NONE, NONE, TEMP,  4, 15,  0,  0, &macros[2725] fl(17,575) },
 { 10,  2, DXAX, DXAX, NONE,   AX,  7, 15,  0,  0, &macros[2727] fl(17,582) },
 { 10,  3, DXAX, DXAX, NONE,   AX,  7, 15,  0,  0, &macros[2729] fl(17,588) },
 { 10,  2,   AX, NONE, NONE, TEMP,  9,  1,  0,  0, &macros[2730] fl(17,595) },
 { 10,  3,   AX, NONE, NONE, TEMP,  9,  1,  0,  0, &macros[2732] fl(17,602) },
 { 10,  2,   AX, NONE, NONE, TEMP,  4,  1,  0,  0, &macros[2736] fl(17,609) },
 { 10,  3,   AX, NONE, NONE, TEMP,  4,  1,  0,  0, &macros[2742] fl(17,616) },
 { 34,  2,   AX,   AX, NONE, TEMP,  7,  1,  0,  0, &macros[2748] fl(17,623) },
 { 34,  3,   AX,   AX, NONE, TEMP,  7,  1,  0,  0, &macros[2750] fl(17,629) },
 { 18,  1, NONE, NONE, NONE, NONE,  9,  1,  0,  0, &macros[2752] fl(17,647) },
 { 18,  1,   AX, NONE, NONE, NONE, 16,  1,  0,  0, &macros[2758] fl(17,654) },
 { 18, 16,   AX, NONE, NONE, NONE, 16, 12,  0,  0, &macros[2765] fl(17,661) },
 { 35,  4, NONE, NONE, NONE, NONE,  9,  4,  0,  0, &macros[2768] fl(17,668) },
 { 35,  4,   AX, NONE, NONE, NONE,  4,  4,  0,  0, &macros[2774] fl(17,675) },
 { 20,  4, NONE, NONE, NONE, NONE,  9,  4,  0,  0, &macros[2782] fl(17,683) },
 { 20,  4,   AX, NONE, NONE, NONE, 16,  4,  0,  0, &macros[2790] fl(17,690) },
 { 36,  4, NONE, NONE, NONE, NONE,  9,  4,  0,  0, &macros[2798] fl(17,697) },
 { 36,  4,   AX, NONE, NONE, NONE, 16,  4,  0,  0, &macros[2815] fl(17,708) },
 { 37,  4, NONE, NONE, NONE, NONE,  9,  4,  0,  0, &macros[2832] fl(17,719) },
 { 37,  4,   AX, NONE, NONE, NONE, 16,  4,  0,  0, &macros[2850] fl(17,730) }
/* #ifdef NDPDEF */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */,
 { 38,  0, NONE, NONE, NONE, NONE,  0,  0,  0,  0, &macros[2866] fl(17,774) },
 { 39,  0, NONE, NONE, NONE, NONE,  0,  0,  0,  0, &macros[2869] fl(17,780) },
 { 18,  1, NONE, ANYR, NONE, NONE,  7,  1,  0,  0, &macros[2870] fl(17,786) }
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifndef NDPDEF */,
 { 40, 13, NONE, NONE, NONE, NONE,  9, 13,  0,  0, &macros[2876] fl(17,820) },
 { 40, 13, NONE, NONE, NONE, NONE, 16, 13,  0,  0, &macros[2882] fl(17,827) },
 { 40, 17, NONE, NONE, NONE, NONE, 16, 14,  0,  0, &macros[2891] fl(17,834) }
/* #endif */,
 { 41,  1, NONE, NONE, NONE, NONE,  5,  1,  0,  0, &macros[2895] fl(17,851) },
 { 42,  1, ANYR, NONE, NONE, NONE, 19,  0,  0,  0, &macros[2898] fl(17,857) },
 { 42,  1, ANYR, NONE, NONE, NONE,  5,  1,  0,  0, &macros[2904] fl(17,864) },
 { 42,  1, ANYR, NONE, NONE, NONE, 20,  1,  0,  0, &macros[2909] fl(17,871) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 42,  1, NONE, NONE, NONE, NONE, 16,  1,  0,  0, &macros[2914] fl(17,959) },
 { 42, 12, NONE, NONE, NONE, NONE,  9, 16,  0,  0, &macros[2914] fl(17,963) },
 { 41,  4, NONE, NONE, NONE, NONE,  5,  4,  0,  0, &macros[2916] fl(17,969) },
 { 42, 15, ANYR, NONE, NONE, NONE, 19,  0,  0,  0, &macros[2922] fl(17,976) },
 { 42,  4, ANYR, NONE, NONE, NONE,  5,  4,  0,  0, &macros[2934] fl(17,984) },
 { 42, 15, NONE, NONE, NONE, NONE, 16, 15,  0,  0, &macros[2949] fl(17,993) }
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifdef NDPDEF */
/* #endif */
/* #ifndef NDPDEF */,
 { 42, 13, NONE, NONE, NONE, NONE,  9, 13,  0,  0, &macros[2951] fl(17,1020) },
 { 42, 13, NONE, NONE, NONE, NONE, 16, 13,  0,  0, &macros[2955] fl(17,1026) }
/* #endif */,
 {  7, 20, NONE, NONE, NONE, NONE,  4, 20,  0,  0, &macros[2972] fl(17,1042) },
 {  7, 21, NONE, NONE, NONE, NONE,  9, 21,  0,  0, &macros[2973] fl(17,1056) }
/* #ifndef NDPDEF */,
 {  7, 13, NONE, NONE, NONE, NONE,  9, 13,  0,  0, &macros[2974] fl(17,1063) }
/* #endif */
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/mul.t */
PAT p18[] = {
 { 27,  1, ANYR, NONE, NONE, TEMP, 16,  1,  5,  1, &macros[2975] fl(18,10) },
 { 26,  1, ANYL, NONE, NONE, TEMP, 16,  1,  5,  1, &macros[2975] fl(18,14) },
 { 27,  1, ANYR, NONE, NONE, TEMP,  5,  1, 16,  1, &macros[2980] fl(18,20) },
 { 26,  1, ANYL, NONE, NONE, TEMP,  5,  1, 16,  1, &macros[2980] fl(18,24) },
 { 11,  1, DXAX,   AX, NONE,   AX,  7,  5, 16,  1, &macros[2985] fl(18,30) },
 { 11,  1, DXAX,   AX, NONE,   AX,  7,  9, 16,  1, &macros[2988] fl(18,36) }
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/neg.t */
PAT p19[] = {
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  0,  0, &macros[2991] fl(19,10) },
 { 15,  4, ANYR, ANYR, NONE, TEMP,  7,  4,  0,  0, &macros[2994] fl(19,17) }
/* #ifndef NDPDEF */,
 { 24, 13, FPAC, FPAC, NONE, FPAC,  7, 13,  0,  0, &macros[3007] fl(19,27) }
/* #endif */
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/not.t */
PAT p20[] = {
 { 24,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  0,  0, &macros[3015] fl(20,10) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4,  0,  0, &macros[3018] fl(20,16) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/or.t */
PAT p21[] = {
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  4,  1, &macros[3024] fl(21,10) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4, 12,  4, &macros[3028] fl(21,17) },
 { 15,  4, ANYR, ANYR, NONE, TEMP,  7,  4,  6,  4, &macros[3032] fl(21,23) },
 { 15,  4, ANYR, ANYR, NONE, TEMP,  7,  4,  4,  4, &macros[3036] fl(21,30) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/relop.t */
PAT p22[] = {
 { 40,  0, NONE, NONE, NONE, NONE, 16, 12, 18,  1, &macros[3043] fl(22,34) },
 { 40,  0, NONE, NONE, NONE, NONE, 21,  1,  5,  1, &macros[3047] fl(22,41) },
 { 40,  0, NONE, NONE, NONE, NONE, 22,  1,  4,  1, &macros[3047] fl(22,45) },
 { 44,  0, ANYR, ANYR, NONE, NONE,  7, 12,  4, 12, &macros[3051] fl(22,52) },
 { 40,  0,   AX, NONE, NONE, NONE, 16, 12,  4, 12, &macros[3057] fl(22,59) },
 { 44,  0, ANYR, ANYR, NONE, NONE,  7,  1,  4,  1, &macros[3063] fl(22,67) },
 { 23,  1, NONE, NONE, NONE, NONE, 16,  4,  5,  4, &macros[3066] fl(22,81) },
 { 23,  0, NONE, ANYR, NONE, NONE,  7,  4, 12,  4, &macros[3082] fl(22,91) },
 { 23,  0, NONE, ANYR, NONE, NONE,  7,  4,  6,  4, &macros[3094] fl(22,101) },
 { 23,  0, NONE, ANYR, NONE, NONE,  7,  4,  4,  4, &macros[3108] fl(22,111) }
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */,
 { 22,  0, NONE, NONE, NONE, NONE, 16,  4,  5,  4, &macros[3115] fl(22,156) },
 { 22,  0, NONE, ANYR, NONE, NONE,  7,  4, 12,  4, &macros[3129] fl(22,165) },
 { 22,  0, NONE, ANYR, NONE, NONE,  7,  4,  6,  4, &macros[3141] fl(22,174) },
 { 22,  0, NONE, ANYR, NONE, NONE,  7,  4,  4,  4, &macros[3156] fl(22,183) }
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */,
 { 45,  0, NONE, NONE, NONE, NONE, 16,  4,  5,  4, &macros[3163] fl(22,269) },
 { 45,  0, NONE, ANYR, NONE, NONE,  7,  4, 12,  4, &macros[3180] fl(22,280) },
 { 45,  0, NONE, ANYR, NONE, NONE,  7,  4,  4,  4, &macros[3194] fl(22,291) }
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/rem.t */
PAT p23[] = {
 { 17,  1, DXAX,   AX, NONE,   DX,  7,  5, 16,  1, &macros[3201] fl(23,10) },
 { 17,  1, DXAX,   AX, NONE,   DX,  7,  9, 16,  1, &macros[3204] fl(23,17) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/shl.t */
PAT p24[] = {
 { 46,  1, ANYR, ANYR, NONE, TEMP,  7,  1, 18,  1, &macros[3207] fl(24,12) },
 { 47,  1, ANYL, ANYL, NONE, TEMP,  7,  1, 18,  1, &macros[3207] fl(24,16) },
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  2,  0, &macros[3211] fl(24,23) },
 { 14,  1, ANYL, ANYL, NONE, TEMP,  7,  1,  2,  0, &macros[3211] fl(24,27) },
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  1, 15,  0, &macros[3213] fl(24,34) },
 { 14,  1, ANYL, ANYL, NONE, TEMP,  7,  1, 15,  0, &macros[3213] fl(24,38) },
 { 43,  1, ANYR, ANYR,   CX, TEMP,  7,  1,  5,  1, &macros[3223] fl(24,53) },
 { 14,  1, ANYL, ANYL,   CX, TEMP,  7,  1,  5,  1, &macros[3223] fl(24,57) },
 { 24,  1, ANYR, ANYR,   CX, TEMP,  7,  1, 16,  1, &macros[3233] fl(24,65) },
 { 14,  1, ANYL, ANYL,   CX, TEMP,  7,  1, 16,  1, &macros[3233] fl(24,69) },
 { 24,  4, DXAX, DXAX,   CX, DXAX,  7,  4,  5,  1, &macros[3242] fl(24,84) },
 { 24,  4, DXAX, DXAX,   CX, DXAX,  7,  4, 16,  1, &macros[3260] fl(24,93) }
/* #ifndef ONLYSMALL */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/shr.t */
PAT p25[] = {
 { 46,  1, ANYR, ANYR, NONE, TEMP,  7,  5, 18,  1, &macros[3281] fl(25,15) },
 { 46,  1, ANYR, ANYR, NONE, TEMP,  7,  9, 18,  1, &macros[3285] fl(25,22) },
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  5,  2,  0, &macros[3289] fl(25,29) },
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  5, 15,  0, &macros[3292] fl(25,36) },
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  9,  2,  0, &macros[3298] fl(25,44) },
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  9, 15,  0, &macros[3301] fl(25,51) },
 { 43,  1,   AX,   AX,   CX, TEMP,  7,  5,  5,  1, &macros[3307] fl(25,66) },
 { 43,  1,   AX,   AX,   CX, TEMP,  7,  9,  5,  1, &macros[3314] fl(25,74) },
 { 24,  1, ANYR, ANYR,   CX, TEMP,  7,  5, 16,  1, &macros[3321] fl(25,82) },
 { 24,  1, ANYR, ANYR,   CX, TEMP,  7,  9, 16,  1, &macros[3328] fl(25,89) },
 { 24,  4, DXAX, DXAX,   CX, DXAX,  7, 10,  5,  1, &macros[3335] fl(25,106) },
 { 24,  4, DXAX, DXAX,   CX, DXAX,  7, 11,  5,  1, &macros[3349] fl(25,115) },
 { 24,  4, ANYR, ANYR,   CX, TEMP,  7, 10, 16,  1, &macros[3357] fl(25,124) },
 { 24,  4, ANYR, ANYR,   CX, TEMP,  7, 11, 16,  1, &macros[3373] fl(25,135) }
};
/* /newbits/bin/cc/c/n1/i8086/tables/sub.t */
PAT p26[] = {
 { 13,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  2,  0, &macros[3383] fl(26,12) },
 { 14,  1, ANYL, ANYL, NONE, TEMP,  7,  1,  2,  0, &macros[3383] fl(26,16) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 13,  1, ANYR, ANYR, NONE, TEMP,  7,  1, 15,  0, &macros[3386] fl(26,43) },
 { 14,  1, ANYL, ANYL, NONE, TEMP,  7,  1, 15,  0, &macros[3386] fl(26,47) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 13,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  4,  1, &macros[3390] fl(26,74) },
 { 14,  1, ANYL, ANYL, NONE, TEMP,  7,  1,  4,  1, &macros[3390] fl(26,78) }
/* #ifndef ONLYSMALL */
/* #endif */,
 { 15,  4, DXAX, DXAX, NONE, DXAX,  7,  4,  4,  4, &macros[3394] fl(26,97) }
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifndef ONLYSMALL */
/* #endif */
/* #ifdef NDPDEF */
/* #endif */
};
/* /newbits/bin/cc/c/n1/i8086/tables/xor.t */
PAT p27[] = {
 { 43,  1, ANYR, ANYR, NONE, TEMP,  7,  1,  4,  1, &macros[3403] fl(27,12) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4, 12,  4, &macros[3407] fl(27,19) },
 { 24,  4, ANYR, ANYR, NONE, TEMP,  7,  4, 13,  4, &macros[3411] fl(27,25) },
 { 15,  4, ANYR, ANYR, NONE, TEMP,  7,  4,  6,  4, &macros[3419] fl(27,32) },
 { 15,  4, ANYR, ANYR, NONE, TEMP,  7,  4, 11,  4, &macros[3423] fl(27,39) },
 { 15,  4, ANYR, ANYR, NONE, TEMP,  7,  4,  4,  4, &macros[3428] fl(27,47) }
};
PATX patx[] = {
	p3,	7,
	p26,	7,
	p18,	6,
	p16,	2,
	p23,	2,
	p7,	20,
	p21,	4,
	p27,	6,
	p24,	12,
	p25,	14,
	p1,	29,
	p13,	29,
	p6,	11,
	p4,	10,
	p9,	8,
	p2,	21,
	p8,	22,
	p14,	24,
	p10,	31,
	p11,	36,
	p22,	17,
	p22,	17,
	p22,	17,
	p22,	17,
	p22,	17,
	p22,	17,
	p22,	17,
	p22,	17,
	p22,	17,
	p22,	17,
	NULL,	0,
	NULL,	0,
	p19,	3,
	p20,	2,
	NULL,	0,
	NULL,	0,
	NULL,	0,
	p15,	9,
	p15,	9,
	p5,	9,
	p5,	9,
	NULL,	0,
	NULL,	0,
	NULL,	0,
	NULL,	0,
	p17,	72,
	p17,	72,
	NULL,	0,
	NULL,	0,
	p12,	35,
	NULL,	0,
	NULL,	0,
	NULL,	0,
	p17,	72,
	p17,	72,
	NULL,	0
};
PATFLAG	patcache[] = {
	PEFFECT|PRVALUE|PREL,
	PLVALUE,
	PEFFECT|PRVALUE|PLT|PGE,
	PEFFECT|PRVALUE|PREL|P_SRT,
	PLVALUE|P_SRT,
	PRVALUE|PREL,
	PEFFECT,
	PRVALUE|PLT|PGE|P_SRT,
	PRVALUE|PLT|PGE,
	PRVALUE,
	PEFFECT|PRVALUE,
	PEFFECT|PRVALUE|P_SRT,
	PEFFECT|PRVALUE|PSREL|P_SLT,
	PLVALUE|P_SLT,
	PEFFECT|PRVALUE|PGE|PLT|P_SLT,
	PEFFECT|PRVALUE|PLVALUE|P80186,
	PEFFECT|PRVALUE|PLVALUE,
	PSREL,
	PEFFECT|PRVALUE|PEREL|P_SLT,
	PLT|PGE,
	PEQ|PNE,
	PNE,
	PEQ,
	PEFFECT|PRVALUE|P_SLT,
	PEFFECT|PRVALUE|PREL|P80186,
	PLVALUE|P80186,
	PEFFECT|PRVALUE|P80186,
	PEFFECT|PEREL|P_SRT,
	PRVALUE|P_SRT,
	PEFFECT|PRVALUE|PEREL|P_SRT,
	PEFFECT|PRVALUE|PEREL,
	PEFFECT|PRVALUE|PSREL,
	PRVALUE|PEFFECT,
	PRVALUE|P_SLT,
	PEREL,
	PLE,
	PGT,
	PUGE,
	PULT,
	PREL,
	PFNARG|P80186,
	PFNARG,
	PEFFECT|PRVALUE|PREL|P_SLT,
	PREL|P_SLT,
	PNEREL,
	PEFFECT|PRVALUE|PREL|P_SLT|P80186,
	PLVALUE|P_SLT|P80186
};
int patcsize=sizeof(patcache)/sizeof(PATFLAG);
TYPESET	typecache[] = {
	WORD,
	FS8,
	FU8,
	LONG,
	FS16,
	FU16,
	FFLD8,
	FFLD16,
	UWORD,
	FS32,
	FU32,
	BYTE,
	FF64,
	FF32,
	LONG|LPTX,
	WORD|BYTE,
	FF32|FF64,
	WORD|FS8,
	WORD|FU8,
	FBLK,
	NFLT
};
FLAG	flagcache[] = {
	T_ADR|T_LV,
	T_1|T_MMX,
	T_REG|T_MMX,
	T_ADR|T_IMM,
	T_IMM|T_MMX,
	T_LHC|T_MMX,
	T_TREG,
	T_DIR|T_MMX,
	T_RREG|T_MMX,
	T_EASY|T_MMX,
	T_LHS|T_MMX,
	T_UHC|T_MMX,
	T_UHS|T_MMX,
	T_DIR|T_IMM|T_MMX,
	T_2|T_MMX,
	T_ADR,
	T_LREG|T_MMX,
	T_BYTE|T_MMX,
	T_0|T_MMX,
	T_LEA|T_MMX,
	T_ADR|T_MMX,
	T_RREG|T_LREG|T_MMX
};
ival_t	ivalcache[] = {
	0x8,
	0x1,
	0x0,
	0x7F80,
	0x80
};
lval_t	lvalcache[] = {
	0
};
char	*gidcache[] = {
	"lrdiv",
	"vrdiv",
	"lrmul",
	"vrmul",
	"lrrem",
	"vrrem",
	"ddtest",
	"dpush"
};
#if !TINY
char	*namecache[] = {
	"prefac.f",
	"aadd.t",
	"aand.t",
	"add.t",
	"adiv.t",
	"aft.t",
	"amul.t",
	"and.t",
	"aor.t",
	"arem.t",
	"ashl.t",
	"ashr.t",
	"assign.t",
	"asub.t",
	"axor.t",
	"bef.t",
	"div.t",
	"leaves.t",
	"mul.t",
	"neg.t",
	"not.t",
	"or.t",
	"relop.t",
	"rem.t",
	"shl.t",
	"shr.t",
	"sub.t",
	"xor.t"
};
#endif
