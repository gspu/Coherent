#include <errno.h>
#include <math.h>
#include <stdio.h>

void show(value, name)
double value; char *name;
{
	if (errno)
		perror(name);
	else
		printf("%10g %s\n", value, name);
	errno = 0;
}

#define display(x) show((double)(x), #x)

main()
{
	extern char *gets();
	double	x;
	char string[64];

	for (;;) {
		printf("Enter a number: ");
		fflush(stdout);
	if (gets(string) == NULL)
		break;
	x = atof(string);
	display(x);
	display(cos(x));
	display(sin(x));
	display(tan(x));
	display(acos(cos(x)));
	}
	putchar('\n');
}


