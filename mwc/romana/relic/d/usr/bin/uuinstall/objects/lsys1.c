/*
 * Screen definition file
 * This file generated from ../lsys1
 */
#include <lsys1.h>

static char L0001[] = "Sitename of remote system.";
static char L0002[] = "ACU, None or a DIR line (2nd entry) in devices.";
static char L0003[] = "110, 300, 600, 1200, 2400, 4800, 9600 or 19200";
static char L0004[] = "Must be in the format understood by your dialer.";
static char L0005[] = "Su Mo Tu We Th Fr Sa Wk Any Never";
static char L0006[] = "0000 to 2400 or null for any";
static char L0007[] = "0000 to 2400";
static char L0008[] = "Expected message from remote site.";
static char L0009[] = "Reply to remote site.";
backGrnd lsys1_data[] = {
"Connected system list",
	0, 16,
"System",
	1, 0,
"Line",
	2, 0,
"baud rate",
	3, 0,
"phone number",
	4, 0,
"Day to call     Time From    Time To",
	5, 0,
"expect                              send",
	13, 0,
"expect                              send",
	14, 0,
"expect                              send",
	15, 0,
"expect                              send",
	16, 0,
"expect                              send",
	17, 0,
"expect                              send",
	18, 0,
"expect                              send",
	19, 0,
"expect                              send",
	20, 0,
	NULL
};
char sys[21];
char Line[8];
char baudRate[7];
char phoneNo[15];
char day[7][16];
char timeFrom[7][5];
char timeTo[7][5];
char *expect[8];
char *send[8];

loc lsys1_locs[] = {
	{ sys, 20, sys, nonNull, 0, 1, 16, 0, L0001 },
	{ Line, 7, Line, checkLine, 0, 2, 16, 0, L0002 },
	{ baudRate, 6, baudRate, numeric, 0, 3, 16, 0, L0003 },
	{ phoneNo, 14, phoneNo, noWhite, 0, 4, 16, 0, L0004 },
	{ day[0], 15, day[0], checkDay, 0, 6, 0, 20, L0005 },
	{ timeFrom[0], 4, timeFrom[0], checkTime, 0, 6, 16, 1, L0006 },
	{ timeTo[0], 4, timeTo[0], checkTime, 0, 6, 29, 0, L0007 },
	{ day[1], 15, day[1], checkDay, 0, 7, 0, 17, L0005 },
	{ timeFrom[1], 4, timeFrom[1], checkTime, 0, 7, 16, 1, L0006 },
	{ timeTo[1], 4, timeTo[1], checkTime, 0, 7, 29, 0, L0007 },
	{ day[2], 15, day[2], checkDay, 0, 8, 0, 14, L0005 },
	{ timeFrom[2], 4, timeFrom[2], checkTime, 0, 8, 16, 1, L0006 },
	{ timeTo[2], 4, timeTo[2], checkTime, 0, 8, 29, 0, L0007 },
	{ day[3], 15, day[3], checkDay, 0, 9, 0, 11, L0005 },
	{ timeFrom[3], 4, timeFrom[3], checkTime, 0, 9, 16, 1, L0006 },
	{ timeTo[3], 4, timeTo[3], checkTime, 0, 9, 29, 0, L0007 },
	{ day[4], 15, day[4], checkDay, 0, 10, 0, 8, L0005 },
	{ timeFrom[4], 4, timeFrom[4], checkTime, 0, 10, 16, 1, L0006 },
	{ timeTo[4], 4, timeTo[4], checkTime, 0, 10, 29, 0, L0007 },
	{ day[5], 15, day[5], checkDay, 0, 11, 0, 5, L0005 },
	{ timeFrom[5], 4, timeFrom[5], checkTime, 0, 11, 16, 1, L0006 },
	{ timeTo[5], 4, timeTo[5], checkTime, 0, 11, 29, 0, L0007 },
	{ day[6], 15, day[6], checkDay, 0, 12, 0, 2, L0005 },
	{ timeFrom[6], 4, timeFrom[6], checkTime, 0, 12, 16, 1, L0006 },
	{ timeTo[6], 4, timeTo[6], checkTime, 0, 12, 29, 0, L0007 },
	{ expect + 0, 28, expect + 0, noWhite, 4, 13, 7, 0, L0008 },
	{ send + 0, 28, send + 0, noWhite, 4, 13, 41, 0, L0009 },
	{ expect + 1, 28, expect + 1, noWhite, 4, 14, 7, 0, L0008 },
	{ send + 1, 28, send + 1, noWhite, 4, 14, 41, 0, L0009 },
	{ expect + 2, 28, expect + 2, noWhite, 4, 15, 7, 0, L0008 },
	{ send + 2, 28, send + 2, noWhite, 4, 15, 41, 0, L0009 },
	{ expect + 3, 28, expect + 3, noWhite, 4, 16, 7, 0, L0008 },
	{ send + 3, 28, send + 3, noWhite, 4, 16, 41, 0, L0009 },
	{ expect + 4, 28, expect + 4, noWhite, 4, 17, 7, 0, L0008 },
	{ send + 4, 28, send + 4, noWhite, 4, 17, 41, 0, L0009 },
	{ expect + 5, 28, expect + 5, noWhite, 4, 18, 7, 0, L0008 },
	{ send + 5, 28, send + 5, noWhite, 4, 18, 41, 0, L0009 },
	{ expect + 6, 28, expect + 6, noWhite, 4, 19, 7, 0, L0008 },
	{ send + 6, 28, send + 6, noWhite, 4, 19, 41, 0, L0009 },
	{ expect + 7, 28, expect + 7, noWhite, 4, 20, 7, 0, L0008 },
	{ send + 7, 28, send + 7, noWhite, 4, 20, 41, 0, L0009 },
	NULL
};
