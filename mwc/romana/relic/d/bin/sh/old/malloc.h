#define isfree(x) (((unsigned)(x)) & FREE)
#define roundup(i,j) (((i) + (1 << (j)) - 1) & ~((1 << (j)) - 1))
#define adr(x) ((char *) x)
#define realsize(x) ((x) & ~FREE)
#define BADSBRK ((char *) - 1)
#define POW2	1			/* pieces taken in increments of 2 */
#define LEASTFREE sizeof(unsigned)	/* free nothing smaller than this */
#define FREE 1

static struct mblock {
	unsigned blksize;
	union {
		char	 *next;
		unsigned usera[];
	} uval;
};
