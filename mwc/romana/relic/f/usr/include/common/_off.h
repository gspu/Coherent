#ifndef	__COMMON__OFF_H__
#define	__COMMON__OFF_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the POSIX.1 data type "off_t". To support cross-compilation environments,
 * this header file uses a variety of feature tests to avoid conflict with
 * definitions supplied by the translator headers.
 */

#include <common/__off.h>

typedef	__off_t		off_t;

#endif	/* ! defined (__COMMON__OFF_H__) */
