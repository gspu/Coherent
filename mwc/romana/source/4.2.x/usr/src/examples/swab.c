#include <stdio.h>

main()
{
	int word;

	printf("Enter an integer: \n");
	scanf("%d", &word);
	printf("The word is 0x%x\n", word);
	swab(&word, &word, 2);
	printf("The word with bytes swapped is 0x%x\n", word);
}

