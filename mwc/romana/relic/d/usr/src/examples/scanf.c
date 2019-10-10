#include <stdio.h>

main()
{
	int left, right;

	printf("No. of fingers on your left hand:  ");
	/* force message to appear on screen */
	fflush(stdout);
	scanf("%d", &left);

	/* eat newline char */
	while(getchar() != '\n');

	printf("No. of fingers on your right hand:  ");
	fflush(stdout);
	scanf("%d", &right);

	/* again, eat newline */
	while(getchar() != '\n');

	printf("You've %d left fingers, %d right, & %d total\n",
		left, right, left+right);
}
