#ifndef	__COMMON___OFF_H__
#define	__COMMON___OFF_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the internal data type "__off_t", exactly equivalent to the POSIX.1 data
 * type "off_t" but given an internal name so that header files may refer
 * to the type without exporting the user-level type unnecessarily.
 */

typedef	long		__off_t;

#endif	/* ! defined (__COMMON___OFF_H__) */
