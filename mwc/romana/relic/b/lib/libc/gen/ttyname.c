#define	NEWTTYS	1
/*
 * libc/gen/ttyname.c
 * Coherent I/O Library.
 * Return the name of the terminal device
 * associated with the given file descriptor.
 * The names in /etc/ttys define the domain.
 */

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static char dev_buff [PATH_MAX + 8] = "/dev/console";

char *
ttyname(fd) int fd;
{
	if (_ttyslot (fd) >= 0)
		return dev_buff;
	return NULL;
}

/*
 * Return the /etc/ttys slot for fd, -1 on error.
 * The /dev/... name is left in dev_buff.
 * Used by ttyname() and ttyslot().
 */
int
_ttyslot(fd) int fd;
{
	register FILE *fp;
	register short fd_mode;
	register dev_t fd_rdev;
	register int slot;
	struct stat sb;

	if (fstat(fd, &sb) < 0 ||
	    (fp = fopen("/etc/ttys", "r")) == NULL)
		return -1;
	fd_mode = sb.st_mode;
	fd_rdev = sb.st_rdev;
	for (slot = 0; ; slot += 1) {
		if (getc (fp) == '\n' || /* Missing status */
#if NEWTTYS
		    getc (fp) == '\n' || /* Missing line type, ie loc or rem */
#endif
		    getc (fp) == '\n' ||	/* Missing getty arg */
		    fgets (dev_buff + 5, NAME_MAX, fp) == NULL ||
		    strchr (dev_buff, '\n') == NULL) {
			slot = -1;
			break;
		}
		* strchr (dev_buff, '\n') = 0;
		if (stat (dev_buff, & sb) >= 0 && sb.st_mode == fd_mode &&
		    sb.st_rdev == fd_rdev)
			break;
	}
	fclose (fp);
	return slot;
}

/* end of libc/gen/ttyname.c */
