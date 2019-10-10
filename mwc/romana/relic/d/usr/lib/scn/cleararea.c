/*
 * Clear an area on the screen.
 */
#include <scn.h>
void
clearArea(row, col, len)
{
	move(row, col);
	while (len--)
		addch(' ');
}
