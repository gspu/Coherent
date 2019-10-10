/*
 * Routine compatible with obsolete time system
 * call (using ftime system call).
 */

#include <sys/timeb.h>
#define	NULL	((char *)0)

time_t
time(tp)
time_t *tp;
{
	struct timeb tb;

	ftime(&tb);
	if (tp != NULL)
		*tp = tb.time;
	return (tb.time);
}
