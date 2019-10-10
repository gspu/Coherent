/*
 * libc/sys/i386/setpgrp.c
 * Copyright (c) Bureau d'Etudes Ciaran O'Donnell,1987,1990,1991.
 */

int
getpgrp()
{
	return _pgrp(0);
}

int
setpgrp()
{
	return _pgrp(1);
}

/* end of libc/sys/i386/setpgrp.c */
