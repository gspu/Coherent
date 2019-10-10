/*
 * Standard I/O Library
 * Close all files and call sys exit
 */

#include <stdio.h>

int
exit(s)
{
	_finish();
	_exit(s);
}
