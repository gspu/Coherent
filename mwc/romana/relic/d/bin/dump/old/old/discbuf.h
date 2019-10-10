/*
 * This is the format of a
 * disc buffer as managed by the
 * user level cache manager.
 */
typedef	struct	DISCBUF
{
	struct	DISCBUF	*db_fp;		/* Link */
	int	db_refc;		/* Reference count */
	long	db_block;		/* Block */
	int	db_flag;		/* Some flags */
	char	db_data[BUFSIZ];	/* Data block */
} 	DISCBUF;

#define	DB_DIRT	01			/* Dirty buffer */
#define	DB_OK	02			/* Valid buffer */

DISCBUF	*dbread();
DISCBUF	*dbgrab();
DISCBUF *dbzero();
FILE	*dbfp;
