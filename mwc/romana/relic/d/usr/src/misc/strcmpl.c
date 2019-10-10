/*
 * Case insensative string compare.
 */
strcmpl(s1, s2)
register unsigned char *s1, *s2;
{
    register c, d;

    for (;;) {
	switch(c = *s1++ - (d = *s2++)) {
	case 0:
		if(!d)
			break;
		continue;
	case ('A' - 'a'):
		if((d < 'a') || (d > 'z'))
			break;
		continue;
	case ('a' - 'A'):
		if((d < 'A') || (d > 'Z'))
			break;
		continue;
	}
	return(c);
    }
}

#ifdef TEST
#include "misc.h"
main()
{
    char s1[40], s2[40];

    for (;;) {
	ask(s1, "string1");
	ask(s2, "string2");
	printf("strcmpl(%s, %s) == %d\n", s1, s2, strcmpl(s1, s2));
    }
}
#endif
