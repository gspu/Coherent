/*
 * mkdir.c
 * 5/10/90
 * Usage: mkdir [ -rp ] dir ...
 * Make directories.
 *
 * Changes by michael 5/10/93
 *      implemented -p option
 *
 * Changes by steve 5/10/90:
 *	in getchild(), bumped strncpy count by 1 to return NUL-terminated result
 *	in getparent(), hacked bogus "ending slash" code so that e.g.
 *		getparent("/foo") returns "/" rather than ""
 *
 * Changes by steve 4/12/90:
 *	corrected fatal() call
 *	added success return status to mkdir()
 *	improved return status of getchild()
 *	implemented -r option
 */

#include	<sys/dir.h>
#include	<sys/ino.h>
#include	<signal.h>
#include	<stdio.h>
#include	<errno.h>


#define	equal(s1, s2)	(strcmp(s1, s2) == 0)

int	interrupted;
int	rflag;

char	*getparent(),
	*getchild(),
	*concat(),
	*malloc(),
	*strcat(),
	*strcpy(),
	*strncpy();

/*
 * main
 * Interrupts are handled to prevent the formation of mangled directories.
 */
main(argc, argv)
register char	**argv;
{
	register int status = 0;

	catch(SIGINT);
	catch(SIGHUP);
	signal(SIGQUIT, SIG_IGN);

	if (argc > 1 && argv[1][0] == '-' &&
			(argv[1][1] == 'r' || argv[1][1] == 'p')) {
		++rflag;
		--argc;
		++argv;
	}
	if (*++argv == NULL) {
		fprintf(stderr, "Usage: mkdir [-rp] dir ...\n");
		exit(1);
	} else
		while (*argv) {
			if (mkdir(*argv++) < 0)
			 	status = 1;		/* error */
			if (interrupted)
				exit(1);
		}
	exit(status);
}

/*
 * Make a directory.
 * If the parent exists and is writeable, the directory and its "." and
 * ".." links are created.
 */
int
mkdir(dir)
register char	*dir;
{
	register char	*parent,
			*child;

	if ((child = getchild(dir)) == NULL) {
		error("cannot get child name %s", dir);
		return -1;
	} else if (equal(child, ".") || equal(child, "..")) {
		error("%s not allowed", dir);
		return -1;
	}
	if ((parent = getparent(dir)) == NULL)
		return -1;
	if (access(parent, 03)) {
		if (rflag && errno == ENOENT) {
			/* Make parent recursively. */
			if (mkdir(parent) != 0) {
				free(parent);
				return -1;
			}
		} else {
			switch (errno) {
			case ENOENT:
				error("parent directory %s does not exist", parent);
				break;
			case EACCES:
				error("no permission to mkdir in %s", parent);
				break;
			default:
				noway(dir);
				break;
			}
			free(parent);
			return -1;
		}
	}
	if (mknod(dir, IFDIR|0777, 0)) {
		switch (errno) {
		case EEXIST:
			error("%s already exists", dir);
			break;
		case EPERM:
			error("not the super-user");
			break;
		default:
			noway(dir);
			break;
		}
		free(parent);
		return -1;
	} else if (link(dir, concat(dir, "/."))) {
		linkerr(dir, ".");
		free(parent);
		return -1;
	} else if (link(parent, concat(dir, "/.."))) {
		linkerr(dir, "..");
		free(parent);
		return -1;
	}
	free(parent);
	return (chown(dir, getuid(), getgid()) < 0) ? -1 : 0;
}


/*
 * Return name of parent in an allocated buffer.
 */
char	*
getparent(dir)
char	*dir;
{
	register	i;
	register char	*p;
	char		*par;
	int 		tmp;

	i = strlen(dir);
	par = malloc(i+1);
	if (par == NULL) {
		nomemory();
		return NULL;
	}
	strcpy(par, dir);

	for (p=par+i; p>par; )
		if (*--p != '/')
			break;
	for (++p; *--p!='/'; )
		if (p == par) {
			*p = '.';
			break;
		}
	*++p = 0;
	tmp = strlen(par) - 1;
	if (tmp > 0 && par[tmp] == '/')
		par[tmp] = 0;			/* kill any ending slash */
	return par;
}

/*
 * Return rightmost component of pathname
 * in a statically allocated buffer.
 */
char	*
getchild(dir)
register char	*dir;
{
	register char	*p,
			*q;
	int		i;
	static char	ch[DIRSIZ+1];

	p = &dir[strlen(dir)];
	do {
		if (p == dir)
			fatal("illegal directory name");
	} while (*--p == '/');
	q = p;
	while (q > dir)
		if (*--q == '/') {
			++q;
			break;
		}
	i = p+1 - q;
	if (i > DIRSIZ)
		i = DIRSIZ;
	return strncpy(ch, q, i+1);
}


/*
 * Return concatenation of 's1' and 's2' in a malloc'ed buffer.
 * Free the previous buffer.
 */
char	*
concat(s1, s2)
char	*s1,
	*s2;
{
	static char	*str;

	if (str)
		free(str);
	str = malloc(strlen(s1)+strlen(s2)+1);
	if (str == NULL) {
		nomemory();
		return -1;
	}
	strcpy(str, s1);
	return strcat(str, s2);
}


/*
 * Recover from link failure.
 * In the event that "." or ".." cannot be created, remove all traces
 * of the directory.
 */
linkerr(dir, name)
char	*dir,
	*name;
{
	unlink(concat(dir, "/."));
	unlink(concat(dir, "/.."));
	unlink(dir);
	error("link to %s failed", name);
	return -1;
}


nomemory()
{
	error("out of memory");
	return -1;
}


noway(dir)
char	*dir;
{
	error("cannot make %s", dir);
	return -1;
}


onintr()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	++interrupted;
}


catch(sig)
{
	if (signal(sig, SIG_IGN) == SIG_DFL)
		signal(sig, onintr);
}

error(arg1)
char	*arg1;
{
	fprintf(stderr, "mkdir: %r\n", &arg1);
}

fatal(arg1)
char 	*arg1;
{
	error(arg1);
	exit(1);
}

/* end of mkdir.c */
