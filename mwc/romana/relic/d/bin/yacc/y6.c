/*
 * COCOA optimizer
 * gets memory, reads in optimizer temp file
 * spits out parser
 */
#include "action.h"

struct actn *atab;
struct go2n *gotab;
int *patab, *pgotab;
int *pdl;
#include "yacc.h"

callopt()
{
	extern yyredns, yypact;
	frlset();
	atab = (struct actn *)yalloc(yypact, sizeof *atab);
	gotab = (struct go2n *)yalloc(yyredns, sizeof *gotab);
	patab = (int *)yalloc(nstates, sizeof *patab);
	pgotab = (int *)yalloc(nnonterm, sizeof *pgotab);
	pdl = (int *)yalloc(nprod, sizeof *pdl);
	rewopt();
	pronts();
	prodls();
	rdgos();
	rdacts();
	cpyparse();
}

pronts()
{
	register i;
	for (i=0; i<nprod; i++)
		pdl[i] = -prdptr[i]->p_left - NTBASE;
	fprintf(tabout, "#include <action.h>\n");
	warray("yypdnt", pdl, nprod, 0);
}

rdgos()
{
	register i, size;
	struct go2n g2, *pgo;
	int oldi;

	pgo = gotab;
	for(i=1; i<nnonterm; i++) {
		pgotab[i] = 2*(pgo-gotab);
		fread(&g2, sizeof g2, 1, optout);
		if( g2.from != (YYGOTO<<YYACTSH | i) )
			tmperr();
		size = g2.to;
		fread(pgo, sizeof *pgo, size, optout);
		if( (oldi = findgo2(pgo, size, i)) >= 0 )
			pgotab[i] = oldi;
		else
			pgo += size;
	}
	warray("yypgo", pgotab, nnonterm, 0);
	warray("yygo", gotab, 2*(pgo-gotab), 1);
}

rdacts()
{
	register i, size, j;
	struct actn act, *pa;
	int oldi;

	pa = atab;
	for(i=0; i<nstates; i++) {
		patab[i] = 2*(pa-atab);
		fread(&act, sizeof act, 1, optout);
		if( act.a_no != (YYPACTION<<YYACTSH | i) )
			tmperr();
		size = act.a_chr;
		fread(pa, sizeof *pa, size, optout);
		for(j=0; j<size; j++)
			if( pa[j].a_chr!=YYOTHERS )
				pa[j].a_chr = trmptr[pa[j].a_chr]->s_val;
		if( (oldi = findact(pa, size, i)) >= 0 )
			patab[i] = oldi;
		else
			pa += size;
	}
	warray("yypa", patab, nstates, 0);
	warray("yyact", atab, 2*(pa-atab), 1);
}

prodls()
{
	register i;
	for(i=0; i<nprod; i++)
		pdl[i] = prodl(prdptr[i]);
	warray("yypn", pdl, nprod, 0);
}

warray(s, a, n, sw)
char *s;
int *a, n, sw;
{
	register i;
	char *type;

	if (!sw) {	/* !sw means array of array refs */
		type = "char";
		for (i = 0; i < n; i++) {
			if (a[i] > 255) {
				type = "short";
				break;
			}
		}
	} else
		type = "int";
	fprintf(tabout, "unsigned %s %s[%d] = {\n", type, s, n--);

	i = 0;
	do {
		fprintf(tabout, "%d", a[i]);
		if (i != n)
			fputc(',', tabout);
		fputc(((++i%8 == 0) ? '\n' : ' '), tabout);
	} while (i <= n);

	if (i%8 != 0)
		fputc('\n', tabout);
	fprintf(tabout, "};\n");
}

tmperr()
{
	yyerror(NLNO|FATAL, "temp file error in optimizer");
}

cpyparse()
{
	register FILE *fparse, *actin;
	register c;
	if( (fparse = fopen(parser, "r")) == NULL )
		yyerror(NLNO|FATAL, "can't find parser");
	if( (actin = fopen(acttmp, "r")) == NULL )
		yyerror(NLNO|FATAL, "someone lost action temp file");
	while( (c = getc(fparse)) != EOF ) {
		if( c=='$' ) {
			if( (c = getc(fparse))=='A' ) {
				while( (c = getc(actin)) != EOF )
					putc(c, tabout);
				continue;
			}
			putc('$', tabout);
		}
		putc(c, tabout);
	}
	fclose(actin);
	fclose(fparse);
	fclose(tabout);
}

findgo2(gtp,s,n)
struct go2n *gtp;
int s, n;
{
	register os;
	register struct go2n *np, *op;
	int i;
	for(i=1; i<n; i++) {
		os = (pgotab[i+1] - pgotab[i]) / 2;
		if( os != s )
			continue;
		op = gotab + pgotab[i]/2;
		np = gtp;
		while( op->from==np->from && op->to==np->to && os-- ) {
			op++;
			np++;
		}
		if( os==0 ) {
			ndupgos += (pgotab[i+1] - pgotab[i])/2;
			return( pgotab[i] );
		}
	}
	return(-1);
}

findact(atp, s, n)
struct actn *atp;
{
	register struct actn *op, *np;
	register os;
	int i;

	for(i=0; i<n; i++) {
		os = (patab[i+1] - patab[i])/2;
		if( os!=s )
			continue;
		np = atab + patab[i]/2;
		op = atp;
		while(op->a_no==np->a_no && op->a_chr==np->a_chr && os--){
			op++;
			np++;
		}
		if( os==0 ) {
			ndupacts += (patab[i+1] - patab[i])/2;
			return( patab[i] );
		}
	}
	return(-1);
}
