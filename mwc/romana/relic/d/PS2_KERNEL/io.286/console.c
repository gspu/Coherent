/* $Header: /kernel/kersrc/io.286/console.c,v 1.1 92/07/17 15:24:08 bin Exp Locker: bin $ */
/*
 * Tiny console driver.
 * 8086/8088 Coherent, IBM PC.
 *
 * $Log:	console.c,v $
 * Revision 1.1  92/07/17  15:24:08  bin
 * Initial revision
 * 
 * Revision 2.1	88/09/03  13:03:39	src
 * *** empty log message ***
 * 
 * Revision 1.1	88/03/24  17:04:25	src
 * Initial revision
 * 
 * 86/11/19	Allan Cornish		/usr/src/sys/i8086/drv/console.c
 * putchar() initializes the (new) (IO).io_flag field to 0.
 */
#include <sys/coherent.h>
#include <sys/inode.h>
#include <sys/stat.h>
#include <sys/con.h>
#include <sys/io.h>

dev_t condev = makedev(2,0);

putchar(c)
int c;
{
	static coninit;
	IO iob;

	if (coninit == 0) {
		++coninit;
		dopen( condev, IPW, DFCHR );
	}

	if (c == '\n')
		putchar('\r');

	iob.io_seg  = IOSYS;
	iob.io_ioc  = 1;
	iob.io_base = &c;
	iob.io_flag = 0;
	dwrite( condev, &iob );
}
