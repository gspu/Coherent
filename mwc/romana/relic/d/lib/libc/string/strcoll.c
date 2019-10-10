/*
 * strcoll.c
 * ANSI 4.11.4.3.
 * Lexicographic comparison of two strings, locale-specific.
 * This version is identical to strcmp().
 */

#include <string.h>

int strcoll(s1, s2) register char *s1, *s2;
{
	return(strcmp(s1, s2));
}
