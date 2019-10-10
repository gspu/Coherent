#include <math.h>
#include <errno.h>
#include <stdio.h>
#define display(x) dodisplay((double)(x), #x)

dodisplay(value, name)
double value; char *name;
{
	if (errno) 
		perror(name);
	else 
		printf("%10g %s\n", value, name);
	errno = 0;
}

main()
{
	extern char *gets();
	double x;
	char string[64];

	for (;;) {
		printf("Enter number: ");
		if (gets(string) == NULL)
			break;
		x = atof(string);
		display(x);
		display(cosh(x));
		display(sinh(x));
		display(tanh(x));
	}
	putchar('\n');
}

