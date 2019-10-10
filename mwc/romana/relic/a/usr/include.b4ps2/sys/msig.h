/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Machine dependent signals.
 */

#ifndef	MSIG_H
#define	MSIG_H	MSIG_H

#define	SIGDIVE	12			/* Divide error */
#define SIGOVFL	13			/* Overflow */
#define SIGNO14	14			/* Signal 14 */
#define SIGNO15	15			/* Signal 15 */
#define SIGNO16	16			/* Signal 16 */
#define NSIG	16			/* Number of signals */

/*
 * Special arguments to signal.
 */
#define	SIG_DFL	((int(*)())0)		/* Default */
#define	SIG_IGN	((int(*)())1)		/* Ignore */

#endif
