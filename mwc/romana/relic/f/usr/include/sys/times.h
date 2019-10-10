/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure returned by the `times' system call.
 */

#ifndef	 __SYS_TIMES_H__
#define	 __SYS_TIMES_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/_clock.h>

#if	_I386

struct tms {
	__clock_t	tms_utime;		/* Process user time */
	__clock_t	tms_stime;		/* Process system time */
	__clock_t	tms_cutime;		/* Child user time */
	__clock_t	tms_cstime;		/* Child system time */
};

__EXTERN_C_BEGIN__

int		times		__PROTO ((struct tms * _buffer));

__EXTERN_C_END__

#else	/* if ! _I386 */

struct tbuffer {
	long		tb_utime;		/* Process user time */
	long		tb_stime;		/* Process system time */
	long		tb_cutime;		/* Child user time */
	long		tb_cstime;		/* Child system time */
};

#endif	/* ! _I386 */

#endif	/* ! defined (__SYS_TIMES_H__) */
