/*
 * Screen definition file
 * This file generated from ../escapes
 */
#include <escapes.h>

backGrnd escapes_data[] = {
"|\t\t\t\t\t\t\t\t\t    |",
	0, 0,
"|\tSubentries\t\t\t\t\tEscape Codes        |",
	1, 0,
"An expect entry may take the form\t    \"\"           Expect a null string",
	2, 0,
"expect-subsend-subexpect\t\t    \\b                 Send backspace",
	3, 0,
"Where subsend is a possibly\t\t    \\c    If send string ends with \\c",
	4, 0,
"empty string enclosed in hyphens\t        suppress newline after string",
	5, 0,
"to be sent if the expect string\t\t    \\d   Delay 1 second after sending",
	6, 0,
"is not recieved. For example:\t\t    \\n                 Send a newline",
	7, 0,
"ogin:--ogin:\t\t\t\t    \\r          Send a carrage return",
	8, 0,
"If ogin: is not recieved send a\t\t    \\s                   Send a space",
	9, 0,
"newline and wait for ogin: again\t    \\t                     Send a tab",
	10, 0,
	NULL
};

loc escapes_locs[] = {
	NULL
};
