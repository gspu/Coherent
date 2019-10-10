#ifndef lint
static char	*sccsid = "@(#)svbinmail.c	2.5 (smail) 9/15/87";
#endif
/* */
/* This program will be used in place of /bin/mail on SVR2 sites.
/* It looks at the arguments and decides whether to call
/* SENDER for sending mail, or READER for reading mail.
/*
/* before installing as /bin/mail, move the stock /bin/mail to /bin/lmail
/*
/*  */

#include <stdio.h>
#include "defs.h"

#ifdef SENDMAIL
#define SENDER	SENDMAIL
#else
#define	SENDER	"/bin/rmail"
#endif

#define	READER	"/bin/lmail"

#define TRUE 1
#define FALSE 0

char prog[128];

void	perror(), exit(), usage();

main(argc, argv)
int argc;
char *argv[];
{
	extern int optind;
	extern char *optarg;

	int i, j, c;
	int reading, sending;

	reading = sending = FALSE;

	(void) strcpy(prog, argv[0]);

	if(argc == 1) {
		reading = TRUE;
	} else {
		while((c = getopt(argc, argv, "epqrtf:")) != EOF) {
			switch(c) {
			case 'e':
			case 'p':
			case 'q':
			case 'r':
			case 'f':
				reading = TRUE;
				break;
			case 't':
				sending = TRUE;
				break;
			default:
				usage();
				return(1);
			}
		}
	}

	/* any arguments left over -> sending */
	if(argc > optind) {
		sending = TRUE;
	}

	if((reading == TRUE) && (sending == TRUE)) {
		usage();
		return(1);
	}

	if(sending == TRUE) {
		argv[0] = SENDER;
		for(i = 1, j = optind; j < argc; i++, j++) {
			argv[i] = argv[j];
		}
		argv[i] = NULL;
	} else {
		argv[0] = READER;
	}

	(void) execvp(argv[0], argv);
	(void) fprintf(stderr, "%s: execvp(\"%s\", argv) failed: ",
		prog, argv[0]);
	perror("");
	return(1);
}

void
usage()
{
	(void) fprintf(stderr, "usage:\t%s [ -epqr ] [ -f file ]\n", prog);
	(void) fprintf(stderr, "\t%s [ -t ] persons\n", prog);
}
