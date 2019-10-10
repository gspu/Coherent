/*
 * Create a new copy of a string.
 */
#include "misc.h"
extern char *strcpy();

char *
newcpy(s)
register char *s;
{
    if (NULL == s)
    	return (NULL);
    return (strcpy(alloc(strlen(s) + 1), s));
}
