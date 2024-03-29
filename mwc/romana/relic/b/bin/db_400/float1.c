/* float1.c - test of floating point */

#include <stdio.h>

#if 0
#define FORCE(var,val)	 {*(int *)(&var) = val;dumpl("FORCE ",var);}
#else
#define FORCE(var,val)
#endif

main()
{
	float x = 1.0, y = 2.0, z;

	xpr(&x);
	dumpl("x = ", x);
	FORCE(x, 0x3F800000);
	xpr(&y);
	dumpl("y = ", y);
	FORCE(y, 0x40000000);
	fflush(stdout);

	z = x + y;
	xpr(&z);
	dumpl("z = ", z);
	FORCE(z, 0x40400000);
	fflush(stdout);

	printf("%f + %f = %f\n", x, y, z);
	printf("that was formatted %%f output\n");
	fflush(stdout);
}

/* Dump a double as 16 hex digits */
dumpl(name,flo,fhi)
char * name;
int flo, fhi;
{
	int exponent;
	int schi;
	char sgn;

	printf("%s%08x,%08x  ", name, fhi, flo);

	sgn = (fhi & 0x80000000) ? '-' : '+';
	exponent = (fhi >> 20) & 0x7FF;
	exponent -= 1023;
	schi = fhi & 0xFFFFF;

	printf("%c1.%05x,%08xx2**%d\n", sgn, schi, flo, exponent);
}

/* dump a float as 8 hex digits - called with address of the float */
xpr(fa)
int * fa;
{
	printf("%08x  ", *fa);
}
