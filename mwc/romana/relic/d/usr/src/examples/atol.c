main()
{
	extern char *gets();
	extern long atol();
	char string[64];
	
	for (;;) {
		printf("Enter numeric string: ");
		if (gets(string))
			printf("%ld\n", atol(string));
		else
			break;
	}
	putchar('\n');
}

