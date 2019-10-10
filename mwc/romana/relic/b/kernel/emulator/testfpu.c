/*
 * Test the linnux FPU-emu
 */
#include <misc.h>
#include <math.h>
#include <signal.h>

extern void math_emulate();

main()
{
	double a = 1.5, b = 0.5;

	signal(SIGFPE, math_emulate);
	a += b;
	xdump(&a, sizeof(a));
}
