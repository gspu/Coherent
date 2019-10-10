/*
 *	Various manifest constants.
 */

#define HASHSIZE	1000	/* hash table size */
#define MAXWORD		100	/* longest word size */
#define TRUE	(0 == 0)	/* logical true */
#define FALSE	(0 != 0)	/* logical false */



/*
 *	The struct wordlist is used to form a list of words.
 *	It is used for two purposes:
 *		1. To keep a list of all words which must precede a
 *		   given word.
 *		2. To keep a list of all words which have the same
 *		   hash index in the hash table.
 *		3. To keep a list of all the words which have not yet
 *		   been written out.
 */

struct wordlist {
	struct word *element;
	struct wordlist *next;
};



/*
 *	The struct word is what keeps all information on a given
 *	word.  This consists of the word itself and the list of
 *	words which must precede the word.
 */

struct word {
	char *name;
	struct wordlist *ancestors;
};



/*
 *	Non-int returning functions.
 */

struct word *insert();		/* enters names into hash table */
struct wordlist *cmphash();	/* compress hash to linked list */
struct word *newword();		/* allocate new word */
struct wordlist *newwordl();	/* allocate new wordlist */
void getcon();			/* read in constraints */
void die();			/* fail with message */
void order();			/* order and dump words */



/*
 *	External variables.
 */

extern struct wordlist *words;	/* head of list of words not output yet */
