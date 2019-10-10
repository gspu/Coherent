#include <stdio.h>

main(argc,argv)
int argc; 
char *argv[];
{
	FILE *fp;
	int fd;

	if (argc !=2) {
		fprintf(stderr, "Usage: fileno filename\n");
		exit(1);
	}

	if ((fp = fopen(argv[1], "rw")) == NULL) {
		fprintf(stderr, "Cannot open input file %s\n",
		 argv[1]);
		exit(1);
	}

	fd = fileno(fp);
	printf("The file descriptor for %s is %d\n",
		argv[1], fd);
	exit(0);
}

