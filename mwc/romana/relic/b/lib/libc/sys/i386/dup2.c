/*
 * libc/sys/i386/dup2.c
 * Copyright (c) Bureau d'Etudes Ciaran O'Donnell,1987,1990,1991.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

int
dup2(fd1, fd2) register int fd1, fd2;
{
	register int save;
	struct stat statb;

	save = errno;
	if (fstat(fd1, &statb)<0) {
		errno = EBADF;
		return -1;
	}
	if (fd1==fd2)
		return fd1;
	close(fd2);
	errno = save;
	return fcntl(fd1, F_DUPFD, fd2);
}

/* end of libc/sys/i386/dup2.c */
