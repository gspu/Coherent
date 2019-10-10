/*
 * n1/i386/table0.c
 * Machine specific tables used by code generator tree modification routines.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * Information about the subgoals of the various C operators.
 * Used by the leaf insert code in mtree3.c.
 * Indexed by ldtab[op-MIOBASE].
 */
int	ldtab[]	= {

	ndown(MRVALUE,	MRADDR),	/* +  */
	ndown(MRVALUE,	MRADDR),	/* -  */
	ndown(MRVALUE,	MRADDR),	/* *  */
	ndown(MRVALUE,	MRADDR),	/* /  */
	ndown(MRVALUE,	MRADDR),	/* %  */
	ndown(MHARD,    MRADDR),	/* &  */
	ndown(MRVALUE,	MRADDR),	/* |  */
	ndown(MRVALUE,	MRADDR),	/* ^  */
	ndown(MRVALUE,	MHARD),		/* << */
	ndown(MRVALUE,	MHARD),		/* >> */

	ndown(MLADDR,	MRVALUE),	/* +=  */
	ndown(MLADDR,	MRVALUE),	/* -=  */
	ndown(MLADDR,	MRVALUE),	/* *=  */
	ndown(MLADDR,	MRVALUE),	/* /=  */
	ndown(MLADDR,	MRVALUE),	/* %=  */
	ndown(MLADDR,	MRVALUE),	/* &=  */
	ndown(MLADDR,	MRVALUE),	/* |=  */
	ndown(MLADDR,	MRVALUE),	/* ^=  */
	ndown(MLADDR,	MHARD),		/* <<= */
	ndown(MLADDR,	MHARD),		/* >>= */

	ndown(MRVALUE,	MRADDR),	/* == */
	ndown(MRVALUE,	MRADDR),	/* != */
	ndown(MRVALUE,	MRADDR),	/* >  */
	ndown(MRVALUE,	MRADDR),	/* >= */
	ndown(MRVALUE,	MRADDR),	/* <= */
	ndown(MRVALUE,	MRADDR),	/* <  */
	ndown(MRVALUE,	MRADDR),	/* >  unsigned */
	ndown(MRVALUE,	MRADDR),	/* >= unsigned */
	ndown(MRVALUE,	MRADDR),	/* <= unsigned */
	ndown(MRVALUE,	MRADDR),	/* <  unsigned */

	ndown(MRADDR,	MJUNK),		/* * */
	ndown(MLADDR,	MJUNK),		/* & */
	ndown(MRVALUE,	MJUNK),		/* - */
	ndown(MRVALUE,	MJUNK),		/* ~ */
	ndown(MFLOW,	MJUNK),		/* ! */
	ndown(MFLOW,	MPASSED),	/* ? */
	ndown(MPASSED,	MPASSED),	/* : */
	ndown(MLADDR,	MRADDR),	/* ++ prefix  */
	ndown(MLADDR,	MRADDR),	/* -- prefix  */
	ndown(MLADDR,	MRADDR),	/* ++ postfix */
	ndown(MLADDR,	MRADDR),	/* -- postfix */
	ndown(MEFFECT,	MPASSED),	/* , */
	ndown(MLADDR,	MFNARG),	/* Call */
	ndown(MFLOW,	MFLOW),		/* && */
	ndown(MFLOW,	MFLOW),		/* || */
	ndown(MRADDR,	MRADDR),	/* Type cast */
	ndown(MRADDR,	MRADDR),	/* Conversion */
	ndown(MLADDR,	MRADDR),	/* Field select */
	ndown(MJUNK,	MJUNK),		/* Sizeof */
	ndown(MLADDR,	MRVALUE),	/* = */
	ndown(MJUNK,	MJUNK),		/* Nop */
	ndown(MJUNK,	MJUNK),		/* Init type */
	ndown(MFNARG,	MFNARG),	/* Argument list */
	ndown(MRVALUE,	MJUNK),		/* Leaf */
	ndown(MRVALUE,	MJUNK),		/* Fix up */
	ndown(MRVALUE,	MRVALUE)	/* Block move */
};

/* end of n1/i386/table0.c */
