/* 
 * The example shows the usage of getuid(), geteuid() COHERENT SYstem calls
 * and getpwnam, getpwuid, setpwent, getpwent, and endpwent functions.
 */
#include	<pwd.h>
#include	<stdio.h>

main()
{
	int		euid,		/* Effective user id */
			ruid;		/* Real user id */
	struct passwd	*pstp;	
	int		i;

	/* Print out all users and  home directories */
	i = 0;
	setpwent();	/* Rewind file /etc/passwd */
	while ((pstp = getpwent()) != NULL) 
		printf("%d: user name is %s, home directory is %s.\n",
					++i, pstp->pw_name, pstp->pw_dir);

	/* Find real user name. 
	 *
	 * NOTE: functions getpwuid and getpwnam will rewind /etc/passwd file
	 * by calling setpwent.
	 */
	ruid = getuid();
	if ((pstp = getpwuid(ruid)) == NULL) 
/* If this message come out too bad. Possible file /etc/passwd corrupted. */
		fprintf(stderr, "Cannot find user with id number %d\n", pstp);
	else
		printf("Real user name is %s\n", pstp->pw_name);

	/* Find the user id for superuser root */
	((pstp = getpwnam("root")) == NULL) ?
		fprintf(stderr, "Do you have user root on your system?\n") :
		printf("root id is  %d\n", pstp->pw_uid);

	/* Check is the effective process id is the superuser id *
	 * 
	 * NOTE: if you would like to see how to enable root prileges,
	 * you can run next command from command line:
	 * cc pwfun.c
	 * su root chown root pwfun
	 * su root chmod 4511 pwfun
	 */
	euid = geteuid();
	printf("%d %d\n", ruid, euid);
	printf("Process ");
	(euid == pstp->pw_uid) ? printf("has ") : printf("doesn't have ");
	printf("the root priveleges\n");
}
