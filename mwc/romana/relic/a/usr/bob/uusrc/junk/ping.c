#include <stdio.h>
#include <signal.h>
FILE	*fmp;
FILE	*fp;
char	bigargs[1024];
char	exarg[BUFSIZ];
main(argc, argv)
int	argc;
char	**argv;
{
	char	*arg;
	int	retval;
	int	c;
	int	count;

	retval = atoi(argv[1]);
	(void) signal(SIGPIPE, SIG_IGN);
	strcpy(bigargs, "mail wgl ");
	if ((fmp = popen(bigargs, "w")) == NULL)
		exit(-1);
	fprintf(fmp, "Ping says: ");
	count = 1;
	while ((arg = *argv++) != NULL) {
		fprintf(fmp, "[%s]:\n ", arg);
		if (count++ > 1) {
			if ((fp = fopen(arg, "r")) == NULL)
				fprintf(fmp, "unable to open %s\n", arg);
			else {
				while ((c = getc(fp)) != EOF)
					putc(c, fmp);
				fclose(fp);
			}
		} else
			fprintf(fmp, "\n");
	}
	fprintf(fmp, "\n\n");
	exit(pclose(fmp) | retval);
}
