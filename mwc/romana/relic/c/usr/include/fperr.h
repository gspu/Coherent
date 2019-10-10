/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Floating Point Exception codes for SIGFPE handlers
 */

#ifndef	FPERR_H
#define	FPERR_H	FPERR_H

enum	fperr	{
	FPE_DV0,		/* divide by 0 */
	FPE_UFL,		/* underflow */
	FPE_OFL,		/* overflow */
	FPE_IOF,		/* integer overflow */
	FPE_NAN,		/* illegal number */
	FPE_UNK			/* unknown */
};
enum	fpeact	{		/* fp err actions */
	FPE_ST0,		/* set result to 0 */
	FPE_SIG			/* generate a signal */
};
char	*fperrstr[] = {
		"divide by 0",
		"underflow",
		"overflow",
		"integer overflow",
		"illegal number",
		"unknown"
	};

#endif
