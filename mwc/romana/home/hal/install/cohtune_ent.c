/*
 * File:	cohtune_ent.c
 *
 * Purpose:	Invoke "cohtune" script from a C program.
 *
 * Revised: Sun Aug  1 14:14:16 1993 CDT
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include "build0.h"

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define COHTUNE_SH	"/etc/conf/bin/cohtune"

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
void cohtune_ent __PROTO((char * devName, char * tag, char * line));

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
 * cohtune_ent
 *
 ***********************************************************************/
#if __USE_PROTO__
void cohtune_ent (char * devName, char * tag, char * line)
#else
void
cohtune_ent (devName, tag, line)
char * devName;
char * tag;
char * line;
#endif
{
	char line1[512];
	char line2[512];

	sprintf(line1, "%s %s %s '\"%s\"'", COHTUNE_SH, devName, tag, line);
	sprintf(line2, "/bin/echo %s >> %s", line1, IDCMDFILE);
	sys(line2, S_FATAL);
}
