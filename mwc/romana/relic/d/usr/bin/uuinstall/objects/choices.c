/*
 * Screen definition file
 * This file generated from ../choices
 */
#include <choices.h>

static char L0001[] = " ";
backGrnd choices_data[] = {
"H Help for screens",
	10, 56,
"M Modify this entry",
	11, 56,
"N Next entry",
	12, 56,
"P Previous entry",
	13, 56,
"A Add new entry",
	14, 56,
"C add Comment",
	15, 56,
"D Delete this entry",
	16, 56,
"X eXit  [ ]",
	17, 56,
	NULL
};
char code[2];

loc choices_locs[] = {
	{ code, 1, L0001, noVerify, 0, 17, 65, 0, NULL },
	NULL
};
