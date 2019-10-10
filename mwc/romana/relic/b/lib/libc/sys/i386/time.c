/*
 * libc/sys/i386/time.c
 * Copyright (c) Bureau d'Etudes Ciaran O'Donnell,1987,1990,1991.
 * time() function recoded in C for sm90.
 */

#include <sys/types.h>

extern	time_t _time();

time_t
time(tloc) register long *tloc;
{
	register time_t tvec;

	if ((tvec = _time()) != (time_t)-1L && tloc != (char *)0)
		*tloc = tvec;
	return tvec;
}

/* end of libc/sys/i386/time.c */
