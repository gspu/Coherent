/*
 *      PANIC: out of memory
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>

nomem()
{
	endwin();
	cursoron();
	fprintf(stderr, "Programm crashed, out of memory\n");
	exit(1);
}
