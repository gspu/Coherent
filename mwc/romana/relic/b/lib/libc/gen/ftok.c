/* 
 * libc/gen/ftok.c
 * Return key based on path and id.
 * Return -1 if path is not accesible.
 */

#include <sys/ipc.h>
#include <sys/stat.h>

key_t
ftok(name, id)
char	*name;		/* name of file, could be special file or directory */
int	id;				/* project id */
{
	register key_t	key;		/* key value */
	struct	stat	s;		/* stat buffer */

	return ((key = stat(name, &s)) < 0) ? key
		: (key_t)(s.st_ino<<8 | ((key_t)(id & 0xFF))<<24 | minor(s.st_dev));
}

/* end of ftok.c */
