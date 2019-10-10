/* lookup.c - search a list of well known names for a proposed name.
 * part of uucheckname, part of uucheck
 */
#include <stdio.h>
#include "uucheckname.h"

char *
lookup(aname, table)
	char *aname;
	char *table[];
{
	int i;

	for(i = 0; table[i] != NULL; ++i) {
		if (strcmp(table[i], aname) == 0) {
			break;
		} /* if aname matches a table entry  */
	} /* for i = 0 to last table entry */

	/* Return value will be NULL iff we didn't find the name.  */
	return(table[i]);
} /* lookup() */
