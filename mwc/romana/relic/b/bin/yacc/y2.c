/*
 * LALR-1 parser generator
 *     read in rules
 *       - read in definitions
 *       - gobble rules
 *       - copy anything complicated to the output
 */

#include <string.h>
#include "yacc.h"
#include "action.h"

static int phonysemi;	/* allow missing ; aftter } */
struct sym *defsym(), *elook();
YYSTYPE yylval;
struct
{
	int	lex;
	YYSTYPE val;
} last = {-1};

readrules()
{
	struct sym *saccept, *seof, *serror;
	int t, t1, t2, nt, asc, pred, tokno, toktyp, c;
	struct sym *sp;


#if DGC
	printf("Read rules\n");
#endif
	saccept = defsym("$accept", TNTERM);
	seof = defsym("$end", TTERM);
	serror = defsym("error", TTERM);
	seof->s_val = YYEOFVAL;
	serror->s_val = YYERRVAL;

	/* part 1 - read in rules */
	while( (t = yylex()) != MARK ) {
#if DGC
		printf("t = %d\n", t);
#endif
		switch( t ) {

		case EOF:
			yyerror(FATAL, "end of file in definition section\n");
			break; /* stupid C switch statement */

		case START:
			if( yylex() != IDENT )
				yyerror(!FATAL, "bad %start syntax");
			else {
				defsym(yylval.sptr->s_name,TNTERM);
				startsym = yylval.sptr->s_no;
			}
			break;

		case UNION:
			copyunion();
			break;

		case TOKEN:
		case LEFT:
		case RIGHT:
		case NONASSOC:
			asc = t-TOKEN;
			if( t!=TOKEN )
				pred = predlev++;
			else
				pred = UNKNOWN;

		case TYPE:
			toktyp = UINT;
			while( (t1 = yylex()) == IDENT || t1 == T_IDENT ) {
				if( t1==T_IDENT ) {
					toktyp = yylval.sptr->s_no;
					continue;
				}
				sp = yylval.sptr;
				if( (t2 = yylex()) == INTEGER )
					if( t==TYPE )		/*MWC DSC*/
						yyerror(!FATAL, "token value assignment not allowed for %type");
					else
						tokno = yylval.ival;
				else {
					backup(t2);
					if( sp->s_name[0]!='\'' )
						tokno = tno++;
					else
						tokno = UNKNOWN;
				}
				if( t==TYPE && toktyp==UINT )
					yyerror(!FATAL,"Type id needed for %%type");
				switch( t ) {
				case TOKEN:
				case LEFT:
				case RIGHT:
				case NONASSOC: /* C needs ranges on case */

					deftok(sp, toktyp, asc, pred, tokno);
					break;

				case TYPE:
					defnttyp(sp, toktyp);
					break;
				}
				if( (t2 = yylex()) != COMMA ) backup(t2);
			}
			backup(t1);
			break;
		default:
			yyerror(FATAL, "unexpected symbol in definition section",
				t);
		}
	}

	wrtdefs();
	wrthdr();

	/* part 2 - read in rules */
	prdptr [0] = (struct prod *) yalloc (1, PROD_TOTAL_SIZE (3));
	PROD_EXTRA_INIT (prdptr [0]);

	prdptr [0]->p_prodno = 0;
	prdptr [0]->p_prc = prdptr [0]->p_ass = UNKNOWN;
	prdptr [0]->p_left = -saccept->s_no;
	PROD_RIGHT (prdptr [0]) [0] = startsym;
	PROD_RIGHT (prdptr [0]) [1] = seof->s_no;
	PROD_RIGHT (prdptr [0]) [2] = -1;

	nprod = 1;
	while ((phonysemi || ((t = yylex())) == C_IDENT)) {
		phonysemi = 0;
		defsym(yylval.sptr->s_name, TNTERM);
		nt = yylval.sptr->s_no;
		while( getrule(nt) );
	}
	if( t==MARK ) { /* gobble up rest of file */
		linepos(tabout);
		while( (c = llgetc()) != EOF )
			putc(c, tabout);
	} else if( t!=EOF )
		yyerror(FATAL, "bad rule syntax");
	wrtnames();
	fclose(defin);
	fclose(actout);
}

/*
 * lexical input reader
 *  - copy comments and percent curlies to the output w/o change
 *  - lookup the list of reserved keywords
 *  - enter identifiers and character constants in the symbol table
 *      character constants are special because they start with a apostrophe
 *      as do internally generated non terminals
 *  - the "value" of the token read is left in the global variable yylval
 *    this is either a symbol table pointer or an integer (for NUMBER)
 */
yylex()
{
	int c, i, lesk;
	char s[SYMSIZE];

	if( last.lex != -1 ) {
		yylval = last.val;
		lesk = last.lex;
		last.lex = -1;
		return( lesk );
	}
	last.lex = -1;

read:
	while( whitespace( c = llgetc() ) ) ;
	switch( c ) {
	case '|':
		return( VBAR );

	case ';':
		return( SEMICOLON );


	case '/':
		if( (c = llgetc()) != '*' )
			goto error;
		copycomment("*/", NULL, 0); /* write to the rathole */
		goto read;

	case '<':
		gettype(s);
		yylval.sptr = defsym(s, TTYPE);
		return( T_IDENT );

	case '{':
		return( LBRAC );

	case ',':
		return( COMMA );

	case '%':
		if( (c = llgetc()) == '{' ) {
			linepos(tabout);
			copycomment("%}", tabout, 0);
			goto read;
		} else if( c=='%' )
			return( MARK );
		llungetc(c);
		getword(s);
		for( i=0; restab[i].r_name ; i++ )
			if( strcmp(s, restab[i].r_name)==0 )
				return( restab[i].r_val );
		yyerror(!FATAL|SKIP, "illegal \"%%keyword\"");
		goto read;

	case '\'':
		c = readchar();
		if( llgetc() != '\'' ) {
			yyerror(!FATAL|SKIP, "bad character constant\n");
			goto read;
		}
		sprintf(s, "%s", prsym(c));
		yylval.sptr = defsym(s, TTERM);
		yylval.sptr->s_val = c;
		return( IDENT );

	case EOF:
		return(EOF);
	default:
		if( digit(c) ) {
			yylval.ival = readnum(c);
			return( INTEGER );
		} else if( alpha(c) ) {
			llungetc(c);
			getword(s);
			yylval.sptr = defsym(s, UNKNOWN);
			while( whitespace(c = llgetc()) );
			if( c==':' )
				return( C_IDENT );
			llungetc(c);
			return(IDENT);
		} else {
		error:
			yyerror(!FATAL|SKIP, "illegal character %c", c);
			goto read;
		}
	}
}

/* read a production (the bi's for a <- b0 .. bn ) until a ';' or '|' */

getrule(nt)
int nt;
{
	int precused, n, actpres, t;
	char s[SYMSIZE];
	register struct prod *pp;
	register struct sym *sp;
#ifdef IAPX86	/* workaround cc1 'too many stores' bug */
	char *tmp;
#endif

	nitprod->p_left = -nt;
	nitprod->p_prc = nitprod->p_ass = UNKNOWN;
	actpres = n = precused = 0;
	t = yylex();
	while( t==PREC || t==IDENT || t==LBRAC ) {
		switch( t ) {
		case PREC:
			t = yylex();
			sp = yylval.sptr;
			if( sp->s_genre!=TTERM || sp->s_prc<0 )
				yyerror(!FATAL,"bad %prec construct");
			else {
				nitprod->p_prc = sp->s_prc;
				nitprod->p_ass = sp->s_ass;
				precused++;
			}
			break;

		case IDENT:
			sp = yylval.sptr;
			if( sp->s_genre == UNKNOWN )
				defsym(sp->s_name, TNTERM);
			if( sp->s_genre==TTERM && sp->s_ass>UNASSOC &&
			    !precused ) {
				nitprod->p_ass = sp->s_ass;
				nitprod->p_prc = sp->s_prc;
			}
			if( n >= maxprodl-1 )
				yyerror(FATAL, "production too long");
			PROD_RIGHT (nitprod) [n++] = sp->s_no;
			break;

		case LBRAC:
			cpyact(n, ntrmptr[nt-NTBASE]);
			if( (t = yylex()) == IDENT ) { /* action inside rule */
				pp = (struct prod *) 
					yalloc (1, PROD_TOTAL_SIZE (1));
				PROD_EXTRA_INIT (pp);

				sprintf(s, "$$%d", nprod);
				sp = defsym(s, TNTERM);
				pp->p_prodno = nprod;
				pp->p_prc = pp->p_ass = UNKNOWN;
				pp->p_left = -sp->s_no;
				PROD_RIGHT (pp) [0] = -1;
				bounded(nprod, maxprod, "productions");
				prdptr[nprod++] = pp;
				PROD_RIGHT (nitprod) [n++] = sp->s_no;
			} else
				actpres++;
			continue;
		}
		t = yylex();
	}
	if( !actpres && ntrmptr[nt-NTBASE]->s_type>=0 ) {
		if( n==0 )
			yyerror(!FATAL, "must return value since lhs has type\n");
#ifndef IAPX86		 /* work around 'too many stores bug'. */
		else
			if (elook(PROD_RIGHT (nitprod) [0])->s_type !=
			    ntrmptr[nt-NTBASE]->s_type )
				yyerror(WARNING, "default action may cause type clash");
#else
		else {
			tmp = elook(PROD_RIGHT (nitprod) [0]);
			if (tmp->s_type != ntrmptr[nt-NTBASE]->s_type )
				yyerror(WARNING, "default action may cause type clash");
		}
#endif		/* cc bug workaround */
	}
	if (t == C_IDENT) {
		phonysemi = 1;
		t = SEMICOLON;
	}
	if (t!=VBAR && t!=SEMICOLON )
		yyerror(FATAL, "rule terminator not ';' or '|'");
	bounded(nprod, maxprod, "productions");
	nitprod->p_prodno = nprod;
	PROD_RIGHT (nitprod) [n++] = -1;

	prdptr [nprod] = (struct prod *) yalloc (1, PROD_TOTAL_SIZE (n));
	* prdptr [nprod] = * nitprod;
	PROD_EXTRA_INIT (prdptr [nprod]);

	if (PROD_EXTRA_SIZE (n) > 0)
		memcpy (prdptr [nprod]->p_ord, nitprod->p_ord,
			PROD_EXTRA_SIZE (n));

	nprod ++;
	return t == VBAR;
}

struct sym *
defsym(s, typ)
char *s;
{
	register struct sym *sp;
	register struct genre *gp;			/* MWC DSC */
	register i;
	int start;

	i = start = hash(s);
	while( (sp = symtab[i])!=NULL && strcmp(s, sp->s_name) )
		if( (i = (i+1) % maxsym ) == start )		/* MWC DSC */
			yyerror(FATAL, "symbol table overflow");

	if( sp==NULL ) {
#if DEBUG
		printf("defsym: %s new\n", s);
#endif
		sp = symtab[i] = (struct sym *)yalloc(1, sizeof *sp);
#ifdef DAVELIB
		sprintf(sp->s_name, "%.?s", SYMSIZE-1, s);
#else
		sprintf(sp->s_name, "%.*s", SYMSIZE-1, s);
#endif
		sp->s_no = -1;
		sp->s_prc = sp->s_ass = UNKNOWN;
		sp->s_type = UINT;
		sp->s_genre = UNKNOWN;
	}
	if( typ!=UNKNOWN && sp->s_genre!=typ ) {
		if( sp->s_genre!=UNKNOWN )
			yyerror(FATAL, "internal error - type redefinition");
		sp->s_genre = typ;
		if( (sp->s_no = gtab[typ].g_ordno++) >= gtab[typ].g_maxord )
			yyerror(FATAL, "too many %s, actual limit %d",
				gtab[typ].g_name, gtab[typ].g_maxord);
		gp = &gtab[typ];
		(*gp->g_sptr) [ sp->s_no ] = sp;
		sp->s_no += gp->g_base;
#if DEBUG
		printf("defsym: %s %d %d\n", s, typ, sp->s_no);
#endif
	}
	return(sp);
}

int hash(s)		/* Could have more scatter on 32-bit int machine */
register char *s;
{
	register unsigned sum;
	if( (sum = *s) == '\0' )
		return(0);
	do {
		sum += s[0] | s[1]<<8;
		s += 2;
	} while( s[-1]!='\0' && s[0]!='\0' );
	return( sum % maxsym );		/* MWC DSC */
}

gettype(s)
char *s;
{
	register c;

	while( whit2space( c = llgetc() ) )
		; /* just for howard */
	llungetc(c);
	getword(s);
	if( llgetc()!='>' ) yyerror(!FATAL|SKIP, "missing '>' in type ref");
}

deftok(sp, typ, asc, pred, tokno)
struct sym *sp;
int typ, asc, pred, tokno;
{
	if( yydebug )
		fprintf(listout, "deftok(%s) = type %d, ass %d, pred %d, # %d\n",
		sp->s_name, typ, asc, pred, tokno);
	if( typ!=UNKNOWN ) sp->s_type = typ;
	if( asc!=UNKNOWN ) sp->s_ass = asc;
	if( pred!=UNKNOWN ) sp->s_prc = pred;
	if( tokno!=UNKNOWN ) sp->s_val = tokno;
	defsym(sp->s_name, TTERM);
}

wrtdefs()
{
	register i;
	register struct sym *sp;

	for(i=0; i<nterm; i++)  {
		sp = trmptr[i];
		if( alpha(sp->s_name[0]) && strcmp(sp->s_name, "error") )
			fprintf(fhdr, "#define %s %d\n", sp->s_name,
			    sp->s_val);
	}
	if (ntype==0)
		fprintf(fhdr, "typedef	int	YYSTYPE;\n");
	fprintf(fhdr, "#ifdef YYTNAMES\n");
	fprintf(fhdr, "extern struct yytname\n{\n");
	fprintf(fhdr, "\tchar\t*tn_name;\n\tint\ttn_val;\n} yytnames[];\n");
	fprintf(fhdr, "#endif\n");
	fprintf(fhdr, "extern	YYSTYPE	yylval;\n");
	fclose(fhdr);
}

wrthdr()
{
	fprintf(tabout, "\n#include \"%s\"\n", ytabh);
	fprintf(tabout, "#define YYCLEARIN yychar = -1000\n");
	fprintf(tabout, "#define YYERROK yyerrflag = 0\n");
	fprintf(tabout, "extern int yychar;\n");
	fprintf(tabout, "extern short yyerrflag;\n");
	fprintf(tabout,"#ifndef YYMAXDEPTH\n#define YYMAXDEPTH 150\n");
	fprintf(tabout,"#endif\n");
	fprintf(tabout, "YYSTYPE yyval, yylval;\n");
}

wrtnames()
{
	register i;
	register char *sp;

	fprintf(tabout, "#ifdef YYTNAMES\n");
	fprintf(tabout, "struct yytname yytnames[%d] =\n{\n", nterm+1);
	for(i=0; i<nterm; i++) {
		fprintf(tabout, "\t\"");
		sp = trmptr[i]->s_name;
		while( *sp ) {
			if( *sp=='\\' || *sp=='"' )
				putc('\\', tabout);
			putc(*sp, tabout);
			sp++;
		}
		fprintf(tabout, "\", %d, \n", trmptr[i]->s_val);
	}
	fprintf(tabout, "\tNULL\n} ;\n");
	fprintf(tabout, "#endif\n");
}

defnttyp(sp, typ)
struct sym *sp;
{
	sp->s_type = typ;
}

cpyact(nel, ntp)
struct sym *ntp;
{
	register c, istyp, n;
	int accolade, sign, c1;
	char s[SYMSIZE];
	struct sym *sp;

	accolade = 1;
	fprintf(actout, "\ncase %d: {\n", nprod);
	linepos(actout);

	do {
		c = llgetc();
		switch( c ) {
		case EOF:
			yyerror(FATAL, "end of file in action");
			break;

		case '{':
			accolade++;
			break;

		case '}':
			accolade--;
			break;

		case '/':
			if( (c1 = llgetc()) == '*' ) {
				fprintf(actout, "/*");
				copycomment("*/", actout, 0);
				fprintf(actout, "*/");
				continue;
			}
			llungetc(c1);
			break;

		case '"':
			putc('"', actout);
			copycomment("\"", actout, 1);
			putc('"', actout);
			continue;

		case '\'':
			putc('\'', actout);
			copycomment("'", actout, 1);
			putc('\'', actout);
			continue;

		case '$':
			if( istyp = (c = llgetc())=='<' )
				gettype(s);
			else
				llungetc(c);
			switch( c = llgetc() ) {

			case '$':
				if( !istyp && (istyp = ntp->s_type!=UINT) )
					strcpy(s, typeptr[ntp->s_type]->s_name);
				fprintf(actout, "yyval");
				if( istyp ) 
					fprintf(actout,".%s",s);
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
				if( sign = c=='-' )
					c = llgetc();
				n = readnum(c);
				n += -2*sign*n; /* jazzy linear eq. sign switch */
				if( n>nel )  {
					yyerror(!FATAL, "illegal $%d construct", n);
					break;
				}
#ifndef IAPX86	/* 'too many stores' workaround */
				if( n>0 && !istyp &&
				    (sp = elook(PROD_RIGHT (nitprod) [n-1]))-> 
				    s_type!=UINT ) {
#else
				sp = elook(PROD_RIGHT (nitprod) [n-1]);
				if (n > 0 && !istyp && sp->s_type != UINT) {
#endif
					sp = typeptr[sp->s_type];
					istyp++;
					strcpy(s, sp->s_name);
				}
				fprintf(actout,"yypvt[%d]", n-nel);
				if( istyp )
					fprintf(actout,".%s",s);
				break;
			default:
				yyerror(!FATAL|SKIP, "illegal construct $%c", c);
				break;
			}
			continue;
		}
		putc(c, actout);
	} while( accolade );
	fprintf(actout, "break;\n");
}

backup(t)
{
	last.lex = t;
	last.val = yylval;
}

linepos(f)
FILE *f;
{
	fprintf(f, "\n#line %d \"%s\"\n", yyline, gramy);
}

copycomment(s, f,  flag)
register char *s;
FILE *f;
{
	register c, c1;

	do {
		c = llgetc();
		while( c=='\\' && flag ) {
			if( f!=NULL )
				fprintf(f, "\\%c", llgetc());
			c = llgetc();
		}
		if( c==EOF )
			yyerror(FATAL, "end of file god knows where");
		if (c=='\n' && flag)
			yyerror(FATAL, "newline in string");
		if(c!=s[0]){
			if(f!=NULL)
				putc(c, f);
		}
		else if( s[1]!='\0' && (c1 = llgetc()) != s[1] ){
			if(f!=NULL)
				putc(c, f);
			llungetc(c1);
		}
	} while( c!=s[0] || (s[1]!='\0' && (c1!=s[1])) );
}

getword(s)
char *s;
{
	register char *sp;
	register c;

	sp = s;
	c = llgetc();
	do {
		if( sp>=&s[SYMSIZE-1] )
			yyerror(FATAL, "symbol too long");
		*sp++ = c;
	} while( alphanum(c = llgetc()) );
	*sp++ = '\0';
	llungetc(c);
}

llgetc()
{
	register c;

	if( (c = getc(defin)) == '\n' )
		yyline++;
	return(c);
}

llungetc(c)
register c;
{
	if( c=='\n' )
		yyline--;
	ungetc(c, defin);
}

readchar()
{
	register c;

	if( (c = llgetc()) == '\\' ) {
		switch( c = llgetc() ) {
		case 'n':
			return( '\n' );

		case 'r':
			return( '\r' );

		case 't':
			return( '\t' );

		case '\'':
			return( '\'' );

		case 'b':
			return( '\b' );

		case 'f':
			return( '\f' );

		default:
			if( c>='0' && c<='7' )
				return( readnum(c) );
			else
				return(c);
		}
	}
	return(c);
}

copyunion()
{
	register c, accolade;

	while( whitespace( c = llgetc() ) );
	if( c!='{' )
		yyerror(!FATAL,"Bad %union syntax");
	linepos(fhdr);
	fprintf(fhdr, "typedef union {");
	accolade = 1;

	do {
		if( (c = llgetc()) == EOF )
			yyerror(FATAL, "eof in union declaration");
		putc(c, fhdr);
		if( c=='{' ) accolade++;
		else if( c=='}' ) accolade--;
		else if( c=='/' ) {
			if( (c = llgetc()) == '*' ) {
				putc('*', fhdr);
				copycomment("*/", fhdr, 0);
				fprintf(fhdr, "*/");
			} else
				llungetc(c);
		}
	} while( accolade );
	fprintf(fhdr, " YYSTYPE;\n");
}

readnum(c)
register c;
{
	register n;

	n = c - '0';
	while( digit( c=llgetc() ) )
		n = n*10 + c - '0';
	llungetc(c);
	return(n);
}
alpha(c)
{
	return( (c>='A' && c<='Z') || (c>='a' && c<='z') || c=='_' );
}

digit(c)
{
	return( c>='0' && c<='9' );
}

alphanum(c)
{
	return( (c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0' && c<='9')
	    || c=='_' );
}

whitespace(c)
{
	return( c==' ' || c=='\t' || c=='\f' || c=='\n' );
}

whit2space(c)
{
	return( c==' ' || c=='\t' || c=='\f' );
}

struct sym *
elook(n)
register n;
{
	if( n>=NTBASE )
		return( ntrmptr[n-NTBASE] );
	else
		return( trmptr[n] );
}

min(a,b)
{
	return( a<b ? a : b);
}
