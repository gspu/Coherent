/*
 * close up curses.
 */
#include <scn.h>
void
closeUp()
{
	noraw();
	echo();
	clear();
	move(0, 0);
	refresh();
	endwin();
}
