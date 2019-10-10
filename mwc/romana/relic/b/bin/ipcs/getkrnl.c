/*
 * get_kernel_vals(). This is code borrowed from Hal's asypatch command.
 * This code will get the following values from the kernel:
 *	NSHMID:		max number of allowable shared memory segments
 *	SEMMNI:		max number of allowable semaphores
 *	NMSQID:		max number of allowable message queues
 *
 * The name of the symbol and kernel file are passed from the calling
 * function.
 */

#include <stdio.h>
#include <coff.h>
#include <fcntl.h>

get_krnl_vals(symbl, fname)
char *symbl, *fname;
{

static SYMENT se;
int data;		/* buffer */
int ret;		/* return status of coffpatch */

	se._n._n_n._n_zeroes = 0;		/* stuff for coffnlist */
	se._n._n_n._n_offset = sizeof(long);
	se.n_type = -1;

	ret = coffnlist(fname, &se, symbl, 1); /* fill SYMENT struct */

	/* get values from kernel for requested symbol. Abort on failure */

	if(ret){
		ret = coffpatch(fname, &se, symbl, &data, sizeof(data), 1);
	}else{
		printf("Error obtaining value for {%s} from %s!\n",
			symbl, fname);
		exit(1);
	}

	return data;	/* return value of symbol found in kernel*/
}
