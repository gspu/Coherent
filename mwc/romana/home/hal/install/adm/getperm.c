/*
 * getperm.c
 *
 * Tue Aug 30 11:16:47 1994 CDT
 *
 * Usage: getperm file...
 *
 * Print a line describing the arg file to stdout:
 * filename   perms   ownership   group
 */

#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sys/compat.h>
#include <string.h>

#define MSGLEN	200
static char msg [MSGLEN];

#if	__USE_PROTO__
int main (int argc, char ** argv)
#else
main (argc, argv)
int argc;
char *argv[];
#endif
{
	struct stat	s;
	struct passwd	* pp;
	struct group	* gp;
	int		argn;
	int		result;

	for (argn = 1; argn < argc; argn++) {

		char * filename;

		/* Ignore leading "./" in file name. */

		filename = argv [argn];
		if (strncmp (filename, "./", 2) == 0)
			filename += 2;

		/* Stat the file. */

		result = stat (filename, & s);
		if (result) {
			sprintf (msg, "%s : can't stat %s",
			  argv [0], filename);
			perror (msg);
			return 1;
		}

		/* Get user name. */

		pp = getpwuid (s.st_uid);
		if (pp == NULL) {
			fprintf (stderr,
			  "%s, file %s : Can't get passwd info, uid = %d\n",
			  argv [0], filename, s.st_uid);
			return 1;
		}

		/* Get group name. */

		gp = getgrgid (s.st_gid);
		if (gp == NULL) {
			fprintf (stderr,
			  "%s, file %s : Can't get group info, gid = %d\n",
			  argv [0], filename, s.st_gid);
			return 1;
		}

		/* Write file name, perms, owner, group to stdout. */
		printf("%04o\t%s\t%s\t%s\n", s.st_mode & 07777,
		  pp->pw_name, gp->gr_name, filename);
	}

	return 0;
}
