main(argc, argv)
int argc; char *argv[];
{
	int i;
	for (i	= 1; i < argc; ) {
		printf("%s", argv[i]);
		if (++i < argc)
		putchar(' ');
	}
	putchar('\n');
	return 0;
}

