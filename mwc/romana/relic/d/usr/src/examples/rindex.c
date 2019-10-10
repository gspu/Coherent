#include <stdio.h>
#define PATHSEP '/'	/* path name separator */

extern char *rindex();
char *basename();

main()
{
	char *testpath = "/foo/bar/baz"; 

	printf("Before massaging: %s\n", testpath);
	printf("After massaging: %s\n", basename(testpath));
}

char *basename(path)
char *path;
{
	char *cp;
	return (((cp = rindex(path, PATHSEP)) == NULL)
		 ? path : ++cp);
}

