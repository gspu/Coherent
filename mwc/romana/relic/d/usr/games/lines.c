/*
 * lines.c
 * 4/2/87
 * Copyright (C) 1984-1987 by Stephen A. Ness.
 * Lines of action, a game by Claude Soucie.
 * Reference:  Sid Sackson, "A Gamut of Games," Random House 1969, pp. 34-39.
 */

/*
 * This program maintains a lot of information about the current position
 * to facilitate speedy generation and evaluation of possible next moves.
 * Moving or unmoving a piece is rather complicated as a result.
 * It also uses pointer arithmetic to do array calculations within loops.
 * The number of positions generated is roughly k*((width+1)^(depth-1)),
 * where k is the number of legal moves for a position (about 36 at start).
 * Not all generated positions need to be evaluated, however.
 * On an IBM PC, this program evaluates about 48 positions per second.
 */

#include <stdio.h>

#if	COHERENT
#include <sys/timeb.h>
#endif
#if	MSDOS
#include <timeb.h>
#define	IBMPC	1		/* Use IBM PC graphic characters. */
#endif
#ifdef	DEBUG
#include <assert.h>
#endif

/* General definitions. */
#define	VERSION	"1.2"		/* Version number. */
#define	DEPTH	3		/* Initial tree search depth. */
#define WIDTH	25		/* Initial tree search width. */
#define	MAXWIDTH 96		/* Max number of next moves (12 * 8). */
#define	MAXDEPTH 5		/* Max move tree search depth. */
#define	NMOVES	200		/* Max number of moves in game. */
#define	EDGE	-1		/* Edge marker for board. */
#define	WIN	32767		/* Value for winning position. */
#define	LOSS	-32767		/* Value for losing position. */
#define	MAN(x)	((x)?MAN1:MAN0)	/* Graphic character for x's men. */

/* Graphic characters for printable board. */
#if	IBMPC
#define SPACE	32
#define VERT	179
#define	HORIZ	196
#define	TOPL	218
#define	TOPM	194
#define	TOPR	191
#define	MIDL	195
#define	MIDM	197
#define	MIDR	180
#define	BOTL	192
#define	BOTM	193
#define	BOTR	217
#define	MAN0	4
#define MAN1	79
#else
#define SPACE	' '
#define VERT	'|'
#define	HORIZ	'-'
#define	TOPL	'+'
#define	TOPM	'+'
#define	TOPR	'+'
#define	MIDL	'+'
#define	MIDM	'+'
#define	MIDR	'+'
#define	BOTL	'+'
#define	BOTM	'+'
#define	BOTR	'+'
#define	MAN0	'X'
#define MAN1	'O'
#endif

char *rules = "\
Object:  to get  all of  player's pieces  into  one  connected  group.\n\
Horizontally,  vertically or diagonally adjacent pieces are connected.\n\
Movement: A piece may move horizontally, vertically or diagonally, but\n\
it  MUST  move exactly the number of squares given by the total number\n\
of  pieces  (friendly and enemy)  in  the  row, column or diagonal  of\n\
movement.  A piece may jump over friendly pieces,  but it may not jump\n\
over  enemy  pieces.  A piece may land on an enemy piece,  removing it\n\
from the game, but it may not land on a friendly piece.";

char *help = "\
Commands:\n\
\td3\tSet tree search depth to 3.\n\
\te\tEvaluate current position.\n\
\th\tPrint help information.\n\
\ti\tInitialize new board.\n\
\tl\tList game on console.\n\
\tlfile\tList game to file.\n\
\tm\tGenerate next move.\n\
\tm1234\tMove from 12 to 34, then generate reply.\n\
\tM1234\tMove from 12 to 34, do not generate reply.\n\
\tp\tPrint the board.\n\
\tq\tQuit.\n\
\tr\tPrint rules.\n\
\tu\tUndo most recent move.\n\
\tv\tPrint move generation information.\n\
\tw25\tSet tree search width to 25.\n\
\txfile\tExecute moves from list file.\n\
\t!cmd\tPass cmd to system for execution (COHERENT, MSDOS only).\n\
A command must be followed by a <return>.";

/* Structure to represent moves. */
typedef	struct	move {
 	int	m_val;		/* Value. */
	char	m_from;		/* From. */
	char	m_to;		/* To. */
	char	m_man;		/* Man number. */
	char	m_cap;		/* Captured man number. */
} MOVE;

/* Globals. */
int	board[100];		/* Board with edges added. */
int	boarddist[100][100];	/* Distances between squares on board. */
int	cindex[100][4];		/* Indices into count for [pos][dir]. */
char	command[81];		/* Command. */
int	count[46];		/* Number of men in row, col, diagonal. */
int	depth = DEPTH;		/* Search depth. */
int	firstman[2];		/* First man number for each player. */
int	group[25];		/* Grouping of men. */
int	lastman[2];		/* Last man number for each player. */
int	loc[25];		/* Locations of men. */
int	maxgen;			/* Largest width of moves generated. */
MOVE	movelist[NMOVES];	/* Moves to date. */
long	neval;			/* Number of positions evaluated. */
MOVE	nextmove[MAXDEPTH][MAXWIDTH];	/* Move generation tree. */
long	ngen;			/* Number of moves generated. */
int	nmove;			/* Move number. */
int	npos;			/* Number of positions from which moves generated. */
int	offset[4];		/* Board offsets for moves. */
int	opp;			/* Opponent. */
char	pboard[18][37];		/* Printable representation of board. */
int	plr;			/* Player to move next. */
int	val[2];			/* Value of player's position. */
int	vflag = 0;		/* Verbose flag. */
int	width = WIDTH;		/* Search width. */

#if	COHERENT || MSDOS
time_t	start;			/* Move starting time. */
extern	time_t	time();
#endif

main()
{
	register int flag, n;
	MOVE m;
	MOVE *mp = &m;

	staticinit();
	printf("Lines of Action V%s\n(c) 1984-1987 by Stephen A. Ness.\n", VERSION);
	puts("Type 'h' followed by <return> if you need help.");
	initialize();
	for (;;) {
		printf("? ");
		if (gets(command)==NULL)
			exit(0);
		switch(command[0]) {
		case 'd':
			setparam(&depth, "depth", MAXDEPTH);
			break;
		case 'e':
			n = value();
			printf("board(%c)=%d board(%c)=%d value=%d\n", MAN0, val[0], MAN1, val[1], n);
			break;
		case 'h':
			printf("%s\nPlayer %c moves next.\n", help, MAN(plr));
			break;
		case 'i':
			initialize();
			break;
		case 'l':
			listgame();
			break;
		case 'm':
			flag = 1;
			if (command[1]=='\0')
				goto reply;
			goto moveit;
		case 'M':
			flag = 0;
moveit:			if (legalmove(atoi(&command[1]),mp)) {
				dodisp(mp, 0);
				domove(mp);
				if (gameover())
					break;
			}
			else {
				puts("Illegal move -- try again");
				break;
			}
			if (flag)
				printboard();
reply:			if (flag) {
				if (vflag) {
#if	COHERENT || MSDOS
					start = time(NULL);
#endif
					ngen = neval = 0L;
					npos = maxgen = 0;
				}
				findbest(mp, depth, LOSS);
				if (mp->m_man == 0) {	/* Stalemate */
					puts("Game drawn by stalemate.");
					puts("Type i to play again or q to quit.");
					break;
				}
				if (vflag) {
#if	COHERENT || MSDOS
					printf("Elapsed time %ld seconds.\n", time(NULL)-start);
#endif
					printf("Generated %ld moves from %d positions.\nEvaluated %ld positions.\nMaximum width %d.\n", ngen, npos, neval, maxgen);
				}
				printf("%c moves %d%c%d.\n", MAN(plr), mp->m_from, (mp->m_cap)?'*':'-', mp->m_to);
				dodisp(mp, 0);
				domove(mp);
				if (gameover())
					break;
			}
			printboard();
			break;			
		case 'p':
			printboard();
			break;
		case 'q':
			exit(0);
			break;
		case 'r':
			puts(rules);
			break;
		case 'u':
			domove(NULL);
			dodisp(&movelist[nmove], 1);
			printboard();
			break;
		case 'v':
			vflag = 1;
			break;
		case 'w':
			setparam(&width, "width", MAXWIDTH);
			break;
		case 'x':
			execfile();
			break;
#if	COHERENT || MSDOS
		case '!':
			system(&command[1]);
			break;
#endif
		default:
			puts("Command not recognized -- try again.");
			puts("Type h<return> if you need help.");
			break;
		}
	}
}

/* Compare next moves.  Called indirectly by qsort. */
compnext(p1, p2) char *p1, *p2;
{
	register int val1, val2;

	val1 = ((MOVE *)p1)->m_val;
	val2 = ((MOVE *)p2)->m_val;
	return ((val1 < val2) ? -1 : ((val1 == val2) ? 0 : 1));
}

/* Display character c at pos on printable board. */
display(c,pos)
{
	pboard[2*(pos/10)-1][4*(pos%10)-2] = c;
}

/* Update printable display for move *mp. */
/* Separated from domove() for speed. */
dodisp(mp, undo) register MOVE *mp; int undo;
{
	if (undo) {		/* Undo move */
		display(SPACE, mp->m_to);
		display(MAN(plr), mp->m_from);
		if (mp->m_cap)
			display(MAN(opp), mp->m_to);
	}
	else {
		display(SPACE, mp->m_from);
		display(MAN(plr), mp->m_to);
	}
}

/* Combine adjacent pieces into groups. */
dogroup(n) int n;
{
	int i, j, first, last, man, mgroup, igroup;
	register k;
	register int *gp;
	int *bp, *offp, *gfp;

	first = firstman[n];
	last = lastman[n];
	gp = gfp = &group[first];
	for (k=first; k<=last; k++)
		*gp++ = k;		/* Put man i in group i initially */
	for (i=first; i<=last; i++) {
		bp = &board[loc[i]];
		offp = &offset[0];
		igroup = group[i];	/* Note that i<>group[i] sometimes */
		for (j=0; j<4; j++) {
			man = *(bp+*offp++);	/* Occupant of adjacent square */
			if ((first<=man)&&(man<=last)&&(group[man]!=igroup)) {
				/* Join group[man] to group[i] */
				mgroup = group[man];
				gp = gfp;
				for (k=first; k<=last; k++) {
					if (*gp==mgroup)
						*gp = igroup;
					gp++;
				}
			}
		}
	}
}

/* Make a legal move. */
/* Does an unmove if mp is NULL. */
/* Note that unmove is opp moving rather than plr. */
/* The printable display is not updated. */
domove(mp)
register MOVE *mp;
{
	int i, from, to, man, cap, last, uflag;
	register int *cifp, *citp;

	if (mp==NULL) {			/* Unmove */
		if (nmove == 0) {
			puts("Cannot unmove.");
			return;
		}
		uflag = 1;
		mp = &movelist[--nmove];
		from = mp->m_to;
		to = mp->m_from;
		val[opp] = -1;		/* Value of opp's position changes */
	}
	else {				/* Normal move */
		if (nmove >= NMOVES) {
			puts("Out of space on move list");
			exit(1);
		}
		uflag = 0;
		movelist[nmove++] = *mp;	/* Structure assignment */
		from = mp->m_from;
		to = mp->m_to;
		val[plr] = -1;		/* Value of plr's position changes */
	}
	man = mp->m_man;
	cap = mp->m_cap;
#if	DEBUG
	printf("%smove %d from=%d to=%d cap=%d\n", (uflag)?"un":"", man, from, to, cap);
#endif
	board[from] = 0;		/* Vacate from location */
	if (cap) {			/* Capture */
		if (uflag) {		/* Rename cap as last, resurrect cap */
			val[plr] = -1;	/* Value of plr's position changes */
			last = ++lastman[plr];
			i = loc[last] = loc[cap];
			board[i] = last;
			cifp = &cindex[from][0];
			for (i=0; i<4; i++)
				++count[*cifp++];
			i = from;
		}
		else {			/* Rename last man as cap */
			val[opp] = -1;	/* Value of opp's position changes */
			citp = &cindex[to][0];
			for (i=0; i<4; i++)
				--count[*citp++];
			i = loc[lastman[opp]--];
		}
		board[i] = cap;
		loc[cap] = i;
	}
	board[to] = man;		/* Occupy to location */
	loc[man] = to;
	cifp = &cindex[from][0];
	citp = &cindex[to][0];
	for (i=0; i<4; i++) {		/* Adjust counts at from and to */
		--count[*cifp++];
		++count[*citp++];
	}
	opp = plr;
	plr = 1 - plr;
}

/* Evaluate position of player n and return its value. */
/* The value is the sum of the distances between players not in same group. */
eval(n) int n;
{
	register int value;
	register int *jp;
	int i, j, first, last, igroup;
	int *ip, *bdp, *gp, *gfp;
	dogroup(n);				/* Find groups of men */
	value = 0;
	first = firstman[n];
	last = lastman[n];
	gfp = &group[first];
	ip = &loc[first];
	for (i=first; i<last; i++) {		/* Man number i */
#if	DEBUG
		assert(ip==&loc[i]);
		assert(gfp==&group[i]);
#endif
		igroup = *gfp++;		/* igroup = group[i] */
		bdp = &boarddist[*ip++][0];	/* &boarddist[ipos][0] */
		gp = gfp;
		jp = ip;
		for (j=i+1; j<=last; j++) {	/* Man number j */
#if	DEBUG
			assert(gp==&group[j]);
			assert(jp==&loc[j]);
#endif
			if (*gp++ == igroup)	/* group[j] == group[i] */
				jp++;
			else
				value += *(bdp+(*jp++));	/* boarddist[ipos][jpos] */
		}
	}
	return(value);
}

/* Execute moves from list file. */
execfile()
{
	char *filename;
	FILE *fp;
	int count, from0, to0, from1, to1, eflag;
	MOVE m;
	register MOVE *mp;

	filename = &command[1];
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open list file \"%s\"\n", filename);
		return;
	}
	mp = &m;
	eflag = 1;
	while ((count = fscanf(fp, "%*d.%d%*c%d%d%*c%d", &from0, &to0, &from1, &to1)) != EOF) {
		if ((count != 2) && (count != 4))
			goto done;
		if (legalmove(100*from0+to0, mp)) {
			dodisp(mp, 0);
			domove(mp);
		}
		else
			goto done;
		if (count == 4) {
			if  (legalmove(100*from1+to1, mp)) {
				dodisp(mp, 0);
				domove(mp);
			}
			else
				goto done;
		}
	}
	eflag = 0;
done:
	printboard();
	if (eflag)
		puts("Bad list file");
	if (fclose(fp) == EOF)
		printf("Cannot close list file \"%s\"\n", filename);
}

/* Recursively find best move, searching to max depth d. */
/* Store the move through mp (if non-NULL) and return its value. */
/* Value less than limit means better move already known at previous level. */
/* Value after move is from opp's point of view, hence lowest best for plr. */
findbest(mp, d, limit) MOVE *mp; int d, limit;
{
	register int n;
	register MOVE *np;
	int i, bestn, count;
	MOVE *nextp, *bestnp;

	nextp = &nextmove[d-1][0];
	count = genmoves(nextp);
#if	DEBUG
	printf("findbest(%d) count=%d limit=%d:\n", d, count, limit);
#endif
	if (count == 0) {		/* Stalemate -- draw or loss? */
		if (mp)
			mp->m_man = 0;	/* To indicate stalemate */
		return(0);		/* Let's call it a draw for now... */
	}
	/* Find static values of next moves. */
	/* Needed for d>1 to determine WIN or LOSS positions. */
	bestnp = np = nextp;
	bestn = WIN;
	for (i=0; i<count; i++) {
		domove(np);		/* Try a move */
		np->m_val = n = value();	/* Evaluate position */
#if	DEBUG
		printf("\tfrom=%d to=%d val=%d\n", np->m_from, np->m_to, n);
#endif
		domove(NULL);		/* And undo it */
		if (n < bestn) {
			bestn = n;
			bestnp = np;
			if ((n == LOSS) || ((n <= limit) && (d == 1)))
				goto out;	/* Look no further */
		}
		np++;
	}
	if (d > 1) {
		/* If too wide, find best to look at. */
		if (count > width) {
			qsort((char *)nextp, count, sizeof(MOVE), compnext);
			count = width;
		}
		/* Find values of next moves recursively. */
		d--;			/* Depth for next level */
		bestnp = np = nextp;
		bestn = WIN;
		for (i=0; i<count; i++) {
			if (np->m_val != WIN) {
				domove(np);
				n = -findbest(NULL, d, -bestn);
#if	DEBUG
				printf("\tfrom=%d to=%d val=%d\n", np->m_from, np->m_to, n);
#endif
				domove(NULL);	/* And undo it */
				if (n < bestn) {
					bestn = n;
					bestnp = np;
					if (n <= limit)
						goto out;	/* Look no further */
				}
			}
			np++;
		}
	}
out:
	if (mp != NULL)
		*mp = *bestnp;	/* Structure assignment */
#if	DEBUG
	printf("BEST: from=%d to=%d val=%d\n", bestnp->m_from, bestnp->m_to, bestnp->m_val);
#endif
	return(bestn);
}

/* Test for winning or losing position. */
gameover()
{
	value();
	if ((val[0] != 0) && (val[1] != 0))
		return(0);
	printboard();
	if ((val[0] == 0) && (val[1] == 0))
		puts("Drawn game.");
	else
		printf("Player %c wins.\n", MAN(val[0]));
	puts("Type i to play again or q to quit.");
	return(1);
}

/* Generate all legal moves from current position, return count. */
genmoves(mp) MOVE *mp;
{
	register int off;
	register int *bp;
	int i, j, k, first, last, ofirst, olast, pos, d, dir, toff, nmoves;
	int *locp, *cip;

	first = firstman[plr];
	last = lastman[plr];
	ofirst = firstman[opp];
	olast = lastman[opp];
	nmoves = 0;				/* Moves generated */
	locp = &loc[first];			/* loc pointer */
	for (i=first; i<=last; i++) {		/* Player's man i */
#if	DEBUG
		assert(locp==&loc[i]);
		printf("Gen player=%d man=%d loc=%d\n", plr, i, *locp);
#endif
		pos = *locp++;			/* Position of i */
		cip = &cindex[pos][0];
		for (j=0; j<4; j++) {		/* Direction of movement */
			d = count[*cip++];	/* Distance */
			off = offset[j];	/* Offset in given direction */
			toff = d * off;		/* Total offset to target */
			dir = 0;
again:
#if	DEBUG
			printf("\tdir=%d dist=%d off=%d ", j+4*dir, d, off);
#endif
			bp = &board[pos];	/* Ptr to pos */
			for (k=1; k<d; k++) {
				bp += off;	/* Next position */
				if ((*bp==EDGE) || ((*bp>=ofirst)&&(*bp<=olast)))
					goto reject;	/* Edge or occupied by opp */
			}
			bp += off;		/* Ptr to target position */
			if ((*bp==EDGE) || ((*bp>=first)&&(*bp<=last)))
				goto reject;	/* Edge or occupied by plr */
			/* Found legal move */
			nmoves++;
			mp->m_from = pos;
			mp->m_to = pos + toff;
			mp->m_man = i;
			mp->m_cap = *bp;
			mp++;
#if	DEBUG
			printf("legal move to=%d cap=%d", pos+toff, *bp);
#endif
reject:
#if	DEBUG
			putchar('\n');
#endif
			if (dir == 0) {		/* Try opposite direction */
				dir++;
				off = -off;
				toff = -toff;
				goto again;
			}
		}
	}
#if	DEBUG
	printf("genmoves returning %d moves\n", nmoves);
#endif
	if (vflag) {
		npos++;
		ngen += ((long)nmoves);
		if (nmoves > maxgen)
			maxgen = nmoves;
	}
	return(nmoves);	
}

/* Initialize everything. */
initialize()
{
	register char *cp;
	register int j;
	int i, n;
	int *ip;

	/* Globals. */
	ip = &board[0];
	for (i=0; i<100; i++)
		*ip++ = ((i%10==0)||(i%10==9)||(i/10==0)||(i/10==9))?EDGE:0;
	plr = nmove = 0;
	opp = 1;
	val[0] = val[1] = -1;
	firstman[0] = 1;
	lastman[0] = 12;
	firstman[1] = 13;
	lastman[1] = 24;
	for (j=0; j<46; j++)
		count[j] = 2;
	count[0] = count[7] = count[8] = count[15] = 6;
	count[16] = count[23] = count[30] = count[31] = count[38] = count[45] = 0;

	/* Initialize the printable display. */
	cp = &pboard[0][0];
	for (i=0; i<17; i++) {
		for (j=0; j<33; j++)
			*cp++ = ((i%2)
				 ?	((j%4)?SPACE:VERT)
				 : 
				 ((j%4)
				  ? HORIZ
				  :
				  ((i==0)
				   ?	((j==0)?TOPL:((j==32)?TOPR:TOPM))
				   :
				   ((i==16)
				    ?	((j==0)?BOTL:((j==32)?BOTR:BOTM))
				    :	((j==0)?MIDL:((j==32)?MIDR:MIDM))
				))));
		*cp++ = SPACE;
		*cp++ = ((i%2) ? '1' + i/2 : SPACE);
		*cp++ = ((i%2) ? '0' : SPACE);
		*cp++ = '\n';
	}
	for (j=1; j<=8; j++) {
		*cp++ = SPACE;
		*cp++ = SPACE;
		*cp++ = '0' + j;
		*cp++ = SPACE;
	}
	*cp++ = '\0';

	/* Initial positions. */
	n = 1;				/* Man number */
	for (i=12; i<=17; i++) {
		display (MAN0, i);
		loc[n] = i;
		board[i] = n++;
	}
	for (i=82; i<=87; i++) {
		display (MAN0, i);
		loc[n] = i;
		board[i] = n++;
	}
	for (i=21; i<=71; i+=10) {
		display (MAN1, i);
		loc[n] = i;
		board[i] = n++;
	}
	for (i=28; i<=78; i+=10) {
		display (MAN1, i);
		loc[n] = i;
		board[i] = n++;
	}

	printf("%c moves first.\n", MAN0);
	printboard();
}

/* Test whether n is legal move and fill in *mp accordingly. */
legalmove(n,mp) int n; MOVE *mp;
{
	register int *bp;
	register int i;
	int from, to, fromx, fromy, tox, toy, dir, off, dist;

	mp->m_from = from = n/100;
	mp->m_to = to = n%100;
	mp->m_man = board[from];
	if ((board[from]<firstman[plr]) || (board[from]>lastman[plr]))
		return(0);
	mp->m_cap = board[to];
	fromx = from/10;
	fromy = from%10;
	tox = to/10;
	toy = to%10;
	if (fromx==tox) dir = 0;
	else if (fromy==toy) dir = 1;
	else if (fromx+fromy==tox+toy) dir = 2;
	else if (fromx-fromy==tox-toy) dir = 3;
	else return(0);
	off = offset[dir];
	if (from>to)
		off = -off;
	dist = count[cindex[from][dir]];
	bp = &board[from];
	for (i=1; i<dist; i++) {
		bp += off;
		if ((*bp==EDGE)||((*bp>=firstman[opp])&&(*bp<=lastman[opp])))
			return(0);
	}
	bp += off;
	if ((*bp==EDGE)||((*bp>=firstman[plr])&&(*bp<=lastman[plr]))||(bp!=&board[to]))
		return(0);
	return(1);
}

/* List game. */
listgame()
{
	register MOVE *mp;
	register int i;
	char c;
	FILE *fp = stdout;
	char *filename = &command[1];

	if (*filename) {
		fp = fopen(filename, "w");
		if (fp == NULL) {
			printf("Cannot open list file \"%s\"\n", filename);
			return;
		}
	}
	mp = &movelist[0];
	for (i=0; i<nmove; i++) {
		if (i%2 == 0)
			fprintf(fp, "\n%d. ", i/2+1);
		c = (mp->m_cap) ? '*' : '-';
		fprintf(fp, "%d%c%d ", mp->m_from, c, mp->m_to);
		mp++;
	}
	putc('\n', fp);
	if ((fp != stdout) && (fclose(fp) == EOF))
		printf("Cannot close list file \"%s\"\n", filename);
}

/* Print the board. */
printboard()
{
	puts(pboard);
}

/* Set parameter (depth or width). */
setparam(sp, s, max) int *sp; char *s; int max;
{
	register int i;

	printf("Old search %s is %d.\n", s, *sp);
	if (command[1]=='\0')
		return;
	i = atoi(&command[1]);
	if (i < 1 || i > max) {
		printf("%s arg must be between 1 and %d -- try again.\n", s, max);
		return;
	}
	*sp = i;
	printf("New search %s is %d.\n", s, *sp);
}

/* Initialize readonly global data. */
staticinit()
{
	register int *ip;
	register int j;
	int i, ix, iy, dx, dy, n;

 	offset[0] = 1;
	offset[1] = 10;
	offset[2] = 9;
	offset[3] = 11;
	ip = &boarddist[0][0];
	for (i=0; i<100; i++) {
		ix = i/10;
		iy = i%10;
		for (j=0; j<100; j++) {
			dx = ix - j/10;		/* x distance i to j */
			dy = iy - j%10;		/* y distance i to j */
			if (dx < 0)
				dx = -dx;
			if (dy < 0)
				dy = -dy;
			if (dx < dy)
				dx = dy;
			*ip++ = --dx;
		}
	}
	for (i=0; i<8; i++)
		for (j=0; j<8; j++) {
			n = 10*(i+1) + j+1;
			cindex[n][0] = i;
			cindex[n][1] = 8+j;
			cindex[n][2] = 16 + i + j;
			cindex[n][3] = 38 + i - j;
		}
}

/* Return value of current position. */
/* Reevaluate a position only if it has changed. */
value()
{
	register int pval, oval;

	neval++;
	pval = val[plr];
	oval = val[opp];
	if (pval < 0)
		pval = val[plr] = eval(plr);
	if (oval < 0)
		oval = val[opp] = eval(opp);
	if (pval == 0)
		return((oval) ? WIN : 0);
	return((oval) ? oval-pval : LOSS);
}

/* end of lines.c */
