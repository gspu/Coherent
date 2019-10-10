/*********************************************************************
*                         COPYRIGHT NOTICE                           *
**********************************************************************
*        This software is copyright (C) 1982 by Pavel Curtis         *
*                                                                    *
*        Permission is granted to reproduce and distribute           *
*        this file by any means so long as no fee is charged         *
*        above a nominal handling fee and so long as this            *
*        notice is always included in the copies.                    *
*                                                                    *
*        Other rights are reserved except as explicitly granted      *
*        by written permission of the author.                        *
*                Pavel Curtis                                        *
*                Computer Science Dept.                              *
*                405 Upson Hall                                      *
*                Cornell University                                  *
*                Ithaca, NY 14853                                    *
*                                                                    *
*                Ph- (607) 256-4934                                  *
*                                                                    *
*                Pavel.Cornell@Udel-Relay   (ARPAnet)                *
*                decvax!cornell!pavel       (UUCPnet)                *
*********************************************************************/

/*
**
**	lib_mvcur.c
**
**	mvcur() and its subroutines
**
** $Log:	lib_mvcur.c,v $
 * Revision 1.8  93/04/12  14:13:47  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:43  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  19:25:30  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:47:54  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:46:40  pavel
 * Beta-one Test Release
 * 
**
**	Revisions needed:
**		implement c_save instead of multiple tputs() calls
**		routine revisions
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_mvcur.c,v 1.8 93/04/12 14:13:47 bin Exp Locker: bin $";
#endif

#include "term.h"
#include "curses.h"
#include "curses.priv.h"


#define BUFSIZE	128			/* size of strategy buffer */

struct Sequence
{
	int	vec[BUFSIZE],	/* vector of operations */
		*end,		/* end of vector */
		cost;		/* cost of vector */
};

/*
**	#define
**	Make_seq_best(s1, s2)
**	
**	Make_seq_best() swaps the values
**	of the pointers if s1->cost > s2->cost.
*/

#define Make_seq_best(s1, s2)		\
	if (s1->cost > s2->cost)	\
	{				\
	    struct Sequence	*temp;	\
					\
	    temp = s1;			\
	    s1 = s2;			\
	    s2 = temp;			\
	}				


FILE	*out_file;				/* pointer to output file */

static int	c_count;		/* used for counting tputs output */

/*rev c_save not yet used*/
/*static char	*c_save;*/		/* used for saving tputs output */

#define	INFINITY	1000		/* biggest, impossible sequence cost */

#define NUM_OPS		16		/* num. term. control sequences */
#define NUM_NPARM	9		/* num. ops wo/ parameters */

	/* operator indexes into op_info */

#define	CARRIAGE_RETURN	0		/* watch out for nl mapping */
#define	CURS_DOWN	1
#define	CURS_HOME	2
#define	CURS_LEFT	3
#define	CURS_RIGHT	4
#define	CURS_TO_LL	5
#define	CURS_UP		6
#define	TAB		7
#define	BACK_TAB	8
#define	ROW_ADDR	9
#define	COL_ADDR	10
#define	P_DOWN_CURS	11
#define	P_LEFT_CURS	12
#define	P_RIGHT_CURS	13
#define	P_UP_CURS	14
#define	CURS_ADDR	15

static bool	loc_init = FALSE;	/* set if op_info is init'ed */

static bool	rel_ok;			/* set if we really know where we are */

/*
 *	op_info[NUM_OPS]
 *
 *	op_info[] contains for operations with no parameters
 *  the cost of the operation.  These ops should be first in the array.
 *	For operations with parameters, op_info[] contains
 *  the negative of the number of parameters.
 */

static int	op_info[NUM_OPS] =
{
	0,		/* carriage_return */
	0,		/* cursor_down */
	0,		/* cursor_home */
	0,		/* cursor_left */
	0,		/* cursor_right */
	0,		/* cursor_to_ll */
	0,		/* cursor_up */
	0,		/* tab */
	0,		/* back_tab */
	-1,		/* row_address */
	-1,		/* column_address */
	-1,		/* parm_down_cursor */
	-1,		/* parm_left_cursor */
	-1,		/* parm_right_cursor */
	-1,		/* parm_up_cursor */
	-2		/* cursor_address */
};


/*
**
**	mvcur(oldrow, oldcol, newrow, newcol)
**
**	mvcur() optimally moves the cursor from the position
**  specified by (oldrow, oldcol) to (newrow, newcol).  If
**  (oldrow, oldcol) == (-1, -1), mvcur() does not use relative
**  cursor motions.  If the coordinates are otherwise
**  out of bounds, it mods them into range.
**
**	Revisions needed:
**		eat_newline_glitch, auto_right_margin
*/

mvcur(oldrow, oldcol, newrow, newcol)
int	oldrow, oldcol,
	newrow, newcol;
{
	struct Sequence	seqA, seqB,	/* allocate work structures */
			col0seq,	/* sequence to get from col0 to nc */
			*best,		/* best sequence so far */
			*try;		/* next try */

#ifdef TRACE
	if (_tracing)
	    _tracef("mvcur(%d,%d,%d,%d) called",
						oldrow, oldcol, newrow, newcol);
#endif

	update_ops();			/* make sure op_info[] is current */

	if (oldrow < 0  ||  oldcol < 0)
	    rel_ok = FALSE;		/* relative ops ok? */
	else
	{
	    rel_ok = TRUE;

	    oldrow %= lines;		/* mod values into range */
	    oldcol %= columns;
	}
	newrow %= lines;
	newcol %= columns;

	best = &seqA;
	try = &seqB;

		/* try out direct cursor addressing */

	zero_seq(best);
	add_op(best, CURS_ADDR, newrow, newcol);

		/* try out independent row/column addressing */

	if (rel_ok)
	{
	    zero_seq(try);
	    row(try, oldrow, newrow);
	    column(try, oldcol, newcol);
	    Make_seq_best(best, try);
	}

	zero_seq(&col0seq);		/* store seq. to get from c0 to nc */
	column(&col0seq, 0, newcol);

	if(col0seq.cost < INFINITY)	/* can get from col0 to newcol */
	{
		    /* try out homing and then row/column */

	    if (! rel_ok  ||  newcol < oldcol  ||  newrow < oldrow)
	    {
		zero_seq(try);
		add_op(try, CURS_HOME, 1);
		row(try, 0, newrow);
		add_seq(try, &col0seq);
		Make_seq_best(best, try);
	    }

		    /* try out homing to last line  and then row/column */

	    if (! rel_ok  ||  newcol < oldcol  ||  newrow > oldrow)
	    {
		zero_seq(try);
		add_op(try, CURS_TO_LL, 1);
		row(try, lines - 1, newrow);
		add_seq(try, &col0seq);
		Make_seq_best(best, try);
	    }
	}

#ifdef TRACE
	if (_tracing)
	    _tracef("\tmvcur: result follows");
#endif

	out_seq(best);

#ifdef TRACE
	if (_tracing)
	    _tracef("\tmvcur: end of result");
#endif
}


/*
**	row(outseq, oldrow, newrow)
**
**	row() adds the best sequence for moving
**  the cursor from oldrow to newrow to seq.
**	row() considers row_address, parm_up/down_cursor
**  and cursor_up/down.
*/

static
row(outseq, orow, nrow)
int		orow, nrow;		/* old, new cursor locations */
struct Sequence	*outseq;		/* where to put the output */
{
	struct Sequence	seqA, seqB,
			*best,		/* best sequence so far */
			*try;		/* next try */
	int	parm_cursor, one_step;

	best = &seqA;
	try = &seqB;

	if (nrow == orow)
	    return;

	if (nrow < orow)
	{
	    parm_cursor = P_UP_CURS;
	    one_step = CURS_UP;
	}
	else
	{
	    parm_cursor = P_DOWN_CURS;
	    one_step = CURS_DOWN;
	}

		/* try out direct row addressing */

	zero_seq(best);
	add_op(best, ROW_ADDR, nrow);

		/* try out paramaterized up or down motion */

	if (rel_ok)
	{
	    zero_seq(try);
	    add_op(try, parm_cursor, abs(orow - nrow));
	    Make_seq_best(best, try);
	}
		/* try getting there one step at a time... */

	if (rel_ok)
	{
	    zero_seq(try);
	    add_op(try, one_step, abs(orow-nrow));
	    Make_seq_best(best, try);
	}

	add_seq(outseq, best);
}


/*
**	column(outseq, oldcol, newcol)
**
**	column() adds the best sequence for moving
**  the cursor from oldcol to newcol to outseq.
**	column() considers column_address, parm_left/right_cursor,
**  simp_col(), and carriage_return followed by simp_col().
*/

static
column(outseq, ocol, ncol)
struct Sequence	*outseq;			/* where to put the output */
int		ocol, ncol;			/* old, new cursor  column */
{
	struct Sequence	seqA, seqB,
			*best, *try;
	int		parm_cursor;	/* set to either parm_up/down_cursor */

	best = &seqA;
	try = &seqB;

	if (ncol == ocol)
	    return;

	if (ncol < ocol)
	    parm_cursor = P_LEFT_CURS;
	else
	    parm_cursor = P_RIGHT_CURS;

		/* try out direct column addressing */

	zero_seq(best);
	add_op(best, COL_ADDR, ncol);

		/* try carriage_return then simp_col() */

	if (! rel_ok  ||  (ncol < ocol))
	{
	    zero_seq(try);
	    add_op(try, CARRIAGE_RETURN, 1);
	    simp_col(try, 0, ncol);
	    Make_seq_best(best, try);
	}
	if (rel_ok)
	{
		/* try out paramaterized left or right motion */

	    zero_seq(try);
	    add_op(try, parm_cursor, abs(ocol - ncol));
	    Make_seq_best(best, try);

		/* try getting there with simp_col() */

	    zero_seq(try);
	    simp_col(try, ocol, ncol);
	    Make_seq_best(best, try);
	}

	add_seq(outseq, best);
}


/*
** 	simp_col(outseq, oldcol, newcol)
**
**	simp_col() adds the best simple sequence for getting
**  from oldcol to newcol to outseq.
**	simp_col() considers (back_)tab and cursor_left/right.
**
**  Revisions needed:
**	Simp_col asssumes that the cost of a (back_)tab
**  is less then the cost of one-stepping to get to the same column.
**	Should sometimes use overprinting instead of cursor_right.
*/

static
simp_col(outseq, oc, nc)
struct Sequence	*outseq;		/* place to put sequence */
int		oc, nc;			/* old column, new column */
{
	struct Sequence	seqA, seqB, tabseq,
			*best, *try;
	int		mytab, tabs, onepast,
			one_step, opp_step; 

	if (! rel_ok)
	{
	    outseq->cost = INFINITY;
	    return;
	}

	if (oc == nc)
	    return;

	best = &seqA;
	try  = &seqB;

	if (oc < nc)
	{
	    mytab = TAB;
	    if (init_tabs > 0  &&  op_info[TAB] < INFINITY)
	    {
		tabs = nc / init_tabs - oc / init_tabs;
		onepast = ((nc / init_tabs) + 1) * init_tabs;
		if (tabs)
		    oc = onepast - init_tabs;	/* consider it done */
	    }
	    else
		tabs = 0;

	    one_step = CURS_RIGHT;
	    opp_step = CURS_LEFT;
	}
	else
	{
	    mytab = BACK_TAB;
	    if (init_tabs > 0  &&  op_info[BACK_TAB] < INFINITY)
	    {
		tabs = oc / init_tabs - nc / init_tabs;
		onepast = ((nc - 1) / init_tabs) * init_tabs;
		if (tabs)
		    oc = onepast + init_tabs;	/* consider it done */
	    }
	    else
		tabs = 0;

	    one_step = CURS_LEFT;
	    opp_step = CURS_RIGHT;
	}

		/* tab as close as possible to nc */

	zero_seq(&tabseq);
	add_op(&tabseq, mytab, tabs);

		/* try extra tab and backing up */

	zero_seq(best);

	if (onepast >= 0  &&  onepast < columns)
	{
	    add_op(best, mytab, 1);
	    add_op(best, opp_step, abs(onepast - nc));
	}
	else
	    best->cost = INFINITY;	/* make sure of next swap */

		/* try stepping to nc */

	zero_seq(try);
	add_op(try, one_step, abs(nc - oc));
	Make_seq_best(best, try);
	
	if (tabseq.cost < INFINITY)
	    add_seq(outseq, &tabseq);
	add_seq(outseq, best);
}


/*
**	zero_seq(seq)
**	add_seq(seq1, seq2)
**	out_seq(seq)
**
**	zero_seq() empties seq.
**	add_seq() adds seq1 to seq2.
**	out_seq() outputs a sequence.
*/

static
zero_seq(seq)
struct Sequence	*seq;
{
	seq->end = seq->vec;
	seq->cost = 0;
}


static
add_seq(seq1, seq2)
register struct Sequence *seq1, *seq2;
{
	int	*vptr;

	if(seq1->cost >= INFINITY  ||  seq2->cost >= INFINITY)
	    seq1->cost = INFINITY;
	else
	{
	    vptr = seq2->vec;
	    while (vptr != seq2->end)
		*(seq1->end++) = *(vptr++);
	    seq1->cost += seq2->cost;
	}
}


static
out_seq(seq)
register struct Sequence *seq;
{
	char		*tparm();
	register int	*opptr;
	int		prm[9], ps, p, op, outc();
	int		count;
	char		*sequence();

	if (seq->cost >= INFINITY)
	    return;

	for (opptr = seq->vec;  opptr < seq->end;  opptr++)
	{
	    op = *opptr;			/* grab operator */
	    ps = -op_info[op];
	    if(ps > 0)				/* parameterized */
	    {
		for (p = 0;  p < ps;  p++)	/* fill in needed parms */
		    prm[p] = *(++opptr);

		tputs(tparm(sequence(op),
			        prm[0], prm[1], prm[2], prm[3], prm[4],
				prm[5], prm[6], prm[7], prm[8]), 1, outc);
	    }
	    else
	    {
		count = *(++opptr);
		    /*rev should save tputs output instead of mult calls */
		while (count--)			/* do count times */
		    tputs(sequence(op), 1, outc);
	    }
	}
}


/*
**	update_ops()
**
**	update_ops() makes sure that
** the op_info[] array is updated and initializes
** the cost array for SP if needed.
*/

static
update_ops()
{
	char	*index();

	if (SP)				/* SP structure exists */
	{
	    register int op; 

	    out_file = SP->_ofp;	/* set output file pointer */

	    if (! SP->_costinit)	/* this term not yet assigned costs */
	    {
		loc_init = FALSE;	/* if !SP in the future, new term */
		init_costs(SP->_costs);	/* fill term costs */
		SP->_costinit = TRUE;
	    }

	    for (op = 0;  op < NUM_NPARM;  op++)
		op_info[op] = SP->_costs[op];	/* set up op_info */
	    
		/* check for newline that might be mapped... */
	    if (SP->_nlmapping && (index(sequence(CURS_DOWN), '\n') != NULL))
		op_info[CURS_DOWN] = INFINITY;
	}
	else
	{
	    out_file = stdout;

	    if (! loc_init)			/* using local costs */
	    {
		loc_init = TRUE;
		init_costs(op_info);		/* set up op_info */
	    }
		/* check for newline that might be mapped... */
	    if (index(sequence(CURS_DOWN), '\n') != NULL)
		op_info[CURS_DOWN] = INFINITY;
	}
}


/*
**	init_costs(costs)
**
**	init_costs() fills the array costs[NUM_NPARM]
** with costs calculated by doing tputs() calls.
*/

static
init_costs(costs)
int	costs[];
{
	register int	i;
	int		countc();

	for (i = 0;  i < NUM_NPARM;  i++)
	    if(sequence(i) != (char *) 0)
	    {
#ifdef TRACE
	if (_tracing)
	    _tracef("\tinit_costs: pricing %d: '%s'", i, sequence(i));
#endif

		c_count = 0;
		tputs(sequence(i), 1, countc);
		costs[i] = c_count;
	    }
	    else
		costs[i] = INFINITY;
}


/*
**	countc()
**	outc(c)
**	savec(c)
**	
**	countc() increments global var c_count.
**	outc() outputs a single character.
**	savec() saves c in *c_save and increments c_save and c_count.
*/

static
countc()
{
	c_count++;
}


static
outc(c)
char c;
{
	fputc(c, out_file);
}


/*rev not yet needed 
static
savec(c)
char c;
{
	*(c_save++) = c;
	c_count++;
}
*/


/*
**	add_op(seq, op, p0, p1, ... , p8)
**
**	add_op() adds the operator op and the appropriate
**  number of paramaters to seq.  It also increases the 
**  cost appropriately.
**	if op has no parameters, p0 is taken to be a count.
*/

static
add_op(seq, op, p0, p1, p2, p3, p4, p5, p6, p7, p8)
struct Sequence	*seq;
int		op, p0, p1, p2, p3, p4, p5, p6, p7, p8;
{
	char	*tparm();
	int	num_ps, p;

#ifdef TRACE
	if (_tracing)
	    _tracef("\tadd_op(%o,%d,%d,%d) called", seq, op, p0, p1);
#endif

	num_ps = - op_info[op];		/* get parms or -cost */
	*(seq->end++) = op;

	if (num_ps == (- INFINITY)  ||  sequence(op) == (char *) 0)
	    seq->cost = INFINITY;
	else
	    if (num_ps <= 0)		/* no parms, -cost */
	{
	    seq->cost -= p0 * num_ps;	/* ADD count * cost */
	    *(seq->end++) = p0;
	}
	else
	{
	    int	pms[9];

	    pms[0] = p0;  pms[1] = p1;  pms[2] = p2;
	    pms[3] = p3;  pms[4] = p4;  pms[5] = p5;
	    pms[6] = p6;  pms[7] = p7;  pms[8] = p8;  
	    for(p = 0;  p < num_ps;  p++)
		*(seq->end++) = pms[p];
	    c_count = 0;
	    tputs(tparm(sequence(op), p0, p1, p2, p3, p4, p5, p6, p7, p8),
								 1, countc);
	    seq->cost += c_count;
	}
}


/*
**	char	*
**	sequence(op)
**
**	sequence() returns a pointer to the op's
**  terminal control sequence.
*/

static char	*
sequence(op)
register int op;
{
	
	switch(op)
	{
	    case CARRIAGE_RETURN:
		return (carriage_return);
	    case CURS_DOWN:
		return (cursor_down);
	    case CURS_HOME:
		return (cursor_home);
	    case CURS_LEFT:
		return (cursor_left);
	    case CURS_RIGHT:
		return (cursor_right);
	    case CURS_TO_LL:
		return (cursor_to_ll);
	    case CURS_UP:
		return (cursor_up);
	    case TAB:
		return (tab);
	    case BACK_TAB:
		return (back_tab);
	    case ROW_ADDR:
		return (row_address);
	    case COL_ADDR:
		return (column_address);
	    case P_DOWN_CURS:
		return (parm_down_cursor);
	    case P_LEFT_CURS:
		return (parm_left_cursor);
	    case P_RIGHT_CURS:
		return (parm_right_cursor);
	    case P_UP_CURS:
		return (parm_up_cursor);
	    case CURS_ADDR:
		return (cursor_address);
	    default:
		return ((char *) 0);
	}
}
