/*
 * Set Up screen.
 */
#include <scn.h>

void
setUpScreen(lines, at)
int lines;	/* Number of lines devoted to error window */
int at;		/* Top line number of error window */
{
	initscr();	/* init curses */
	if (lines) {
		static char msg[] = "Cannot establish error window";

		if ((lines  + at) > LINES)
			fatal(msg);
		errWindow = newwin(lines, 80, at, 0);
		if (NULL == errWindow)
			fatal(msg);	
	}
	raw();	
	noecho();
}
