main()
{
	unsigned short foo, bar;

	bar = (unsigned short) -1;

	/*
	 * BUG
	 * ( foo = bar) is being treated as (long).
	 */
	if (((unsigned short)(foo = bar)) == (unsigned short) -1) {
		printf("Everything is OK.\n");
	} else {
		printf("Compiler bug.\n");
	}

} /* main() */
