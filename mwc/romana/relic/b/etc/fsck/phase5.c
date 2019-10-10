/*
 *	phase 5 of fsck - Check Free List
 */

#include "fsck.h"

daddr_t	badcnt;			/* Number of bad blocks in free list */
daddr_t dupcnt;			/* Number of dup blocks in free list */
int	badflag;		/* Bad free list flag */
int 	fixerup;		/* Flag to salvage free list */

phase5()
{
	struct fblk *fbp;
	unsigned size;
	
	if (!qflag)
		printf("Phase 5 : Check Free List\n");
	fbp = (struct fblk *) &sbp->s_nfree;
	size = (unsigned) ((fsize+NBPC-1)/NBPC) * sizeof(char);
#if SMALLMODEL
	copyV(size);
#else
	memcpy(dupPtr, blockPtr, size);
#endif
	badcnt = dupcnt = 0;
	badflag = FALSE;
	freetrav(fbp);
	fixerup = FALSE;
	if (badflag || sflag) 
		asksalvage();
}


#define	IGNORE	3		/* Return Possibilities from chk() */
#define	ABORT	4		/* as well as perhaps GOOD and BAD */

freetrav(fbp)
struct fblk *fbp;
{
	daddr_t total=0;
	int i, flag;

	while ( fbp->df_nfree != 0 ) {
		if ( (unsigned) fbp->df_nfree > NICFREE ) {
			badflag = TRUE;
			printf("Bad freeblock count.\n");
			break;
		}
		for (i=1; i<fbp->df_nfree; i++) {
			if ( (flag=chk(fbp->df_free[i])) == GOOD ) {
				total++;
				continue;
			}
			if (flag == BAD)
				continue;
			break;		/* break for IGNORE or ABORT	*/
		}
		if ( (flag == IGNORE) || (flag == ABORT) )
			break;
		if ( chk(fbp->df_free[0]) != GOOD )
			break;
		total++;
		bread(fbp->df_free[0], databuf);
		fbp = (struct fblk *) databuf;
		if ( !canfblk(fbp) ) {
			badflag = TRUE;
			printf("Bad freeblock count.\n");
			break;
		}
	}

#if 0
	printf("Total traversed free blocks     = %lu\n", total);
	printf("Total free blocks by Superblock = %lu\n", sbp->s_tfree);
	printf("Running total free blocks       = %lu\n", totfree);
#endif

	if ( flag == ABORT ) { 		/* To Terminate fsck on this 	*/
		badflag = FALSE;	/* File System, because of	*/
		return;			/* excessive dup or bad blocks	*/
	}

	if ( sbp->s_tfree != totfree )
		if ( !qflag || (daction==NO) ) {
			printf("Free Block count wrong in superblock.  ");
			if ( action(fixit) == TRUE ) {
				sbp->s_tfree = totfree;
				sbpfix = TRUE;
			}
		} else if ( daction != NO ) {
			sbp->s_tfree = totfree;
			sbpfix = TRUE;
		}

	report("%U Bad Block%s in Free List\n", badcnt);
	report("%U Dup Block%s in Free List\n", dupcnt);
	report("%U Block%s missing\n", totfree - total);

}

report(message, num)
char *message;
daddr_t num;
{
	if ( num != 0 ) {
		if ( !qflag )
			printf(message, num, ( (num>1) ? "s" : "" ) );
		badflag = TRUE;
	}
}

chk(bn)
register daddr_t bn;
{
	if ( (bn<isize) || (bn>=fsize) ) {
		badflag = TRUE;
		if (++badcnt > MAXBADFREE) 
			return( stopit("bad") );
		return(BAD);
	}

	if ( testdup(bn) ) {
		badflag = TRUE;
		if (++dupcnt > MAXDUPFREE)
			return( stopit("dup") );
		return(BAD);
	} else {
		markdup(bn);
		return(GOOD);
	}
}
			
stopit(type)
char *type;
{
	printf("Excessive %s blocks in free list ", type);
	if ( query("(Continue)") == YES )
		return(IGNORE);
	else
		return(ABORT);
}
		
		
canfblk(fbp)
register struct fblk *fbp;
{
	register short i;

	canshort(fbp->df_nfree);
	if ( (unsigned) fbp->df_nfree > NICFREE )
		return(FALSE);

	for (i=0; i<fbp->df_nfree; i++)
		candaddr(fbp->df_free[i]);
	return(TRUE);
}

asksalvage()
{
	if ( (daction!=NO) && (qflag || sflag) ) {
		fixerup = TRUE;
		return;
	}
	if ( action("Bad Free List (SALVAGE)") )
		fixerup = TRUE;
}
