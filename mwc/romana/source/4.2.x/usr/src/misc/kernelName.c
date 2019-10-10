#define __KERNEL__ 1
#include <kernel/typed.h>
#undef KERNEL
#include <fcntl.h>

#ifndef T_NULL
#define T_NULL ((char *)0)
#endif

/*
 * Return name of current kernel.
 * Usefull for things like ps and drvld that need to read the
 * kernel's symbol table.
 *
 * If we can't open /dev/boot_gift, guess "/coherent".
 * If boot_gift does not have a command line, guess "/autoboot".
 * Otherwise, pull the boot file from the command line.
 */
char *
kernelName()
{
	int fd;
	FIFO *ffp;
	typed_space *tp;
	int found;
	TYPED_SPACE(boot_gift, BG_LEN, T_FIFO_SIC);

	/* 16 characters are for "/", a 14 character name, and a NUL.  */
	static char retval[16];

	if (-1 == (fd = open("/dev/boot_gift", O_RDONLY))) {
		/* Can't open boot_gift, guess "/coherent".  */
		strcpy(retval, "/coherent");
		return(retval);
	}
	
	if (0 == read(fd, &boot_gift, BG_LEN)) {
		close(fd);
		/* Can't read boot_gift, guess "/coherent".  */
		strcpy(retval, "/coherent");
		return(retval);
	}

	close(fd);

	found = 0; /* We are looking for the command line.  */
	if (F_NULL != (ffp = fifo_open(&boot_gift, 0))) {
		for (; !found && T_NULL != (tp = fifo_read(ffp)); ) {
			if (T_STR_ARGF == tp->ts_type) {
				found = 1;
				break;
			}
		}
		fifo_close(ffp);

		if (!found) {
			strcpy(retval, "/autoboot");
		} else {
			/* Recast the argument list, so we can read it.  */
			RETYPE(tp, T_FIFO_SIC);
			ffp = fifo_open(tp, 0);
			if (T_NULL == (tp = fifo_read(ffp))) {
				strcpy(retval, "/autoboot");
			} else {
				sprintf(retval, "/%.14s", tp->ts_data);
			}
			fifo_close(ffp);
		}
	} else {
		strcpy(retval, "/autoboot");
	}

	return(retval);

} /* pick_nfile() */

