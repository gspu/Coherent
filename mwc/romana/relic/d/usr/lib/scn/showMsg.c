#include <scn.h>
#define CTRL(c) (c - '@')

/*
 * Show a message and wait.
 */
int
showMsg(s)
char *s;
{
	char work[80], c;

	if (NULL == errWindow)
		fatal("No error window");
	sprintf(work, "%r", &s);
	wmove(errWindow, 0, 0);
	wstandout(errWindow);
	waddstr(errWindow, work);
	wstandend(errWindow);
	wrefresh(errWindow);
	if (CTRL('C') == (c = wgetch(errWindow)))
		exit(0);
	return(c);
}
