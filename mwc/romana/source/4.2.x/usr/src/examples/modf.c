#include <stdio.h>

main()
{
	extern char *gets();
	extern double modf(), atof();
	double real, fp, *ip;
	char string[64];

	for (;;) {
		printf("Enter number: ");
		if (gets(string) == 0)
			break;

		real = atof(string);
		fp = modf(real,ip);
		printf("%lf is the integral part of %lf\n",
			*ip, real);
		printf("%lf is the fractional part of %lf\n",
			fp, real);
	}
	putchar('\n');
}

