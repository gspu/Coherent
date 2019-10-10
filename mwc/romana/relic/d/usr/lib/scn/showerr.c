#include <scn.h>
static int errSw;

/*
 * Show any error.
 */
void
showError(s)
char *s;
{
	char work[80];

	if (NULL == errWindow)
		fatal("No error window");
	sprintf(work, "Error: %r", &s);
	wmove(errWindow, 0, 0);
	wstandout(errWindow);
	waddstr(errWindow, work);
	wstandend(errWindow);
	wrefresh(errWindow);
	errSw = 1;
}

/*
 * Get a char from screen, Erase error window if not clear.
 */
getChr()
{
	int c;

	c = getch();
	if (errSw) {
		wmove(errWindow, 0, 0);
		wclrtoeol(errWindow);
		wrefresh(errWindow);
		errSw = 0;
	}
	return (c);
}
