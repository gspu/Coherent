/*
 * bio.c -- buffer handling code.
 *
 * This code REALLY assumes single-threaded execution.  All locks are
 * advisory only and may be revoked without notice.
 *
 * See also bread() in diskio.c.
 */

#include <sys/types.h>
#include <sys/buf.h>
#include <sys/stat.h>

#include "tboot.h"

#define NBUF	23	/* Number of disk buffers in cache.  */

static BUF bufl[NBUF];	/* Buffer structures.  */

static char blockp[NBUF * BLOCK];	/* The blocks themselves.  */
static int buf_inited = (1==2);		/* Has bufinit been called?  */


/*
 * Initialise buffer headers.
 */
void
bufinit()
{
	int i;		/* Counter for buffer structures.  */
	BUF *bp;	/* Handy pointer for buffer shorthand.  */
	paddr_t p;	/* Physical memory of the buffers themselves.  */

	sanity_check("Entering bufinit()");

	if (buf_inited) {
		puts("\r\nbufinit() called too many times.\r\n");
		puts("Harmlessly ignored.\r\n");
		return;
	}
	buf_inited = (1==1);
	
	/* We're going to chop blockp up into disk buffers.  */
	p = (paddr_t) blockp;

	for (i = 0; i < NBUF; ++i) {
		bp = &(bufl[i]);	/* Shorthand... */
		bp->b_dev = NODEV;	/* Buffer is not associated with a device.  */
		bp->b_bno = (daddr_t) 0;
		bp->b_paddr = p;	/* Attach a physical buffer.  */
		sanity_check("bufinit() about to call gate_unlock()");
		gate_unlock(bp->b_gate);	/* Unlock the buffer.  */

		p += BLOCK;		/* Point to the next buffer.  */
	}
	sanity_check("Leaving bufinit()");
} /* bufinit() */


/*
 * Claim a buffer for a block.
 * Finds the buffer if it has already been accessed, otherwise,
 * picks another buffer.
 */
BUF *
bclaim(block)
	daddr_t block;
{
	int i;
	BUF *bp;
	BUF *best_buf;

	/* Be sure to initialize buffers the first time.  */
	if (!buf_inited) {
		bufinit();
	}

	sanity_check("Start of bclaim");

	best_buf = (BUF *) NULL;

	/* Look to see if this block is cached.  */
	for (i = 0; i < NBUF; ++i) {
		bp = &(bufl[i]);

		/* If this is exactly the buffer we want, grab it!  */
		if (block == bp->b_bno) {
#if 0
			puts("!");	/* DEBUG buffer hit */
#endif /* 0 */
			best_buf = bp;
		}
	}

	/*
	 * If we couldn't find an exact match--pick another one.
	 */	
	if ((BUF *) NULL == best_buf) {
#if 0
		puts("?");	/* DEBUG buffer miss*/
#endif
		sanity_check("About to call bpick()");
		best_buf = bpick(1);	/* Fetch a new block.  */
		sanity_check("bclaim() from bpick() to gate_unlock()");
	}

	/* ASSERTION: At this point we have a buffer we can claim.  */


	gate_lock(best_buf->b_gate);

	sanity_check("Returning from bclaim()");
	return(best_buf);
} /* bclaim() */


/*
 * Pick the next buffer for allocation.
 * Uses a round-robin scheme of buffer allocation, skipping over
 * locked buffers.  If all buffers are locked, they are forcibly
 * unlocked and the first one is picked.
 */
BUF *
bpick()
{
	static int next_bp = NBUF;

	int i;

	BUF *retval;

	retval = (BUF *) NULL;	/* Nothing found yet.  */

	for (i = 0; ((BUF *)NULL == retval) && (i < NBUF); ++i) {
		/* Wrap next_bp back to the beginning if needed.  */
		next_bp %= NBUF;

		/* If we've found an unlocked buffer, return it.  */
		if (!gate_locked(bufl[next_bp].b_gate)) {
			/* Trash the contents.  */
			bufl[next_bp].b_dev = (dev_t) NODEV;
			bufl[next_bp].b_bno = (daddr_t) 0;

			sanity_check("bpick().2");
			retval = &(bufl[next_bp]);
		}
		++next_bp;
	}

	if ((BUF *)NULL == retval) {
		buf_inited = FALSE;	/* Force bufinit() to actually run.  */
		bufinit();
		sanity_check("bpick().1");
		retval = &(bufl[0]);
		next_bp = 1;
	}
	
	return(retval);
} /* bpick() */


/*
 * Release a buffer for a block.
 */
void
brelease(bp)
	BUF *bp;
{

	/* Make sure the buffer is unlocked.  */
	sanity_check("brelease() about to call gate_unlock()");
	gate_unlock(bp->b_gate);

	if (0 != (BFMOD & bp->b_flag)){
		puts("Warning:  I don't know how to brelease() a modified buffer.\r\n");
	}
	sanity_check("Returning from brelease()");
} /* brelease() */


/*
 * Attempt to lock the gate 'g'.
 * Return TRUE on success, FALSE if it was already locked.
 */
int
gate_lock(g)
	GATE g;
{
	if (g[0]) {
		return(FALSE);
	}

	g[0] = TRUE;

	return(TRUE);
} /* gate_lock() */


/*
 * Check to see if the gate 'g' is locked.
 */
int
gate_locked(g)
	GATE g;
{
	return(g[0]? TRUE : FALSE);
} /* gate_locked() */

/*
 * Unlock the gate 'g'.
 */
void
gate_unlock(g)
	GATE g;
{

	sanity_check("Start of gate_unlock()");

	g[0] = FALSE;

	sanity_check("End of gate_unlock()");
} /* gate_unlock() */

/*
 * Sanity checker.
 * Prints contents of "message" if something is amiss and dumps you
 * into the monitor.
 *
 * Add tests and messages as needed.
 */

void
sanity_check(message)
	char *message;
{
	extern char sects, heads;
	extern BUF bufl[];
	
	static int d_data_inited = FALSE;	/* Is disk data inited?  */
	static uint16 osects;			/* Correct number of sectors.  */
	static uint16 oheads;			/* Correct number of heads.  */

	int i;
	static int paddr_ok_once = FALSE;
	int paddr_ok;

	if (!d_data_inited) {
		osects = sects;
		oheads = heads;
		d_data_inited = TRUE;
	} else {
		/* Test to see if disk information has been stomped on.  */
		if (osects != sects) {
			puts(message);
			puts(": dead sects.\r\n");
			monitor();
		}
	
		if (oheads != heads) {
			puts(message);
			puts(": dead heads.\r\n");
			monitor();
		}
	}


	/*
	 * Don't flunk this test until it has been passed once.
	 * Check to see if the b_paddr entries for buffers have
	 * been stomped on.
	 */
	paddr_ok = TRUE;

	for (i = 0; i < NBUF; ++i) {
		if (0 == (uint16) (bufl[i].b_paddr)) {
			paddr_ok = FALSE;	/* Test failed.  */
			break;
		}
	}

	if (paddr_ok) {
		paddr_ok_once = TRUE;
	} else if (paddr_ok_once) {
		puts(message);
		puts(": bad paddr in buffer.\r\n");
		monitor();
	}

} /* sanity_check() */
