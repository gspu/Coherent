#define	NEWTTYS	1
/*
 * Coherent I/O Library
 * Return the name of the terminal device
 * associated with the given file descriptor.
 * The names in /etc/ttys define the domain.
 */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/dir.h>

static char dev_buff[DIRSIZ+8] = "/dev/console";

char *
ttyname(fd)
int fd;
{
	if (fttyslot(fd) >= 0)
		return (dev_buff);
	return (NULL);
}

/*
 * Return the /etc/ttys slot for fd, -1 on error.
 * The /dev/... name is left in dev_buff.
 * Used by ttyname and ttyslot.
 */
fttyslot(fd)
{
	register FILE *fp;
	register short fd_mode;
	register dev_t fd_rdev;
	register int slot;
	struct stat sb;
	extern char *index();

	if (fstat(fd, &sb) < 0
	 || (fp = fopen("/etc/ttys", "r")) == NULL)
		return (-1);
	fd_mode = sb.st_mode;
	fd_rdev = sb.st_rdev;
	for (slot = 0; ; slot += 1) {
		if (getc(fp) == '\n'	/* Missing status */
#if NEWTTYS
		 || getc(fp) == '\n'	/* Missing line type, ie loc or rem */
#endif
		 || getc(fp) == '\n'	/* Missing getty arg */
		 || fgets(dev_buff+5, DIRSIZ, fp) == NULL
		 || index(dev_buff, '\n') == NULL) {
			slot = -1;
			break;
		}
		*index(dev_buff, '\n') = 0;
		if (stat(dev_buff, &sb) >= 0
		 && sb.st_mode == fd_mode
		 && sb.st_rdev == fd_rdev)
			break;
	}
	fclose(fp);
	return (slot);
}
