/*
 * return 1 if args are within epsilon else 0
 */

double epsilon = 1.0e-20;

approx(a, b)
double a, b;
{
	if (0 > (a -= b))
		a = -a;
	return (a <= epsilon);
}
