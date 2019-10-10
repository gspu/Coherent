/*
 * libc/gen/lockf.c
 * File locking.
 */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

lockf(fd, cmd, size) register int fd, cmd; long size;
{
	struct flock fl;
	register int fcmd;

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_CUR;
	fl.l_start = 0;
	fl.l_len = size;

	switch (cmd) {

	case F_ULOCK:
		fl.l_type = F_UNLCK;
		/* fall through... */

	case F_TLOCK:
		fcmd = F_SETLK;
lab:
		if (fcntl(fd, fcmd, &fl) == -1) {
			if (errno == ENOLCK)
				errno = EDEADLK;
			return -1;
		}
		break;

	case F_LOCK:
		fcmd = F_SETLKW;
		goto lab;
		break;

	case F_TEST:
		if (fcntl(fd, F_GETLK, &fl) == -1)
			return -1;
		if (fl.l_type != F_UNLCK) {
			errno = EAGAIN;
			return -1;
		}
		break;

	default:
		errno = EINVAL;
		return -1;

	}
	return 0;
}

/* end of lockf.c */
