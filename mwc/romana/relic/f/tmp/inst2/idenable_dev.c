/*
 * File:	idenable_dev.c
 *
 * Purpose:	Write a line "variable_name"  "value" to the right file
 *
 * Revised: Fri Jul 30 11:38:24 1993 CDT
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <stdio.h>
#include "build0.h"

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
void idenable_dev __PROTO((char * devName));

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/************************************************************************
 * idenable_dev
 *
 ***********************************************************************/
#if __USE_PROTO__
void idenable_dev (char * devName)
#else
void
idenable_dev (devName)
char * devName;
#endif
{
	FILE * fp;

	if ((fp = fopen(IDCMDFILE, "a")) == NULL) {
		fprintf(stderr, "Can't append to %s\n", IDCMDFILE);
		exit(1);
	}

	fprintf(fp, "/etc/conf/bin/idenable -f %s\n", devName);

	fclose(fp);
}
