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
