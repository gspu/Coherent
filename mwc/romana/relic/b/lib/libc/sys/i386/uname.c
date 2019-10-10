/*
 * libc/sys/uname.c
 */

#include <sys/utsname.h>

uname(name) struct utsname *name;
{
	return _utssys(name, 0, 0);
}

/* end of libc/sys/uname.c */
