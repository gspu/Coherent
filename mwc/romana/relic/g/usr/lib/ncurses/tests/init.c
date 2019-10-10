/*
 * test ncurses.
 */
#include <curses.h>
#include <signal.h>

static char *diemsg = "Signal recieved";

die()
{
	noraw();
	echo();
	signal(SIGINT, SIG_IGN);
	mvcur(0, COLS - 1, LINES - 1, 0);
	endwin();
	printf("%s\n", diemsg);
	exit(0);
}

rg(win)
WINDOW *win;
{
	if (!memok()) {
		diemsg = "memok trouble";
		die();
	}
	wrefresh(win);
	if ('q' == getch())
		die();
}

main()
{
	register WINDOW *win, *win2;
	int c;

	initscr();
	raw();
	noecho();
	signal(SIGINT, die);

	box(stdscr, '|', '-');
	refresh();
	win = newwin(LINES - 2, COLS - 2, 1, 1);
	wmove(win, 10, 10);
	waddstr(win, "134567890 missing ");
	waddch(win, '2');
	rg(win);

	wmove(win, 10, 11);
	winsch(win, '2');
	wmove(win, 10, 20);
	wclrtoeol(win);
	wmove(win, 11, 10);
	waddstr(win, "2 inserted");
	rg(win);

	wmove(win, 10, 11);
	c = winch(win);
	wdelch(win);
	wmove(win, 11, 10);
	wstandout(win);
	wprintw(win, "%c found at 11, 10 and removed", c);
	wstandend(win);
	rg(win);

	wmove(win, 11, 10);
	winsertln(win);
	rg(win);

	delwin(win);
	touchwin(stdscr);
	rg(stdscr);

	win  = newwin(5, 20, 1, 1);
	win2 = newwin(5, 20, 10, 10);
	waddstr(win, "A long line in a small window");
	wmove(win2, 2, 0);
	waddstr(win2, "Window 2");
	wrefresh(win2);
	rg(win);

	mvwin(win, 15, 10);	/* move the window */
	touchwin(stdscr);	/* refresh whats below */
	refresh();
	touchwin(win2);
	wrefresh(win2);
	rg(win);		/* show moved copy */

	clear();
	scrollok(stdscr, TRUE);
	setscrreg(2, LINES-4);
	mvaddstr(2, 2, "scroll this out");
	mvaddstr(3, 2, "this goes to the top");
	mvaddstr(LINES-4, 2, "bottom of scroll region");
	move(LINES-7, 2);
	rg(stdscr);

	scroll(stdscr);
	rg(stdscr);

	move(10, 0);
	clrtobot();
	move(2, 0);	
	deleteln();
	rg(stdscr);

	mvaddstr(2, 0, "line 1\nline 2\nline 3");
	move(2, 0);	
	deleteln();
	rg(stdscr);

	erase();
	rg(stdscr);

	diemsg = "normal end";
	die();
}
