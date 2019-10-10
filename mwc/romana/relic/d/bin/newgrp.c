/*
 * newgrp groupname
 * this is exec'd by the shell
 * to make it work:
 *			chmod u+s /bin/newgrp
 */
#include <stdio.h>
#include <grp.h>

extern char **environ;

main(argc,argv,envp)
char **argv,**envp;
{
	register char *unam;
	register int *flag;
	register struct group *grp;
	char *cp;
	char *crypt(),*getpass(),*getlogin();
	int namect=0,inflg=0,outflg=0;

	environ=envp;
	if (argc<2)
		perrx("Usage: newgrp groupname");
	if ((grp=getgrnam(argv[1]))==NULL)
		perrx("non-existent group");
	unam=getlogin();
	while (*grp->gr_mem!=NULL) {
		if (**grp->gr_mem=='!') {
			++*grp->gr_mem;
			flag=&outflg;
		} else {
			flag=&inflg;
			++namect;
		}
		if (strcmp(*grp->gr_mem,unam)==0)
			++*flag;
		++grp->gr_mem;
	}
	if (outflg)
		perrx("access explicitly denied");
	if (!inflg && *grp->gr_passwd=='\0' && namect)
		perrx("not in access list");
	if (!inflg && *grp->gr_passwd!='\0') {
		if ((cp=getpass("Password:"))==NULL || *cp=='\0')
			perrx("no");
		cp=crypt(cp,grp->gr_passwd);
		if (strcmp(cp,grp->gr_passwd)!=0)
			perrx("no");
	}
	setgid(grp->gr_gid);
	setuid(getuid());
	execle("/bin/sh","-",NULL,environ);
}

perrx(s)
char *s;
{
	fprintf(stderr,"%s\n",s);
	setuid(getuid());
	execle("/bin/sh","-",NULL,environ);
}
