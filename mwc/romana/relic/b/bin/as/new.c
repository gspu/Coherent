/*
 * Turn table.386 into a better form.
 */
#include <misc.h>
#include <string.h>
#include <ctype.h>

main()
{
	char buf[100];

	while (NULL != gets(buf)) {
		int x;
		char c, *p, y[10];

		if ('#' == buf[0]) {
			puts(buf);
			continue;
		}
		sscanf(buf, "%x", &x);
		y[0] = '\0';

		for (p = buf; c = *p; p++)
			if (' ' == c || '\t' == c)
				break;

		if (!x) 	/* General op code */
			strcpy(y, "G");
		else {
#define foo(n, c) if (n & x) strcat(y, #c);
		/* flag one is trashed */
			foo(0x02, w)
			foo(0x04, d)
			foo(0x08, m)
			foo(0x10, a)
			foo(0x20, p)
			foo(0xC0, P);	/* lumped */
					/* 100 eliminated */
			foo(0x200, t)
			foo(0x400, i)
			foo(0x800, r)
			foo(0x1000, R)
			foo(0x2000, l)
			foo(0x4000, L)
			foo(0x8000, X)
#undef foo
			if (!y[0])
				strcpy(y, "-");
		}
		printf("%-4s%s\n", y, p);
	}
}
	
