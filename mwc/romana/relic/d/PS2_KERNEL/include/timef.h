/* (-lgl
 * 	COHERENT Version 3.0
 *	Copyright 1989 by Inetco Systems, Ltd.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef	TIMEF_H
#define	TIMEF_H
/*
 * User Level Timed Functions.
 */

typedef struct timef_s {
	struct timef_s *t_next;
	struct timef_s *t_last;
	void	      (*t_func)();
	char *		t_farg;
	unsigned long	t_tick;
} timef_t;

extern	int	timef();	/* schedule/cancel timed  functions */
extern	int	timerun();	/* execute all timed out functions  */

#endif
