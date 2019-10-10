/*
 * Handle the indirections in Unix-style file system.
 *
 * Uses a recursive scheme to follow up indirections.
 *
 * Needs optimization.  A good place to start would be caching of
 * lookup tables.
 *
 * La Monte H. Yarroll <piggy@mwc.com>, September 1991
 */

#include <sys/types.h>
#include <sys/buf.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <canon.h>
#include <sys/ptypes.h>

extern BUF *bread();
daddr_t vmap();
daddr_t indirect();
daddr_t ind_lookup();
uint16 ind_index();

/* Convert the given virtual block to a physical block for the given inode.
 * ip points to the in-core inode for a file.
 * vblockno is a block number relative to the start of that file.
 */
daddr_t
vmap(ip, vblockno)
	struct inode *ip;
	daddr_t vblockno;
{
	uint16 ind_level;
	daddr_t ind_table;
	daddr_t vbno;

	if (vblockno < (daddr_t) ND){
		/* Direct block.  */
		return(ip->i_a.i_addr[vblockno]);
	} else if (vblockno < (daddr_t) (ND + NBN)) {
		/* Single indirect block.  */
		ind_level = 1;
		ind_table = ip->i_a.i_addr[ND];
		vbno = vblockno - ND;	/* Skip over direct blocks.  */
	} else if (vblockno < (daddr_t) (ND + (NBN*NBN))) {
		/* Double indirect block.  */
		ind_level = 2;
		ind_table = ip->i_a.i_addr[ND+1];
		vbno = vblockno - (ND + NBN);	/* Skip over direct
						 * and indirect blocks.
						 */
		
	} else {
		/* Triple indirect block.  */
		ind_level = 3;
		ind_table = ip->i_a.i_addr[ND+2];
		/* Skip over direct, indirect blocks,
		 * and double indirect blocks.
		 */
		vbno = vblockno - (ND + NBN + NBN*NBN);
	}

	return(indirect(ind_level, ind_table, vbno));
} /* vmap() */

/* indirect(uint16 ind_level, daddr_t ind_table_ptr, daddr_t vblockno)
 * Recursively follow an indirection for a given virtual block number
 * vblockno.
 * ind_level must be the level of indirection still un-resolved.
 * ind_table is the physical block number of the next indirection.
 */
daddr_t
indirect(ind_level, ind_table_ptr, vblockno)
	uint16 ind_level;
	daddr_t ind_table_ptr;
	daddr_t vblockno;
{
	BUF *bp;
	daddr_t *my_block;
	daddr_t next_ptr;

	/* Base case.  Direct block.  */
	if (0 == ind_level) {
		/* At this point, the block number in ind_table_ptr
		 * is exactly the physical block number we've been
		 * looking for.
		 */
		return(ind_table_ptr);
	}

	/* Recursive case.  Some level of indirection.  */

	/* Read the next table.  */
	bp = bread(ind_table_ptr);
	
	/* Pick out the actual disk block.  */
	my_block = (daddr_t *) bp->b_paddr;
	
	/* Fetch the next indirection.  */
	next_ptr = ind_lookup(ind_level, my_block, vblockno);

	/* Canonicalize it.  */
	candaddr(next_ptr);

#if 0
	/* Normally, buffers containing indirection blocks should not
	 * be brelease()'d.  This is how we try to assure that they
	 * do not need to be read off of disk too often.
	 */
	sanity_check("indirect() about to brelease() and recurse");
	brelease(bp);	/* DEBUG */
#endif

	indirect(ind_level - 1,
		 next_ptr,
		 vblockno);
} /* indirect() */

/* ind_lookup(uint16 ind_level, daddr_t *ind_table, daddr_t vblockno)
 * Look up the next level of block in table ind_table, for virtual
 * block number vblockno.
 * Note that this table is in DISK CANNONICAL format.  If the local
 * notion of daddr_t is a different size from DISK CANONICAL daddr_t
 */
daddr_t
ind_lookup(ind_level, ind_table, vblockno)
	uint16 ind_level;
	daddr_t *ind_table;
	daddr_t vblockno;
{
	return(ind_table[ind_index(ind_level, vblockno)]);
}

/* uint16 ind_index(uint16 ind_level, daddr_t vblockno);
 * Calculate the index needed for virtual block vblockno into
 * a table of the given indirection level.
 */
#define SEVENONES 0x7f	/* Mask of address bits for table lookups.
			 * NBN = 128 entries = 7 bit address.
			 */
uint16
ind_index(ind_level, vblockno)
	uint16 ind_level;
	daddr_t vblockno;
{
	/* Move the appropriate 7 bits to the lowest position,
	 * and return them.
	 */
	return((vblockno >> (7 * (ind_level - 1))) & SEVENONES);
}
