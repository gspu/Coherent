#include <stdio.h>

main()
{
	register char *ptr, *ptr2;
	extern char *calloc(), *realloc();
	unsigned count, size;

	count = 4;
	size = sizeof(char *);

	if ((ptr = calloc(count, size)) != NULL)
		printf("%u blocks of size %u calloced\n",
		        count, size);
	else
		printf("Insuff. memory for %u blocks of size %u\n",
		     count, size);

	if ((ptr2 = realloc(ptr,(count*size) + 1)) != NULL)
		printf("1 block of size %u realloced\n",
			(count*size)+1);
}
