/*
 * libc/sys/ustat.c
 */

#include <sys/types.h>

ustat(dev, buf) dev_t dev; struct ustat *buf;
{
	return _utssys(buf, dev, 2);
}

/* end of libc/sys/ustat.c */
