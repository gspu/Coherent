/*
 * Screen definition file
 * This file generated from ../permis
 */
#include <permis.h>

static char L0001[] = "Allow remote login?";
static char L0002[] = "n";
static char L0003[] = "eg: rmail:rnews";
static char L0004[] = "eg: /usr/spool/uucppublic:/tmp";
static char L0005[] = "eg: /tmp/private";
static char L0006[] = "eg: /usr/spool/uucppublic:/tmp";
static char L0007[] = "eg: /tmp/private";
backGrnd permis_data[] = {
"/usr/lib/uucp/Permissions Items in all lists are separated by : (colon)",
	0, 1,
"Remote site name",
	2, 1,
"Provide an entry for that site calling in <y/n>",
	3, 1,
"LOGNAME",
	4, 6,
"Add an entry to /etc/passwd <y/n>",
	5, 1,
"Identify myself as",
	6, 1,
"Can the remote site request file transfers from this computer <y/n/c>",
	7, 1,
"Can this computer initiate file transfers to the remote site <y/n>",
	8, 1,
"Commands which can be executed at this computer by this remote site",
	11, 1,
"Read directory list",
	13, 1,
"Exceptions to the read directory list",
	15, 1,
"Write directory list",
	17, 1,
"Exceptions to the write directory list",
	19, 1,
	NULL
};
char perSite[21];
char perCallIn[2];
char perLogn[21];
char perEtc[2];
char perMyName[21];
char perRequest[2];
char perSendFiles[2];
char *perComm;
char *perRead;
char *perNoRead;
char *perWrite;
char *perNoWrite;

loc permis_locs[] = {
	{ perSite, 20, perSite, vSite, 0, 2, 18, 0, NULL },
	{ perCallIn, 1, perCallIn, yesNo, 0, 3, 49, 0, L0001 },
	{ perLogn, 20, perLogn, noVerify, 2, 4, 18, 0, NULL },
	{ perEtc, 1, L0002, yesNo, 0, 5, 35, 0, NULL },
	{ perMyName, 20, perMyName, noVerify, 0, 6, 20, 0, NULL },
	{ perRequest, 1, perRequest, yesNoCall, 0, 7, 71, 0, NULL },
	{ perSendFiles, 1, perSendFiles, yesNo, 0, 8, 68, 0, NULL },
	{ &perComm, 70, &perComm, noWhite, 4, 12, 3, 0, L0003 },
	{ &perRead, 70, &perRead, noWhite, 4, 14, 3, 0, L0004 },
	{ &perNoRead, 70, &perNoRead, noWhite, 4, 16, 3, 0, L0005 },
	{ &perWrite, 70, &perWrite, noWhite, 4, 18, 3, 0, L0006 },
	{ &perNoWrite, 70, &perNoWrite, noWhite, 4, 20, 3, 0, L0007 },
	NULL
};
