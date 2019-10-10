#define	YYACTSH		13
#define	YYAMASK		017777
#define	YYSHIFTACT	0
#define	YYREDACT	1
#define	YYACCEPTACT	2
#define	YYERRACT	3
#define	YYGOTO		4
#define	YYPACTION	5
#define	YYEOFVAL	(-1)
#define	YYERRVAL	(-2)
#define	YYOTHERS	(-1000)

#include <common/ccompat.h>

$D

YYSTYPE		yylval;

#if	! defined (YYALLOCA) && ! YYMALLOC
/*
 * By default, we use the alloca () function to get ourselves more memory if
 * the parser stack overflows.
 */
# define	YYALLOCA	1
#endif

#if	YYALLOCA
/*
 * In order to use alloca (), we must re-allocate every time the parser is
 * entered.
 */
# define	YYALLOC(s)	alloca (s)
# define	YYFREE(x)	((void) 0)
# define	YYSTAT

#elif	YYMALLOC
/*
 * With malloc (), by default we remember our allocations, although we can
 * be persuaded not to.
 */
# define	YYALLOC(s)	malloc (s)
# define	YYFREE(x)	free (x)
# ifndef YYSTAT
#  define	YYSTAT		static
# endif

#endif

#ifdef	YYALLOC
/*
 * There is no standard header from which to retrieve a prototype for the
 * alloca () function. <stdlib.h> and <string.h> seem the most likely places,
 * and since we want <string.h> for the mem... () functions in this situation
 * anyway, we'll try there.
 */
# include <string.h>
#endif

#ifndef	YYMAXDEPTH
/*
 * Choose a default parser-stack size; if we can dynamically grow, start out
 * small. Otherwise, start out with lots.
 */
# ifdef	YYALLOC
#  define	YYMAXDEPTH	100
# else
#  define	YYMAXDEPTH	256
# endif
#endif

/*
 * Alternate definitons of YYACCEPT and YYABORT are only permissible if the
 * storage system selected does not require deallocation.
 */

#if	(defined (YYACCEPT) || defined (YYABORT)) && \
	defined (YYALLOC) && (1 - YYSTAT - 1) == 2 && ! YYALLOCA
# error	Incompatible parser options selected
#endif

#ifndef	YYACCEPT
# define	YYACCEPT   do { yyerrflag = 0; goto yyparsexit; } while (0)
#endif

#ifndef	YYABORT
# define	YYABORT	   do { yyerrflag = 1; goto yyparsexit; } while (0)
#endif

#ifndef	YYERROR
# define	YYERROR		goto YYusererr
#endif

#ifndef	YYRECOVERING
# define	YYRECOVERING()	(yyerrflag != 0)
#endif

#ifndef	YYCLEARIN
# define	YYCLEARIN	yychar = YYNOCHAR
#endif

#ifndef	yyclearin
# define	yyclearin
#endif

#ifndef	YYERROK
# define	YYERROK		yyerrflag = 0
#endif

#ifndef	yyerrok
# define	yyerrok		YYERROK
#endif

#ifndef	YYABRTMSG
/*
 * Prototypes for write () and exit () are in <unistd.h> and <stdlib.h>
 * respectively.
 */
# include <unistd.h>
# include <stdlib.h>
# define	YYABRTMSG(msg)	(write (2, msg, sizeof (msg) - 1), exit (1))
#endif

#define YYNOCHAR		(-1000)

#ifdef YYDEBUG
int yydebug = 1;
# include <stdio.h>
#endif

#ifndef	yylex
extern	int		yylex		__PROTO ((void));
#endif


#if	__USE_PROTO__
int yyparse (void)
#else
int
yyparse ()
#endif
{
#ifdef	YYALLOC

YYSTAT	int	      *	yystack;
YYSTAT	YYSTYPE	      *	yyvstack;
YYSTAT	unsigned	yystksize = 0;

#else	/* static allocation */

static int		yystack [YYMAXDEPTH];
static YYSTYPE		yyvstack [YYMAXDEPTH];
#define	yystksize	YYMAXDEPTH

#endif

	YYSTYPE	      *	yypvt;
	int		act;
	unsigned      *	ip;
	int		yystate = 0;
	int		pno;
	short		yyerrflag = 0;
	int	      *	yys;
	YYSTYPE	      *	yyv;
	int		yychar = YYNOCHAR;
	YYSTYPE		yyval;	/* copy of yylval to deal with lookahead */

#ifdef	YYALLOC
	if (yystksize == 0) {
		yystksize = YYMAXDEPTH;
		yystack = (int *) YYALLOC (YYMAXDEPTH * (sizeof (int) +
							 sizeof (YYSTYPE)));
		if (yystack == NULL)
			YYABRTMSG ("Unable to allocate parser stack");
		yyvstack = (YYSTYPE *) (yystack + YYMAXDEPTH);
	}
#endif

	yys = yystack - 1;
	yyv = yyvstack - 1;

stack:
	if (++ yys == yystack + yystksize) {
#ifdef	YYALLOC
		int	      *	yysnew;
		YYSTYPE	      *	yyvnew;
		unsigned	newsize = yystksize + yystksize / 2;

		yysnew = (int *) YYALLOC (newsize * (sizeof (int) +
						     sizeof (YYSTYPE)));
		if (yysnew == 0)
			YYABRTMSG ("yyparse : stack overflow");
		yyvnew = (YYSTYPE *) (yysnew + newsize);

		memcpy (yysnew, yystack, yystksize * sizeof (int));
		memcpy (yyvnew, yyvstack, yystksize * sizeof (YYSTYPE));

		YYFREE (yystack);

		yys = yysnew + yystksize;
		yyv = yyvnew + yystksize - 1;

		yystack = yysnew;
		yyvstack = yyvnew;
		yystksize = newsize;
#else
		YYABRTMSG ("yyparse : stack overflow");
#endif
	}
	* yys = yystate;
	* ++ yyv = yyval;
#ifdef YYDEBUG
	if (yydebug)
		fprintf (stdout, "Stack state %d, char %d\n", yystate,
			 yychar);
#endif

read:
	ip = yyact + yypa [yystate];
	if (ip [1] != YYNOCHAR) {
		if (yychar == YYNOCHAR) {
			yychar = yylex ();
#ifdef YYDEBUG
			if (yydebug)
				fprintf (stdout, "lex read char %d, val %d\n",
					 yychar, yylval);
#endif
				if (yychar == 0)
					yychar = YYEOFVAL;
		}
		while (ip [1] != YYNOCHAR) {
			if (ip [1] == yychar)
				break;
			ip += 2;
		}
	}
	act = ip [0];
	switch (act >> YYACTSH) {
	case YYSHIFTACT:
		if (ip [1] == YYNOCHAR)
			goto YYerract;
		if (yychar != YYEOFVAL)
			yychar = YYNOCHAR; /* dont throw away EOF */
		yystate = act & YYAMASK;
		yyval = yylval;
#ifdef YYDEBUG
		if (yydebug)
			fprintf(stdout, "shift %d\n", yystate);
#endif
		if (yyerrflag)
			-- yyerrflag;
		goto stack;

	case YYACCEPTACT:
#ifdef YYDEBUG
		if (yydebug)
			fprintf (stdout, "accept\n");
#endif
		return 0;

	case YYERRACT:
YYerract:
		if (yyerrflag == 0)
			yyerror ("Syntax error");
		goto YYusererr;	/* suppress warnings from Coherent 'cc' */
YYusererr:
		switch (yyerrflag) {
		case 0:
		case 1:
		case 2:
			yyerrflag = 3;
			while (yys >= yystack) {
				ip = & yyact [yypa [* yys]];
				while (ip [1] != YYNOCHAR)
					ip += 2;
				if ((* ip & ~ YYAMASK) ==
				    (YYSHIFTACT << YYACTSH)) {
					yystate = * ip & YYAMASK;
					goto stack;
				}
#ifdef YYDEBUG
				if (yydebug)
					fprintf (stderr,
						 "error recovery leaves state %d, uncovers %d\n",
						 * yys, yys [-1]);
#endif
				yys --;
				yyv --;
			}
#ifdef YYDEBUG
			if (yydebug)
				fprintf(stderr, "no shift on error; abort\n");
#endif
			YYABORT;

		case 3:
#ifdef YYDEBUG
			if (yydebug)
				fprintf (stderr,
					 "Error recovery clobbers char 0x%x\n",
					 yychar);
#endif
			if (yychar == YYEOFVAL)
				YYABORT;

			yychar = YYNOCHAR;
			goto read;
		}

	case YYREDACT:
		pno = act & YYAMASK;
#ifdef YYDEBUG
		if (yydebug)
			fprintf (stdout, "reduce %d\n", pno);
#endif
		yypvt = yyv;
		yyv -= yypn [pno];
		yys -= yypn [pno];
		yyval = yyv [1];
		switch(pno) {
$A
		}
		ip = yygo + yypgo [yypdnt [pno]];
		while (* ip != * yys && * ip != YYNOCHAR )
			ip += 2;
		yystate = ip [1];
		goto stack;
	}

	/*
	 * Perform an exit from the parser which can include optional cleanup.
	 * Note that our test for whether YYSTAT is empty (similar in form to
	 * a test in <common/_feature.h> for __STDC__) can fail if the user
	 * defines it to something other that 'static' or empty. For the
	 * uninitiated, 'static' will be replaced by '0' in the #if-expansion
	 * phase.
	 */
yyparsexit:
#if	(1 - YYSTAT - 1) == 2	/* if YYSTAT empty, 1 - - 1 == 2, else 0 */
	YYFREE (yystack);
	yystksize = 0;
#endif
	return yyerrflag;
}
