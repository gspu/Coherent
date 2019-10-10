/* $Header: /src386/kernel/io.386/RCS/console.c,v 1.3 92/08/28 09:02:49 bin Exp Locker: bin $ */
/*
 * Tiny console driver.
 * 8086/8088 Coherent, IBM PC.
 *
 * $Log:	console.c,v $
 * Revision 1.3  92/08/28  09:02:49  bin
 * update by hal for kernel 61
 * 
 * Revision 1.2  92/01/06  12:26:44  hal
 * Compile with cc.mwc.
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

#define P_LEN 512	/* Keep about six full lines of characters.  */
/*
 * Stash messages in p_buf[] until we can print them.
 * Note that p_buf[] can not be safely kalloc()'d because even
 * those routines could generate printf()'s.
 */
static char	p_buf[P_LEN];
static int	p_off;	/* Offset into p_buff.  */
int ok_to_use_dev = 0;	/* Can we use the console device yet?  */

int coninit = 0;
dev_t condev = makedev(2,0);

putchar(c)
int c;
{
	IO iob;


	/*
	 * If we get a printf before the devices have been initialized,
	 * keep as many characters as we can until we can print them.
	 */
	if (condev != makedev(2,0) && !ok_to_use_dev) {
		if (p_off == P_LEN) {
			p_buf[P_LEN-1] = '*';	/* Mark an overrun.  */
		} else {
			p_buf[p_off++] = c;	/* Stash the character.  */
		}
		return;
	}

	if (coninit == 0) {
		++coninit;
		dopen( condev, IPW, DFCHR );
	}

	if (c == '\n')
		putchar('\r');

	iob.io_seg  = IOSYS;
	iob.io_ioc  = 1;
	iob.io.vbase = &c;
	iob.io_flag = 0;
	dwrite( condev, &iob );
} /* putchar() */


/*
 * putchar_init() is called from main() once devices have been
 * initialized.  It marks the condev as usable, and then prints
 * anything that has been stored away.
 */
putchar_init()
{
	int i;

	/* Mark condev as usable.  */
	ok_to_use_dev = 1;

	/* nothing to do if CRT is console device */
	if (condev == makedev(2,0))
		return;

	/* Dump everything we've stored away.  */
	for (i = 0; i < p_off; ++i) {
		putchar(p_buf[i]);
	}
	
	printf("\n-\n");
	if ('*' == p_buf[P_LEN-1]) {
		printf("\npb buffer overrun detected.\n");
	}
} /* putchar_init() */
