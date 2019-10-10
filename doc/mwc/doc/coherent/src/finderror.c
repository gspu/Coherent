#include <stdio.h>

main()
{
	char buffer[100];
	int i;

	for (i = 0; gets(buffer) != NULL; i++)
		if (buffer[0] != '+' && buffer[0] != '%')
			printf("%d: %s\n", i, buffer);
}
