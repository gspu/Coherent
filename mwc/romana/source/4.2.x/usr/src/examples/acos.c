#include <errno.h>
#include <math.h>
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
		display(cos(x));
		display(sin(x));
		display(tan(x));
		display(acos(cos(x)));
		display(asin(sin(x)));
		display(atan(tan(x)));
		display(atan2(sin(x),cos(x)));
		display(hypot(sin(x),cos(x)));
		display(cabs(sin(x),cos(x)));
	}
}

