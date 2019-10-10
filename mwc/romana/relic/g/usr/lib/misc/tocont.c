/*
 * Enter <NL> to continue.
 */
#include <misc.h>

void
tocont()
{
	printf("Enter <NL> to continue ");
	fflush(stdout);

	for (;;) {
		switch (getchar()) {
		case '\n':
			return;
		case EOF:
			fatal("Unexpected EOF");
		}
	}
}

#ifdef TEST
main()
{
	for (;;) {
		tocont();
	}
}
#endif
