/*
 * Sbrk - grow memory in data segment by
 * a specified increment.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/malloc.h>

extern	int	errno;

char *
sbrk(incr)
unsigned int	incr;
{
	extern	char	*brk();
	register vaddr_t send,
			rend;

	rend = (vaddr_t)brk(NULL);
	if (incr == 0)
		return (rend);
	send = rend + incr;
	if (send < rend)
		return (BADSBRK);
	errno = 0;
	brk(send);
	if (errno)
		return (BADSBRK);
	return (rend);
}
