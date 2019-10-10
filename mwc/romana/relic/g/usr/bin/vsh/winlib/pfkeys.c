/*
 *      Functions for programmable function keys
 *
 *      Copyright (c) 1993 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include "winfun.h"

struct pfkey pfkeys[9];                   /* function keys */

/*
 *      display the function key lables in last line
 */
print_pfkeys()
{
	register int i;
	register char *p;
	char empty[PFKLABLEN];
	char space[80];

	/* compute free space between the labels:        */
	/* 9 labels of size PFKLABLEN and 8 empty spaces */
	p = space;
	i = (COLS - ((PFKLABLEN + 1) * 9)) / 8;
	while (i--)
		*p++ = ' ';
	*p = '\0';

	/* create the string for one empty label */
	p = &empty[0];
	i = PFKLABLEN;
	while (i--)
		*p++ = ' ';
	*p = '\0';

	/* output all labels */
	move(LINES-1, 0);
	for (i = 1; i <= 9; i++) {
		printw("%1d", i);
		attrset(A_REVERSE);
		if (*pfkeys[i-1].label)
			addstr(pfkeys[i-1].label);
		else
			addstr(empty);
		attrset(A_NORMAL);
		if (i < 9)
			addstr(space);
	}
	
	refreshall();
}
