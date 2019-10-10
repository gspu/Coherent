/*
 * ask a question yes or no via stderr.
 */
#include <misc.h>

yn(question)
char * question;
{
	char reply[80];

	for (;;) {
		fprintf(stderr, "%r <y/n> ", &question);
		fflush(stderr);
		if (NULL == gets(reply))
			fatal("unexpected eof on stdin");
		switch (*reply) {
		case 'Y':
		case 'y':
			return (1);
		case 'n':
		case 'N':
			return (0);
		}
		printf("You must answer Y or N\n");
	}
}
#ifdef TEST
/*
 * This test is a usefull program.
 *
 * Inquire on all items in argv and then put out the approved ones.
 * An example of use is.
 * pr `yn *.[ch]`
 * This would inquire on all .c and .h files and send them through pr.
 */
main(argc, argv)
register char *argv[];
{
	register char *arg;

	while (NULL != (arg = *++argv))
		if (yn("%s", arg))
			puts(arg);
}
#endif
