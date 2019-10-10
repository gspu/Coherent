
/*
 * rmdir -- remove directories
 */
#include	<sys/stat.h>
#include	<sys/dir.h>
#include	<signal.h>
#include	<stdio.h>
#include	<errno.h>


#define	equal( s1, s2)	(strcmp( s1, s2) == 0)

int		fflag,			/* force flag */
		interrupted;
struct stat	dot;

char	*getparent(),
	*getchild(),
	*strncpy(),
	*strcat(),
	*strcpy(),
	*concat();

/*
 * main
 * Interrupts are handled to prevent the formation of mangled directories.
 */
main( argc, argv)
register char	**argv;
{
	register int status = 0;

	if (*++argv!= NULL && equal( *argv, "-f")) {
		if (getuid())
			fatal("only superuser can force");
		++fflag;
		++argv;
	}
	if (stat( ".", &dot))
		fatal("can't find .");
	catch(SIGINT);
	catch(SIGHUP);
	signal(SIGQUIT, SIG_IGN);

	if (*argv == NULL) {
		fprintf(stderr, "Usage: rmdir [ -f ] dir ...\n");
		exit(1);
	}
	else
		while (*argv) {
			if (rmdir(*argv++) < 0)
			        status = 1;  /* error */
			if (interrupted)
				exit(1);
		}
	exit(status);
}


/*
 * remove a directory
 * The directory and its "." and ".." entries are unlinked if everything
 * looks kosher.  The force option causes most checking to be suppressed
 * and unlinks only what is specifically named.  `rmdir' takes a dim view
 * to non-hierarchical directories.
 */
rmdir(dir)
register char	*dir;
{
	register char	*child;
	register FILE	*f;
	struct stat	s;
	struct direct	d;


	if ((int) (child = getchild( dir)) < 0) {
		error("can't get child dir name %s", dir);
		return(-1);
	}
	if (access( dir, 0) || stat( dir, &s)) {
		error("can't find %s", dir);
		return(-1);
	}
	if ((s.st_mode&S_IFMT) != S_IFDIR) {
		error("%s not a directory", dir);
		return(-1);
	}
	if (access( getparent( dir), 3)) {
		error("no permission to remove %s", dir);
		return(-1);
	}
	if (fflag == 0) {
		if (equal( child, ".") || equal( child, "..")) {
			error("%s not allowed", dir);
			return(-1);
		}
		if (s.st_ino==dot.st_ino && s.st_dev==dot.st_dev) {
			error("but %s is your working directory", dir);
			return(-1);
		}
		f = fopen( dir, "r");
		if (f==NULL || access( dir, 4)) {
			error("can't read %s", dir);
			return(-1);	
		}
		while (fread( &d, sizeof d, 1, f) == 1) {
			if (d.d_ino == 0)
				continue;
			if (equal( d.d_name, ".") || equal( d.d_name, ".."))
				continue;
			error("%s not empty", dir);
			return(-1);
		}
		fclose(f);

		if (s.st_nlink <= 2) {
			unlink( concat( dir, "/."));
			unlink( concat( dir, "/.."));
		}
	}

	if (unlink( dir)) {
		error("unlink %s failed", dir);
		return(-1);
	}
	return(0);
}


/*
 * return name of parent
 */
char	*
getparent( dir)
char	*dir;
{
	register	i;
	register char	*p;
	static char	*par;
	int		tmp;

	if (par)
		free( par);
	i = strlen( dir);
	par = malloc( i+1);
	if (par == NULL)
		nomemory( );
	strcpy( par, dir);

	for (p=par+i; p>par; )
		if (*--p != '/')
			break;
	for (++p; *--p!='/'; )
		if (p == par) {
			*p = '.';
			break;
		}
	*++p = '\0';
	if (par[tmp = strlen(par)-1] == '/')
	   par[tmp] = 0;  /* kill any ending slash */
	return (par);
}


/*
 * return rightmost component of pathname
 */
char	*
getchild( dir)
register char	*dir;
{
	register char	*p,
			*q;
	int		i;
	static char	ch[DIRSIZ+1];

	p = &dir[strlen( dir)];
	do {
		if (p == dir) {
			error("not permitted");
			return(-1);
		}
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
	return (strncpy( ch, q, i));
}


/*
 * return concatenation of `s1' and `s2'
 */
char	*
concat( s1, s2)
char	*s1,
	*s2;
{
	static char	*str;

	if (str)
		free( str);
	str = malloc( strlen( s1)+strlen( s2)+1);
	if (str == NULL)
		nomemory( );
	strcpy( str, s1);
	return (strcat( str, s2));
}


nomemory( )
{
	fatal("out of memory");
}


onintr( )
{

	signal( SIGINT, SIG_IGN);
	signal( SIGHUP, SIG_IGN);
	++interrupted;
}


catch( sig)
{

	if( signal( sig, SIG_IGN) == SIG_DFL)
		signal( sig, onintr);
}


error(arg1)
char	*arg1;
{
	fprintf( stderr, "rmdir: %r\n", &arg1);
}

fatal(arg1)
char 	*arg1;
{
	error(arg1);
	exit(1);
}
