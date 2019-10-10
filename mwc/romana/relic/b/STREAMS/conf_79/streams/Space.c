/*
 * Make the statically-allocated STREAMS buffer memory tunable.
 */

#define	_DDI_DKI	1
#define	_SYSV4		1

#include <sys/types.h>
#include "conf.h"

size_t	_streams_size = STREAMS_HEAP;
uchar_t	_streams_heap [STREAMS_HEAP];
