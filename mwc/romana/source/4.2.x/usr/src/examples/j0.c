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

main() {
	extern char *gets();
	double x;
	char string[64];

	for (;;) {
		printf("Enter number: ");
		if (gets(string) == NULL)
			break;
		x = atof(string);

		display(x);
		display(j0(x));
		display(j1(x));
		display(jn(0,x));
		display(jn(1,x));
		display(jn(2,x));
		display(jn(3,x));
	}
	putchar('\n');
}

