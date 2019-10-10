#include <stdio.h>

main()
{
	int i;
	char *retval;

	i = 0;
	while(-1 != sbrk(102400)) {
		++i;
		putchar('.');
		fflush(stdout);
	}

	printf("%d00K available\n", i);

} /* main () */

