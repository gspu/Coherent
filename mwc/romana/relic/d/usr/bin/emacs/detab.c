/*
 * Turn tabs to spaces.
 */
#include <stdio.h>

main(argc, argv)
char **argv;
{
	extern char *getenv();
	register int tabsiz, loc, to, c;
	char *env;

	switch (argc) {
	case 1:		/* no argument */
		if (NULL == (env = getenv("TABSIZ")))
			tabsiz = 8;
		else
			tabsiz = atoi(env);
		break;
	case 2:		/* 1 argument */
		tabsiz = atoi(argv[1]);
		break;
	default:
		fprintf(stderr, "usage: detab tabsize\n");
		exit(1);
	}

	for (loc = 0;;) {
		switch (c = getchar()) {
		case EOF:
			exit(0);
		case '\n':
		case '\r':
			loc = -1;	/* next char will be at 0 */
			break;
		case '\a':
			loc--;		/* alarm takes no space */
			break;
		case '\b':
			loc -= 2;	/* backspace */
			break;
		case '\t':
			for (to = loc + tabsiz - (loc % tabsiz); loc < to; loc++)
				putchar(' ');
			continue;
		}
		loc++;
		putchar(c);
	}
}
