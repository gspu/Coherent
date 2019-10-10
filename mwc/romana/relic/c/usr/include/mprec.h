/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 *	Definitions of all user multi-precision types, macros and
 *	not-int returning functions.
 */

#ifndef	MPREC_H
#define	MPREC_H	MPREC_H

#ifndef NULL
#define	NULL		((char *)0)
#endif
#define TRUE		(0 == 0)
#define FALSE		(0 != 0)
#define	BASE		128
#define	L2BASE		7
#define	NEFL		(BASE - 1)	/* leading byte of negative numbers */
#define	NORSIZ		4		/* maximum size for int value */


/*
 *	Multi-precision integer type.
 */

typedef struct {
	unsigned len;
	char *val;
} mint;


/*
 *	Minit initializes a mint so that it may be used by the other
 *	multi-precision routines.  This simply consists of setting the
 *	val field to NULL so that mpfree will know that there is nothing
 *	to free.  Note that the mint so initialized has no value and
 *	should not be used as an operand, but may be used as the result
 *	of any of the multi-precision routines.
 */

#define	minit(mp)	(mp)->val = NULL


/*
 *	Mvfree frees the value associated to a pointer to a mint.
 *	Note that it does not re-initialize the mint.
 */

#define	mvfree(mp)	mpfree((mp)->val)


/*
 *	Functions returning non-ints.
 */

void	gcd(),		/* greatest common divisor of 2 mints */
	madd(),		/* add 2 mints */
	mcopy(),	/* copy mint */
	mdiv(),		/* divide 2 mints */
	min(),		/* read in mint from stdin */
	mintfr(),	/* free all space used by a mint */
	mitom(),	/* set a mint to the value of an int */
	mneg(),		/* negate a mint */
	mout(),		/* write out mint onto stdout */
	mperr(),	/* print error on stdout and exit */
	mpfree(),	/* free space allocated by mpalc */
	msma(),		/* shift-multiply-add inplace */
	msqrt(),	/* square root of mint */
	msub(),		/* subtract 2 mints */
	mult(),		/* multiply 2 mints */
	norm(),		/* normalize a mint */
	pow(),		/* raise mint to a mint power mod a mint */
	rpow(),		/* raise mint to a mint power */
	sdiv(),		/* divide a mint by a char */
	smult(),	/* multiply a mint by a char */
	spow(),		/* raise mint to an unsigned power */
	xgcd();		/* extended greatest common divisor */
char	*mpalc(),	/* allocate space */
	*mtos();	/* convert mint to string */
mint	*itom();	/* allocate space for mint initialized to int */


	/* external variables */

extern mint	*mzero,		/* a mint which has a value of zero */
		*mone,		/* a mint which has a value of one */
		*mminint,	/* min value that fits in an int */
		*mmaxint;	/* max value that fits in an int */
extern int	ibase,		/* input base (2 <= ibase <= 16 assumed) */
		obase;		/* output base (2 <= obase <= 16 assumed) */

#endif
