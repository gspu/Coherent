/*
 * C compiler.
 * Operator and type conversion
 * information ("dope")  tables.
 */
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * This table, indexed by operator
 * number (less MIOBASE) is used to get the legal
 * left and right operand rules.
 */
short   opdope[] = {

	13 + NSTR+NSTL,					/* + */
	13 + NSTR+NSTL,					/* - */
	14 + NSTR+NSTL+NPTR+NPTL,			/* * */
	14 + NSTR+NSTL+NPTR+NPTL,			/* / */
	14 + NSTR+NSTL+NPTR+NPTL+NFLT,			/* % */
	9  + NSTR+NSTL+NPTR+NPTL+NFLT,			/* & */
	7  + NSTR+NSTL+NPTR+NPTL+NFLT,			/* | */
	8  + NSTR+NSTL+NPTR+NPTL+NFLT,			/* ^ */
	12 + NSTR+NSTL+NPTR+NPTL+NFLT,			/* << */
	12 + NSTR+NSTL+NPTR+NPTL+NFLT,			/* >> */

	2  + NSTR+NSTL+NPTR+ASGN+RLVL+RAS,		/* += */
	2  + NSTR+NSTL+NPTR+ASGN+RLVL+RAS,		/* -= */
	2  + NSTR+NSTL+NPTR+NPTL+ASGN+RLVL+RAS,		/* *= */
	2  + NSTR+NSTL+NPTR+NPTL+ASGN+RLVL+RAS,		/* /= */
	2  + NSTR+NSTL+NPTR+NPTL+NFLT+ASGN+RLVL+RAS,	/* %= */
	2  + NSTR+NSTL+NPTR+NPTL+NFLT+ASGN+RLVL+RAS,	/* &= */
	2  + NSTR+NSTL+NPTR+NPTL+NFLT+ASGN+RLVL+RAS,	/* |= */
	2  + NSTR+NSTL+NPTR+NPTL+NFLT+ASGN+RLVL+RAS,	/* ^= */
	2  + NSTR+NSTL+NPTR+NPTL+NFLT+ASGN+RLVL+RAS,	/* <<= */
	2  + NSTR+NSTL+NPTR+NPTL+NFLT+ASGN+RLVL+RAS,	/* >>= */

	10 + NSTR+NSTL,					/* == */
	10 + NSTR+NSTL,					/* != */
	11 + NSTR+NSTL,					/* > */
	11 + NSTR+NSTL,					/* >= */
	11 + NSTR+NSTL,					/* <= */
	11 + NSTR+NSTL,					/* < */
	0,						/* > unsigned */
	0,						/* >= unsigned */
	0,						/* <= unsigned */
	0,						/* < unsigned */

	0  + NSTL+NFLT,					/* Ind */
	0  + RLVL,					/* Addr */
	0  + NSTL+NPTL,					/* - u */
	0  + NSTL+NPTL+NFLT,				/* ~ */
	0  + RTOL,					/* ! */
	4  + RTOL,					/* ? */
	4  + RAS,					/* : */
	15 + NSTL+RLVL+RAS,				/* ++x */
	15 + NSTL+RLVL+RAS,				/* --x */
	15 + NSTL+RLVL+RAS,				/* x++ */
	15 + NSTL+RLVL+RAS,				/* x-- */
	1,						/* , */
	0  + NSTL,					/* Call */
	6  + RTOL+RTOR,					/* && */
	5  + RTOL+RTOR,					/* || */
	0,						/* Cast of types */
	0,						/* Conversion */
	0,						/* Field marker */
	0  + RAS,					/* Sizeof */
	2  + ASGN+RLVL+RAS,				/* = */
	0,						/* Drain conversions */
	0,						/* Init */
	0,						/* Arglist */
	0,						/* Leaf */
	0,						/* Fix up */
	0						/* Block move */

};

/*
 * Conversions.
 * Indexed by 12*(lt-T_CHAR) + (rt-T_CHAR).
 * Linear types are the same as the abstract
 * types, with all pointers mapped to T_PTR
 * and nothing greater than T_STRUCT.
 * Flags are:
 *      GOAL    Mask to get goal type.
 *      GTR     Goal is same as right.
 *      GTL     Goal is same as left.
 *      CVR     Convert right.
 *      CVL     Convert left.
 *      CVRA    If op is ASGN, do CVR.
 *	CARA	If op is ASGN, do CAST right side.
 *      UREL    Use unsigned relations.
 */
short	cvdope[] = {

	T_INT   + CVL + CVR + CVRA,			/*  c ::  c */
	T_UINT	+ CVL + CVR + CVRA + UREL,		/*  c :: uc */
	T_INT   + CVL + CVR + CVRA,			/*  c ::  s */
	T_UINT  + CVL + CVR + CVRA + UREL,		/*  c :: us */
	T_INT   + CVL,					/*  c ::  i */
	T_UINT  + CVL + CVRA + UREL,			/*  c :: ui */
	GTR     + CVL + CARA + UREL,			/*  c ::  p */
	T_LONG	+ CVL + CVRA,				/*  c ::  l */
	T_ULONG + CVL + CVRA + UREL,			/*  c :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  c ::  f */
	T_DOUBLE+ CVL + CVRA,				/*  c ::  d */
	0,						/*  c ::  v */
	0,						/*  c ::  t */

	T_UINT	+ CVL + CVR + CVRA + UREL,		/* uc ::  c */
	T_UINT	+ CVL + CVR + CVRA + UREL,		/* uc :: uc */
	T_UINT  + CVL + CVR + CVRA + UREL,		/* uc ::  s */
	T_UINT  + CVL + CVR + CVRA + UREL,		/* uc :: us */
	T_UINT  + CVL + CVR + UREL,			/* uc ::  i */
	T_UINT  + CVL + CVRA + UREL,			/* uc :: ui */
	GTR     + CVL + CARA + UREL,			/* uc ::  p */
	T_ULONG + CVL + CVR + CVRA + UREL,		/* uc ::  l */
	T_ULONG + CVL + CVRA + UREL,			/* uc :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/* uc ::  f */
	T_DOUBLE+ CVL + CVRA,				/* uc ::  d */
	0,						/* uc ::  v */
	0,						/* uc ::  t */

	T_INT   + CVL + CVR + CVRA,			/*  s ::  c */
	T_UINT  + CVL + CVR + CVRA + UREL,		/*  s :: uc */
	T_INT   + CVL + CVR + CVRA,			/*  s ::  s */
	T_UINT  + CVL + CVR + CVRA + UREL,		/*  s :: us */
	T_INT   + CVL,					/*  s ::  i */
	T_UINT  + CVL + CVRA + UREL,			/*  s :: ui */
	GTR     + CVL + CARA + UREL,			/*  s ::  p */
	T_LONG  + CVL + CVRA,				/*  s ::  l */
	T_ULONG + CVL + CVRA + UREL,			/*  s :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  s ::  f */
	T_DOUBLE+ CVL + CVRA,				/*  s ::  d */
	0,						/*  s ::  v */
	0,						/*  s ::  t */

	T_UINT  + CVL + CVR + CVRA + UREL,		/* us ::  c */
	T_UINT  + CVL + CVR + CVRA + UREL,		/* us :: uc */
	T_UINT	+ CVL + CVR + CVRA + UREL,		/* us ::  s */
	T_UINT	+ CVL + CVR + CVRA + UREL,		/* us :: us */
	T_UINT  + CVL + CVR + CVRA + UREL,		/* us ::  i */
	T_UINT  + CVL + UREL,				/* us :: ui */
	GTR     + CVL + CARA + UREL,			/* us ::  p */
	T_ULONG + CVL + CVR + CVRA + UREL,		/* us ::  l */
	T_ULONG + CVL + CVRA + UREL,			/* us :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/* us ::  f */
	T_DOUBLE+ CVL + CVRA,				/* us ::  d */
	0,						/* us ::  v */
	0,						/* us ::  t */

	T_INT   + CVR + CVRA,				/*  i ::  c */
	T_UINT  + CVL + CVR + CVRA + UREL,		/*  i :: uc */
	T_INT   + CVR + CVRA,				/*  i ::  s */
	T_UINT	+ CVL + CVR + CVRA + UREL,		/*  i :: us */
	T_INT,						/*  i ::  i */
	T_UINT  + CVL + CVRA + UREL,			/*  i :: ui */
	GTR     + CVL + CARA + UREL,			/*  i ::  p */
	T_LONG  + CVL + CVRA,				/*  i ::  l */
	T_ULONG + CVL + CVRA + UREL,			/*  i :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  i ::  f */
	T_DOUBLE+ CVL + CVRA,				/*  i ::  d */
	0,						/*  i ::  v */
	0,						/*  i ::  t */

	T_UINT  + CVR + CVRA + UREL,			/* ui ::  c */
	T_UINT  + CVR + CVRA + UREL,			/* ui :: uc */
	T_UINT  + CVR + CVRA + UREL,			/* ui ::  s */
	T_UINT  + CVR + CVRA + UREL,			/* ui :: us */
	T_UINT  + CVR + CVRA + UREL,			/* ui ::  i */
	T_UINT  + UREL,					/* ui :: ui */
	GTR     + CVL + CARA + UREL,			/* ui ::  p */
	T_ULONG + CVL + CVR + CVRA + UREL,		/* ui ::  l */
	T_ULONG + CVL + CVRA + UREL,			/* ui :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/* ui ::  f */
	T_DOUBLE+ CVL + CVRA,				/* ui ::  d */
	0,						/* ui ::  v */
	0,						/* ui ::  t */

	GTL     + CVR + CVRA + UREL,			/*  p ::  c */
	GTL     + CVR + CVRA + UREL,			/*  p :: uc */
	GTL     + CVR + CVRA + UREL,			/*  p ::  s */
	GTL     + CVR + CVRA + UREL,			/*  p :: us */
	GTL     + CVR + CVRA + UREL,			/*  p ::  i */
	GTL     + CVR + CVRA + UREL,			/*  p :: ui */
	GTL     + UREL,					/*  p ::  p */
	GTL     + CVR + CVRA + UREL,			/*  p ::  l */
	GTL     + CVR + CVRA + UREL,			/*  p :: ul */
	0,						/*  p ::  f */
	0,						/*  p ::  d */
	0,						/*  p ::  v */
	0,						/*  p ::  t */
		
	T_LONG  + CVR + CVRA,				/*  l ::  c */
	T_ULONG + CVL + CVR + CVRA + UREL,		/*  l :: uc */
	T_LONG  + CVR + CVRA,				/*  l ::  s */
	T_ULONG + CVL + CVR + CVRA + UREL,		/*  l :: us */
	T_LONG  + CVR + CVRA,				/*  l ::  i */
	T_ULONG + CVL + CVR + CVRA + UREL,		/*  l :: ui */
	GTR     + CVL + CARA + UREL,			/*  l ::  p */
	T_LONG,						/*  l ::  l */
	T_ULONG + CVL + CVRA + UREL,			/*  l :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  l ::  f */
	T_DOUBLE+ CVL + CVRA,				/*  l ::  d */
	0,						/*  l ::  v */
	0,						/*  l ::  t */
	
	T_ULONG + CVR + CVRA + UREL,			/* ul ::  c */
	T_ULONG + CVR + CVRA + UREL,			/* ul :: uc */
	T_ULONG + CVR + CVRA + UREL,			/* ul ::  s */
	T_ULONG + CVR + CVRA + UREL,			/* ul :: us */
	T_ULONG + CVR + CVRA + UREL,			/* ul ::  i */
	T_ULONG + CVR + CVRA + UREL,			/* ul :: ui */
	GTR     + CVL + CARA + UREL,			/* ul ::  p */
	T_ULONG + CVR + CVRA + UREL,			/* ul ::  l */
	T_ULONG + UREL,					/* ul :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/* ul ::  f */
	T_DOUBLE+ CVL + CVRA,				/* ul ::  d */
	0,						/* ul ::  v */
	0,						/* ul ::  t */
	
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f ::  c */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f :: uc */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f ::  s */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f :: us */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f ::  i */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f :: ui */
	0,						/*  f ::  p */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f ::  l */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f :: ul */
	T_DOUBLE+ CVL + CVR + CVRA,			/*  f ::  f */
	T_DOUBLE+ CVL + CVRA,				/*  f ::  d */
	0,						/*  f ::  v */
	0,						/*  f ::  t */

	T_DOUBLE+ CVR + CVRA,				/*  d ::  c */
	T_DOUBLE+ CVR + CVRA,				/*  d :: uc */
	T_DOUBLE+ CVR + CVRA,				/*  d ::  s */
	T_DOUBLE+ CVR + CVRA,				/*  d :: us */
	T_DOUBLE+ CVR + CVRA,				/*  d ::  i */
	T_DOUBLE+ CVR + CVRA,				/*  d :: ui */
	0,						/*  d ::  p */
	T_DOUBLE+ CVR + CVRA,				/*  d ::  l */
	T_DOUBLE+ CVR + CVRA,				/*  d :: ul */
	T_DOUBLE+ CVR + CVRA,				/*  d ::  f */
	T_DOUBLE,					/*  d ::  d */
	0,						/*  d ::  v */
	0,						/*  d ::  t */

	0,						/*  v ::  c */
	0,						/*  v :: uc */
	0,						/*  v ::  s */
	0,						/*  v :: us */
	0,						/*  v ::  i */
	0,						/*  v :: ui */
	0,						/*  v ::  p */
	0,						/*  v ::  l */
	0,						/*  v :: ul */
	0,						/*  v ::  f */
	0,						/*  v ::  d */
	0,						/*  v ::  v */
	0,						/*  v ::  t */

	0,						/*  t ::  c */
	0,						/*  t :: uc */
	0,						/*  t ::  s */
	0,						/*  t :: us */
	0,						/*  t ::  i */
	0,						/*  t :: ui */
	0,						/*  t ::  p */
	0,						/*  t ::  l */
	0,						/*  t :: ul */
	0,						/*  t ::  f */
	0,						/*  t ::  d */
	0,						/*  t ::  v */
	GTL						/*  t ::  t */
};
