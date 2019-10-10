#ifndef	__COMMON__SSIZE_H__
#define	__COMMON__SSIZE_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the POSIX.1 data type "ssize_t", which is required to be defined by both
 * <sys/types.h> and <unistd.h> independently.
 *
 * "ssize_t" should (but is not required to) be able to represent all the
 * values that can be represented by a "size_t" as well as the constant value
 * -1. The maximum value that can be stored in an "ssize_t" can be determined
 * from the constant SSIZE_MAX defined in <limits.h>
 */

#include <common/feature.h>

#if	__BORLANDC__

typedef	int		ssize_t;

#elif	__GNUDOS__

typedef	long		ssize_t;

#elif	__COHERENT__

typedef	long		ssize_t;

#else

#error	ssize_t not known for this system

#endif


#endif	/* ! defined (__COMMON__SSIZE_H__) */
