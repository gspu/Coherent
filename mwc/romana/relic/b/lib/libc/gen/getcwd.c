/*
 * libc/gen/getcwd.c
 */

#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

extern	char	*_getwd();
extern	char	*malloc();

/*
 * Set errno and return NULL.
 */
static
char *
error(n) register int n;
{
	errno = n;
	return NULL;
}

/*
 * Get current working directory.
 * Cf. SVID Appendix 2.2 and Posix 5.2.2.
 * This calls the COHERENT function _getwd(),
 * which takes no args and returns a pointer
 * to a statically allocated string.
 */
char *
getcwd(buf, size) char *buf; size_t size;
{
	register char *dir;
	register size_t len;

	if (size == 0)
		return error(EINVAL);
	if ((dir = _getwd()) == NULL)
		return NULL;			/* cannot find cwd */
	len = strlen(dir);
	if (size < len + 1)
		return error(ERANGE);
	/*
	 * The following behavior is specified by SVID,
	 * Posix says the behavior is undefined in this case.
	 */
	if (buf == NULL && (buf = malloc(size)) == NULL)
		return error(ENOMEM);
	return strcpy(buf, dir);	
}

/* end of libc/gen/getcwd.c */
