/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* savebuf.c: save the accumulated text in a session file */

#include <stdio.h>

extern errno;

char *filename = "session.log";

setenv(){} /* no environment */

main(argc, argv)
int argc;
char **argv;
{
	FILE *f;
	register char c;

	if(argc > 1) filename = argv[1];
	if(argc > 2) 	{
		fprintf(stderr, "usage: savebuf [file]\n");
		exit(1);
	}

	f = fopen(filename, "w");
	if(!f) 	{
		fprintf(stderr, "savebuf: cannot create file %s\n", filename);
		exit(1);
	}

while(1) {
c = getch();
if(c == '\32') break;
if(c == '\r') c = '\n';
putc(c, f);
}

putc('\n', f);
} /* main */

