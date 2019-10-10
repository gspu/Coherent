#include <curses.h>
#include <ctype.h>
#define	NORMAL 0
#define	INY  1
#define	INX  2
main()
{
     int c, y, x, state;
     initscr();		 /* initialize curses */
     noecho();
     raw();
     clear();
     move(0, 0);
     for(state = NORMAL;;) {
	 refresh();
	 c = getch();
	 if(isdigit(c))	{
		 switch	(state)	{
		 case NORMAL:
			  y = x	= 0;
			  state	= INY;
		 case INY:
			  y *= 10;
			  y += c - '0';
			  break;
		 case INX:
			  x *= 10;
			  x += c - '0';
		 }
	 } else	{
		 if (3 == c) { /* ctrl-C	*/
			  noraw();
			  echo();
			  endwin();
			  exit(0);
		 }
		 switch	(state)	{
		 case INX:
			  state	= NORMAL;
			  move(y, x);
		 case NORMAL:
			  addch(c);
			  break;
		 case INY:
			  state	= INX;
		 }
	 }
     }
}

