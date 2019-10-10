/*
 * libc/sys/i8086/sbrk.c
 * sbrk()
 * Grow memory in data segment by a specified increment.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/malloc.h>

extern	char	*brk();
extern	int	errno;

char *
sbrk(incr) unsigned int incr;
{
	register vaddr_t send, rend;
	register int oerrno;
	
	rend = (vaddr_t)brk(NULL);
	if (incr == 0)
		return rend;
	send = rend + incr;
	if (send < rend)
		return BADSBRK;
	oerrno = errno;
	errno = 0;
	brk(send);
	if (errno)
		return BADSBRK;
	errno = oerrno;
	return rend;
}

/* end of libc/sys/i8086/sbrk.c */
