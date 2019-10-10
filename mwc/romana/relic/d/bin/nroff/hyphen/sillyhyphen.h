/* (-lgl
 * 	The information contained herein is a trade secret of Mark Williams
 * 	Company, and  is confidential information.  It is provided  under a
 * 	license agreement,  and may be  copied or disclosed  only under the
 * 	terms of  that agreement.  Any  reproduction or disclosure  of this
 * 	material without the express written authorization of Mark Williams
 * 	Company or persuant to the license agreement is unlawful.
 * 
 * 	troff-nroff Version 002
 * 	Copyright (c) 1984-1986.
 * 	An unpublished work by Mark Williams Company, Chicago.
 * 	All rights reserved.
 -lgl) */
/*
 * A text formatter.
 * Hyphenation header file.
 */
#define LNUL	0			/* Null */
#define LEOK	1			/* End of key */
#define LHYP	2			/* Hyphenation point */
#define LNEW	3			/* Get another char */
#define LBRT	4			/* Branch on true */
#define LBRF	5			/* Branch on false */
#define LCON	6			/* Continue */

#define EXCSIZE	347

extern	char	*exctab[];
extern	char	hyptab[];
