/*
 * This header file contains definitions used by
 * the SMALL and LARGE model C compiler for the Intel iAPX-86.
 * mch.h has set either IEEE or DECVAX to 1.
 * Both formats use 8 byte double constants with the sign bit in the same place.
 *
 * Some definitions for the portable floating point input code.
 * Used by the multiprecision integer package.
 */

#define	NBIG	8			/* Loop limit, etc */
typedef	unsigned char	BIG[NBIG];	/* Unsigned, holds 64 bits */
#define	t5ne(n)	((n[0]&0370)!=0)	/* Check if top 5 bits clear */
#define	cbit(n)	(((n)>>8)&0377)		/* Get 'carry' for addition */
#define	MSBBIG	0200			/* Top bit of each thing */

#if	IEEE
#define	IROUND	6			/* IEEE format */
#define	BROUND	010			/* 52 bits */
#define	EXPBIAS	1023			/* binary exponent bias */
#define	EXPMAX	2046			/* maximum biased binary exponent */
#else
#define	IROUND	6			/* DEC VAX-11 format */
#define	BROUND	01			/* 55 bits */
#define	EXPBIAS	129			/* binary exponent bias */
#define	EXPMAX	255			/* maximum biased binary exponent */
#endif

/* This macro determines which bit field base types are promoted */
#define doalign(t)	0
