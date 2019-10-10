#include <stdio.h>

main()
{
	extern char *gets();
	extern double frexp(), atof();
	double real, fraction;
	int ep;

	char string[64];

	for (;;) {
		printf("Enter number: ");
		if (gets(string) == NULL)
			break;

		real = atof(string);
		fraction = frexp(real, &ep);
		printf("%lf is the fraction of %lf\n",
			fraction, real);
		printf("%d is the binary exponent of %lf\n",
			ep, real);
	}
	putchar('\n');
}

