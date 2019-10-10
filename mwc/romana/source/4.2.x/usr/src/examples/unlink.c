main(argc, argv)
int argc; char *argv[];
{
	int i;

	for (i = 1; i < argc; i++) {
		if (unlink(argv[i]) == -1) {
			printf("Cannot unlink \"%s\"\n", argv[i]);
			exit(1);
		}
	}
	exit(0);
}
