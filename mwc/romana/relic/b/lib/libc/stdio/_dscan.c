/*
 * libc/stdio/_dscan.c
 * ANSI-compliant C standard i/o library internals.
 * _dscan(), _dassign()
 * ANSI 4.9.6.2.
 * Floating point support for _scanf().
 * Kept in a separate source to allow scanf() et al. to link
 * without floating point support.
 */

#include <stdio.h>
#include <stdlib.h>

#if	0
#include <locale.h>
#else
#define	_decimal_point	'.'
#endif

/* Compile-time options. */
#define	LONGDOUBLE	0	/* iff sizeof(double) != sizeof(long double) */

/*
 * Read an ASCII floating point number
 * containing at most 'width' characters from 'fp' into 'buf'.
 * Convert the result using strtod() and store it through *dp.
 * Return the number of characters read.
 * Use the finite state machine shown below.  In the diagram,
 *	states are in (parens)
 *	inputs are in [brackets]
 * For all inputs not shown, transition is to (end) state.
 *                                 --
 *                               /    \
 *                              v      \
 *                ----------> [0-9] -> (3)
 *              /               ^       | \
 *             /               /        |  \
 *            /               /         v   \
 *           /               /         [.]   \               ->[+-] -
 *          /               /           |     \             /         \
 *         /               /            |      \           /           \
 *        /               /             v       v         /             v
 *     (0) --> [+-] -> (1)             (4) --> [Ee] -> (6) -> [0-9] -> (7)
 *        \           /               /         ^               ^      /
 *         \         /               /         /                 \    /
 *          \       /               /         /                    --
 *           \     /               /         /
 *            \   /               /         /
 *             \ /               /         /
 *              v               v         /
 *             [.] --> (2) -> [0-9] -> (5)
 *                              ^      /
 *                               \    /
 *                                 --
 * This cannot be done correctly with the single character pushback
 * guaranteed by ungetc(); for example, rejecting "+y", "1Ex" or "1E+x"
 * requires two or three character pushback.
 */
int
_dscan(buf, fp, width, dp) register char *buf; FILE *fp; int width; double *dp;
{
	register int c, state, count;
	register char *cp;

	cp = buf;
	for (state = count = 0; count < width; ++count) {
		*cp++ = c = getc(fp);
		switch (c) {
		case '+':
		case '-':
			if (state != 0 && state != 6)
				break;
			state++;
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (state == 0 || state == 1 || state == 3)
				state = 3;
			else if (state == 2 || state == 4 || state == 5)
				state = 5;
			else if (state == 6 || state == 7)
				state = 7;
			else
				break;
			continue;
		case 'E':
		case 'e':
			if (state < 3 || 5 < state)
				break;
			state = 6;
			continue;
		default:
			if (c != _decimal_point)
				break;
			if (state <= 1)
				state = 2;
			else if (state == 3)
				state++;
			else
				break;
			continue;
		}
		--cp;
		ungetc(c, fp);
		break;
	}
	*cp = '\0';
	*dp = strtod(buf, (char **)NULL);
	return ((int)(cp - buf));
}

/*
 * Assign *dp through *vp, width depending on flag.
 */
void
_dassign(vp, dp, flag) __VOID__ *vp; double *dp; int flag;
{
#if	LONGDOUBLE
	if (flag == 'L')
		*(long double *)vp = (long double)*dp;
	else if (flag == 'l')
#else
	if (flag == 'L' || flag == 'l')
#endif
		*(double *)vp = *dp;
	else
		*(float *)vp = (float)*dp;
}

/* end of libc/stdio/_dscan.c */
