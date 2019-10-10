/*
 * ask for a string and return a reply.
 */
#include <stdio.h>

char *
ask(reply, msg)
char *reply, *msg;
{
	printf("Enter: %r ", &msg);
	fflush(stdout);
	return (gets(reply));
}
