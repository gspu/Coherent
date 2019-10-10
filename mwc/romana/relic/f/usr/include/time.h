/* (-lgl
 * 	COHERENT Version 4.0.2
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * time.h
 * COHERENT time function header.
 */

#ifndef	__TIME_H__
#define	__TIME_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/_size.h>
#include <common/_time.h>
#include <common/_clock.h>
#include <common/_clktck.h>

struct	tm	{
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};

__EXTERN_C_BEGIN__

clock_t		clock		__PROTO ((void));
double		difftime	__PROTO ((time_t _time1, time_t time0));
time_t		mktime		__PROTO ((struct tm * _timeptr));
time_t		time		__PROTO ((time_t * _timer));
char	      *	asctime		__PROTO ((__CONST__ struct tm * _timeptr));
char	      *	ctime		__PROTO ((__CONST__ time_t * _timer));
struct tm     *	gmtime		__PROTO ((__CONST__ time_t * _timer));
struct tm     *	localtime	__PROTO ((__CONST__ time_t * _timer));
__size_t	strftime	__PROTO ((char * _s, size_t _maxsize,
					  __CONST__ char * _format,
					  __CONST__ struct tm * _timeptr));

__EXTERN_C_END__

#if	! _STDC_SOURCE

extern	char	      *	tzname[2];

#if	! _POSIX_SOURCE

extern	long		timezone;

#endif
#endif	/* ! _STDC_SOURCE */

#endif	/* ! defined (__TIME_H__) */
