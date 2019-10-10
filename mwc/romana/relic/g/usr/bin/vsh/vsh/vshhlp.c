/*
 *      vsh: help functions
 *
 *      Copyright (c) 1992-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>

extern chtype seattrib;

do_copyright()
{
	extern char *copyright_box[];

	select_box(copyright_box, 1, seattrib);
}
