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
/* /src386/bin/c/n1/i386/tables/prefac.f */
#define	BYTE	(FS8|FU8)			
#define	WORD	(FS16|FU16)			
#define	SHORT	(BYTE|WORD)			
#define	LONG	(FS32|FU32)			
#define	PTX	(FPTR|FPTB)			
#define	DWORD	(LONG|PTX)			
#define	SINT	(FS32|FS16|FS8)			
#define	UINT	(FU32|FU16|FU8)			
#define	FLD	(FFLD32|FFLD16|FFLD8)		
#define	FLOAT	(FF32|FF64)			
#define	NFLT	(BYTE|WORD|DWORD)		
#define	NDPARG	(FLOAT|FS16|FS32)		
#define	PEREL	(PEQ|PNE)			
#define	PSREL	(PEQ|PNE|PGT|PGE|PLT|PLE)	
#define	PREL	(PEQ|PNE|PGT|PGE|PLT|PLE|PUGT|PUGE|PULT|PULE)	
#define	PVALUE	(PRVALUE|PLVALUE)		
/* /src386/bin/c/n1/i386/tables/aadd.t */
PAT p1[] = {
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  2,  2,  0, &macros[   0] fl(1,17) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  2,  3,  3, &macros[  14] fl(1,26) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  4,  1, &macros[  14] fl(1,31) },
 {  2,  1, ANYR, NONE, ANYR, TEMP,  1,  2,  5,  1, &macros[  20] fl(1,40) },
 {  3,  3, ANYR, NONE, NONE, TEMP,  1,  4,  6,  3, &macros[  27] fl(1,58) },
 {  4,  5, EDXEAX, NONE, EDXEAX, TEMP,  1,  5,  5,  5, &macros[  48] fl(1,74) },
 {  4,  5, EDXEAX, NONE, EDXEAX, TEMP,  1,  6,  5,  5, &macros[  73] fl(1,86) },
 {  5,  7, FPAC, NONE, FPAC, FPAC,  1,  7,  5,  5, &macros[  96] fl(1,98) }
};
/* /src386/bin/c/n1/i386/tables/add.t */
PAT p2[] = {
 {  6,  1, ANYR, ANYR, NONE, TEMP,  5,  1,  2,  0, &macros[ 110] fl(2,17) },
 {  6,  1, ANYR, ANYR, NONE, TEMP,  5,  1,  6,  1, &macros[ 114] fl(2,25) },
 {  7,  5, EDXEAX, EDXEAX, NONE, TEMP,  5,  5,  6,  5, &macros[ 119] fl(2,38) },
 {  8,  7, FPAC, FPAC, NONE, FPAC,  5,  5,  7,  8, &macros[ 134] fl(2,48) }
};
/* /src386/bin/c/n1/i386/tables/adiv.t */
PAT p3[] = {
 {  9,  3, EDXEAX, NONE, NONE,  EAX,  1,  9,  7,  3, &macros[ 138] fl(3,17) },
 {  9,  3, EDXEAX, NONE, NONE,  EAX,  1, 10,  7, 11, &macros[ 138] fl(3,22) },
 {  9,  3, EDXEAX, NONE, NONE,  EAX,  1, 12,  7, 11, &macros[ 157] fl(3,33) },
 {  9,  3, EDXEAX, NONE, NONE,  EAX,  1, 13,  7, 10, &macros[ 185] fl(3,46) },
 {  9,  3, EDXEAX, NONE, NONE,  EAX,  1,  4,  7,  3, &macros[ 194] fl(3,62) },
 {  4,  5, EDXEAX, NONE, EDXEAX, TEMP,  1,  5,  5,  5, &macros[ 223] fl(3,80) },
 {  4,  5, EDXEAX, NONE, EDXEAX, TEMP,  1,  6,  5,  5, &macros[ 234] fl(3,92) },
 {  5,  7, FPAC, NONE, FPAC, FPAC,  1,  7,  5,  5, &macros[ 241] fl(3,104) }
};
/* /src386/bin/c/n1/i386/tables/aft.t */
PAT p4[] = {
 {  9,  1, ANYR, NONE, NONE, TEMP,  7,  2,  2,  0, &macros[ 245] fl(4,21) },
 {  9,  1, ANYR, NONE, NONE, TEMP,  7,  1,  3,  3, &macros[ 255] fl(4,29) },
 { 10,  3, ANYR, NONE, NONE, TEMP,  1,  4,  3,  3, &macros[ 264] fl(4,46) }
};
/* /src386/bin/c/n1/i386/tables/alogic.t */
PAT p5[] = {
 {  1,  3, ANYR, NONE, NONE, TEMP,  4,  3,  8,  3, &macros[ 298] fl(5,22) },
 {  1,  3, ANYR, NONE, NONE, TEMP,  1, 14,  3,  3, &macros[ 298] fl(5,27) },
 {  2,  3, ANYR, NONE, ANYR, TEMP,  1, 14,  5,  3, &macros[ 304] fl(5,36) },
 {  9, 10, ANYR, NONE, NONE, TEMP,  1,  4,  3,  3, &macros[ 306] fl(5,56) },
 { 11, 10, ANYR, NONE, ANYR, TEMP,  1,  4,  5,  3, &macros[ 317] fl(5,64) },
 {  9, 11, ANYR, NONE, NONE, TEMP,  1,  4,  3,  3, &macros[ 324] fl(5,72) },
 { 11, 11, ANYR, NONE, ANYR, TEMP,  1,  4,  5,  3, &macros[ 341] fl(5,81) }
};
/* /src386/bin/c/n1/i386/tables/amul.t */
PAT p6[] = {
 {  9,  3, ANYR, NONE, NONE, TEMP,  1,  3,  9,  3, &macros[ 348] fl(6,22) },
 {  9,  3, EDXEAX, NONE, NONE,  EAX,  1,  3,  7,  3, &macros[ 356] fl(6,30) },
 {  9,  3, EDXEAX, NONE, NONE,  EAX,  1, 15,  7,  3, &macros[ 367] fl(6,44) },
 {  9,  3, EDXEAX, NONE, NONE,  EAX,  1,  4,  7,  3, &macros[ 376] fl(6,58) },
 {  4,  5, EDXEAX, NONE, EDXEAX, TEMP,  1,  5,  5,  5, &macros[ 388] fl(6,75) },
 {  4,  5, EDXEAX, NONE, EDXEAX, TEMP,  1,  6,  5,  5, &macros[ 399] fl(6,87) },
 {  5,  7, FPAC, NONE, FPAC, FPAC,  1,  7,  5,  5, &macros[ 406] fl(6,99) }
};
/* /src386/bin/c/n1/i386/tables/and.t */
PAT p7[] = {
 { 12,  0, NONE, NONE, NONE, NONE,  7, 16,  3,  1, &macros[ 410] fl(7,14) },
 { 13,  0, ANYR, ANYR, NONE, TEMP,  5,  1,  6,  1, &macros[ 418] fl(7,22) }
};
/* /src386/bin/c/n1/i386/tables/arem.t */
PAT p8[] = {
 {  9,  3, EDXEAX, NONE, NONE,  EDX,  1,  9,  7,  3, &macros[ 421] fl(8,19) },
 {  9,  3, EDXEAX, NONE, NONE,  EDX,  1, 10,  7, 11, &macros[ 421] fl(8,24) },
 {  9,  3, EDXEAX, NONE, NONE,  EDX,  1, 12,  7, 11, &macros[ 440] fl(8,35) },
 {  9,  3, EDXEAX, NONE, NONE,  EDX,  1, 13,  7, 10, &macros[ 468] fl(8,48) },
 {  9,  3, EDXEAX, NONE, NONE,  EDX,  1,  4,  7,  3, &macros[ 477] fl(8,64) }
};
/* /src386/bin/c/n1/i386/tables/ashift.t */
PAT p9[] = {
 {  1,  3, ANYR, NONE, NONE, TEMP,  1, 14, 10,  3, &macros[ 506] fl(9,30) }
/* #ifdef	BOGUS */
/* #endif */,
 {  9,  3, ANYR, NONE, NONE, TEMP,  1,  3,  7,  3, &macros[ 508] fl(9,55) },
 { 10,  3, ANYR, NONE, NONE, TEMP,  1, 17,  7,  3, &macros[ 528] fl(9,73) },
 { 10,  3, ANYR, NONE, NONE, TEMP,  1,  4, 10,  3, &macros[ 561] fl(9,90) },
 { 10,  3, ANYR, NONE, NONE, TEMP,  1,  4,  7,  3, &macros[ 571] fl(9,102) }
};
/* /src386/bin/c/n1/i386/tables/assign.t */
PAT p10[] = {
 { 14,  1, NONE, NONE, NONE, NONE,  4,  1, 11,  0, &macros[ 594] fl(10,27) },
 { 14,  1, NONE, NONE, NONE, NONE,  4,  1,  8,  1, &macros[ 598] fl(10,34) },
 { 14,  1, NONE, NONE, NONE, NONE,  1,  1,  4,  1, &macros[ 598] fl(10,39) },
 { 14,  1, NONE, NONE, NONE, NONE,  4,  1, 12,  1, &macros[ 602] fl(10,46) },
 { 14,  1, NONE, NONE, NONE, NONE,  1,  2,  3,  1, &macros[ 607] fl(10,53) },
 { 15,  1, ANYR, NONE, ANYR, TEMP,  1,  1,  5,  1, &macros[ 613] fl(10,60) },
 { 14,  1, NONE, NONE, NONE, NONE,  1, 18,  3, 18, &macros[ 615] fl(10,71) },
 { 16,  3, ANYR, NONE, ANYR, TEMP,  1, 18,  5, 18, &macros[ 619] fl(10,78) },
 { 14,  3, NONE, NONE, NONE, NONE,  1, 19,  3, 19, &macros[ 638] fl(10,92) },
 {  2,  3, ANYR, NONE, ANYR, TEMP,  1, 19,  5, 19, &macros[ 642] fl(10,99) },
 { 11,  1, ANYR, NONE, ANYR, TEMP,  1,  1,  5, 17, &macros[ 660] fl(10,115) },
 { 11,  3, ANYR, NONE, ANYR, TEMP,  1, 17,  5,  1, &macros[ 668] fl(10,123) },
 {  9, 10, ANYR, NONE, NONE, TEMP,  1,  4,  3,  3, &macros[ 681] fl(10,142) },
 {  9, 11, ANYR, NONE, NONE, TEMP,  1,  4,  3,  3, &macros[ 691] fl(10,151) },
 { 11, 10, ANYR, NONE, ANYR, TEMP,  1,  4,  5,  3, &macros[ 701] fl(10,161) },
 { 11, 11, ANYR, NONE, ANYR, TEMP,  1,  4,  5,  3, &macros[ 717] fl(10,170) },
 { 17,  5,  EAX, NONE, NONE, NONE,  1,  5, 11,  0, &macros[ 733] fl(10,188) },
 { 17,  5, NONE, NONE, NONE, NONE,  1,  5,  3,  5, &macros[ 745] fl(10,197) },
 { 18,  5, EDXEAX, NONE, EDXEAX, EDXEAX,  1,  5,  5,  5, &macros[ 756] fl(10,205) },
 { 19,  5,  EAX, NONE, NONE, NONE,  1,  6, 11,  0, &macros[ 758] fl(10,214) },
 { 20,  5, NONE, NONE, NONE, NONE,  1,  6,  3,  5, &macros[ 764] fl(10,222) },
 { 21,  5, EDXEAX, NONE, NONE, TEMP,  1,  6,  3,  5, &macros[ 767] fl(10,229) },
 { 22,  5, EDXEAX, NONE, EDXEAX, EDXEAX,  1,  6,  5,  7, &macros[ 779] fl(10,240) },
 { 23,  5, EDXEAX, NONE, EDXEAX, EDXEAX,  1,  6,  5,  7, &macros[ 784] fl(10,247) },
 { 24,  5, FPAC, NONE, FPAC, NONE,  1,  7,  5,  7, &macros[ 786] fl(10,257) },
 { 25,  5, FPAC, NONE, FPAC, FPAC,  1,  7,  5,  7, &macros[ 790] fl(10,264) }
};
/* /src386/bin/c/n1/i386/tables/asub.t */
PAT p11[] = {
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  2,  2,  0, &macros[ 794] fl(11,17) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  2,  3,  3, &macros[ 796] fl(11,26) },
 {  1,  1, ANYR, NONE, NONE, TEMP,  1,  1,  4,  1, &macros[ 796] fl(11,31) },
 {  2,  1, ANYR, NONE, ANYR, TEMP,  1,  2,  5,  1, &macros[ 798] fl(11,40) },
 {  3,  3, ANYR, NONE, NONE, TEMP,  1,  4,  6,  3, &macros[ 800] fl(11,58) },
 {  4,  5, EDXEAX, NONE, EDXEAX, TEMP,  1,  5,  5,  5, &macros[ 802] fl(11,74) },
 {  4,  5, EDXEAX, NONE, EDXEAX, TEMP,  1,  6,  5,  5, &macros[ 813] fl(11,86) },
 {  5,  7, FPAC, NONE, FPAC, FPAC,  1,  7,  5,  5, &macros[ 820] fl(11,98) }
};
/* /src386/bin/c/n1/i386/tables/bef.t */
PAT p12[] = {
 { 26,  1, ANYR, NONE, NONE, TEMP,  1,  2,  2,  0, &macros[ 824] fl(12,20) },
 { 26,  1, ANYR, NONE, NONE, TEMP,  1, 20,  3,  3, &macros[ 826] fl(12,29) },
 { 10,  3, ANYR, NONE, NONE, TEMP,  1,  4,  3,  3, &macros[ 833] fl(12,45) }
};
/* /src386/bin/c/n1/i386/tables/div.t */
PAT p13[] = {
 {  9,  1, EDXEAX,  EAX, NONE,  EAX,  5, 10,  7,  3, &macros[ 835] fl(13,18) },
 {  9,  1, EDXEAX,  EAX, NONE,  EAX,  5, 11,  7,  3, &macros[ 839] fl(13,26) },
 {  7,  5, EDXEAX, EDXEAX, NONE, TEMP,  5,  5,  6,  5, &macros[ 847] fl(13,39) },
 {  8,  7, FPAC, FPAC, NONE, FPAC,  5,  5,  7,  8, &macros[ 858] fl(13,49) }
};
/* /src386/bin/c/n1/i386/tables/leaves.t */
PAT p14[] = {
 { 27, 21, ANYR, NONE, NONE, TEMP, 11,  0,  0,  0, &macros[ 862] fl(14,38) },
 { 27,  1, ANYR, NONE, NONE, TEMP, 12,  1,  0,  0, &macros[ 866] fl(14,45) },
 { 27,  1, ANYR, NONE, NONE, TEMP,  6,  1,  0,  0, &macros[ 871] fl(14,57) },
 { 27, 18, ANYR, NONE, NONE, TEMP,  6, 18,  0,  0, &macros[ 871] fl(14,62) },
 {  3, 19, ANYR, NONE, NONE, TEMP,  6, 19,  0,  0, &macros[ 871] fl(14,67) },
 { 28,  5, EDXEAX, NONE, NONE, TEMP,  3,  5,  0,  0, &macros[ 877] fl(14,75) },
 { 28,  5, EDXEAX,  EDX, NONE, TEMP,  7,  5,  0,  0, &macros[ 888] fl(14,85) },
 { 29,  7, EDXEAX, NONE, NONE, TEMP,  6,  6,  0,  0, &macros[ 890] fl(14,95) },
 { 30,  7, EDXEAX, NONE, NONE, TEMP,  6,  6,  0,  0, &macros[ 900] fl(14,103) },
 { 31,  5, FPAC, NONE, NONE, FPAC, 11,  0,  0,  0, &macros[ 908] fl(14,112) },
 { 31,  5, FPAC, NONE, NONE, FPAC,  2,  0,  0,  0, &macros[ 910] fl(14,119) },
 { 31,  5, FPAC, NONE, NONE, FPAC,  7,  5,  0,  0, &macros[ 912] fl(14,126) },
 { 32,  1, ANYR, NONE, NONE, TEMP,  7, 17,  0,  0, &macros[ 915] fl(14,143) },
 { 32, 18, ANYR, NONE, NONE, TEMP,  7, 19,  0,  0, &macros[ 915] fl(14,148) },
 { 33,  5, EDXEAX, NONE, NONE, TEMP,  3, 10,  0,  0, &macros[ 919] fl(14,156) },
 { 33,  5, EDXEAX, NONE, NONE, TEMP,  7, 22,  0,  0, &macros[ 927] fl(14,164) },
 { 33,  5, EDXEAX, NONE, NONE, TEMP,  3, 23,  0,  0, &macros[ 931] fl(14,172) },
 { 33,  5, EDXEAX, NONE, NONE, TEMP,  7, 24,  0,  0, &macros[ 939] fl(14,180) },
 { 33,  5, EDXEAX, NONE, NONE, TEMP,  6,  6,  0,  0, &macros[ 943] fl(14,188) },
 { 31,  7, FPAC, NONE, NONE, FPAC,  7, 25,  0,  0, &macros[ 945] fl(14,196) },
 { 32, 18, ANYR, NONE, NONE, TEMP,  3,  1,  0,  0, &macros[ 948] fl(14,211) },
 { 34, 18, ANYR, ANYR, NONE, TEMP,  5,  1,  0,  0, &macros[ 951] fl(14,218) },
 { 32, 18, ANYR, NONE, NONE, TEMP, 13,  1,  0,  0, &macros[ 952] fl(14,225) },
 { 32, 18, ANYR, NONE, NONE, TEMP,  7,  1,  0,  0, &macros[ 956] fl(14,233) },
 { 32, 19, ANYR, NONE, NONE, TEMP,  3,  1,  0,  0, &macros[ 959] fl(14,241) },
 { 34, 19, ANYR, ANYR, NONE, TEMP,  5, 26,  0,  0, &macros[ 966] fl(14,248) },
 { 32, 19, ANYR, NONE, NONE, TEMP, 13, 26,  0,  0, &macros[ 967] fl(14,255) },
 { 32, 19, ANYR, NONE, NONE, TEMP,  7, 26,  0,  0, &macros[ 971] fl(14,263) },
 { 33, 10, EDXEAX,  EDX, NONE,  EAX,  6,  5,  0,  0, &macros[ 973] fl(14,273) },
 { 33, 12, EDXEAX,  EDX, NONE,  EAX,  6,  5,  0,  0, &macros[ 987] fl(14,283) },
 { 33, 23, EDXEAX,  EDX, NONE,  EAX,  6,  5,  0,  0, &macros[1007] fl(14,294) },
 { 33, 27, EDXEAX,  EDX, NONE,  EAX,  6,  5,  0,  0, &macros[1021] fl(14,304) },
 { 33, 10,  EAX, NONE, NONE, TEMP,  6,  6,  0,  0, &macros[1038] fl(14,314) },
 { 33, 12,  EAX, NONE, NONE, TEMP,  6,  6,  0,  0, &macros[1045] fl(14,322) },
 { 33, 23,  EAX, NONE, NONE, TEMP,  6,  6,  0,  0, &macros[1057] fl(14,331) },
 { 33, 27,  EAX, NONE, NONE, TEMP,  6,  6,  0,  0, &macros[1064] fl(14,339) },
 { 12,  3, NONE, NONE, NONE, NONE,  4,  3,  0,  0, &macros[1074] fl(14,357) },
 { 35, 20, NONE, NONE, NONE, NONE,  4, 20,  0,  0, &macros[1074] fl(14,362) },
 { 12,  3, NONE, NONE, NONE, NONE,  7,  3,  0,  0, &macros[1080] fl(14,371) },
 { 36, 20, NONE, NONE, NONE, NONE,  7, 20,  0,  0, &macros[1080] fl(14,376) },
 { 12,  0, NONE, NONE, NONE, NONE,  7, 17,  0,  0, &macros[1087] fl(14,384) },
 { 37,  0, NONE, NONE, NONE, NONE,  0,  0,  0,  0, &macros[1090] fl(14,392) },
 { 38,  0, NONE, NONE, NONE, NONE,  0,  0,  0,  0, &macros[1093] fl(14,399) },
 { 12,  3, NONE, ANYR, NONE, NONE,  5,  3,  0,  0, &macros[1094] fl(14,406) },
 { 36, 20, NONE, ANYR, NONE, NONE,  5, 20,  0,  0, &macros[1094] fl(14,411) },
 { 39,  5, NONE, NONE, NONE, NONE,  7,  5,  0,  0, &macros[1100] fl(14,419) },
 { 40,  5, NONE, NONE, NONE, NONE,  7,  5,  0,  0, &macros[1108] fl(14,427) },
 { 41,  7, NONE, NONE, NONE, NONE,  7,  6,  0,  0, &macros[1116] fl(14,435) },
 { 42,  5, NONE, NONE, NONE, NONE,  4,  5,  0,  0, &macros[1119] fl(14,443) },
 { 43,  1, ANYR, NONE, NONE, NONE, 12,  1,  0,  0, &macros[1125] fl(14,457) },
 { 43,  1, NONE, NONE, NONE, NONE,  3,  1,  0,  0, &macros[1132] fl(14,465) },
 { 43,  1, NONE, NONE, NONE, NONE,  7,  1,  0,  0, &macros[1132] fl(14,470) },
 { 44,  5, EDXEAX, EDXEAX, NONE, NONE,  4,  5,  0,  0, &macros[1135] fl(14,482) },
 { 44,  5, NONE, NONE, NONE, NONE,  7,  5,  0,  0, &macros[1142] fl(14,490) },
 { 45,  5, NONE, NONE, NONE, NONE, 13,  5,  0,  0, &macros[1149] fl(14,501) },
 { 45,  5, NONE, NONE, NONE, NONE,  4,  5,  0,  0, &macros[1152] fl(14,510) },
 { 14, 28, NONE, NONE, NONE, NONE,  6, 28,  0,  0, &macros[1156] fl(14,525) },
 { 14, 21, NONE, NONE, NONE, NONE,  4, 21,  0,  0, &macros[1157] fl(14,532) },
 { 17,  5, NONE, NONE, NONE, NONE,  4,  5,  0,  0, &macros[1158] fl(14,539) },
 { 46,  5, NONE, NONE, NONE, NONE,  4,  5,  0,  0, &macros[1159] fl(14,546) }
};
/* /src386/bin/c/n1/i386/tables/mul.t */
PAT p15[] = {
 {  9,  1, ANYR, NONE, NONE, TEMP,  7,  3,  9,  3, &macros[1161] fl(15,19) },
 {  9,  1, EDXEAX,  EAX, NONE,  EAX,  5,  3,  7,  3, &macros[1166] fl(15,26) },
 {  7,  5, EDXEAX, EDXEAX, NONE, TEMP,  5,  5,  6,  5, &macros[1170] fl(15,38) },
 {  8,  7, FPAC, FPAC, NONE, FPAC,  5,  5,  7,  8, &macros[1181] fl(15,48) }
};
/* /src386/bin/c/n1/i386/tables/neg.t */
PAT p16[] = {
 { 47,  3, ANYR, ANYR, NONE, TEMP,  5,  3,  0,  0, &macros[1185] fl(16,14) },
 { 48,  5, EDXEAX, EDXEAX, NONE, EDXEAX,  5,  5,  0,  0, &macros[1191] fl(16,28) },
 {  8,  5, FPAC, FPAC, NONE, FPAC,  5,  5,  0,  0, &macros[1197] fl(16,35) }
};
/* /src386/bin/c/n1/i386/tables/not.t */
PAT p17[] = {
 { 49,  3, ANYR, ANYR, NONE, TEMP,  5,  3,  0,  0, &macros[1199] fl(17,15) }
};
/* /src386/bin/c/n1/i386/tables/orxor.t */
PAT p18[] = {
 { 50,  1, ANYR, ANYR, NONE, TEMP,  5,  1,  6,  1, &macros[1202] fl(18,16) }
};
/* /src386/bin/c/n1/i386/tables/relop.t */
PAT p19[] = {
 { 36,  0, NONE, NONE, NONE, NONE,  4,  1,  6,  1, &macros[1204] fl(19,33) },
 { 51,  0, ANYR, ANYR, NONE, NONE,  5,  1,  3,  1, &macros[1209] fl(19,41) },
 { 36,  0, NONE, NONE, NONE, NONE,  7, 29, 14,  3, &macros[1214] fl(19,49) },
 { 36,  0, NONE, NONE, NONE, NONE,  7, 30,  9,  3, &macros[1214] fl(19,54) },
 { 36,  0, NONE, NONE, NONE, NONE,  7, 31, 15,  3, &macros[1214] fl(19,59) },
 { 36,  0, NONE, NONE, NONE, NONE,  7, 32, 16,  3, &macros[1214] fl(19,64) },
 { 51,  0, ANYR, ANYR, NONE, NONE,  5,  1,  6,  1, &macros[1217] fl(19,72) },
 { 51,  0, ANYR, ANYR, NONE, NONE,  5, 18,  6, 18, &macros[1217] fl(19,77) },
 { 52,  0, ANYR, ANYR, NONE, NONE,  5, 19,  6, 19, &macros[1217] fl(19,82) },
 { 53,  0, EDXEAX, EDXEAX, NONE, NONE,  5,  5,  6,  5, &macros[1224] fl(19,96) },
 { 54,  0, NONE, FPAC, NONE, NONE,  5,  5,  7,  8, &macros[1236] fl(19,106) }
};
/* /src386/bin/c/n1/i386/tables/rem.t */
PAT p20[] = {
 {  9,  1, EDXEAX,  EAX, NONE,  EDX,  5, 10,  7,  3, &macros[1245] fl(20,20) },
 {  9,  1, EDXEAX,  EAX, NONE,  EDX,  5, 11,  7,  3, &macros[1248] fl(20,28) }
};
/* /src386/bin/c/n1/i386/tables/shift.t */
PAT p21[] = {
 { 50,  1, ANYR, ANYR, NONE, TEMP,  5,  1, 10,  3, &macros[1251] fl(21,22) },
 { 49,  1,  EAX,  EAX,  ECX,  EAX,  5,  1,  7,  3, &macros[1254] fl(21,31) }
};
/* /src386/bin/c/n1/i386/tables/sub.t */
PAT p22[] = {
 {  6,  1, ANYR, ANYR, NONE, TEMP,  5,  1,  2,  0, &macros[1264] fl(22,17) },
 { 27,  3, ANYR, NONE, NONE, TEMP, 17, 20, 17, 20, &macros[1266] fl(22,27) },
 {  6,  1, ANYR, ANYR, NONE, TEMP,  5,  1,  6,  1, &macros[1273] fl(22,35) },
 {  7,  5, EDXEAX, EDXEAX, NONE, TEMP,  5,  5,  6,  5, &macros[1275] fl(22,48) },
 {  8,  7, FPAC, FPAC, NONE, FPAC,  5,  5,  7,  8, &macros[1286] fl(22,58) }
};
PATX patx[] = {
	p2,	4,
	p22,	5,
	p15,	4,
	p13,	4,
	p20,	2,
	p7,	2,
	p18,	1,
	p18,	1,
	p21,	2,
	p21,	2,
	p1,	8,
	p11,	8,
	p6,	7,
	p3,	8,
	p8,	5,
	p5,	7,
	p5,	7,
	p5,	7,
	p9,	5,
	p9,	5,
	p19,	11,
	p19,	11,
	p19,	11,
	p19,	11,
	p19,	11,
	p19,	11,
	p19,	11,
	p19,	11,
	p19,	11,
	p19,	11,
	NULL,	0,
	NULL,	0,
	p16,	3,
	p17,	1,
	NULL,	0,
	NULL,	0,
	NULL,	0,
	p12,	3,
	p12,	3,
	p4,	3,
	p4,	3,
	NULL,	0,
	NULL,	0,
	NULL,	0,
	NULL,	0,
	p14,	60,
	p14,	60,
	NULL,	0,
	NULL,	0,
	p10,	26,
	NULL,	0,
	NULL,	0,
	NULL,	0,
	p14,	60,
	p14,	60,
	NULL,	0
};
PATFLAG	patcache[] = {
	PEFFECT|PVALUE|PREL,
	PEFFECT|PVALUE|PREL|P_SRT|PBYTE,
	PVALUE|PBYTE,
	PEFFECT|PVALUE|P_SRT|PDECVAX|PIEEE,
	PEFFECT|PVALUE|PNDP,
	PEFFECT|PVALUE|PSREL|P_SLT,
	PEFFECT|PVALUE|P_SLT|PDECVAX|PIEEE,
	PVALUE|P_SLT|PNDP,
	PEFFECT|PVALUE,
	PEFFECT|PVALUE|PBYTE,
	PEFFECT|PVALUE|P_SRT|PBYTE,
	PSREL,
	PEFFECT|PVALUE|PEREL|P_SLT,
	PEFFECT,
	PEFFECT|PVALUE|P_SRT,
	PEFFECT|PVALUE|PREL|P_SRT,
	PEFFECT|PIEEE|PDECVAX,
	PEFFECT|PVALUE|P_SRT|PIEEE|PDECVAX,
	PEFFECT|PDECVAX|PIEEE,
	PEFFECT|PDECVAX,
	PEFFECT|PVALUE|PDECVAX,
	PEFFECT|PVALUE|P_SRT|PDECVAX,
	PEFFECT|PVALUE|P_SRT|PIEEE,
	PEFFECT|P_SRT|PNDP,
	PVALUE|P_SRT|PNDP,
	PEFFECT|PVALUE|PSREL,
	PVALUE,
	PVALUE|PIEEE|PDECVAX,
	PVALUE|PDECVAX,
	PVALUE|PIEEE,
	PVALUE|PNDP,
	PVALUE|PEFFECT,
	PVALUE|PEFFECT|PDECVAX|PIEEE,
	PVALUE|PEFFECT|P_SLT,
	PEREL,
	PREL,
	PUGE,
	PULT,
	PREL|PIEEE,
	PREL|PDECVAX,
	PREL|PIEEE|PDECVAX,
	PREL|PNDP,
	PFNARG,
	PFNARG|PIEEE|PDECVAX,
	PFNARG|PNDP,
	PEFFECT|PNDP,
	PEFFECT|PVALUE|PGE|PLT|P_SLT,
	PEFFECT|PVALUE|P_SLT|PIEEE|PDECVAX,
	PEFFECT|PVALUE|P_SLT,
	PEFFECT|PVALUE|PREL|P_SLT,
	PREL|P_SLT,
	PREL|P_SLT|PBYTE,
	PREL|PDECVAX|PIEEE,
	PREL|P_SLT|PNDP
};
int patcsize=sizeof(patcache)/sizeof(PATFLAG);
TYPESET	typecache[] = {
	DWORD,
	DWORD|SHORT,
	LONG,
	FLD,
	FF64,
	FF32,
	FLOAT,
	NDPARG,
	UINT,
	FS32,
	FU32,
	FS16|FS8,
	SINT,
	SINT|UINT,
	WORD|BYTE,
	DWORD|WORD|BYTE,
	SHORT,
	WORD,
	BYTE,
	PTX,
	NFLT,
	FS8|FS16|FS32,
	FU32|PTX,
	FU8|FU16|FU32|PTX,
	FS16|FS32|FF32,
	WORD|DWORD,
	FU16|FU8,
	FBLK,
	FS16,
	FS8,
	FU16,
	FU8
};
FLAG	flagcache[] = {
	T_ADR|T_LV,
	T_1|T_MMX,
	T_IMM|T_MMX,
	T_REG|T_MMX,
	T_TREG,
	T_ADR|T_IMM,
	T_ADR,
	T_EASY|T_MMX,
	T_SBYTE|T_MMX,
	T_SHCNT|T_MMX,
	T_0|T_MMX,
	T_LEA|T_MMX,
	T_DIR|T_MMX,
	T_SWORD|T_MMX,
	T_UWORD|T_MMX,
	T_UBYTE|T_MMX,
	T_DIR|T_IMM|T_MMX
};
ival_t	ivalcache[] = {
	0x8,
	0x4,
	0x0,
	0x7ff00000,
	0x7f800000,
	0x80000000
};
lval_t	lvalcache[] = {
	0
};
char	*gidcache[] = {
	"_dadd",
	"_fadd",
	"_fdcvt",
	"_dfcvt",
	"_ddiv",
	"_fdiv",
	"_dmul",
	"_fmul",
	"_dsub",
	"_fsub",
	"_drdiv",
	"_dicvt",
	"_ducvt",
	"_idcvt",
	"_udcvt",
	"_ifcvt",
	"_ufcvt",
	"_tstccp",
	"_dp87",
	"_dcmp",
	"_cfcc",
	"_drsub"
};
#if !TINY
char	*namecache[] = {
	"prefac.f",
	"aadd.t",
	"add.t",
	"adiv.t",
	"aft.t",
	"alogic.t",
	"amul.t",
	"and.t",
	"arem.t",
	"ashift.t",
	"assign.t",
	"asub.t",
	"bef.t",
	"div.t",
	"leaves.t",
	"mul.t",
	"neg.t",
	"not.t",
	"orxor.t",
	"relop.t",
	"rem.t",
	"shift.t",
	"sub.t"
};
#endif
