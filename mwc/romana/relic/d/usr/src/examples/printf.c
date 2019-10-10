void demo_r(string)
char *string;
{
	printf("%r\n", (char **)&string);
}

main()
{
	extern void demo_r();
	int precision = 1;
	int integer = 10;
	float decimal = 2.75;
	double bigdec = 27590.21;
	char letter = 'K';
	char buffer[20];

	strcpy (buffer, "This is a string.\n");

	printf("This is an int:   %d\n", integer);
	printf("This is a float:  %f\n", decimal);
	printf("Another float:    %3.*f\n", precision, decimal);
	printf("This is a double: %lf\n", bigdec);
	printf("This is a char:   %c\n", letter);
	printf("%s", buffer);
	printf("%s\n", "This is also a string.");

	demo_r("Print everything: %d %f %lf %c",
		integer, decimal, bigdec, letter);
}


