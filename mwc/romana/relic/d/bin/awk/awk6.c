/*
 * AWK -- built-in functions.
 * The assumption is that awk will
 * not call the functions if there
 * is an argument mismatch to
 * remove some checking from each of
 * the common single level routines.
 */

#include "awk.h"
#include <math.h>
#include <sys/mdata.h>

NODE *
f_length(np, na)
NODE *np;
int na;
{
	register CHAR *s;

	if (na == 0)
		s = inline; else
		s = evalstring(np);
	return (inode((INT)strlen(s)));
}

/*
 * Square root function.
 */
NODE *
f_sqrt(np, na)
register NODE *np;
int na;
{
	return (fnode((FLOAT)sqrt((double)evalfloat(np))));
}

/*
 * Natural logarithm function.
 */
NODE *
f_log(np, na)
register NODE *np;
int na;
{
	return (fnode((FLOAT)log((double)evalfloat(np))));
}

/*
 * Exponential function.
 */
NODE *
f_exp(np, na)
register NODE *np;
int na;
{
	return (fnode((FLOAT)exp((double)evalfloat(np))));
}

/*
 * Convert FLOAT to INT
 */
NODE *
f_int(np, na)
register NODE *np;
int na;
{
	return (inode((INT)evalfloat(np)));
}

/*
 * Substring function...
 * substr(string, n, m)
 * If `m' is missing, it is infinity.
 * Return the string starting at position
 * `n' (origin 1) of `string' for `m'
 * (or end of string) characters.
 */
NODE *
f_substr(np, na)
NODE *np;
int na;
{
	register CHAR *cp;
	register CHAR *ocp;
	register unsigned m;
	register unsigned n;
	register CHAR *acp;

	m = MAXUINT;
	if (na > 2)
		m = evalint(fargn(np, 3));
	n = evalint(fargn(np, 2));
	cp = evalstring(fargn(np, 1));
	while (--n != 0)
		if (*cp == '\0')
			break;
		else
			cp++;
	n = strlen(cp);
	if (n > m)
		n = m;
	acp = ocp = xalloc(n + sizeof(CHAR));
	while (n--)
		*ocp++ = *cp++;
	*ocp = '\0';
	return (snode(acp, T_ALLOC));
}

/*
 * String index match function...
 * index(s1, s2)
 * Return the position (origin 1) where `s2'
 * is found in string `s1' or 0.
 */
NODE *
f_index(np, na)
register NODE *np;
int na;
{
	register CHAR *s1, *s2;
	register CHAR *ss1;
	register unsigned n;

	ss1 = s1 = evalstring(fargn(np, 1));
	s2 = evalstring(fargn(np, 2));
	n = strlen(s2);
	for ( ; *s1 != '\0'; s1++)
		if (strncmp(s1, s2, n) == 0)
			return (inode((INT)(s1-ss1+1)));
	return (&xzero);
}

/*
 * Printf onto a string function.
 * Handled by special case in the `printf'
 * keyword.
 */
NODE *
f_sprintf(np, na)
register NODE *np;
int na;
{
	register CHAR *ap;

	xprintf(np, NULL, wordbuf);
	ap = xalloc(strlen(wordbuf) + sizeof(CHAR));
	strcpy(ap, wordbuf);
	fsmapinit(FS);
	return (snode(ap, T_ALLOC));
}

/*
 * Function to split a string into the standard
 * fields.
 * n = split(string, array, sep)
 * If `sep' is missing, FS is used.
 * `string' is split into fields into
 * `array[1]', `array[2]', ..., `array[n]'
 * and the number of fields (`n') is
 * returned.
 */
NODE *
f_split(np, na)
NODE *np;
int na;
{
	register CHAR *cp;
	register int c;
	register CHAR *scp;
	register CHAR *acp;
	NODE *array;
	NODE *index;
	STRING string;

	index = inode((INT)0);
	string = evalstring(fargn(np, 1));
	array = fargn(np, 2);
	if (array->t_op!=ATERM || (array->t_flag&T_VAR)==0)
		awkerr("Split not given an array");
	if (na >= 3)
		fsmapinit(evalstring(fargn(np, 3)));

	for (cp = string; *cp;) {
		for (scp = cp; (c = *cp) && !FSMAP[c]; cp++) /* find the end */
			;
		acp = string = xalloc(1 + cp - scp); /* get space to store */
		while (scp < cp)		     /* copy */
			*acp++ = *scp++;
		*acp = '\0';
		index->t_INT++;
		xassign(xarray(array, index), snode(string, T_ALLOC));

		if (c == '\0')
			break;

		cp++;
		if (whitesw)	/* pass further the delimeters */
			while (FSMAP[*cp])
				cp++;
	}
	fsmapinit(FS);
	return (index);
}

/*
 * Absolute value function.
 */
NODE *
f_abs(np, na)
register NODE *np;
register int na;
{
	FLOAT f;

	if ((f = evalfloat(fargn(np, 1))) < 0)
		f = -f;
	return (fnode(f));
}

/*
 * Return the number
 * of args to a function.
 */
fnargs(np)
register NODE *np;
{
	register unsigned nargs;

	for (nargs = 0; np != NULL; nargs++)
		if (np->n_op == ALIST)
			np = np->n_O2;
		else
			np = NULL;
	return (nargs);
}

/*
 * Return the n-th argument based on the
 * list.  If it is out of range, return
 * a dummied up node.
 */
NODE *
fargn(np, an)
register NODE *np;
register unsigned an;
{
	register NODE *rnp;

	rnp = NULL;
	while (an--!=0 && np!=NULL)
		if (np->n_op == ALIST) {
			rnp = np->n_O1;
			np = np->n_O2;
		} else {
			rnp = np;
			np = NULL;
		}
	if (rnp == NULL)
		rnp = snode(SNULL, 0);
	return (rnp);
}
