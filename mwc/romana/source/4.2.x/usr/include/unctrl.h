/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__UNCTRL_H__
#define	__UNCTRL_H__

/*
 * The macro defined below is part of curses, and causes a character to
 * be converted into a printable graphic string, even if the character is
 * a control (in which case it is prefixed with an up-caret).
 *
 * The _unctrl [] array definition below is not permitted in Standard
 * headers, but this is not a Standard header. Caveat utilitor.
 */

# define	unctrl(ch)	(_unctrl[ch])

extern char	*_unctrl[];

#endif	/* ! defined (__UNCTRL_H__) */
