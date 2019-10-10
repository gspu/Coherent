/*
 * show the background for a screen.
 */
#include <scn.h>

void
wshowBak(w, back)
WINDOW *w;
register backGrnd *back;
{
	register char *p;

	for (; NULL != (p = back->data); back++)	/* put out background */
		mvwaddstr(w, back->row, back->col, p);
	refresh();
}
