/*
 * File:	bufq.c
 *
 * Purpose:
 *	Queueing routines for SCSI driver.
 *	Should be generalizable for other hard drives.
 *
 * $Log:	bufq.c,v $
 * Revision 1.2  91/05/21  23:23:36  hal
 * Enhanced debug printout.
 * 
 * Revision 1.1  91/05/21  13:54:11  root
 * First running version.
 * 
 */

/*
 * Includes.
 */
#include <sys/coherent.h>
#include <sys/buf.h>

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
typedef struct {
	BUF	* head;	/* point to first node */
	BUF	* tail;	/* point to last node */
	int	count;	/* number of nodes in the queue */
} bufq_type;

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
static int	num_q;		/* number of queues in use */
static bufq_type  * bufq_q;	/* pointer to allocated queue structs */

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int bufq_init();
void bufq_rlse();
void bufq_wr_tail();
BUF * bufq_rd_head();
BUF * bufq_rm_head();

/*
 * Debug macros.
 */
#if (DEBUG >= 3)
#define QSIZE	printf("Q%d:%d ", s_id, bqp->count)
#else
#if (DEBUG >= 2)
#define QSIZE	{if (bqp->count>1)printf("Q%d:%d ", s_id, bqp->count);}
#else
#define QSIZE
#endif
#endif

/*
 * bufq_init()
 *
 * Set up the desired number of queues.
 *
 * Return 1 if ok, 0 if kalloc() failed.
 */
int bufq_init(qcount)
int qcount;
{
	int ret;

	if (qcount > 0 && (bufq_q = kalloc(qcount*sizeof(bufq_type)))) {
		ret = 1;
		kclear(bufq_q, qcount*sizeof(bufq_type));
		num_q = qcount;
#if (DEBUG >= 2)
printf("%d queues allocated\n", qcount);
#endif
	} else
		ret = 0;

	return ret;
}

/*
 * bufq_rlse()
 *
 * Deallocate buffer queue structs.
 */
void bufq_rlse()
{
	num_q = 0;
	if (bufq_q)
		kfree(bufq_q);
}

/*
 * bufq_wr_tail()
 *
 * Append a BUF object to the doubly-linked queue.
 * Object to be inserted has been allocated by the caller.
 * Run at high priority.
 */
void bufq_wr_tail(s_id, bp)
int s_id;
BUF * bp;
{
	int s;
	bufq_type * bqp;

	if (s_id < num_q) {
		bqp = bufq_q + s_id;
		s = sphi();
		if (bqp->count == 0) {
			bqp->head = bqp->tail = bp;
			bp->b_actf = bp->b_actl = NULL;
		} else {
			bqp->tail->b_actf = bp;
			bp->b_actf = NULL;
			bp->b_actl = bqp->tail;
			bqp->tail = bp;
		}
		bqp->count++;
QSIZE;
		spl(s);
	}
}

/*
 * bufq_rd_head()
 *
 * Nondestructively fetch the head entry in the queue - i.e., this routine
 * does not remove an entry from the queue (see ss_rm_head() for that).
 * Return NULL if queue is empty, else return pointer to head item.
 */
BUF * bufq_rd_head(s_id)
int s_id;
{
	bufq_type * bqp;

	if (s_id < num_q) {
		bqp = bufq_q + s_id;
		return bqp->head;
	} else
		return NULL;
}

/*
 * bufq_rm_head()
 *
 * Delete head item from the queue.  Return a pointer to the node deleted,
 * or NULL if the queue was already empty.
 * Run at high priority.
 *
 * This routine does NOT deallocate the node.  That must be done by the
 * calling function after this routine runs.
 */
BUF * bufq_rm_head(s_id)
int s_id;
{
	BUF * ret;
	int s;
	bufq_type * bqp;

	if (s_id < num_q) {
		bqp = bufq_q + s_id;
		s = sphi();
		if (bqp->count > 0) {
			ret = bqp->head;
			if (bqp->count == 1) {
				bqp->head = bqp->tail = NULL;
			} else {
				bqp->head = bqp->head->b_actf;
				bqp->head->b_actl = NULL;
			}
			bqp->count--;
QSIZE;
		} else
			ret = NULL;
		spl(s);
	} else
		ret = NULL;

	return ret;
}
