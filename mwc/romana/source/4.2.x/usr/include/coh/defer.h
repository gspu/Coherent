#ifndef	__COH_DEFER_H__
#define	__COH_DEFER_H__

/*
 * A __DEFERRED_FN_PTR is a pointer to a function of char * that returns void.
 *
 * Useful for arg #1 of defer ().
 */
typedef void (* __DEFERRED_FN_PTR) __PROTO((char *));

/* prototypes from defer.c */

void	defer		__PROTO ((__DEFERRED_FN_PTR, char *));
void	defend		__PROTO ((void));

#endif	/* ! defined (__COH_DEFER_H__) */
