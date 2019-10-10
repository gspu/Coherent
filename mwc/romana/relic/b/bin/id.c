#include <stdio.h>
#include <pwd.h>
#include <grp.h>

main()
{
	register struct passwd	*pstp;
	register struct group	*psgr;
	int			ruid, 	/* real user id */
				rgid,	/* real group id */
				euid,	/* effective user id */
				egid;	/* effetcive group id */


	ruid = getuid();
	rgid = getgid();
	euid = geteuid();
	egid = getegid();
	
	if ((pstp = getpwuid(ruid)) == NULL) {
		perror("id");
		exit(1);
	}
	if ((psgr = getgrgid(rgid)) == NULL) {
		perror("id");
		exit(1);
	}
	printf("uid=%d(%s) gid=%d(%s)", ruid, pstp->pw_name, 
							rgid, psgr->gr_name);
	if (ruid != euid) {
		if ((pstp = getpwuid(euid)) == NULL) {
			perror("id");
			exit(1);
		}
		if ((psgr = getgrgid(rgid)) == NULL) {
			perror("id");
			exit(1);
		}
		printf(" euid=%d(%s) egid=%d(%s)\n", 
				euid, pstp->pw_name, egid, psgr->gr_name);
	} else
		putchar('\n');
} 
