/*
 * Screen definition file
 * This file generated from ../devices
 */
#include <devices.h>

static char L0001[] = "DIR for direct connect via null modem cable, ACU for modem.";
static char L0002[] = "eg: com1l, com2l, or an entry in /etc/ttys like tty16.";
static char L0003[] = "If modem control device, com1r etc. If null modem cable, -";
static char L0004[] = "110, 300, 600, 1200, 2400, 4800, 9600 or 19200";
static char L0005[] = "direct for null modem cable or a brand name in /etc/modemcap";
backGrnd devices_data[] = {
"L-devices",
	3, 8,
"Type",
	5, 0,
"Line",
	6, 0,
"Remote",
	7, 0,
"Baudrate",
	8, 0,
"brand",
	9, 0,
	NULL
};
char devType[11];
char devLine[11];
char devRemote[11];
char devBaud[11];
char devBrand[11];

loc devices_locs[] = {
	{ devType, 10, devType, nonNull, 0, 5, 9, 0, L0001 },
	{ devLine, 10, devLine, nonNull, 0, 6, 9, 0, L0002 },
	{ devRemote, 10, devRemote, nonNull, 0, 7, 9, 0, L0003 },
	{ devBaud, 10, devBaud, numeric, 0, 8, 9, 0, L0004 },
	{ devBrand, 10, devBrand, nonNull, 0, 9, 9, 0, L0005 },
	NULL
};
