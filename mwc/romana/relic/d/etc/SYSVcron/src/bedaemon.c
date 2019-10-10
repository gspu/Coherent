/* 
 * Disassociate from controling terminal and process
 * group. Close all open files.
 */
#include	<stdio.h>
#include	<signal.h>
#include 	<sys/param.h>

bedaemon()
{
	int	child_pid;
	int	fd;
	
	/* cron does its own sigsetup. Do it to be standard module for all 
	 * daemon processes do this extra signal here
	 */
	signal(SIGHUP, SIG_IGN);	/* immune from the group leader death */

	/* When we did not start from background fork and let parent die. */
	if ((child_pid = fork()) < 0) {
		fprintf(stderr, "cron: cannot fork\n");
		exit(1);
	} else
		if (child_pid > 0)	/* parent */
			exit(0);
	
	/* Disassociate from controlling terminal & process group */
	setpgrp();
	
	/* This fork should garantee that process can't reacquire a new 
	 * controlling terminal.
	 */
	if ((child_pid = fork()) < 0) {
		fprintf(stderr, "cron: cannot fork\n");
		exit(1);
	} else
		if (child_pid > 0)	/* parent */
			exit(0);
	
	/* Close all open files, chdir "/", and umask(0). */
	for (fd = 0; fd < NOFILE; fd++)
		close(fd);
	
	chdir("/");
	umask(0);	/* We do not want any surprise with file creation */
}
