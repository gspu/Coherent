/*
 * uuchecklock - Check for dead lock files.
 */

#include "uucheck.h"

uuchecklock()
{
	FILE *fp;

	REALLYVERBOSE("\n\nChecking for lock files...\n");

	sprintf(bigbuf, "ls -d %s/LCK* 2> /dev/null", SPOOL);

	if ((fp = popen(bigbuf, "r")) == (FILE *) NULL) {
		FATAL("Can't execute %s\n", bigbuf);
	} /* if popen on ls failed */
		
	/* Read all the output there is to read.
	 * If we don't do this, a long output from ls could hang on
	 * a full pipe.
	 */
	while (fgetc(fp) != EOF) {
		/* do nothing */
	}
	
	/* Result code of 0 means that ls found something.  */
	if (pclose(fp) == 0) {
		WARNING("There are lock files.\n");
		VERBOSE("Use /usr/lib/uucp/uurmlock to remove them.\n");
		if (reallyverbose) {
			REALLYVERBOSE("They are:\n");
			sprintf(bigbuf, "ls -CdF %s/LCK*", SPOOL);
			system(bigbuf);
		}
		/* We probably ought to get fancy here and check to
		 * see if uucico is running.  But not now.
		 */
		FIX(system("/usr/lib/uucp/uurmlock"););
	} /* if (pclose(fp) == 0) */

	if (!error && !warning) {
		VERBOSE("Good, there are no lock files.\n");
	} /* if no error or warning */

	RETURN;
} /* uuchecklock */
