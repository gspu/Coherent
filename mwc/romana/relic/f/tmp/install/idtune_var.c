/*
 * File:	idtune_var.c
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
void idtune_var __PROTO((char * varName, long varValue));

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
 * idtune_var
 *
 ***********************************************************************/
#if __USE_PROTO__
void idtune_var (char * varName, long varValue)
#else
void
idtune_var (varName, varValue)
char * varName;
long varValue;
#endif
{
	FILE * fp;

	if ((fp = fopen(IDVARFILE, "a")) == NULL) {
		fprintf(stderr, "Can't append to %s\n", IDVARFILE);
		exit(1);
	}

	fprintf(fp, "%s\t0x%x\n", varName, varValue);

	fclose(fp);
}
