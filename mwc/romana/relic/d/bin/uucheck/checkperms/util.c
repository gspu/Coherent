/* util.c - handy utility functions.
 * part of uucheck.
 */
#include <stdio.h>
#include "checkperms.h"

/* Return a pointer to the basename of a full path (everthing to the
 * right of the rightmost '/'.
 */
char *
basename(path)
	char *path;
{
	char *retval;

	if ((retval = (char *) strrchr(path, '/')) == NULL) {
		retval = path;
	} /* if there ain't no slash */
	else {
		++retval; /* We want the first character AFTER '/'.  */
	}

	return(retval);
} /* basename() */

/* Copy a string into a new block of memory, free'ing anything being
 * overwritten.
 */
char *
copy_str(dest, src)
	char **dest, *src;
{
	if (*dest != NULL) {
		free(*dest);
	} /* if destination not already empty */

	if ((*dest = malloc(strlen(src) + 1)) == NULL) {
		FATAL("line %d:  Insufficient memory.\n", lineno);
	} /* if malloc failed */

	return(strcpy(*dest, src));
}
