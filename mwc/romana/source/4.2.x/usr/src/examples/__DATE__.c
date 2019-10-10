main()
{
	printf("Date: %s\n", __DATE__);
	printf("Time: %s\n", __TIME__);
	printf("File: %s\n", __FILE__);
	printf("Line No.: %d\n", __LINE__);

	printf("ANSI C? ");
#ifndef __STDC__
	printf("no\n");
#else
	printf("ANSI C? %s(%d)\n", __STDC__ ? "Yes" : "No", __STDC__);
#endif /* _defined(__STD_C__) */
}

