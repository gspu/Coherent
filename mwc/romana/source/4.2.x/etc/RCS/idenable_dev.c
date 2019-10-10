head	1.1;
access
	hal
	udo
	ljg
	jrd
	nigel
	fred;
symbols
	r12:1.1;
locks; strict;
comment	@ *@;


1.1
date	93.09.01.20.00.29;	author rcsadmin;	state Exp;
branches;
next	;


desc
@@


1.1
log
@COHERENT r12
@
text
@/*
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
@
