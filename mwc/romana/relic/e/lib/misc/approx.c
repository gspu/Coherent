/*
 * return 1 if args are within epsilon else 0
 */
double epsilon = 2.3e-16;

approx(a, b)
double a, b;
{
	double e;

	if (0.0 > (e = epsilon * b))
		e = -e;

	if (0.0 > (a -= b))
		a = -a;

	return (a <= e);
}
