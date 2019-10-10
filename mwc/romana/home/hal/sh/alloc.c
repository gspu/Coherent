#include <string.h>
#include <stdlib.h>

#include "shellio.h"
#include "sh.h"

/*
 * Externals.
 */

BUF	* bufap = NULL;			/* Head of allocation list */
BUF	* buffp = NULL;			/* Tail of free list */
BUF	** bufapp = & bufap;		/* Tail of allocation list */


/*
 * Given a string, return a pointer to a copy of it.  If 'f'
 * is set, don't add it onto the free list.
 */

char *
duplstr (cp, f)
char	      *	cp;
{
	return strcpy ((* (f ? salloc : balloc)) (strlen (cp) + 1), cp);
}


/*
 * Similar to the above, only we are duplicating a counted string.
 */

#if	USE_PROTO
CSTRING * dupcstr (CSTRING * cstrp, int f)
#else
CSTRING *
dupcstr (cstrp, f)
CSTRING	      *	cstrp;
int		f;
#endif
{
	CSTRING	      *	temp;
	temp = memcpy ((* (f ? salloc : balloc)) (CSTRING_SIZE (cstrp) + 1),
		       cstrp, CSTRING_SIZE (cstrp));

	/*
	 * Add an extra character, because most users of counted strings do
	 * eventually want to use a C-style string.
	 */

	CSTRING_STRING (temp) [CSTRING_LENGTH (temp)] = 0;
	return temp;
}


/*
 * Create an argument list.
 */
char **
makargl ()
{
	register char ** app;

	app = (char **) balloc ((1 + IALSIZE) * sizeof (char *));
	app ++;
	app [-1] = (char *) IALSIZE;
	app [0] = NULL;
	return app;
}

/*
 * Add an element to an argument list. The input argument is *always*
 * duplicated into temporary space.
 */

char **
addargl (app, sp, cstr)
char	     **	app;
VOID	      *	sp;
int		cstr;	/* non-zero if argument is a counted string */
{
	char ** napp;
	register char ** rapp;
	register unsigned n, i;

	rapp = app;
	n = (int) rapp [-1];
	while (* rapp ++ != NULL)
		;
	if (rapp - app >= n) {
		napp = (char **) balloc ((1 + n * 2) * sizeof (char *));
		napp ++;
		napp [-1] = (char *) (n * 2);
		for (i = 0; i < n; i ++)
			napp [i] = app [i];
		rapp = (app = napp) + n;
	}
	* rapp = NULL;
	if (cstr) {
		CSTRING	      *	cstrp = dupcstr (sp, 0);
		rapp [-1] = CSTRING_STRING (sp);
	} else
		rapp [-1] = duplstr (sp, 0);
	return app;
}

/*
 * Save allocation position.
 */
BUF **
savebuf ()
{
	return bufapp;
}

/*
 * Free everything allocated since passed allocation position
 * was saved.
 */
freebuf (bpp)
BUF ** bpp;
{
	* bufapp = buffp;
	buffp = * bpp;
	* bpp = NULL;
	bufapp = bpp;
}

/*
 * Allocate a buffer 'n' bytes long.
 * Add it onto the allocated buffer list.
 */
char *
balloc (n)
{
	register BUF * bp, ** bpp;

	bpp = & buffp;
	for (;;) {
		if ((bp = * bpp) == NULL) {
			bp = (BUF *) salloc (sizeof (BUF) + n);
			bp->b_size = n;
			break;
		}
		if (bp->b_size == n) {
			* bpp = bp->b_next;
			break;
		}
		bpp = & bp->b_next;
	}
	* bufapp = bp;
	bufapp = & bp->b_next;
	* bufapp = NULL;
	return (char *) bp + sizeof (BUF);
}

/*
 * Allocate 'n' bytes.
 */

char *
salloc (n)
{
	char	      *	cp;

	if ((cp = malloc (n)) == NULL) {
		if (shellerr_begin ("Out of memory"))
			shellerr_endl ();

		reset (RNOSBRK);
	}

	return cp;
}

/*
 * Free something allocated by 'salloc'.
 */

#ifndef	sfree
void
sfree (cp)
VOID * cp;
{
	if (cp == NULL)
		abort ();
	free (cp);
}
#endif


/*
 * Deallocate a vector.
 */

#if	USE_PROTO
void vfree (char * CONST * vecp, int extra)
#else
void
vfree (vecp, extra)
char  * CONST *	vecp;
int		extra;
#endif
{
	char  * CONST *	vpp;

	if (vecp == NULL)
		return;

	for (vpp = vecp + extra ; * vpp != NULL ; vpp += 1)
		sfree (* vpp);
	sfree ((VOID *) vecp);
}


#if	USE_PROTO
char ** vdupl (char * CONST * vecp, int extra)
#else
char **
vdupl (vecp, extra)
char  * CONST *	vecp;
int		extra;
#endif
{
	char  * CONST *	vp;
	char ** nvp, ** tvp;

	for (vp = vecp ; * vp ++ != NULL ;)
		/* DO NOTHING */ ;
	tvp = nvp = (char **) salloc ((vp - vecp + extra) * sizeof (* vp));
	tvp += extra;
	for (vp = vecp ; * vp != NULL ; )
		* tvp ++ = duplstr (* vp ++, 1);
	* tvp = NULL;

	return nvp;
}
