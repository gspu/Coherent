/*
 * Screen definition file
 * This file generated from ../uunam
 */
#include <uunam.h>

static char L0001[] = "uucp name for this machine";
static char L0002[] = "domain info for this machine";
backGrnd uunam_data[] = {
"The sitename",
	0, 16,
"/etc/uucpname",
	2, 0,
"/etc/domain",
	3, 0,
	NULL
};
char uuname[9];
char uudomain[65];

loc uunam_locs[] = {
	{ uuname, 8, uuname, cleanAlnum, 0, 2, 14, 0, L0001 },
	{ uudomain, 64, uudomain, cleanAscii, 0, 3, 14, 0, L0002 },
	NULL
};
