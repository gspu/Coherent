/*
 * Definition of pi.
 * Used for large signs and funny
 * arctangents.
 */
pi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798

/*
 * exp(x) -- exponential function
 * to the scale that is in effect
 * at the time of the call.
 */
define	exp(x) {
	auto i, num, den, term, result;

	num = den = result = 1;
	for (i=1; ; i++) {
		num *= x;
		den *= i;
		if ((term = num/den) == 0)	return (result);
		result += term;
	}
}

/*
 * ln(x) -- natural logarithm
 */
define	ln(x) {
	auto i, xp1, xm1, num, den, term, result;

	if (x <= 0)	return (0);
	xp1 = x+1;
	xm1 = x-1;
	num = 2*xm1;
	den = xp1;
	result = num/den;
	for (i=3; ; i+=2) {
		num *= xm1*xm1;
		den *= xp1*xp1;
		if ((term = num/(i*den)) == 0)	return (result);
		result += term;
	}
}

/*
 * sin(x) -- sine function.
 */
define	sin(x) {
	auto i, num, den, term, result, sign, pi2, minusx2;

	sign = den = 1;
	pi2 = 2*pi;
	if (x < 0) {
		sign = -1;
		x = -x;
	}
	while (x > pi2)		x -= pi2;
	num = result = x;
	minusx2 = -x*x;
	for (i=3; ; i+=2) {
		num *= minusx2;
		den *= (i-1)*i;
		if ((term = num/den) == 0)	return (sign*result);
		result += term;
	}
}

/*
 * cos(x) -- cosine function.
 */
define	cos(x) {
	auto i, num, den, term, result, pi2, minusx2;

	pi2 = 2*pi;
	if (x < 0)	x = -x;
	while (x > pi2)		x -= pi2;
	num = den = result = 1;
	minusx2 = -x*x;
	for (i=2; ; i+=2) {
		num *= minusx2;
		den *= i*(i-1);
		if ((term = num/den) == 0)	return (result);
		result += term;
	}
}

/*
 * atan(x) -- arctangent function of x
 */
define	atan(x) {
	auto i, sign, inverse, num, den, term, x2, x2p1, result;

	sign = 1;
	inverse = 0;
	if (x < 0) {
		sign = -1;
		x = -x;
	}
	if (x > 1) {
		inverse = 1;
		x = 1/x;
	}
	x2 = x*x;
	x2p1 = x2+1;
	result = num = den = 1;
	for (i=2; ; i+=2) {
		num *= x2*i;
		den *= x2p1*(i+1);
		if ((term = num/den) == 0) {
			result = x*result/x2p1;
			if (inverse != 0)	result = pi/2 - result;
			return (sign * result);
		}
		result += term;
	}
}

/*
 * Bessel functions of first kind of
 * integer order `n' for `x'.
 */
define	j(n, x)
{
	auto i, num, den, result;

	if (n%1 != 0)	return (0);
	den = 2;
	num = 1;
	if (n > 0) {
		for (i=0; i<n; i++)	num *= x;
	} else if (n < 0) {
		for (i=0; i<-n; i++)	den *= x;
	}
	return (0);
}
