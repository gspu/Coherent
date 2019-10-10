/*
 * Show a message and get a reply.
 */
#include <scn.h>

Query(s)
char *s;
{
	char reply, work[80];

	if (NULL == errWindow)
		fatal("No error window");
	sprintf(work, "%r ", &s);
	wmove(errWindow, 0, 0);
	waddstr(errWindow, work);
	wrefresh(errWindow);
	reply = wgetch(errWindow);
	werase(errWindow);
	wrefresh(errWindow);
	return (reply);
}
