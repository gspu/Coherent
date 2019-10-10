/*
 *  uuname.c
 *
 *  Display the machines which are known to the UUCP system.
 */

#include <stdio.h>
#include "dcp.h"
#include "lsys.h"

main(argc, argv)
int argc;
char *argv[];
{
	int localflg = 0;

	while ( --argc ) {
		if ( **++argv != '-' )
			usage();
		switch (argv[0][1]) {
		case 'l':
			localflg = 1;
			break;
		case 'v':
		case 'V':
			fatal("uuname: Version %s", VERSION);
			break;
		default:
			usage();
		}
	}

	if ( localflg )
		puts(uucpname());
	else {
		lsys_open();		
		while ( lsys_next() > 0 )
			puts( lsys_value(sys_e) );
		lsys_close();
	}
	exit(0);
}

usage()
{
	fatal("\n\
Usage:	uuname 		display uucp names of known systems\n\
	uuname -l	display local uucp name\n\
");
}
