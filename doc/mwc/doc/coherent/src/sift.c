/* sift out sequences of the form "B^HB" or "_^HB", as output by nroff */

#include <stdio.h>

main()
{
	char buffer1[500], buffer2[500], *ptr1, *ptr2;

	while (gets(buffer1) != NULL) {
		ptr1 = buffer1;
		ptr2 = buffer2;

		while (1) {
			if (*ptr1 == '\0') {
				*ptr2 = '\0';
				printf("%s\n", buffer2);
				break;
			}

			if (*ptr1 == '\b') {
				ptr2--;
				ptr1++;
				continue;
			}

			*ptr2 = *ptr1;
			ptr1++;
			ptr2++;
		}
	}
}
