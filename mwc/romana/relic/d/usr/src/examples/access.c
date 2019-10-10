#include <access.h>
#include <stdio.h>

main(argc, argv)
int argc; char *argv[];
{
	int mode;
	extern int access();

	if (argc != 3) {
		fprintf(stderr, "Usage: access filename mode\n");
		exit(1);
	}
	switch (*argv[2]) {
	case 'x':
		mode = AEXEC;
		break;
	case 'w':
		mode = AWRITE;
		break;
	case 'r':
		mode = AREAD;
		break;
	default:
		fprintf(stderr, 
			"modes: x = execute, w = write, r = read\n");
		exit(1);
		break;
	}
	if (access(argv[1], mode)) {
		printf("file %s not found in mode %d\n", argv[1], mode);
		exit(0);
	} else
		printf("file %s accessible in mode %d\n",
				argv[1], mode);
	exit(0);
}
			
