/*
 * File:	nlp.c
 *
 * Purpose:	find next lower prime from a number
 *		Only use for this module now is calculating NHASH given NBUF.
 *
 *		Compile -DTEST=1 for test version.
 *
 * Revised: Wed Aug  4 12:44:29 1993 CDT
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <common/_tricks.h>
#include <common/ccompat.h>
#include <common/xdebug.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#if TEST
#include <stdio.h>
#define TESTPRINT(args)		(printf args)
int main  __PROTO((void));
#else
#define TESTPRINT(args)
#endif

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
unsigned int		nlp		__PROTO ((unsigned int num));

__LOCAL__ int		isprime		__PROTO ((unsigned int num));
__LOCAL__ unsigned int	isqrt		__PROTO ((unsigned int num));

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/************************************************************************
 * nlp
 *
 * Return the next prime strictly less than the given argument.
 * Return 0 if the argument is 0, 1, or 2.
 ***********************************************************************/
#if __USE_PROTO__
unsigned int nlp (unsigned int num)
#else
unsigned int
nlp (num)
unsigned int num;
#endif
{
	unsigned int ret;

	switch(num) {
	case 0:
	case 1:
	case 2:
		ret = 0;
		break;
	default:
		for (num--;  num > 2; num--) {
			if (isprime(num)) {
				break;
			}
		}
		ret = num;
	}

	return ret;
}

/************************************************************************
 * isprime
 *
 * Return nonzero if the argument is prime, else return 0.
 * Return 0 if the argument is 0 or 1.
 ***********************************************************************/
#if __USE_PROTO__
__LOCAL__ int isprime (unsigned int num)
#else
__LOCAL__ int
isprime (num)
unsigned int num;
#endif
{
	int ret = 1;
	unsigned int nsqrt = isqrt(num);
	unsigned int f;

	switch(num) {
	case 0:
	case 1:
		ret = 0;
		break;
	default:
		if ((num & 1) == 0)
			ret = 0;
		else for (f = 3;  f <= nsqrt;  f++) {
			if ((num % f) == 0) {
				ret = 0;
				break;
			}
		}
	}

	TESTPRINT(("isprime(%u) = %d\n", num, ret));

	return ret;
}

/************************************************************************
 * isqrt
 *
 * Return the smallet integer greater than or equal to the square root
 * of the argument.
 ***********************************************************************/
#if __USE_PROTO__
__LOCAL__ unsigned int isqrt (unsigned int num)
#else
__LOCAL__ unsigned int
isqrt (num)
unsigned int num;
#endif
{
	unsigned int ret;
	int guess;
	int guess2;
	int loopct;

	switch(num) {
	case 0:
	case 1:
		loopct = 0;
		ret = num;
		break;
	default:
		/* Newton's method, adapted for integers. */
		guess = num >> 1;

		/* Use binary to get quick first guess. */
		guess = 1 << (__MOST_BIT_UINT(num) / 2);

		for (loopct = 0;  loopct < 32;  loopct++) {
			guess2 = num / guess;

			TESTPRINT(("g1 = %u  g2 = %u\n", guess, guess2));

			if (guess >= guess2) {
				ret = guess;
				if (guess - guess2 <= 1) {
					break;
				}
			} else {
				ret = guess2;
				if (guess2 - guess <= 1) {
					break;
				}
			}
			guess = (guess2 + guess) / 2;
		}
		if (ret * ret < num)
			ret++;
	}

	TESTPRINT(("isqrt(%d) = %d  loopct = %d\n", num, ret, loopct));

	return ret;
}

#if TEST
#if __USE_PROTO
int main(void)
#else
int main()
#endif
{
	unsigned int num;

	for (;;) {
		printf("enter a number: ");
		if (scanf("%u", &num))
			printf("Next lower prime from %u is %u\n",
			  num, nlp(num));
		else
			break;
	}

	return 0;
}
#endif /* TEST */
