/*
 * File:	putchar.c
 *
 * $Log:	putchar.c,v $
 * Revision 1.2  92/08/04  12:54:17  bin
 * upd for ker59
 * 
 * Revision 1.4  92/06/15  13:06:59  root
 * Use putchar_init on (2,0) as well as async.
 * 
 * Revision 1.3  92/06/11  21:05:27  root
 * *** empty log message ***
 * 
 */
#include <sys/coherent.h>
#include <sys/inode.h>
#include <sys/stat.h>
#include <sys/con.h>
#include <sys/io.h>
#include <sys/devices.h>

#define P_LEN 1024
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
	if (!ok_to_use_dev) {
		if (p_off == P_LEN) {
			p_buf[P_LEN-1] = '*';	/* Mark an overrun.  */
		} else {
			p_buf[p_off++] = c;	/* Stash the character.  */
		}
		return;
	}

	if (coninit == 0) {
		++coninit;
		dopen(condev, IPW, DFCHR);
	}

	if (c == '\n')
		putchar('\r');

	iob.io_seg  = IOSYS;
	iob.io_ioc  = 1;
#ifdef _I386
	iob.io.vbase = &c;
#else
	iob.io_base = &c;
#endif
	iob.io_flag = 0;
	dwrite(condev, &iob);
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

	/* Dump everything we've stored away.  */
	for (i = 0; i < p_off; ++i)
		putchar(p_buf[i]);

	if ('*' == p_buf[P_LEN-1])
		printf("\npb buffer overrun detected.\n");
} /* putchar_init() */
