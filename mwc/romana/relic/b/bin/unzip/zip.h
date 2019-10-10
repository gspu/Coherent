/* This is a dummy zip.h to allow crypt.c from Zip to compile for unzip */

#include "unzip.h"
#define SKIP_TIME_H   /* don't include time.h again in crypt.c */

#define local

extern ULONG crc_32_tab[];
#define crc32(c, b) (crc_32_tab[((int)(c) ^ (b)) & 0xff] ^ ((c) >> 8))
