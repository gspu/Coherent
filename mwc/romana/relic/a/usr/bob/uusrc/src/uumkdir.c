/*
 * uumkdir.c
 *
 * Makes directories for uucico to write in.
 *
 */

#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/ino.h>
#include <pwd.h>
#include <signal.h>
#include <errno.h>
#include "dcp.h"

#ifndef NULL
#define	NULL	((char*) 0)
#endif

#ifndef TRUE
#define TRUE	(0 == 0)
#endif

#define MAXDIR	32
#define	equal(s1, s2)	   (strcmp(s1, s2) == 0)

/*
 * Masks by types of permissions.
 */
#define	AEXEC	(S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6))
#define	AREAD	(S_IREAD|(S_IREAD>>3)|(S_IREAD>>6))
#define	AWRITE	(S_IWRITE|(S_IWRITE>>3)|(S_IWRITE>>6))
#define	ASUID	(S_ISUID|S_ISGID)
#define	ATEXT	S_ISVTX

/*
 * Masks by types of users.
 */
#define	AOWN	(S_ISUID|S_ISVTX|S_IREAD|S_IWRITE|S_IEXEC)
#define	AGRP	(S_ISGID|S_ISVTX|(S_IREAD>>3)|(S_IWRITE>>3)|(S_IEXEC>>3))
#define	AOTH	(S_ISVTX|(S_IREAD>>6)|(S_IWRITE>>6)|(S_IEXEC>>6))
#define	AALL	(AOWN|AGRP|AOTH)

typedef
struct inpdir_s {
	char	* dir;		/* dir string entered */
	char	* start;	/* start of new dir in string */
	char	* end;		/* end of dir string */
	char	* p;		/* end of current parent string */
	char	* c;		/* end of current child string */
} inpdir_t;

char usage[] = "Usage: uumkdir [ -m mode ] [ -p ] dir ...\n";
char badsm[] = "uumkdir: badly formed symbolic mode\n";
char badom[] = "uumkdir: badly formed octal mode\n";

long	int	new;		/* bit-wise child dir condition */
short	uid;			/* user ID */
int		interrupted;	/* interrupt flag */
int		endflag;	/* end od dir string flag */
int		pflag;		/* p option flag */
int		mflag;		/* m option flag */
int		mode = 0755;	/* initial mode setup for m option */

extern	char	*malloc();
extern	char	*strcpy();
extern	char	*strncpy();
extern	char	*strcat();

/*
 *	Interrupts are handled to prevent the formation of mangled directories.
 */

main(argc, argv)
int	argc;
register char	** argv;

{
	register int	status = 0;	/* condition of mkdir:
						0 - success, 1 - error */
	inpdir_t	dirs;		/* structure with dir string info */
	char	      * end;		/* pointer to end of dir string */

	extern	void	fatal();
	extern	int	catch();
	extern	int	readmode();
	extern	int	mkpath();
	extern	int	error();

	if ( (setuid(0) < 0) || (setgid(0) < 0) )
		fatal("uumkdir must be executed setuid root\n");

	catch(SIGINT);
	catch(SIGHUP);
	signal(SIGQUIT, SIG_IGN);

	while((--argc > 0) && ((*++argv)[0] == '-')) {
		switch((*argv)[1]) {
		case 'p':
			if (pflag)
				fatal(usage);
			pflag = TRUE;
			break;
		case 'm':
			if (mflag || (--argc <= 0))
				fatal(usage);
			uid   = getuid();
			mflag = TRUE;
			mode  = readmode(*++argv, mode);
			break;
		case 'v':
		case 'V':
			fatal("uumkdir: Version %s", VERSION);
		default:
			fatal(usage);
		}
	}
	if (argc <= 0)
		fatal(usage);
	else {
		for(; *argv ; ++argv) {
			if (dirs.dir)
				free(dirs.dir);
			for(end = *argv ; *end != '\0' ; ++end)
				;
			do {
				if (end == *argv) {
					error("bad argument (empty string)", 0);
					exit(1);
				}
			} while(*--end == '/');
			*++end = '\0';
			if ((dirs.dir = malloc(end - *argv + 1)) == NULL) {
				error("out of memory ", 0);
				exit(1);
			}
			strcpy(dirs.dir, *argv);
			dirs.end = dirs.dir + (end - *argv);
			if (mkpath(&dirs) < 0)
			 	status = 1;  /* error */
			if (interrupted)
				exit(1);
		}
	}
	exit(status);
}

/**
 * int
 * mkpath(pdir)
 * inpdir_t	* pdir;
 *
 *	Input:	pdir	- pointer to structure.
 *
 *	Action:	Create new directory and all necessary directories
 *		to new directory.
 *
 *	Return:	0 if all directory creations are successful, else -1.
 *
 *	Note:	All directories created before an error is
 *		encountered are not removed from user's file.
 */

int
mkpath(pdir)
inpdir_t	* pdir;
{
	register char	* childnam;	/* dir name of child */
	register char	* child;	/* path name of child */
	register char	* parent;	/* path name of parent */
	register int	n;
	int		  err;		/* error response from dir creation */
	int		  numdir;	/* number of directories minus first
					   parent specified in full path name */
	extern char *	getparent();
	extern char *	getchild();
	extern char *	getpath();
	extern int	mkdir();
	extern int	setmod();
	extern int	error();

	numdir	= 0;
	new	= 0L;
	endflag	= !TRUE;

	if ((child = malloc(pdir->end - pdir->dir + 1)) == NULL)
		return(error("out of memory ", 0));
	parent = getparent(pdir);
	n = pdir->start - pdir->dir;
	strncpy(child, pdir->dir, n);
	child[n] = '\0';
	do {
		++numdir;
		if ((int)(childnam = getchild(pdir)) < 0) {
			error("can't get dir name ", pdir->dir, 0);
			break;
		}
		child = getpath(pdir, child);
		if (pdir->c == pdir->end) {
			/* Last child declared cannot be "." or "..". */
			if (equal(childnam, ".") || equal(childnam, "..")) {
				error(pdir->dir," not allowed", 0);
				break;
			}
 			/* Signal end of dir string. */
			endflag = TRUE;
			if (mkdir(parent, child) == -1)
				break;
			/* Signal new dir. */
			new = (new << 1) | 1L;
			numdir += MAXDIR;
		} else {
			new = (new << 1) | 0L;
			if (!equal(childnam, ".") && !equal(childnam, "..")) {
				if ((err = mkdir(parent, child)) == -1)
					break;
				else if (err == 0)
					new |= 1L;
			}
		}
		/* Get next parent. */
		parent = getpath(pdir, parent);
		pdir->p = pdir->c;
	} while(numdir <= MAXDIR);
	/*  Set user-selected mode if available. */
	if (mflag) {
		if (setmod(parent, numdir, pdir->start, (pdir->p - 1)) < 0)
			return(-1);
	}
	if (numdir > MAXDIR)
		return(0);
	else
		return(-1);
}

/**
 * int
 * setmod(ch, ndir, start, p)
 * char	* ch;
 * char	* start;
 * char	* p;
 * int	  ndir;
 *
 *	Input:	ch	- string holding all new dir names.
 *		start	- start of new dir.
 *		p	- end of new dir.
 *		ndir	- number of directories tested.
 *
 *	Action:	Set mode of all new directories created.
 *
 *	Return:	0 if all mode setups are successful, else -1.
 *
 *	Note:	None.
 */

int
setmod(ch, ndir, start, p)
char	* ch;
char	* start;
char	* p;
int	  ndir;
{
	char	* c;	/* pointer to end of dir string */
	/* Get pointer to end of string. */
	for(c = ch ; *c != '\0' ; ++c)
		;
	if (ndir > MAXDIR)
		ndir -= (MAXDIR - 1);
	/* Reset to mode desired. */
	while(--ndir > 0) {
		/* Change dir mode if new directory. */
		if ((new & 01L) == 1L)
			if (chmod(ch, mode) < 0) {
				write(2, ch, (c - ch));
				write(2, ": ", 2);
				write(2, sys_errlist[ errno ],
					strlen(sys_errlist[ errno ]));
				write(2, "\n", 1);
				return(-1);
			}
		new = new >> 1;
		/* Remove current dir name from string.	 */
		for(++p ; (--p >= start) && (*p != '/') ; *--c = '\0')
			;
		/* Remove trailing slashes. */
		for(++p ; (--p >= start) && (*p == '/') ; *--c = '\0')
			;
	}
	return(0);
}

/**
 * char *
 * getparent(pdir)
 * inpdir_t	* pdir;
 *
 *	Input:	pdir	- pointer to structure.
 *
 *	Action:	Get parent to directory currently working on.
 *
 *	Return:	Pointer to parent string.
 *
 *	Note:	None.
 */

char *
getparent(pdir)
inpdir_t	* pdir;
{
	int	found;		/* 0: parent found, -1: parent not found */
	int	size;		/* size of dir string in structure */
	char  * par;		/* pointer to parent string */
	char  * tail;		/* pointer to end of parent string */
	extern int	error();

	size = pdir->end - pdir->dir;
	if ((par = malloc(size + 3)) == NULL) {
		error("out of memory ", 0);
		exit(1);
	}
 	/* Copy structure string to parent and set tail to end of
	 * parent string.
	 */
	strcpy(par, pdir->dir);
	tail = par + size;
	/* Search for parent string. */
	do {
		/* Remove last dir name in string. */
		for(; (--tail >= par) && (*tail != '/') ; *tail = '\0')
			;
		/* Remove trailing slashes. */
		for(++tail ; (--tail > par) && (*tail == '/') ; *tail = '\0')
			;
		/** Substitute "." for null path. */
		if (++tail == par) {
			*tail = '.';
			*(tail + 1) = '/';
		}
		if ((found = access(par, 03)) == -1)
			switch(errno) {
			case ENOENT:
				break;
			case EACCES:
				error("no permission to mkdir in ", par, 0);
				exit(1);
			default:
				error("can't make ", pdir->dir, 0);
				exit(1);
			}
	} while((found == -1) && pflag);
	/* Set start of first child and pointer to end of first parent. */
	pdir->start = pdir->p = pdir->dir + (tail - par);
	return(par);
}

/**
 * char *
 * getchild(pdir)
 * inpdir_t	* pdir;
 *
 *	Input:	pdir	- pointer to structure.
 *
 *	Action:	Get current child dir name.
 *
 *	Return:	Pointer to child dir name.
 *
 *	Note:	If dir name found exceeds DIRSIZ in length, it will be
 *		truncated to DIRSIZ.
 */

char *
getchild(pdir)
inpdir_t	* pdir;
{
	static char	nam[ DIRSIZ + 1 ];	/* dir name of child */
	char	      * c;			/* pointer to end of child name */
	int		i, j;			/* working counters */
	/* Increment to start of next dir name. */
	for(i = 0, c = pdir->p ; (c < pdir->end) && (*c == '/') ; ++i, ++c)
		;
	/* Increment to end of dir name. */
	for(j = 0 ; (c < pdir->end) && (*c != '/') ; ++j, ++c)
		;
	pdir->c = c;
	/* Copy up to DIRSIZ length of child dir name to static memory. */
	if (j > DIRSIZ)
		j = DIRSIZ;
	strncpy(nam, pdir->p + i, j);
	nam[j] = '\0';
	return(nam);
}

/**
 * char *
 * getpath(pdir, dir)
 * inpdir_t	* pdir;
 * char		* dir;
 *
 *	Input:	pdir	- pointer to structure.
 *		dir	- current path name.
 *	Action:	Get current path name of child.
 *	Return:	Pointer to current path name.
 *	Note:	None.
 */

char *
getpath(pdir, dir)
inpdir_t	* pdir;
char		* dir;
{
	strncat(dir, pdir->p, (pdir->c - pdir->p));
	return(strcat(dir, "\0"));
}

/**
 * int
 * mkdir(parent, child)
 * char	* parent;
 * char	* child;
 *
 *	Input:	parent	- pointer to dir path name to parent.
 *		child	- pointer to dir path name to child.
 *	Action:	Make a directory. If the parent exists and is writeable,
 *		the directory and its "." and ".." links are created.
 *	Return:	0 if successful, 1 if dir existed already and dir is
 *		not the last dir in string. Else -1.
 *	Note:	None.
 */

int
mkdir(parent, child)
char	* parent;
char	* child;
{
	extern	char   * concat();
	extern	int	linkerr();
	extern	int	error();
	struct	passwd	*pw;

	/* Test if parent file is accessible by user. */
	if (access(parent, 03))
		switch(errno) {
		case ENOENT:
			error("parent dir ", parent, " doesn't exist", 0);
			return(-1);
		case EACCES:
			error("no permission to mkdir in ", parent, 0);
			return(-1);
		default:
			return(error("can't make ", child, 0));
		}
	/* Creat new directory. */
	if (mknod(child, IFDIR | 0777, 0)) {
		switch(errno) {
		case EEXIST:
			if (endflag)
				return(error(child, " already exists", 0));
			else
				return(1);
		case EPERM:
			return(error("not the super-user", 0));
		default:
			return(error("can't make ", child, 0));
		}
	}
	if (link(child, concat(child, "/.")))
		return(linkerr(child, "."));
	if (link(parent, concat(child, "/..")))
		return(linkerr(child, ".."));
	/* Set ownership of directory to user.	 */
	if ((pw = getpwnam("uucp")) == NULL)
		return -1;
	if (chown(child, pw->pw_uid, pw->pw_gid) < 0)
		return -1;
	return(0);
}

/**
 * char *
 * concat(s1, s2)
 * char	* s1, * s2;
 *	Input:	s1, s2	- strings to be concatenated.
 *	Action:	Join string s2 to string s1 to form a new string.
 *	Return:	Pointer to start of concatenation of `s1' and `s2'.
 *	Note:	None.
 */

char *
concat(s1, s2)
char	* s1;
char	* s2;
{
	static char	* str;
	if (str)
		free(str);
	/* Allocate memory space for concatenated string. */
	if ((str = malloc(strlen(s1) + strlen(s2) + 1)) == NULL) {
		error("out of memory", 0);
		exit(1);
	}
	strcpy(str, s1);
	return(strcat(str, s2));
}

/**
 * int
 * linkerr(dir, name)
 * char	* dir;
 * char	* name;
 *
 *	input:	dir	- directory where linking failed.
 *		name	- type of link failed, "." or "..".
 *	Action:	Recover from link failure. In the event that "." or ".."
 *		cannot be created, remove all traces of the directory.
 *	return:	-1.
 *	Note:	none.
 */

int
linkerr(dir, name)
char	* dir;
char	* name;
{
	extern char	* concat();
	extern int	  error();

	unlink(concat(dir, "/."));
	unlink(concat(dir, "/.."));
	unlink(dir);
	return(error("link to '", name, "' failed", 0));
}

/**
 * int
 * onintr()
 *
 *	Input:	None.
 *	Action:	Reset SIGINT and SIGHUP and set interrupt counter.
 *	Return:	Pointer to previous action on success, else -1.
 *	Note:	None.
 */

int
onintr()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	++interrupted;
}

/**
 * int
 * catch(sig)
 * 
 *	Input:	sig	- signal caught.
 *	Action:	Reset signals.
 *	Return:	Pointer to previous action on success, else -1.
 *	Note:	None.
 */

int
catch(sig)
{
	extern int	onintr();
	if (signal(sig, SIG_IGN) == SIG_DFL)
		signal(sig, onintr);
}

/**
 * int
 * error(arg1)
 * char	* arg1;
 *
 *	Input:	arg1	- error message string.
 *	Action:	Display error message.
 *	Return:	-1.
 *	Note:	None.
 */

int
error(arg1)
char	* arg1;
{
	register char ** p;
	write(2, "mkdir: ", 7);
	for(p = &arg1 ; *p != 0 ; ++p)
		write(2, *p, strlen(*p));
	write(2, "\n", 1);
	return(-1);
}

/**
 * int
 * readmode(s, mode)
 * char	* s;
 * int	  mode;
 *	
 *	Input:	s	- string containing setting changes.
 *		mode	- current setting of permission.
 *	Action:	Read in the symbolic mode and set the variables `who', `op',
 *		and `mode'. Knows about the old octal modes as well.
 *	Return:	New mode.
 *	Note:	None.
 */

int
readmode(s, mode)
register char * s;
int	mode;
{
	register int	c;
	register int	op;
	register int	m1, m2;
	extern void	fatal();
	extern void	checkmode();
	extern int	getumask();
	extern int	mrepl();

	if ((*s >= '0') && (*s <= '7')) {
		mode = 0;
		while(*s != '\0') {
			if ((*s < '0') || (*s > '7'))
				fatal(badom);
			mode = (mode << 3) | (*s++)-'0';
		}
		checkmode(mode);
		return(mode);
	}
newsym:
	for(m1 = 0 ;;) {
		switch(*s++) {
		case 'u': m1 |= AOWN; continue;
		case 'g': m1 |= AGRP; continue;
		case 'o': m1 |= AOTH; continue;
		case 'a': m1 |= AALL; continue;
		default : s--; break;
		}
		break;
	}
	if (m1 == 0)
		m1 = AALL & ~getumask();
newop:
	if (((c = *s++) == '=') || (c == '+') || (c == '-'))
		op = c;
	else
		fatal(badsm);
	for(m2 = 0 ;;) {
		switch(*s++) {
		case 'r': m2 |= AREAD;  continue;
		case 'w': m2 |= AWRITE; continue;
		case 'x': m2 |= AEXEC;  continue;
		case 's': m2 |= ASUID;  continue;
		case 't': m2 |= ATEXT;  continue;
		case 'u': m2 |= mrepl(mode & AOWN);	     continue;
		case 'g': m2 |= mrepl((mode & AGRP) << 3); continue;
		case 'o': m2 |= mrepl((mode & AOTH) << 6); continue;
		default : s--; break;
		}
		break;
	}
	switch(op) {
		case '-': mode &= ~(m1 & m2); break;
		case '+': mode |= m1 & m2;    break;
		case '=': mode  = (mode & ~m1) | (m1 & m2); break;
	}
	if (*s == '\0') {
		checkmode(mode);
		return(mode);
	}
	if ((*s == '+') || (*s == '-') || (*s == '='))
		goto newop;
	if (*s++ == ',')
		goto newsym;
	fatal(badsm);
}

/**
 * int
 * getumask()
 *
 *	Input:	None.
 *
 *	Action:	Get the value of the umask setting.
 *
 *	Return:		????
 *
 *	Note:	None.
 */

int
getumask()

{
	register int omask;

	omask = umask(0);
	umask(omask);
	return(omask);
}

/**
 * void
 * checkmode(mode)
 * int	mode;
 *
 *	Input:	mode	- current setup of permission.
 *	Action:	Check the mode to see if any problem bits are on.
 *		For now, this is S_ISVTX for non-super-users.
 *	Return:	None.
 *	Note:	None.
 */

void
checkmode(mode)
register int mode;
{
	static char stickwarn[] = 
		"mkdir: Warning: non-super user may not set sticky bit\n";
	if ((uid != 0) && (mode & S_ISVTX)) {
		write(2, stickwarn, 54);
		exit(1);
	}
}

/**
 * int
 * mrepl(m)
 * int	m;
 *	Input:	m	- mode setting to be replicated.
 *	Action:	Replicate the 3-bits of the mode from the owner
 *		position to all positions.
 *	Return:	Replicated mode setup.
 *	Note:	None.
 */

int
mrepl(m)
register int m;
{
	register int m1;

	m1 = m & AOWN;
	m  = m1 | (m1 >> 3) | (m1 >> 6);
	if (m1 & S_ISUID)
		m |= S_ISGID;
	return(m);
}
