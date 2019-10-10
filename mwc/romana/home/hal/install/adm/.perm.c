/*
 * .perm.c 
 *
 * Set permissions and ownership on a file.
 *
 * Minimal error checking here - this command should only be called
 * from doperm.sh.
 *
 * Usage: .perm permfile line_no
 *
 * Some security checking is done on permfile before applying it.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>

#define CHECKFILE "/usr/local/bin/rcs"

#define LBUF_LEN	400

char lbuf [LBUF_LEN];
char mode [LBUF_LEN];
char owner [LBUF_LEN];
char group [LBUF_LEN];
char filename [LBUF_LEN];

int main (int argc, char ** argv) {

	FILE		* fp_pfile;

	struct stat	st_check;
	struct stat	st_pfile;

	int		result;

	int		current_lnum;
	int		needed_lnum;

	char		* cp;

	/* Security check. */

	result = stat (argv [1], & st_pfile);
	if (result) {
		sprintf (lbuf, "%s: can't stat %s:", argv [0], argv [1]);
		perror(lbuf);
		return 1;
	}

	result = stat (CHECKFILE, & st_check);
	if (result)
		return 2;

	if (st_pfile.st_uid != st_check.st_uid
	  || st_pfile.st_gid != st_check.st_gid) {
		return 3;
	}

	/* Try to open permfile. */

	fp_pfile = fopen (argv [1], "r");
	if (fp_pfile == NULL)
		return 4;

	/* Parse needed line number from command line. */

	needed_lnum = strtol (argv [2], & cp, 10);
	if (cp == argv [2])
		return 5;

	/* Skip to the desired line. */

	for (current_lnum = 0; current_lnum < needed_lnum; current_lnum++) {
		if (!fgets (lbuf, LBUF_LEN, fp_pfile)) {
			fclose (fp_pfile);
			return 6;
		}
	}

	/* Desired line is in lbuf. */
	if (sscanf (lbuf, "%s%s%s%s", mode, owner, group, filename) != 4) {
		return 7;
	}

	fclose (fp_pfile);
	execl ("/bin/chmog", "chmog", mode, owner, group, filename, 0);

	return 8;
}
