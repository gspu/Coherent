#include <stdio.h>
#include "mprec.h"


/*
 *	Mout writes on stdout the value of the mint pointed to by "a"
 *	in base "obase".
 */

void
mout(a)
mint *a;
{
	register char *res;
	char *mark;
	register int len;

	mark = res = mtos(a);
	for (len = strlen(res) / 72; len-- > 0; res += 72)
		printf("%.72s\\\n", res);
	printf("%s", res);
	mpfree(mark);
}
