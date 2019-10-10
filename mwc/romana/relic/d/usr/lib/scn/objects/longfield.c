/*
 * Screen definition file
 * This file generated from ../longfield
 */
#include <longfield.h>

backGrnd longfield_data[] = {
"Data entry for potentially long field",
	0, 16,
	NULL
};
char longfield[6][79];

loc longfield_locs[] = {
	{ longfield[0], 78, longfield[0], checkEnd, 0, 1, 0, 5, NULL },
	{ longfield[1], 78, longfield[1], checkEnd, 0, 2, 0, 4, NULL },
	{ longfield[2], 78, longfield[2], checkEnd, 0, 3, 0, 3, NULL },
	{ longfield[3], 78, longfield[3], checkEnd, 0, 4, 0, 2, NULL },
	{ longfield[4], 78, longfield[4], checkEnd, 0, 5, 0, 1, NULL },
	{ longfield[5], 78, longfield[5], checkEnd, 0, 6, 0, 0, NULL },
	NULL
};
