#include <stdio.h>
#include <sys/mdata.h>
#include "tsort.h"


#define BUMP	2048	/* twice minimum space to get from malloc */


/*
 *	Blockp is a pointer to a block of memory from which the
 *	various allocation routines will get memory.  Blockc is
 *	the number of bytes left in the block.  When these bytes
 *	are all used up, then malloc is called for BUMP more
 *	bytes.  This save malloc from going over all the small
 *	blocks.
 */

static char *blockp;
static int blockc = 0;


/*
 *	Alloc functions exactly like malloc with two exceptions
 *		1. It tries to get memory from blockp/blockc if
 *		   possible.
 *		2. It never returns NULL, but instead exits with
 *		   an error message if there is no memory left.
 */

static char *
alloc(size)
register unsigned size;
{
	register char *result;
	char	*malloc();

	/*
	 * for alignment constraints.
	 */
	size += ALSTRUCT - 1;
	size -= size % ALSTRUCT;
	if (blockc >= size) {
		blockc -= size;
		result = blockp;
		blockp += size;
		return (result);
	}
	if (size < BUMP / 2) {
		result = (char *)malloc(size + BUMP);
		blockp = result + size;
		blockc = BUMP;
	} else
		result = (char *)malloc(size);
	if (result == NULL)
		die("out of memory");
	return (result);
}



/*
 *	Newword returns a pointer to a word with the ancestors
 *	field set to NULL and the name field set to "str".
 */

struct word *
newword(str)
register char *str;
{
	register struct word *result;

	result = (struct word *) alloc( sizeof(struct word));
	result->name = alloc(strlen(str) + 1);
	strcpy(result->name, str);
	result->ancestors = NULL;
	return (result);
}


/*
 *	Newwordl returns a pointer to a wordlist with the element
 *	field set to "wrd".
 */

struct wordlist *
newwordl(wrd)
register struct word *wrd;
{
	register struct wordlist *result;

	result = (struct wordlist *) alloc( sizeof(struct wordlist));
	result->element = wrd;
	return (result);
}
