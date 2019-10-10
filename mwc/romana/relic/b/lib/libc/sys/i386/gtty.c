/*
 * libc/sys/i386/gtty.c
 * Copyright (c) Bureau d'Etudes Ciaran O'Donnell,1987,1990,1991.
 */

#include <sgtty.h>

int
stty(u, v) int u, v;
{
	return ioctl(u, TIOCSETP, v);
}

int
gtty(u, v) int u, v;
{
	return ioctl(u, TIOCGETP, v);
}

/* end of libc/sys/i386/gtty.c */
