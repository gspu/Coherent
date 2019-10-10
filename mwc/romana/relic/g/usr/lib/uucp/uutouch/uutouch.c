/*
 *  uutouch.c
 *
 *  Force a command file to the given site which will trigger the need
 *  for a call the next "uucico -sany".
 */

#include <stdio.h>
/* #include "dcp.h" */

#define SPOOLDIR	"/usr/spool/uucp"
#define SITESIG		8
#define VERSION		"COHERENT-Taylor 1.04"

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
			fatal("Improper option usage: %c", ch);
			exit(1);
		}
	}
	for (;optind<argc; optind++) {
		arg = argv[optind];
#ifdef TAYLOR_UUCP
		if ( !check_sys_file(arg) ) {
			printf("Host \"%s\" unknown\n", arg);
#else
		if ( !knowhost(arg) ) {
			nonfatal("Host \"%s\" unknown", arg);
#endif
			error++;
		} else {
			sprintf(ctlfil, "%s/%s", SPOOLDIR, arg);
#ifndef TAYLOR_UUCP
			if ( !ckdir(ctlfil) ) {
				nonfatal("Unable to create directory: %s",
								ctlfil);
				error++;
				continue;
			}
#endif
			sprintf(ctlfil, "%s/%s/C.%.*s0%.4d", SPOOLDIR, arg,
							SITESIG, arg, getpid());
			if ( (fd=creat(ctlfil, 0600)) < 0 ) {
#ifdef TAYLOR_UUCP
				printf("Unable to create file: %s\n", ctlfil);
#else
				nonfatal("Unable to create file: %s", ctlfil);
#endif
				error++;
			} else
				close(fd);
		}
	}
	exit(error);
}

