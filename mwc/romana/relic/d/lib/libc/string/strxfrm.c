/*
 * strxfrm.c
 * ANSI 4.11.4.5
 * Transform string into locale-specific collating sequence.
 * This version just returns the original string.
 */

#include <string.h>

size_t strxfrm(s1, s2, n) char *s1, *s2; size_t n;
{
	strncpy(s1, s2, n);
	return (strlen(s2));
}
