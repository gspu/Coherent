main()
{
	long nowhere;	 /* place to put unused	data */
	int counter;

	srand((int) time(&nowhere));
	for (counter =	0; counter < 20; counter++)
		printf("%d\n", rand()%10);
}


