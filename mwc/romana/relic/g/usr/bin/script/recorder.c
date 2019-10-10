/*
 * File:	recorder.c
 *
 * Purpose:	recording proces
 *
 * $Log:	recorder.c,v $
 * Revision 1.3  93/02/04  12:02:49  bin
 * hal: changed from sys/fcntl.h to fcntl.h in getps* , changed from execvp
 * to execvpe in recorder.c
 * 
 */

/*
 * Includes.
 */
#include <stdio.h>
#include <errno.h>
#include <signal.h>

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define BSIZE	512

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
void exec_shell();
void pass_all();
static void sig_term();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
static int sigcaught;

/*
 * exec_shell()
 */
void
exec_shell(fd, argv, envp)
int fd;
char ** argv, ** envp;
{
	char * shell;
	char * genenv(), * rindex();

	close(0);
	close(1);
	close(2);
	if (dup(fd) != 0 || dup(fd) != 1 || dup(fd) != 2) {
		/*  dup failed */
		exit (1);
	}

	if (*argv == NULL) {
		char * args[2];
		args[1] = NULL;
		if ( (shell = getenv("SHELL")) == NULL)
			shell = "/bin/sh";
		args[0] = shell;
		execvpe(shell, args, envp);
#if 0
		if ( (shell = getenv("SHELL")) == NULL)
			shell = "/bin/sh";
		if ( (argv[0] = rindex(shell, '/')) != NULL)
			argv[0]++;
		else
			argv[0] = shell;
		execvpe(shell, argv, envp);
#endif
	} else
		execvpe(argv[0], argv, envp);

	/* execvpe failed */
	exit (1);
}

/*
 * pass_all()
 */
void
pass_all(fd, lf)
int fd;
char * lf;
{
	int newpid, nread;
	char buff[BSIZE];
	int ppid = getpid();

	if ( (newpid = fork()) < 0) {
		/* pass_all can't fork */
		exit (1);
	} else if (newpid == 0) {
		/*
		 * Child process.
		 * Copy terminal stdin to fd for pty master.
		 */
		for (;;){
			nread = read(0, buff, BSIZE);
			if (nread < 0) {
				/* pass_all read err on stdin */
				exit (1);
			}
			if (nread == 0)
				break;	/* EOF */
			if (write(fd, buff, nread) != nread) {
				/* pass_all write err to fd */
				exit(1);
			}
		}
		kill(ppid, SIGTERM);
		exit(0);
	}

	/*
	 * Parent process.
	 * Copy from pty master fd to terminal stdout.
	 */
{
	int lfd;

	if((lfd = creat(lf, 0666)) == -1) {
		printf("Can't open log file \"%s\"\n", lf);
		goto bye;
	}
	sigcaught = 0;
	signal(SIGTERM, sig_term);
	for (;;) {
		nread = read(fd, buff, BSIZE);
		if (nread <= 0)
			break;	/* error or EOF */
		write(lfd, buff, nread);
		if (write(1, buff, nread) != nread) {
			/* pass_all write error to stdout */
			close(lfd);
			exit (1);
		}
	}
	close(lfd);
bye:
	if (sigcaught == 0) {
		kill(newpid, SIGTERM);
	}
}
}

void
sig_term()
{
	sigcaught = 1;
}
