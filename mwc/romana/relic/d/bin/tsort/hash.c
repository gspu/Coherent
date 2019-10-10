#include <stdio.h>
#include "tsort.h"


/*
 *	Htab is the hash table.
 */

	static struct wordlist *htable[HASHSIZE];


/*
 *	The function insert returns a pointer to the hash table entry
 *	corresponding to the name "str".
 *	If the entry is new, then it initializes the ancestors field to
 *	NULL.
 */

struct word *
insert(str)
register char *str;
{
	register struct wordlist *ptr; 
	unsigned hindex;
	unsigned hash();
	struct word *newword();
	struct wordlist *newwordl();

	hindex = hash(str) % HASHSIZE;
	for (ptr=htable[hindex]; ptr != NULL; ptr=ptr->next)
		if (strcmp(ptr->element->name, str) == 0)
			return (ptr->element);
	ptr = newwordl(newword(str));
	ptr->next = htable[hindex];
	htable[hindex] = ptr;
	return (ptr->element);
}


/*
 *	The function hash computes the hash index of the
 *	string pointed to by "str".
 */

static unsigned
hash(str)
register char *str;
{
	register unsigned result = 0;
	register int ch;

	while ((ch = *str++) != '\0')
		result = 128 * result - result + 16 * (ch % 16) + (ch / 16);
	return (result);
}


/*
 *	Cmphash returns a pointer to a wordlist which is a linked list
 *	of all words in the hash table.
 */

struct wordlist *
cmphash()
{
	register struct wordlist *mrk;
	register struct wordlist *head;
	register struct wordlist **htabp;

	head = NULL;
	for (htabp = htable; htabp - htable < HASHSIZE; ++htabp)
		if (*htabp != NULL) {
			for (mrk = *htabp; mrk->next != NULL; mrk = mrk->next)
				;
			mrk->next = head;
			head = *htabp;
		}
	return (head);
}
