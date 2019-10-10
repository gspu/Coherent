#include <stdio.h>
#include "tsort.h"


/*
 *	Remove removes the word pointed to by "wrdp".  The only effect
 *	of this is to set its "name" to the empty string.
 */

#define	remove(wrdp)	((wrdp)->name[0] = '\0')


/*
 *	Gone is used to determine if a word has already been
 *	written out.  The mechanism used is that when a word
 *	has been written out, its "name" is set to the empty
 *	string.
 */

#define	gone(wrdp)	((wrdp)->name[0] == '\0')


/*
 *	Clean is used to remove elements of a wordlist.  It takes
 *	the wordlist pointed to by *"wrdlp" and removes any elements
 *	for which gone returns true.
 */

static void
clean(wrdlp)
register struct wordlist **wrdlp;
{
	register struct worlist *wrdl;

	for (wrdl = *wrdlp; wrdl != NULL; wrdlp = &wrdl->next, wrdl = *wrdlp)
		if (gone(wrdl->element)) {
			do {
				wrdl = wrdl->next;
			} while (wrdl != NULL && gone(wrdl->element));
			*wrdlp = wrdl;
			if (wrdl == NULL)
				break;
		}
}


/*
 *	Pass makes one pass thru the wordlist pointed to by words,
 *	cleaning up all ancestor lists and writeing out all words
 *	with no ancestors.  When it writes out a word, it "remove"s
 *	it.  When the pass is completed, it makes another pass 
 *	to actually take all written out words out of the list
 *	of words.
 */

static
pass()
{
	register struct wordlist *wrdlp;
	register progress = FALSE;

	for (wrdlp = words; wrdlp != NULL; wrdlp = wrdlp->next) {
		clean(&wrdlp->element->ancestors);
		if (wrdlp->element->ancestors == NULL) {
			progress = TRUE;
			printf("%s\n", wrdlp->element->name);
			remove(wrdlp->element);
		}
	}
	clean(&words);
	return (progress);
}


/*
 *	Dump is used to dump out the remaining words if we find any
 *	cycles.  It simply prints out all the remaining words in
 *	a random order.
 */

static void
dump()
{
	register struct wordlist *wrdlp;

	for (wrdlp = words; wrdlp != NULL; wrdlp = wrdlp->next)
		printf("%s\n", wrdlp->element->name);
}


/*
 *	Order is the routine which actually controls the dumping
 *	of all entries in the tree.  It repeatedly calls pass until
 *	the tree is empty or nothing new can be dumped (indicateing
 *	a cycle).
 */

void
order()
{
	while (pass())
		;
	dump();
	if (words != NULL)
		die("cycles");
}
