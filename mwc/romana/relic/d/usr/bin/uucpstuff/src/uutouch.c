/*
 *  uutouch.c
 *
 *  Force a command file to the given site which will trigger the need
 *  for a call the next "uucico -sany".
 */

#include <stdio.h>
#include "dcp.h"

extern	int optopt, optind;
char	ctlfil[BUFSIZ];

main(argc, argv)
int argc;
char *argv[];
{
	int ch, fd, error=0;
	char *arg;

	while ( (ch=getopt(argc, argv, "vV")) != EOF ) {
		switch (ch) {
		case 'v':
		case 'V':
			fatal("uutouch: Version %s", VERSION);
		default:
			fatal("Improper option usage: %c", optopt);
		}
	}
	for (;optind<argc; optind++) {
		arg = argv[optind];
		if ( !knowhost(arg) ) {
			nonfatal("Host \"%s\" unknown", arg);
			error++;
		} else {
			sprintf(ctlfil, "%s/%s", SPOOLDIR, arg);
			if ( !ckdir(ctlfil) ) {
				nonfatal("Unable to create directory: %s",
								ctlfil);
				error++;
				continue;
			}
			sprintf(ctlfil, "%s/%s/C.%.*s0", SPOOLDIR, arg,
							SITESIG, arg);
			if ( (fd=creat(ctlfil, 0600)) < 0 ) {
				nonfatal("Unable to create file: %s", ctlfil);
				error++;
			} else
				close(fd);
		}
	}
	exit(error);
}

