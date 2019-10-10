/*
 * The routines in this file
 * implement commands that work word at
 * a time. There are all sorts of word mode
 * commands. If I do any sentence and/or paragraph
 * mode commands, they are likely to be put in
 * this file.
 */
#include	<stdio.h>
#if NORMAL_CHARS
#include <ctype.h>
#endif
#include	"ed.h"

/*
 * Word wrap on input.
 * called when data is typed in checks that we are at an 
 * end of line. And then trys to wrap the line.
 */
void
wrapword()
{
	register LINE *clp;
	
	if (llength(clp = curwp->w_dotp) == curwp->w_doto)
		wrapblock(clp, 0, llength(clp), 0);
}

/*
 * Refill region between mark and cursor.
 */
fillregion(f, n)
{
	REGION		region;

	if (getregion(&region) != TRUE)
		return (FALSE);

	if (!bind.fillcol) {
		mlwrite("Use <ctl-x> f to set fill collumn first");
		return (FALSE);
	}
	return (wrapblock(region.r_linep, region.r_offset, region.r_size,
						  truecol(region.r_linep, region.r_offset)));
}

/*
 * wrap a block of data. Gets the start address, size and
 * true collum of the start. Triggers when printable chars
 * are over wrap length and there is white space before them.
 */
wrapblock(lineP, offset, size, trueCol)
register LINE *lineP;
short offset, size, trueCol;
{
	int blackCt, whiteCt;

	for (blackCt = whiteCt = 0; size-- > 0; offset++) {
		if (offset == llength(lineP)) {
			if (!size)
				break;
			whiteCt = blackCt = 0;
			while (!llength(lineP = lforw(lineP))) {
				size--;
				trueCol = 0;
				offset = -1;
			}
			if (-1 == offset)
				continue;
			curwp->w_doto = offset;
			curwp->w_dotp = lineP = lback(lineP);
			ldelete(1, FALSE);
			linsert(whiteCt = 1, ' ');
			trueCol++;
			offset = curwp->w_doto - 1;
			lineP = curwp->w_dotp;
			continue;
		}
		switch (lgetc(lineP, offset)) {
		case '\t':
			taber(trueCol);
		case ' ':
			if (!blackCt)
				whiteCt++;
			else {
				whiteCt = 1;
				blackCt = 0;
			}
			break;
		default:
			if (!blackCt)
				blackCt = 1;
			else
				blackCt++;
		}
		if ((trueCol++ > bind.fillcol) && whiteCt && blackCt) {
			curwp->w_dotp = lineP;
			curwp->w_doto = offset -= whiteCt + blackCt - 1;
			lnewline();
			ldelete(whiteCt, FALSE);
			whiteCt = 0;
			offset = (trueCol = blackCt) - 1;
			lineP = curwp->w_dotp;
		}
	}
	curwp->w_doto  = llength(curwp->w_dotp);
	return (TRUE);
}

/*
 * Return TRUE if the character at dot is a character that is considered
 * to be white space. Used in wordwrap.
 * The whitespace character list is hard coded. Should be setable.
 */
inwhite()
{
	register int	c;

	if (curwp->w_doto == llength(curwp->w_dotp))
		return (FALSE);
	c = lgetc(curwp->w_dotp, curwp->w_doto);
	if (c == ' ' || c == '\t' )
		return (TRUE);
	return (FALSE);
}

/*
 * Move the cursor backward to the last bit of whitespace...
 * All of the details of motion are performed by the "backchar" and "forwchar"
 * routines. Error if you try to move beyond the buffers.
 */
backwhite()
{
	if (backchar(FALSE, 1) == FALSE)
		return (FALSE);
	while (!inwhite()) {
		if (backchar(FALSE, 1) == FALSE)
			return (FALSE);
	}
	return (forwchar(FALSE, 1));
}

/*
 * Move the cursor backward by "n" words.  All of the details of motion
 * are performed by the "backchar" and "forwchar" routines.  Error if you
 * try to move beyond the buffers.
 */
backword(f, n)
{
	if (n < 0)
		return (forwword(f, -n));
	if (backchar(FALSE, 1) == FALSE)
		return (FALSE);
	while (n--) {
		while (inword() == FALSE) {
			if (backchar(FALSE, 1) == FALSE)
				return (FALSE);
		}
		while (inword() != FALSE) {
			if (backchar(FALSE, 1) == FALSE)
				return (FALSE);
		}
	}
	return (forwchar(FALSE, 1));
}

/*
 * Move the cursor forward by the specified number of words.  All of the
 * motion is done by "forwchar".  Error if you try and move beyond the
 * buffer's end.
 */
forwword(f, n)
{
	if (n < 0)
		return (backword(f, -n));
	while (n--) {
		while (inword() == FALSE) {
			if (forwchar(FALSE, 1) == FALSE)
				return (FALSE);
		}
		while (inword() != FALSE) {
			if (forwchar(FALSE, 1) == FALSE)
				return (FALSE);
		}
	}
	return (TRUE);
}

/*
 * There is no pattern I can discover to the upper and lower case
 * of the wierd umlauts etc tacked on to ascii. These tables are the
 * convertable wierd characters upFor has the upper case and loFor
 * the corresponding lower case.
 */
static uchar upFor[8] = { 0x80, 0x9A, 0x90, 0x8E, 0x8F, 0x92, 0x99, 0 };
static uchar loFor[8] = { 0x87, 0x81, 0x82, 0x84, 0x86, 0x91, 0x94, 0 };

/*
 * Turn a char to upper case.
 */
capChar()
{
	register int c;

	if (c = islow(lgetc(curwp->w_dotp, curwp->w_doto))) {
		lputc(curwp->w_dotp, curwp->w_doto, c);
		lchange(WFHARD);
	}
	return (forwchar(FALSE, 1));
}

/*
 * If character is lower case return upper case version.
 * else return 0
 */
islow(c)
register int c;
{
#if NORMAL_CHARS
	return (islower(c) ? toupper(c) : 0);
#else
	int i, d;

	if (c>='a' && c<='z')
		return (c - ('a'-'A'));

	if (c & 0x80)
		for (i = 0; d = loFor[i]; i++)
			if (d == c)
				return (upFor[i]);
	return (0);
#endif
}

/*
 * Turn a char to lower case.
 */
lowChar()
{
	register int c;

	if (c = ishi(lgetc(curwp->w_dotp, curwp->w_doto))) {
		lputc(curwp->w_dotp, curwp->w_doto, c);
		lchange(WFHARD);
	}
	return (forwchar(FALSE, 1));
}

/*
 * If character is upper case return lower case version.
 * else return 0
 */
ishi(c)
register int c;
{
#if NORMAL_CHARS
	return (isupper(c) ? tolower(c) : 0);
#else
	int i, d;

	if (c>='A' && c<='Z')
		return (c - ('A'-'a'));

	if (c & 0x80)
		for (i = 0; d = upFor[i]; i++)
			if (d == c)
				return (loFor[i]);
	return (0);
#endif
}

/*
 * Move the cursor forward by the specified number of words.  As you move,
 * convert any characters to upper case.  Error if you try and move beyond
 * the end of the buffer.
 * Bound to "M-U".
 */
upperword(f, n)
{
	if (n < 0)
		return (FALSE);
	while (n--) {
		while (inword() == FALSE)
			if (forwchar(FALSE, 1) == FALSE)
				return (FALSE);

		while (inword() != FALSE)
			if (capChar() == FALSE)
				return (FALSE);
	}
	return (TRUE);
}

/*
 * Move the cursor forward by the specified number of words.  As you move
 * convert characters to lower case.  Error if you try and move over the
 * end of the buffer.
 * Bound to "M-L".
 */
lowerword(f, n)
{
	register int	c;

	if (n < 0)
		return (FALSE);
	while (n--) {
		while (inword() == FALSE)
			if (forwchar(FALSE, 1) == FALSE)
				return (FALSE);

		while (inword() != FALSE)
			if (lowChar() == FALSE)
				return (FALSE);
	}
	return (TRUE);
}

/*
 * Move the cursor forward by
 * the specified number of words. As you move
 * convert the first character of the word to upper
 * case, and subsequent characters to lower case. Error
 * if you try and move past the end of the buffer.
 * Bound to "M-C".
 */
capword(f, n)
{
	register int	c;

	if (n < 0)
		return (FALSE);
	while (n--) {
		while (inword() == FALSE)
			if (forwchar(FALSE, 1) == FALSE)
				return (FALSE);

		if (inword() != FALSE) {
			if (capChar() == FALSE)
				return (FALSE);
			while (inword() != FALSE)
				if (lowChar() == FALSE)
					return (FALSE);
		}
	}
	return (TRUE);
}

/*
 * Kill forward by "n" words.
 * Remember the location of dot. Move forward
 * by the right number of words. Put dot back where
 * it was and issue the kill command for the
 * right number of characters. Bound to "M-D".
 */
delfword(f, n)
{
	register int	size;
	register LINE	*dotp;
	register int	doto;

	if (n < 0)
		return (FALSE);
	dotp = curwp->w_dotp;
	doto = curwp->w_doto;
	size = 0;
	if ((lastflag&CFKILL) == 0)		/* Clear kill buffer if	*/
		kdelete();			/* last wasn't a kill.	*/
	thisflag |= CFKILL;
	while (n--) {
		while (inword() == FALSE) {
			if (forwchar(FALSE, 1) == FALSE)
				return (FALSE);
			++size;
		}
		while (inword() != FALSE) {
			if (forwchar(FALSE, 1) == FALSE)
				return (FALSE);
			++size;
		}
	}
	curwp->w_dotp = dotp;
	curwp->w_doto = doto;
	return (ldelete(size, TRUE));
}

/*
 * Kill backwards by "n" words.
 * Move backwards by the desired number of
 * words, counting the characters. When dot is
 * finally moved to its resting place, fire off
 * the kill command. Bound to "M-Rubout" and
 * to "M-Backspace".
 */
delbword(f, n)
{
	register int	size;

	if (n < 0)
		return (FALSE);
	if (backchar(FALSE, 1) == FALSE)
		return (FALSE);
	size = 0;
	if ((lastflag&CFKILL) == 0)		/* Clear kill buffer if	*/
		kdelete();			/* last wasn't a kill.	*/
	thisflag |= CFKILL;
	while (n--) {
		while (inword() == FALSE) {
			if (backchar(FALSE, 1) == FALSE)
				return (FALSE);
			++size;
		}
		while (inword() != FALSE) {
			if (backchar(FALSE, 1) == FALSE)
				return (FALSE);
			++size;
		}
	}
	if (forwchar(FALSE, 1) == FALSE)
		return (FALSE);
	return (ldelete(size, TRUE));
}

/*
 * Return TRUE if the character at dot
 * is a character that is considered to be
 * part of a word. The word character list is hard
 * coded. Should be setable.
 */
inword()
{
	register int	c;

	if (curwp->w_doto == llength(curwp->w_dotp))
		return (FALSE);
	c = lgetc(curwp->w_dotp, curwp->w_doto);
	if (c>='a' && c<='z')
		return (TRUE);
	if (c>='A' && c<='Z')
		return (TRUE);
	if (c>='0' && c<='9')
		return (TRUE);
	if (c=='$' || c=='_' || c=='\\')	/* For identifiers */
		return (TRUE);
	if (c & 0x80)
		return (TRUE);
	return (FALSE);
}

#if	LIBHELP
/*
 * Lookup the current word.
 * Error if you try and move past the end of the buffer.
 * Bound to "M-M-".  Count is passed to lookup routine.
 */
lookupword(f, n)
{
	static uchar wordplace[128];
	register uchar *cp;

	while (inword() != FALSE)		/* Get to beginning of word */
		if (backchar(FALSE, 1) == FALSE)
			break;

	while (inword() == FALSE) {		/* Advance to next word	*/
		if (forwchar(FALSE, 1) == FALSE)
			return (FALSE);
	}

	cp = wordplace;
	while (inword() != FALSE) {
		*cp++ = lgetc(curwp->w_dotp, curwp->w_doto);
		if (forwchar(FALSE, 1) == FALSE)
			break;
	}
	*cp = '\0';
	return do_lookup(wordplace, f, n);
}
#endif
