#ifndef	__COH_TIMEOUT_H__
#define	__COH_TIMEOUT_H__

#include <kernel/timeout.h>

/*
 * A __TIMED_FN_PTR is a pointer to a function of int that returns int.
 *
 * Useful for arg #3 of timeout ().
 */
typedef int (* __TIMED_FN_PTR) __PROTO((int));

/* prototypes from timeout.c */
void	timeout		__PROTO ((TIM * tp, unsigned int n, __TIMED_FN_PTR,
			  int a));

#endif	/* ! defined (__COH_TIMEOUT_H__) */
