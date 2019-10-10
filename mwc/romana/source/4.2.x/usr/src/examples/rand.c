#define MAXVAL 32767	/* range of rand: [0,2^15-1] */

main(argc, argv)
int argc; char *argv[];
{
	register int i, hits, threshold, ntrials;

	hits = 0;
	threshold = (argc > 1) ? atoi(argv[1]) : MAXVAL/2;
	ntrials = (argc > 2) ? atoi(argv[2]) : 1000;
	if (argc > 3)
		srand(atoi(argv[3]));

	for (i = 1; i <= ntrials; i++)
		if (rand() > threshold)
			++hits;

	printf("%d values above %d in %d trials (%D%%).\n",
		hits, threshold, ntrials, (100L*hits)/ntrials);
}

