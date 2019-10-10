#include <stdio.h>
#include "dirent.h"


main (argc, argv)
int argc;
char **argv;

{
   	DIR *dirp;
	struct dirent *dp;
	char name[30];
	int i;

	if (argc) 
		strcpy (name, argv[1]);
	else
		strcpy (name, "testdir");


	printf("\nDIRENTBASESIZ = %d.\n\n",(int) DIRENTBASESIZ);
	printf("before opendir(%s)\n",name);
	if (dirp = opendir (name))
		printf("Opened %s.\n",name);
	else {
		printf("Can't open %s.\n",name);
		exit(1);
	}

	printf ("Entry\tinode\toffset\treclen\tname\n\n");

	for (i=1, dp=readdir(dirp); dp != NULL ;i++, dp=readdir(dirp) ) {
	   if (dp ) {	
		printf ("%d\t",i);
		printf ("%ld\t",dp->d_ino);
		printf ("%ld\t",dp->d_off);
		printf ("%d\t",dp->d_reclen);
		printf ("%s\n",dp->d_name);
	   } else {
		printf ("Error reading direct.\n");
		break;
	   }
#if DBG
	printf("Done");
#endif	
	}
}
