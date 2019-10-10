/*
 * This file contains a
 * mini disc buffer cache manager.
 */
#include <stdio.h>
#include <discbuf.h>

DISCBUF	 *dblist;
FILE	 *dbfp;

/*
 * Claim a buffer and
 * read block `block' into it.
 * Return a pointer to the
 * buffer.
 */
DISCBUF	*
dbread(block)
long block;
{
	register DISCBUF *dbp;

	dbp = dbgrab(block);
	if ((dbp->db_flag&DB_OK) == 0) {
		lseek(fileno(dbfp), BUFSIZ*dbp->db_block, 0);
		if (read(fileno(dbfp), dbp->db_data, BUFSIZ) != BUFSIZ)
			dbfatal("cache read error, block %D", block);
		dbp->db_flag |= DB_OK;
	}
	return (dbp);
}

/*
 * This routine grabs a buffer
 * and declares it to be the contents
 * of block `block'.
 * The data in the block is set to
 * all zeros.
 */
DISCBUF *
dbzero(block)
long block;
{
	register DISCBUF *dbp;
	register char *p;
	register n;

	dbp = dbgrab(block);
	dbp->db_flag = DB_OK;
	p = dbp->db_data;
	n = BUFSIZ;
	do {
		*p++ = 0;
	} while (--n);
	return (dbp);
}

/*
 * Buffer grab.
 * Try to find a buffer that
 * has the block in it.
 * If not, use the oldest free
 * buffer.
 */
DISCBUF *
dbgrab(block)
long block;
{
	register DISCBUF *dbp1, *dbp2;
	register DISCBUF *fbp1, *fbp2;

	dbp1 = fbp1 = fbp2 = NULL;
	dbp2 = dblist;
	while (dbp2!=NULL && dbp2->db_block!=block) {
		if (dbp2->db_refc == 0) {
			fbp1 = dbp1;
			fbp2 = dbp2;
		}
		dbp1 = dbp2;
		dbp2 = dbp2->db_fp;
	}
	if (dbp2 == NULL) {
		if (fbp2 == NULL)
			dbfatal("cache full");
		dbp1 = fbp1;
		dbp2 = fbp2;
		if ((dbp2->db_flag&DB_DIRT) != 0)
			dbwrite(dbp2);
		dbp2->db_flag  = 0;
		dbp2->db_block = block;
	}
	++dbp2->db_refc;
	if (dbp1 != NULL) {
		dbp1->db_fp = dbp2->db_fp;
		dbp2->db_fp = dblist;
		dblist = dbp2;
	}
	return (dbp2);
}

/*
 * Flush the buffer cache. All
 * dirty buffers get written out to
 * the disc.
 * If `flag' is true the buffer
 * cache is released.
 */
dbflush(flag)
{
	register DISCBUF *dbp;
	register DISCBUF *fbp;

	dbp = dblist;
	while (dbp != NULL) {
		if ((dbp->db_flag&(DB_OK|DB_DIRT)) == (DB_OK|DB_DIRT))
			dbwrite(dbp);
		if (flag) {
			fbp = dbp;
			dbp = dbp->db_fp;
			free((char *) fbp);
		} else {
			dbp->db_flag = 0;
			dbp->db_refc = 0;
			dbp = dbp->db_fp;
		}
	}
	if (flag)
		dblist = NULL;
}

/*
 * Allocate a buffer cache.
 * The argument `nbuf' is the number
 * of buffers to claim.
 */
dbclaim(nbuf)
{
	register DISCBUF *dbp;

	if (dblist != NULL)
		dbfatal("already claimed");
	while (nbuf--) {
		if ((dbp = (DISCBUF *) malloc(sizeof(DISCBUF))) == NULL)
			dbfatal("no memory");
		dbp->db_flag = 0;
		dbp->db_refc = 0;
		dbp->db_fp   = dblist;
		dblist = dbp;
	}
}

/*
 * Write a buffer to the disc.
 */
dbwrite(dbp)
register DISCBUF *dbp;
{
	lseek(fileno(dbfp), BUFSIZ*dbp->db_block, 0);
	if (write(fileno(dbfp), dbp->db_data, BUFSIZ) != BUFSIZ)
		dbfatal("cache write error, block %D", dbp->db_block);
}

/*
 * Release a buffer.
 * The reference count is updated
 */
dbfree(dbp, flag)
register DISCBUF *dbp;
{
	dbp->db_flag |= flag;
	if (--dbp->db_refc < 0)
		dbfatal("refc < 0");
}

/*
 * Fatal errors.
 */
dbfatal(a)
{
	fprintf(stderr, "discbuf: %r\n", &a);
	exit(1);
}
