/*
 * Lexer and mainline for new screen builder.
 */
#include <scnbld.h>
#include <y.tab.h>
#include <string.h>
#include <ctype.h>

FILE *ifp, *ofp, *ohp;
int line = 1;
int fieldDesig = '!';
static int errors;
static char *name;

#ifdef TRACE
#define RET(val) { printf("%d: trace %d\n", __LINE__, val); return(val); }
#else
#define RET(val) return(val);
#endif
#define POINT printf("%d: trace point\n", __LINE__);

/*
 * Fatal error.
 */
void
fatal(s)
char *s;
{
	printf("%d: %r\n%d error%s detected\n",
		line, &s, (errors + 1), (errors ? "s": ""));
	exit(1);
}

/*
 * Nonfatal error.
 */
yyerror(s)
char *s;
{
	printf("%d: %r\n", line, &s);
	errors++;
}

main(argc, argv)
char *argv[];
{
	char *p, *q, *fileName;
	char file[20];

	if (NULL == (fileName = argv[1]))
		fatal("No work.");

	ifp = xopen(fileName, "r");

	/* Turn file name into .c and .h version in current dir */
	if (NULL == (p = strrchr(fileName, '/')))
		p = fileName;
	else
		p++;

	if (NULL != (q = strrchr(p, '.')))
		*q = '\0';

	sprintf(file, "%s.c", p);
	ofp = xopen(file, "w");

	sprintf(file, "%s.h", p);
	ohp = xopen(file, "w");

	name = newcpy(p);

	if(NULL != q)
		*q = '.';

	fprintf(ofp, "/*\n");
	fprintf(ofp, " * Screen definition file\n");
	fprintf(ofp, " * This file generated from %s\n", fileName);
	fprintf(ofp, " */\n");
	fprintf(ofp, "#include <%s>\n\n", file);

	fprintf(ohp, "/*\n");
	fprintf(ohp, " * Screen definition header\n");
	fprintf(ohp, " * This file generated from %s\n", fileName);
	fprintf(ohp, " */\n");
	fprintf(ohp, "#include <scn.h>\n\n");
	fprintf(ohp, "extern backGrnd %s_data[];\n", name);
	fprintf(ohp, "extern loc %s_locs[];\n", name);

	yyparse();

	finish();
}

typedef enum state state;
enum state { start, inname, innum, instr, bsl, incomm, pct, pctEnd };

struct sym {
	char *name;
	int  ytype;
} table[] = {
	"external",	EXT,
	"readonly",	RONLY,
	"long",		LONG,
	"default",	DEFAULT,
	"skip",		SKIP,
	"group",	GROUP,
	"help",		HELP,
	"verify",	VERIFY,
	"designator",	DESIG,
	"do",		DO,
	"done",		DONE,
	NULL
};

yylex()
{
	register int c;
	register struct sym *tp;
	state st, pst;
	char buf[80], *p;
	
	for (pst = st = start; ; ) {
		if (EOF == (c = fgetc(ifp)))
			fatal("unexpected EOF");

		switch (st) {
		case pctEnd:
			if ('\n' == c) {
				line++;
				RET(EOF)
			}
			continue;

		case pct:
			if ('%' != c) {
				yyerror("Lex error %%%c", c);
				st = start;
				continue;
			}
			st = pctEnd;
			continue;

		case bsl:
			if (start == pst) {
				if ('\n' == c) {
					line++;
					st = start;
					continue;
				}
				yyerror("%c after \\ outside of string");
				st = start;
				continue;
			}
			*p++ = c;
			st = pst;
			continue;

		case instr:
			switch (c) {
			case '\\':
				pst = st;
				st = bsl;
				*p++ = c;
				continue;
			case '\n':
				yyerror("new line in string");
			case '"':
				*p++ = '"';
				*p = 0;
				yylval.string = newcpy(buf);
				RET(STRING)
			default:
				*p++ = c;
				continue;
			}
		case inname:
			if (isalnum(c)) {
				*p++ = c;
				continue;
			}
			st = start;
			ungetc(c, ifp);
			*p = '\0';

			for (tp = table; tp->name != NULL; tp++)
				if (!strcmp(tp->name, buf))
					RET(tp->ytype)

			yylval.string = newcpy(buf);
			RET(NAME)

		case innum:
			if (isdigit(c)) {
				*p++ = c;
				continue;
			}
			st = start;
			ungetc(c, ifp);
			*p = '\0';
			yylval.val = atoi(buf);
			RET(NUMBER)

		case incomm:
			switch (c) {
			case '\\':
				pst = start;
				break;
			case '\n':
				line++;
				st = start;
			}
			continue;
			
		case start:
			switch (c) {
			case '#':
				st = incomm;
				continue;
			case '%':
				st = pct;
				continue;
			case '\n':
				line++;
				RET(NL)
			case '\\':
				pst = st;
				st = bsl;
				continue;
			case '"':
				st = instr;
				p = buf;
				*p++ = c;
				continue;
			default:
				if (isspace(c))
					continue;

				if (isdigit(c)) {
					st = innum;
					p = buf;
					*p++ = c;
					continue;
				}

				if (isalpha(c)) {
					st = inname;
					p = buf;
					*p++ = c;
					continue;
				}
				yyerror("Lexical error %c", c);
			}
		}
	}
}

typedef struct namelist namelist;
struct namelist {
	namelist *next;
	char *name;
};

/*
 * output extern for verify routine.
 */
void
outExtern(n)
char *n;
{
	static namelist *root = NULL;
	register namelist *p;

	for (p = root; p != NULL; p = p->next)
		if (!strcmp(p->name, n))
			return;

	p = alloc(sizeof(*p));
	p->next = root;
	p->name = n;
	root = p;
	fprintf(ohp, "extern int %s();\n", n);
}

/*
 * finish processing the scn tables.
 */
finish()
{
	int c;
	clump *cp;
	loc *lp;
	int times, count, toOut;
	int row, col, started;
	char *p, buf[80];

	fprintf(ofp, "backGrnd %s_data[] = {\n", name);

	for (row = col = started = times = count = 0, lp = locs, cp = clumps;
	     EOF != (c = fgetc(ifp)); ) {
		if (c == fieldDesig) {
			c = ' ';
			if (NULL == lp) {
				yyerror("To few fields defined");
				break;
			}

			if (NULL != cp && (cp->from == lp)  && !times) {
				times = cp->times;
				count = cp->count;
			}
			if (count) {
				if (NULL == lp->these)
					lp->these = alloc(sizeof(pair) * times);
				lp->these[times - 1].row = row;
				lp->these[times - 1].col = col;
			}
			else {
				lp->row = row;
				lp->col = col;
			}
			lp = lp->next;
			if (count && !--count) {	/* last in group */
				if (--times) {		/* go around again */
					count = cp->count;
					lp = cp->from;
				}
				else			/* next group */
					cp = cp->next;
			}
		}
		if (!started) {	/* No chars output for this line yet */
			switch (c) {
			case '\n':
				row++;
				col = 0;
				continue;
			case '\t':
				col |= 7;
			case ' ':
				col++;
				continue;
			}
			started++;
			toOut = col;
			p = buf;
			fputc('"', ofp);
		}
		switch (c) {
		case '\t':
			col |= 7;
			col++;
			*p++ = '\\';
			*p++ = 't';
			continue;
		case '\n':
			fprintf(ofp, "\",\n\t%d, %d,\n", row, toOut);
			row++;
			col = 0;
			started = 0;
			continue;
		case ' ':
			*p++ = c;
			col++;
			continue;
		}
		col++;
		if (buf != p) {	/* we have saved some whitespace */
			*p = 0;
			fprintf(ofp, "%s", buf);
			p = buf;
		}
		switch (c) { /* we are putting stuff into c strings */
		case '\\':
		case '"':
			fputc('\\', ofp);
		}
		fputc(c, ofp);
	}
	fprintf(ofp, "\tNULL\n};\n");
	if (NULL != lp)
		yyerror("To many fields defined");

	/*
	 * Output field descriptors.
	 */
	for (times = count = 0, cp = clumps, lp = locs;
	     lp != NULL;
	     lp = lp->next) {
		if (NULL != cp && (cp->from == lp)) {
			count = cp->count;
			times = cp->times;
		}
		if (count) {
			if(lp->flags & LONGFIELD) {
				if (!(lp->flags & EXTERNAL))
					fprintf(ofp, "char *%s[%d];\n",
						lp->field, times);
				fprintf(ohp, "extern char *%s[%d];\n",
					lp->field, times);
			}
			else {
				if (!(lp->flags & EXTERNAL))
					fprintf(ofp, "char %s[%d][%d];\n",
						lp->field, times, lp->len + 1);
				fprintf(ohp, "extern char %s[%d][%d];\n",
					lp->field, times, lp->len + 1);
			}
			if(!--count)
				cp = cp->next;
		}
		else {
			if(lp->flags & LONGFIELD) {
				if (!(lp->flags & EXTERNAL))
					fprintf(ofp, "char *%s;\n", lp->field);
				fprintf(ohp, "extern char *%s;\n", lp->field);
			}
			else {
				if (!(lp->flags & EXTERNAL))
					fprintf(ofp, "char %s[%d];\n",
						lp->field, lp->len + 1);
				fprintf(ohp, "extern char %s[%d];\n",
					lp->field, lp->len + 1);
			}
		}
		if (NULL == lp->verify)
			lp->verify = "noVerify";
		outExtern(lp->verify);
	}
	fprintf(ofp, "\n");

	/*
	 * Output loc table.
	 */
	fprintf(ofp, "loc %s_locs[] = {\n", name);

	for (times = count = 0, cp = clumps, lp = locs; lp != NULL;) {
		if (NULL != cp && (cp->from == lp)  && !times) {
			times = cp->times;
			count = cp->count;
			toOut = (times * count);
		}

		if (count) {
			toOut--;
			lp->row = lp->these[times - 1].row;
			lp->col = lp->these[times - 1].col;

			if(lp->flags & LONGFIELD)
				sprintf(buf, "%s + %d",
					lp->field, cp->times - times);
			else
				sprintf(buf, "%s[%d]",
					lp->field, cp->times - times);
		}
		else {
			if(lp->flags & LONGFIELD)
				sprintf(buf, "&%s", lp->field);
			else
				sprintf(buf, "%s", lp->field);
		}

		fprintf(ofp, "\t{ %s, %d, %s, %s, %d, %d, %d, %d, %s },\n",
			buf,
			lp->len,
			((NULL == lp->Default) ? buf : lp->Default),
			lp->verify,
			lp->flags & (LONGFIELD|READONLY),
			lp->row,
			lp->col,
			((255 == lp->skipf) ? toOut : lp->skipf),
			((NULL == lp->help) ? "NULL" : lp->help));

		lp = lp->next;
		if (count && !--count) {	/* last in group */
			if (--times) {		/* go around again */
				count = cp->count;
				lp = cp->from;
			}
			else
				cp = cp->next;	/* next group */
		}
	}
	fprintf(ofp, "\tNULL\n};\n");
	if(errors) {
		printf("%d error%s detected\n",
			errors, ((1 == errors) ? "s": ""));
		exit(1);
	}
}
