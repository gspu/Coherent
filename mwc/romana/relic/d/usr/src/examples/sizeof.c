char *arrayname[] = {
	"COHERENT", "Mark Williams C for the Atari ST",
	"Let's C", "Fast Forward"
};

main()
{
	printf("\"arrayname\" has %d entries\n",
		sizeof arrayname/sizeof (char*));
}

