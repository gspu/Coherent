#include <stdio.h>
main()
{
	extern char *gets();
	extern int atoi();
	char string[64];
	for (;;) {
		printf("Enter numeric string: ");
			if (NULL != gets(string))
				printf("%d\n", atoi(string));
			else
				break;
	}
	putchar('\n');
}
