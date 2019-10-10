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
	unsigned long ul;
	char sgn;

	if ((fp = fopen(IDVARFILE, "a")) == NULL) {
		fprintf(stderr, "Can't append to %s\n", IDVARFILE);
		exit(1);
	}

	if (varValue < 0) {
		ul = -varValue;
		sgn = '-';
	} else {
		ul = varValue;
		sgn = ' ';
	}

	fprintf(fp, "%s\t%c0x%x\n", varName, sgn, ul);

	fclose(fp);
}

#if TEST

#include <string.h>

#if __USE_PROTO__
int
main (int argc, char *argv[])
#else
int
main(argc, argv)
int argc;
char * argv[];
#endif
{
	char parname[32];
	long val;

	for(;;) {
		printf("Enter paramter name, value (quit to quit): ");
		scanf("%s%lx", parname, &val);
		if (strcmp(parname, "quit") == 0)
			break;
		printf("\nParameter=%s Value=%x\n-- id-var --\n", parname, val);
		idtune_var (parname, val);
		system("cat " IDVARFILE);
	}
	return 0;
}
#endif
