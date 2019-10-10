/*
 * Print a trace error message and stop. exit on 'q'
 */
#include <scn.h>

Tracer(s)
char *s;
{
	static char buf[80];

	sprintf(buf, "Tracer: %r", &s);
	mvaddstr(15, 1, buf);
	refresh();
	if ('q' == getch()) {
		closeUp();
		exit(1);
	}
}
