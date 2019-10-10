#ifndef	__COMMON___SIZE_H__
#define	__COMMON___SIZE_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the internal data type "__size_t".
 */

#include <common/feature.h>

#if	__BORLANDC__ || __COHERENT__

typedef	unsigned	__size_t;

#elif	__GNUDOS__

typedef	unsigned long	__size_t;

#else

#error	size_t not known for this system

#endif

#endif	/* ! defined (__COMMON___SIZE_H__) */
