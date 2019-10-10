/*
 * Screen driver functions connected with wgetLoc().
 */
#include <scn.h>
#include <longfield.h>

#define CTRL(c) (c - '@')
WINDOW *errWindow = NULL;
static int errSw;	/* clear errWindow at next stdscn input */
static	needsTouch;	/* needs touchwin at next stdscn input */
static  savC;	/* exit char for recursion */

/*
 * loc.field may be a char * or a char **
 * return the char *.
 */
static char *
fieldPtr(f) register loc *f;
{
	return ((f->flags & LONGFIELD) ? *((char **)f->field) : f->field);
}

static void
longField(f) loc *f;
{
	register loc *l;	/* tmp area pointer */
	register char *q, *new;
	WINDOW	*w;
	char	**p, **d;
	int	j, len, same;

	if (NULL == (w = newwin(8, 80, 0, 0)))
		fatal("Cannot open subwindow");

	touchwin(w);
	p = (char **)f->field;
	d = (char **)f->Default;
	same = (*p == *d);	/* is field == default */

	/* zero work fields */
	for (l = longfield_locs; NULL != (q = l->field); l++) {
		l->help = f->help;
		memset(q, '\0', l->len);
	}

	/* move in defaults */
	if (NULL != (new = *d)) {
		for (l = longfield_locs; NULL != (q = l->field); l++) {
			for (j = 0; *new && (j < l->len); j++)
				*q++ = *new++;
		}
		free(*p);
	}

	wscnDriv(w, longfield_data, longfield_locs);

	for (len = 1, l = longfield_locs; NULL != (q = l->field); l++)
		len += strlen(q);

	*p = new = alloc(len);
	if (same)
		*d = new;

	for (l = longfield_locs; NULL != (q = l->field); l++)
		strcat(new, q);

	delwin(w);
}

/*
 * Show any error.
 */
void
showError(s)
char *s;
{
	char work[80];

	if (NULL == errWindow)
		fatal("No error window");
	sprintf(work, "Error: %r", &s);
	wmove(errWindow, 0, 0);
	wstandout(errWindow);
	waddstr(errWindow, work);
	wstandend(errWindow);
	wrefresh(errWindow);
	errSw = 1;
}

/*
 * Show a help message if one exists.
 */
showHelp(msg)
char *msg;
{
	if (NULL != errWindow) {
		wmove(errWindow, 1, 0);
		wclrtoeol(errWindow);
		if (NULL != msg) {
			wmove(errWindow, 1, 0);
			waddstr(errWindow, msg);
		}
		wrefresh(errWindow);
	}
}

/*
 * Get a char from screen, Erase error window if not clear.
 */
static
wgetChr(w) register WINDOW *w;
{
	register c;

	if (needsTouch) {
		needsTouch = 0;
		if (stdscr == w) {
			touchwin(w);
			wrefresh(w);
		}
	}

	c = wgetch(w);

	if (errSw) {
		wmove(errWindow, 0, 0);
		wclrtoeol(errWindow);
		wrefresh(errWindow);
		errSw = 0;
	}
	return (c);
}


/*
 * if sw == 0: Display a fields default copy to field, then get it.
 * if sw == 1: Display field's default.
 * if sw == 2: Display field.
 */
static
wgetLoc(w, f, sw) register WINDOW *w; register loc *f; int sw;
{
	int	got,	/* number of chars processed */
		c,	/* current char */
		y, x,	/* cursor loc */
		i,	/* pos in Default field */
		state;	/* 1 == ESC sequence in process */
	char	*field, *def;

	wmove(w, y = f->row, x = f->col);

	/*
	 * LONGFIELD is defined as 4
	 */
	switch ((f->flags & LONGFIELD) | sw) {
	case 6:	/* display long field */
		def   = *((char **)f->field); /* we have ptr to ptr */
		break;

	case 4:	/* get long field */
		wstandout(w);	/* remind user which */

	case 5:	/* display long field default */
		def   = *((char **)f->Default); /* we have ptr to ptr */
		break;

	case 2:	/* display short field */
		def = f->field;
		break;		

	case 1: /* display short field default */
		def = f->Default;
		break;

	case 0:	/* get short field */
		def   = f->Default;
		field = f->field;
	
		/* copy Default into field and zero fill */
		for (got = i = 0; got <= f->len; got++) {
			if (c = def[i])
				i++;
			field[got] = c;
		}
	}

	/* display whatever */
	if (NULL == def)
		def = "";
	for (got = i = 0; got < f->len; got++) {
		if (c = def[i]) {
			i++;
			waddch(w, c);
		}
		else
			waddch(w, ' ');
	}

	if (f->flags & LONGFIELD)
		wstandend(w);

	refresh();

	if (sw || (f->flags & READONLY))  /* display or read only */
		return (0);

	if (f->flags & LONGFIELD) {
		longField(f);	/* this calls wgetLoc indirectly */

		wmove(w, y, x);
		field = *((char **)f->field);
		for (got = 0; got < f->len; got++) {
			if (c = *field) {
				field++;
				waddch(w, c);
			}
			else
				waddch(w, ' ');
		}

		/*
		 * Doing touchwin here makes a wierd flashing
		 * when there are many long fields.
		 * Set a switch instead.
		 */
		needsTouch = 1;
		wrefresh(w);
		return(savC);	/* return the last thing returned by wgetLoc */
	}

	showHelp(f->help);

	wmove(w, y, x);

	for (state = got = 0; got < f->len; ) {
		wrefresh(w);
		c = wgetChr(w);
		/*
 		 * Up Arrow key is esc [ A
		 * on some systems and
		 * esc A on others.
		 */
		if (state) {	/* got an escape */
			switch (c) {
			case '[':
				continue;
			case 'A':
				c = CTRL('P');
				break;
			case 'B':
				c = CTRL('N');
				break;
			case 'C':
				c = CTRL('F');
				break;
			case 'D':
				c = CTRL('B');
				break;
			}
			state = 0;
		}

		switch (c) {
		case CTRL('['):
			state = 1;
			continue;
		case '\r':	/* close out line */
		case '\n':
			field[got] = '\0';
			for (; got < f->len; got++)
				waddch(w, ' ');
			continue;
		case '\b':	/* backspace */
			if (!got)
				continue;
			got--;
			wmove(w, y, --x);
		case CTRL('D'):
		case 127:	/* delete key */
			if (!field[got])
				continue;
			for (i = got; c = field[i] = field[i + 1]; i++)
				waddch(w, c);
			waddch(w, ' ');

			wmove(w, y, x);
			continue;
		case CTRL('B'):		/* back one char */
			if (!got)
				continue;
			wmove(w, y, --x);
			got--;
			continue;
		case CTRL('P'):		/* go to previous field */
		case '\t':		/* go to next field with verify */
		case CTRL('N'):		/* go to next field */
		case CTRL('Z'):		/* end of screen */
			wrefresh(w);
			return (c);
		case CTRL('C'):		/* Give user a chance to kill */
			userCtlc();
			continue;
		case CTRL('A'):		/* beginning of line */
			wmove(w, y = f->row, x = f->col);
			got = 0;
			continue;
		case CTRL('E'):		/* end of line */
			while (field[got]) {
				got++;
				x++;
			}
			while (got >= f->len) { /* don't trigger end of field */
				got--;
				x--;
			}
			wmove(w, y, x);
			continue;
		case CTRL('F'):		/* forward one char */
			if (!(c = field[got]))
				continue;
		default:
			waddch(w, c);
			x++;
			field[got++] = c;
			c = 0;
		}
	}
	field[got] = '\0';
	wrefresh(w);
	return (c);
}

/*
 * Show all default items.
 */
void
wshowDefs(w, data, fields) WINDOW *w; backGrnd *data; loc *fields;
{
	register loc *f;

	wshowBak(w, data);

	for (f = fields; NULL != f->field; f++)
		wgetLoc(w, f, 1);
}

/*
 * Print background and get all fields.
 */
void
wscnDriv(w, data, fields) WINDOW *w; backGrnd *data; loc *fields;
{
	wclear(w);
	wshowBak(w, data);
	wgetAll(w, fields);
}

/*
 * Get all fields on a given screen.
 * allow emacs style navagation.
 */
void
wgetAll(w, fields) WINDOW *w; loc *fields;
{
	register loc *f;
	char c;

	for (c = 0; c != CTRL('Z'); ) {	
		for (f = fields; (c != CTRL('Z')) && (NULL != f->field); f++) {
			switch (c = wgetLoc(w, f, 0)) {
			case CTRL('Z'):
				if (w != stdscr) {
					savC = c;
					return;
				}
				break;
			case CTRL('N'):
				/* at end down with true field */
				if ((f[1].field == NULL) && (w != stdscr)) {
					savC = c;
					return;
				}
				break;
			case CTRL('P'):
				for (;;) {
					if (f == fields) { /* wrap */
						/* back out of true field */
						if (w != stdscr) {
							savC = c;
							return;
						}
						while (NULL != f->field)
							f++;
					}
					if (!((--f)->flags & READONLY))
						--f;
					break;
				}
				break;
			default:
				switch ((*f->verify)(fieldPtr(f))) {
				case -2:
					savC = ' ';
					return;
				case -1:
					if (f->skipf) {
						f += f->skipf;
						break;
					}
				case 0:
					f--;
					break;
				}
			}
		}
	}

	/* verify all fields */
	for (f = fields; NULL != f->field; f++) {
		switch ((*f->verify)(fieldPtr(f))) {
		case -2:
			return;
		case -1:
			if (f->skipf) {
				f += f->skipf;
				break;
			}
		case 0:
			c = wgetLoc(w, f, 0);
			f--;
			break;
		}
	}
}

/*
 * Get a field by field location.
 */
wgetField(w, table, field) WINDOW *w; loc  *table; char *field;
{
	register loc *f;

	for (f = table; NULL != f->field; f++)
		if (field == fieldPtr(f))
			return (wgetLoc(w, f, 0));
	fatal("Invalid use of getField");
}

/*
 * Put a field by field location.
 */
void
wputField(w, table, field) WINDOW *w; loc  *table; char *field;
{
	register loc *f;

	for (f = table; NULL != f->field; f++)
		if (field == fieldPtr(f)) {
			wgetLoc(w, f, 2);
			return;
		}
	fatal("Invalid use of putField");
}
