/*
 *      vsh: system functions
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#include <stdio.h>
#include <termio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include "vsh.h"

#define ARGINC		20		/* increment to raise argv */

#define sort_function(a,b,c,d) qsort(a,b,c,d)
extern void qsort();

static char **argv;			/* argument vector for execvp */
static int argc;			/* number used entrys in argv */
static int sarg;			/* number available entrys in argv */

#ifndef POSDIR
/*
 *	read all entrys from directory name and put them sorted
 *	into the file list fl
 */
get_files(fl, name, max)
struct filelist *fl;
char *name;
int max;
{
	int cmp();
	register int fd, i;
	register struct filelist *p;

	if ((fd = open(name, 0)) == -1) {
		close(fd);
		return(0);
	}
	p = fl;
	i = 0;
	while (read(fd, (char *) p->f_dir, sizeof(struct direct)) > 0 && i < max) {
		if (p->f_dir->d_ino == 0)
			continue;
		if (strcmp(p->f_dir->d_name, ".") == 0)
			continue;
		if (stat(p->f_dir->d_name, p->f_stat) != 0)
			continue;
		p->f_mflag = 0;
		i++;
		p++;
	}
	close(fd);
	sort_function((char *) fl, i, sizeof(struct filelist), cmp);
	return(i);
}

/*
 *	read directory fn and count number of files in to nf
 *	and number of directorys in to nd
 */
num_files(fn, nf, nd)
char *fn;
int *nf, *nd;
{
	static char path[MAXTMP];
	static struct stat file_st;
	static struct direct file_dir;
	register int fd;

	*nf = *nd = 0;
	if ((fd = open(fn, 0)) == -1) {
		close(fd);
		return;
	}
	while (read(fd, (char *) &file_dir, sizeof(struct direct)) > 0) {
		if (file_dir.d_ino == 0)
			continue;
		if (strcmp(file_dir.d_name, ".") == 0)
			continue;
		if (strcmp(file_dir.d_name, "..") == 0)
			continue;
		strcpy(path, fn);
		strcat(path, "/");
		strcat(path, file_dir.d_name);
		if (stat(path, &file_st) != 0)
			continue;
		if ((file_st.st_mode & S_IFMT) == S_IFDIR)
			*nd += 1;
		else
			*nf += 1;
	}
	close(fd);
}
#else
/*
 *	read all entrys from directory name and put them sorted
 *	into the file list fl
 */
get_files(fl, name, max)
struct filelist *fl;
char *name;
int max;
{
	int cmp();
	register int i;
	register struct filelist *p;
	register DIR *dirp;
	register struct dirent *dp;

	if ((dirp = opendir(name)) == NULL) {
		closedir(dirp);
		return(0);
	}
	p = fl;
	i = 0;
	while ((dp = readdir(dirp)) != NULL && i < max) {
		if (strcmp(dp->d_name, ".") == 0)
			continue;
		if (stat(dp->d_name, p->f_stat) != 0)
			continue;
		memcpy(p->f_dir, dp, SIZE_OF_DIR);
		p->f_mflag = 0;
		i++;
		p++;
	}
	closedir(dirp);
	sort_function((char *) fl, i, sizeof(struct filelist), cmp);
	return(i);
}

/*
 *	read directory fn and count number of files in to nf
 *	and number of directorys in to nd
 */
num_files(fn, nf, nd)
char *fn;
int *nf, *nd;
{
	static char path[MAXTMP];
	static struct stat file_st;
	register DIR *dirp;
	register struct dirent *dp;

	*nf = *nd = 0;
	if ((dirp = opendir(fn)) == NULL) {
		closedir(dirp);
		return;
	}
	while ((dp = readdir(dirp)) != NULL) {
		if (strcmp(dp->d_name, ".") == 0)
			continue;
		if (strcmp(dp->d_name, "..") == 0)
			continue;
		strcpy(path, fn);
		strcat(path, "/");
		strcat(path, dp->d_name);
		if (stat(path, &file_st) != 0)
			continue;
		if ((file_st.st_mode & S_IFMT) == S_IFDIR)
			*nd += 1;
		else
			*nf += 1;
	}
	closedir(dirp);
}
#endif

/*
 *      sub-routine for qsort to sort the directory and file names
 */
static int cmp(p1, p2)
struct filelist *p1, *p2;
{
	register unsigned int mode1 = p1->f_stat->st_mode & S_IFMT;
	register unsigned int mode2 = p2->f_stat->st_mode & S_IFMT;

	if (mode1 == mode2)
		return(strcmp(p1->f_dir->d_name, p2->f_dir->d_name));
	if (mode1 == S_IFDIR)
		return(-1);
	if (mode2 == S_IFDIR)
		return(1);
	return(strcmp(p1->f_dir->d_name, p2->f_dir->d_name));
}

/*
 *	compute size of all files in file list flp
 */
long size_files(flp, count)
register struct filelist *flp;
register int count;
{
	register long i = 0L;

	while (count--) {
		if ((flp->f_stat->st_mode & S_IFMT) == S_IFREG)
			i += flp->f_stat->st_size;
		flp++;
	}
	return(i);
}

/*
 *	compute size of all tagged files in file list flp
 */
long size_tfiles(flp, count)
register struct filelist *flp;
register int count;
{
	register long i = 0L;

	while (count--) {
		if (((flp->f_stat->st_mode & S_IFMT) == S_IFREG) && flp->f_mflag)
			i += flp->f_stat->st_size;
		flp++;
	}
	return(i);
}

/*
 *	reduce a string containing a path to lenght l and
 *	return a pointer to the new string
 */
char *reduce_path(fn, l)
char *fn;
int l;
{
	register char *p1, *p2, *p3;
	static char newfn[MAXTMP];

	strcpy(newfn, fn);
	p1 = strchr(newfn, '/');
	p1 = strchr(++p1, '/');
	p1 = strchr(++p1, '/');
	*++p1 = '.';
	*++p1 = '.';
	*++p1 = '.';
	p3 = ++p1;
	p2 = strchr(p1, '/');
	while (*p2)
		*p1++ = *p2++;
	*p1 = '\0';
	while (strlen(newfn) > l) {
		p1 = p3;
		p2 = strchr(p3+1, '/');
		while (*p2)
			*p1++ = *p2++;
		*p1 = '\0';
	}
	return(&newfn[0]);
}

/*
 *      output a message, flush input and wait for a key hit
 */
int waitkey()
{
	static struct termio old_term, new_term;
	extern char *msgs[];

	fprintf(stdout, "\n%s", msgs[0]);

	ioctl(0, TCGETA, &old_term);
	new_term = old_term;
	new_term.c_lflag &= ~(ICANON | ECHO);
	new_term.c_iflag &= ~(IXON | IXANY | IXOFF);
	new_term.c_iflag &= ~(IGNCR | ICRNL | INLCR);
	new_term.c_cc[VMIN] = 1;
	new_term.c_cc[VTIME] = 0;
	ioctl(0, TCSETAW, &new_term);
	ioctl(0, TCFLSH, 0);

	getchar();

	ioctl(0, TCSETAW, &old_term);
}

/*
 *	fork a new process, exec shell in the child and
 *	execute a command with the shell
 */
callsh(cmd)
char *cmd;
{
	extern unsigned alarm();
	extern char sh[];
	register int pid, rpid;
	int status;

	/* stop alarm timer */
	alarm(0);

	/* fork a child */
	if ((pid = fork()) < 0) {
		status = -1;

	/* child process */
	} else if (pid == 0) {
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGALRM, SIG_DFL);
#ifdef SIGWINCH
		signal(SIGWINCH, SIG_DFL);
#endif
		execlp(sh, sh, "-c", cmd, NULL);
		_exit(-1);

	/* parent process */
	} else {
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		while ((rpid = wait(&status)) != pid)
			;
	}
	return((status & 0xffff) >> 8);
}

/*
 *	create a new process and execute command with arguments
 */
callsystem(cmd, ioflag)
char *cmd;
int ioflag;
{
	extern unsigned alarm();
	register int pid, rpid;
	int status;

	/* stop alarm timer */
	alarm(0);

	/* fork child process */
	if ((pid = fork()) < 0) {
		status = -1;

	/* child process */
	} else if (pid == 0) {
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGALRM, SIG_DFL);
#ifdef SIGWINCH
		signal(SIGWINCH, SIG_DFL);
#endif
		if (ioflag) {
			freopen("/dev/null", "r", stdin);
			freopen("/dev/null", "w", stdout);
			freopen("/dev/null", "w", stderr);
		}
		execvp(cmd, argv);
		_exit(-1);

	/* parent process */
	} else {
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		while ((rpid = wait(&status)) != pid)
			;
	}
	return((status & 0xffff) >> 8);
}

/*
 *	add one argument to the argument array
 */
add_arg(arg)
char *arg;
{
	char *realloc(), *malloc();

	if (argv == (char **) 0) {
		if ((argv = (char **) malloc(ARGINC * sizeof(char *))) == (char **) 0)
			nomem();
		sarg = ARGINC;
	}
	if (++argc >= sarg) {
		sarg += ARGINC;
		if ((argv = (char **) realloc((char *) argv, sarg * sizeof(char *))) == (char **) 0)
			nomem();
	}
	if ((argv[argc-1] = malloc(strlen(arg)+1)) == NULL)
		nomem();
	strcpy(argv[argc-1], arg);
	argv[argc] = NULL;
}	

/*
 *	build argument array from a string
 */
char *build_arg(s)
register char *s;
{
	static char buf[MAXTMP];
	register char *p;

	while (*s) {
		while (isspace(*s))
			s++;
		p = &buf[0];
		while (!isspace(*s) && *s)
			*p++ = *s++;
		*p = '\0';
		add_arg(buf);
	}
	return(argv[0]);
}

/*
 *	free memory of argument array
 */
free_arg()
{
	void free();
	register char **p = argv;

	while (*p)
		free(*p++);
	free((char *) argv);
	argv = (char **) 0;
	argc = sarg = 0;
}

/*
 *	check if a command needs a shell to execute
 */
check_for_sh(s)
register char *s;
{
	if (strchr(s, '|'))
		return(1);
	if (strchr(s, '<'))
		return(1);
	if (strchr(s, '>'))
		return(1);
	if (strchr(s, ';'))
		return(1);
	if (strchr(s, '*'))
		return(1);
	if (strchr(s, '?'))
		return(1);
	if (strchr(s, '['))
		return(1);
	if (strchr(s, '`'))
		return(1);
	return(0);
}
