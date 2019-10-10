/* parse_args - parse arguments to uucheck.
 * Possible arguments are:
 * -s or -silent	no output
 * -v or -verbose	verbose output
 * -f or -fix		fix as much as possible, must be root to use
 */

#include "uucheck.h"

void
parse_args(argc, argv)
	int argc;
	char *argv[];
{
	int i;

	silent = FALSE;	 /* Generate no output if set.  Overrides verbose.  */
	verbose = FALSE; /* Generate verbose output if set.  */
	reallyverbose = FALSE; /* Generate really verbose output if set.  */
	fix = FALSE;	 /* Attempt to fix problems if set.  */

	do_sitename = TRUE;
	do_lockfiles = TRUE;
	do_fileperms = TRUE;
	do_devices = TRUE;
	do_sys = TRUE;
	do_permissions = TRUE;
	do_log = TRUE;
	
	for (i = 1; i < argc; ++i) {
		if ( (strcmp(argv[i], "-s") == 0) ||
		     (strcmp(argv[i], "-silent") == 0) ){
			silent = TRUE;
			continue;
		} /* if argv[i] == -silent */

		if ( (strcmp(argv[i], "-v") == 0) ||
		     (strcmp(argv[i], "-verbose") == 0) ){
			if(verbose) {
				reallyverbose = TRUE;
			} else {
				verbose = TRUE;
			} /* if already verbose */
			continue;
		} /* if argv[i] == -verbose */

		if ( (strcmp(argv[i], "-f") == 0) ||
		     (strcmp(argv[i], "-fix") == 0) ){
#ifndef DEBUG
			if(getuid() == 0) {
				fix = TRUE;
			} else {
				fprintf(stderr, "Only root can fix.\n");
			} /* if I'm root */
#else /* DEBUG */
			fix = TRUE;
#endif /* DEBUG */
			continue;
		} /* if argv[i] == -fix */

		fprintf(stderr, USAGE);
		exit(255);
	} /* for i = 1 to argc */


} /* parse_args() */
