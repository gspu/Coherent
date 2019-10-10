/*
 * libc/gen/getwd.c
 * Return a static string containing the current
 * working directory for this process.
 * The arguments to this function are NOT compatible with
 * the BSD function getwd(), so it is now called _getwd().
 * Warning: this call may change the current directory
 * of the process if for any reason it fails.
 */

#include <stddef.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

#define	MAXNAME	400		/* Longest pathname */

static	int	oerrno;

static
char *
fail()
{
	if (errno == 0)
		errno = oerrno;		/* preserve previous errno */
	return NULL;
}

char *
_getwd()
{
	struct stat d, dd;
	static char fnbuf[MAXNAME];
	register char *cp, *dp;
	register dev_t rdev;
	register ino_t rino;
	DIR	      *	dirp;
	struct dirent *	direntp;

	oerrno = errno;			/* save old errno */
	errno = 0;
	dp = fnbuf + MAXNAME - 1;
	* dp = '\0';
	if (stat ("/", & d) < 0)
		return fail ();
	rdev = d.st_dev;
	rino = d.st_ino;
	while (stat( ".", & d) >= 0 && (d.st_ino!=rino || d.st_dev!=rdev)) {
		if (stat ("..", & dd) < 0 || chdir ("..") < 0)
			return fail ();
		if ((dirp = opendir (".")) == NULL)
			return fail ();
		if (d.st_dev == dd.st_dev) {
			if (d.st_ino == dd.st_ino) {
				closedir (dirp);
				break;
			}
			do {
				if ((direntp = readdir (dirp)) == NULL) {
					closedir (dirp);
					return fail ();
				}
			} while (direntp->d_ino != d.st_ino);
		} else
			do {
				if ((direntp = readdir (dirp)) == NULL) {
					closedir (dirp);
					return fail ();
				}
				if (direntp->d_ino != 0 &&
				    stat (direntp->d_name, & dd) < 0) {
					closedir (dirp);
					return fail ();
				}
			} while (dd.st_ino != d.st_ino ||
				 dd.st_dev != d.st_dev);

		for (cp = direntp->d_name ; * cp != '\0' ; cp ++)
			/* DO NOTHING */ ;

		while (cp > direntp->d_name) {
			if (dp <= fnbuf) {
				closedir (dirp);
				return fail ();
			}
			* -- dp = * -- cp;
		}
		closedir (dirp);

		* -- dp = '/';
	}
	if (errno)
		return NULL;
	if (* dp != '/')
		* -- dp = '/';
	if (chdir (dp) < 0)
		return fail ();
	errno = oerrno;
	return dp;
}

/* end of libc/gen/getwd.c */
