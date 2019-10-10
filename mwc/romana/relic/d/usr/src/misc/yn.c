/*
 * ask a question yes or no.
 */
#include <stdio.h>
yn(question)
char * question;
{
	char reply[80];

	for(;;) {
		printf("%r <y/n> ", &question);
		fflush(stdout);
		gets(reply);
		switch(*reply) {
		case 'Y':
		case 'y':
			return(1);
		case 'n':
		case 'N':
			return(0);
		}
		printf("You must answer Y or N\n");
	}
}
