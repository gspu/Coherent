/*
 * misc. POSIX emulation for COHERENT
 */

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#if COHERENT
char *
getcwd(buf, len)
char *buf;
size_t len;
{
	extern char *getwd();
	char	*cwd;

	if ((cwd = getwd()) == NULL)
		return NULL;
	if (strlen(cwd)+1 >= len) {
		errno = ERANGE;
		return NULL;
	}
	return strcpy(buf, cwd);
}

long
ulimit(cmd, limit)
int cmd;
long limit;
{
	return 0;
}
#endif

