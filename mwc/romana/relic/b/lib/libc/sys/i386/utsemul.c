/*
 * libc/sys/i386/utsemul.c
 * Copyright (c) Bureau d'Etudes Ciaran O'Donnell, 1987, 1990, 1991.
 */

#include <sys/types.h>
#include <sys/utsname.h>

int
uname(name) struct utsname *name;
{
	return _utssys(name, 0, 0);
}

ustat(dev, buff) dev_t dev; struct ustat *buff;
{
	return _utssys(buff, dev, 2);
}

/* end of libc/sys/i386/utsemul.c */
