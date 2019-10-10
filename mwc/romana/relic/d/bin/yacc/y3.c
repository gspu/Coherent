/*
 * LALR-1 parser generator
 * generation of lookahead sets
 * the algorithm is thanks to Frank D. Remer & Thomas J Pennello,
 *  "Efficient computation of LALR(1) lookahead sets",
 *   SIGPLAN conference 1979.
 */

#include "yacc.h"
#include <assert.h>

/* the size of SMAX must be related to the amount of space available
   on the runtime stack for recursive calls to traverse.
   (on systems which do not dynamically allocate the stack like RSX)
   on RSX traverse takes up about 18 bytes per call which means that
   with a 1024 byte stack we can probably make 50-55 levels of recursion
*/
#define SMAX 100

#define INFINITY 10000		/* infinity is small for a mathematician */
#define INITCODE 1
#define COPYCODE 2
#define UNIONCODE 3
#define EOFCODE 4
static struct
{
	int	s_last;
	int	s_vals[SMAX];
} stk;				/* stack for digraph */

static struct trans *transp;	/* temporary vector for traverse */
static struct lset *lsetp, *fsetp;
static int nttrans;
int rread(), rincl();
struct lset *getset();

genlook()
{
	int code = EOFCODE;
	rewopt();
	cttrans();
	transp = (struct trans *)yalloc(nttrans, sizeof *transp);
	cdread();
	rread(0);
	rread(1);
	digraph();
	free(transp->t_trans->ng_rel);
	rincl(0);
	rincl(1);
	digraph();
	free(transp->t_trans->ng_rel);
	free(transp);
	fwrite(&code, sizeof code, 1, optout);
	execute();
	lookback();
}

cdread()
{
	register i, j, k;
	struct state *stp, *stp1;
	struct ntgo *ntp;
	struct lset lset;
	int code = INITCODE;

	for(i=0; i<nstates; i++) {
		stp = &states[i];
		for(j=0; j<stp->s_ntgo; j++) {
			zerolset(&lset);
			ntp = &stp->s_ntgos[j];
			stp1 = &states[ntp->ng_st];
			for(k=0; k<stp1->s_tgo; k++) 
				setbit(&lset, stp1->s_tgos[k].tg_trm);
			fwrite(&code, sizeof code, 1, optout);
			fwrite(&ntp, sizeof ntp, 1, optout);
			fwrite(lset.l_bits, sizeof lset.l_bits, 1, optout);
		}
	}
}



cttrans()
{
	/* count number of nonterminal transations in automaton */
	register i;

	nttrans = 0;
	for(i=0; i<nstates; i++)
		nttrans += states[i].s_ntgo;
}

digraph()
{
	register i;
	stk.s_last = 0;
	zerolev();
	for(i=0; i<nttrans; i++)
		if( transp[i].t_level==0 )
			traverse(i);
}

traverse(x)
register int x;
{
	register i, y;
	int k;
	struct ntgo *ntp;
	int code = UNIONCODE;

	if( stk.s_last>= (SMAX-1) )
		yyerror(FATAL|NLNO, "internal stack overflow - SMAX");
	stk.s_vals[++stk.s_last] = x;
	k = stk.s_last;
	transp[x].t_level = k;
	ntp = transp[x].t_trans;

	for(i=0; i<ntp->ng_rel->r_count; i++) {
		y = ntp->ng_rel->r_list[i];
		if( transp[y].t_level==0 )
			traverse(y);
		if( transp[y].t_level < transp[x].t_level )
			transp[x].t_level = transp[y].t_level;
		fwrite(&code, sizeof code, 1, optout);
		fwrite(&ntp, sizeof ntp, 1, optout);
		fwrite(&transp[y].t_trans, sizeof ntp, 1, optout);
	}

	code = COPYCODE;
	if( transp[x].t_level == k ) {
		transp[x].t_level = INFINITY;
		while( (y = stk.s_vals[stk.s_last--]) != x ) {
			transp[y].t_level = INFINITY;
			fwrite(&code, sizeof code, 1, optout);
			fwrite(&transp[y].t_trans, sizeof ntp, 1, optout);
			fwrite(&ntp, sizeof ntp, 1, optout);
		}
	}
}

/*
 * form the set unions of the read sets and the follow sets, following
 * the codes left in temp file by digraph
 */
execute()
{
	int code;
	struct ntgo *ntp1, *ntp2;
	register struct lset *csetp;

	rewopt();
	csetp = lsetp = (struct lset *)yalloc(nttrans, sizeof *lsetp);
	fsetp = NULL;
	for(;;) {
		if( fread(&code, sizeof code, 1, optout) != 1 )
			yyerror(NLNO|FATAL, "eof on tempfile in execute");
		switch( code ) {
		case INITCODE:
			fread(&ntp1, sizeof ntp1, 1, optout);
			assert( csetp < &lsetp[nttrans] );
			fread(csetp->l_bits, sizeof csetp->l_bits,  1, optout);
/*
			fprintf(listout, "init: "); ptrans(ntp1);
			prlset(csetp); fprintf(listout,"\n");
*/
			ntp1->ng_lset = csetp++;
			break;

		case UNIONCODE:
			fread(&ntp1, sizeof ntp1, 1, optout);
			fread(&ntp2, sizeof ntp2, 1, optout);
/*
			fprintf(listout, "union "); ptrans(ntp1); fprintf(listout," |=");
			ptrans(ntp2);
*/
			setunion(ntp1->ng_lset, ntp2->ng_lset);
/*
			prlset(ntp1->ng_lset); fprintf(listout, "\n");
*/
			break;

		case COPYCODE:
			fread(&ntp1, sizeof ntp1, 1, optout);
			fread(&ntp2, sizeof ntp2, 1, optout);
			xxx("copy", ntp1, ntp2);
			freeset(ntp1->ng_lset);
			ntp1->ng_lset = getset();
			copylset(ntp1->ng_lset, ntp2->ng_lset);
			break;

		case EOFCODE:
			return;
		default:
			yyerror(NLNO|FATAL, "bad temp file; code %o\n", code);
		}
	}
}

lookback()
{
	register i;

	for(i=0; i<nprod; i++)
		reduce(prdptr[i]);

}

reduce(pp)
register struct prod *pp;
{
	register nt, i;
	struct sym *sp;
	struct redn *rdp;
	struct state *stp, *stp1;
	int j, sno, sno1;
	struct ntgo *ntp;

	nt = -pp->p_left;
	sp = ntrmptr[nt-NTBASE];
	for(j=0; j<sp->s_nstates; j++) {
		stp = &states[sno = sp->s_states[j]];
		for(i=0; i<stp->s_ntgo; i++) {
			ntp = &stp->s_ntgos[i];
			if( ntp->ng_nt == nt )
				break;
		}
		assert(pp->p_prodno==0 || i<stp->s_ntgo);
		sno1 = go2star(sno, pp->p_right, pp->p_right+prodl(pp));
		stp1 = &states[sno1];
		if( stp1->s_reds == (struct redn *)NULL ) {	/* MWC DSC */
			stp1->s_reds = (struct redn *)yalloc(stp1->s_nred, sizeof *stp1->s_reds);
			stp1->s_nred = 0;
		}
		rdp = stp1->s_reds;
		for(i=0; i<stp1->s_nred; i++,rdp++)
			if( rdp->rd_prod==pp )
				break;
		if( yydebug ) {
			fprintf(listout, "(%d, pdn %d) ", sno1, pp->p_prodno);
			prlset(ntp->ng_lset);
			fprintf(listout, "\n");
		}
		if( i==stp1->s_nred ) {
			rdp->rd_prod = pp;
			rdp->rd_lset = getset();
			if( pp->p_prodno!=0 ) {
				copylset(rdp->rd_lset, ntp->ng_lset);
			} else {
				zerolset(rdp->rd_lset);
				setbit(rdp->rd_lset, EOFNO);
			}
			stp1->s_nred++;
		} else {
			setunion(rdp->rd_lset, ntp->ng_lset);
		}
	}
}

go2star(sno, ip, fin)
register *ip;
int *fin;
{
	register i;
	register struct state *stp;

	while( ip!=fin ) {
		stp = &states[sno];
		if( *ip>=NTBASE ) {
			for(i=0; i<stp->s_ntgo; i++)
				if( stp->s_ntgos[i].ng_nt == *ip )
					break;
			assert(i<stp->s_ntgo);
			sno = stp->s_ntgos[i].ng_st;
		} else {
			for(i=0; i<stp->s_tgo; i++)
				if( stp->s_tgos[i].tg_trm == *ip )
					break;
			assert(i<stp->s_tgo);
			sno = stp->s_tgos[i].tg_st;
		}
		ip++;
	}
	return(sno);
}

rread(todo)
{
	struct state *stp;
	register struct sym *sp;
	register i, j;
	int p;
	struct ntgo *ntp;

	if( todo )
		zerolev();
	else
		startcount();

	for(i=0; i<nttrans; i++) {
		stp = &states[transp[i].t_trans->ng_st];
		for(j=0; j<stp->s_ntgo; j++) {
			ntp = &stp->s_ntgos[j];
			sp = ntrmptr[ntp->ng_nt-NTBASE];
			if( sp->s_flags&DERIV ) {
				p = transp[i].t_level++;
				if( todo ) {
					transp[i].t_trans->ng_rel->r_list[p] =
					    rsearch(ntp);
					xxx("reads",transp[i].t_trans,ntp);
				}
				break;
			}
		}
	}
	if( !todo )
		endcount();
}

rincl(todo)
{
	register i, j;
	int *pb, *pe, k, p, sno, sno1, ind, nt;
	struct state *stp;
	struct prod *pp;
	struct ntgo *ntp;
	struct sym *sp;

	if( todo )
		zerolev();
	else
		startcount();
	for(i=0; i<nttrans; i++) {
		sno = ssearch(transp[i].t_trans);
		sp = ntrmptr[transp[i].t_trans->ng_nt-NTBASE];
		for(j=0; j<sp->s_nprods; j++) {
			pp = sp->s_prods[j];
			pb = pp->p_right;
			pe = pb + prodl(pp);
			while( pb <= --pe && (nt = *pe) >= NTBASE ) {
				sno1 = go2star(sno, pb, pe);
				stp = &states[sno1];
				for(k=0; k<stp->s_ntgo; k++)
					if( stp->s_ntgos[k].ng_nt==nt )
						break;
				assert(k<stp->s_ntgo);
				ind = rsearch(&stp->s_ntgos[k]);
				p = transp[ind].t_level++;
				if( todo ) {
					ntp = transp[ind].t_trans;
					xxx("includes", ntp, transp[i].t_trans);
					ntp->ng_rel->r_list[p] = i;
				}
				if( (ntrmptr[nt-NTBASE]->s_flags&DERIV)==0 )
					break;
			}
		}
	}
	if( !todo )
		endcount();
}

endcount()
{
	register i, k, *relp; /* relp must be integer pointer */
	struct ntgo *ntp;
	k = 0;
	for(i=0; i<nttrans; i++)
				 {
		k += transp[i].t_level+1; /* include 1 for count */
		}
	relp = (int *)yalloc(k, sizeof *relp);
	for(i=0; i<nttrans; i++) {
		ntp = transp[i].t_trans;
		ntp->ng_rel = relp;
		ntp->ng_rel->r_count = transp[i].t_level;
		relp += transp[i].t_level+1;
	}
}

startcount()
{
	register i, j, k;
	struct state *stp;

	k = 0;
	for(i=0; i<nstates; i++) {
		stp = &states[i];
		for(j=0; j<stp->s_ntgo; j++) {
			transp[k].t_trans = &stp->s_ntgos[j];
			transp[k].t_level = 0;
			k++;
		}
	}
}

zerolev()
{
	register i;
	for(i=0; i<nttrans; i++)
		transp[i].t_level = 0;
}

rsearch(ntp)
struct ntgo *ntp;
{
	register lb, nb;
	register struct ntgo *el;
	int ub;

	lb = 0;
	ub = nttrans-1;
	do {
		nb = (ub+lb)/2;
		if( (el = transp[nb].t_trans) < ntp )
			lb = nb+1;
		else if( el > ntp )
			ub = nb-1;
		else
			return(nb);
	} while( lb<=ub );
	yyerror(NLNO|FATAL, "oops in rsearch");
}

ssearch(ntp)
struct ntgo *ntp;
{
	register lb, nb;
	register struct ntgo *el;
	int ub;

	lb = 0;
	ub = nstates-1;

	do {
		nb = (ub+lb) / 2;
		el = states[nb].s_ntgos;
		if( ntp < el )
			ub = nb-1;
		else if( ntp >= &el[states[nb].s_ntgo] )
			lb = nb+1;
		else
			return(nb);
	} while( lb<=ub );
	yyerror(NLNO|FATAL, "oops in ssearch");
}

prodl(pp)
register struct prod *pp;
{
	register *ip;

	ip = pp->p_right;
	while( *ip++ != -1 );
	return( ip-pp->p_right-1 );		/* BONZO OR NO??? */
}

struct lset *
getset()
{
	register struct lset *lp;
	if( fsetp ) {
		lp = fsetp;
		fsetp = fsetp->l_next;
		return(lp);
	}
	lp = (struct lset *)yalloc(1, sizeof *lp);
	return(lp);
}

freeset(lp)
struct lset *lp;
{
	lp->l_next = fsetp;
	fsetp = lp;
}

frlset()
{
	register struct state *stp;
	register i, j;
	struct lset *lp, *lp1;

	free(states[0].s_tgos);
	free(states[0].s_ntgos);
	for(i=0; i<nstates; i++) {
		stp = &states[i];
		for(j=0; j<stp->s_nred; j++)
			freeset(stp->s_reds[j].rd_lset);
		if( stp->s_reds != (struct redn *)NULL )	/* MWC DSC */
			free(stp->s_reds);
		for(j=0; j<stp->s_ntgo; j++)
			freeset(stp->s_ntgos[j].ng_lset);
	}
	lp = fsetp;
	while( lp ) {
		lp1 = lp->l_next;
		if( lp<&lsetp[0] || lp>=&lsetp[nttrans] )
			free(lp);
		lp = lp1;
	}
	free(lsetp);
}

xxx(s, ntp1, ntp2)
char *s;
struct ntgo *ntp1, *ntp2;
{
	if( !yydebug )
		return;
	ptrans(ntp1); fprintf(listout, " %s ", s); ptrans(ntp2); 
	fprintf(listout, "\n");
}
ptrans(ntp)
struct ntgo *ntp;
{
	int sno;
	sno = ssearch(ntp);
	fprintf(listout, "(%d, %s)", sno, ptosym(ntp->ng_nt));
}

prlset(ls)
struct lset *ls;
{
	register i, once;
	fprintf(listout, "[");
	once = 0;
	for(i=first(ls); i>=0; i=next(ls,i)) {
		if( once++ ) fprintf(listout, ",");
		fprintf(listout, "%s", trmptr[i]->s_name);
	}
	fprintf(listout, "]");
}


