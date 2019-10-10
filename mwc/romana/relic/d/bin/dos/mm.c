char *base[] = {"1234567890" , "123456*890", "?234567890", "123456789*"};

main()
{
	int i;
	
	for (i=0; i<4; i++)
		printf("strcspn(%s, \"*?\") = %d, strlen(%s) = %d\n", base[i],
			 strcspn(base[i], "*?"), base[i], strlen(base[i]));
}
