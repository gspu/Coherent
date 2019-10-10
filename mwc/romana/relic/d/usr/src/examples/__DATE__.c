main()
{
	printf("Date: %s\n", __DATE__);
	printf("Time: %s\n", __TIME__);
	printf("File: %s\n", __FILE__);
	printf("Line No.: %d\n", __LINE__);
	printf("ANSI C? %s\n", __STDC__ ? "Yes" : "No");
}

