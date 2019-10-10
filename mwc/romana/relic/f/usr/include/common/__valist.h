#ifndef	__COMMON___VALIST_H__
#define	__COMMON___VALIST_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the internal data type "__va_alist", exactly equivalent to the ISO C data
 * type "va_alist" but given an internal name so that headers may refer to
 * the type without exporting the name to user-level unnecessarily.
 *
 * Unlike most other cases where we perform this subterfuge, where we do it
 * as a matter of good style, here we do it as a matter of compliance with
 * the namespace provisions of POSIX.1, since the name of the user-level type
 * does not end in "_t" and thus is not implicitly reserved by POSIX.1 as
 * most other type-names are.
 */

typedef	char	      *	__va_list;

#endif	/* ! defined (__COMMON___VALIST_H__) */
