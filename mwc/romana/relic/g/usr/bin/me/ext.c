/*
 * This program builds the emacs' comtab.c and bind.doc
 */
#include <stdio.h>
#include <ed.h>
/*
 * Table of function names and comments.
 */
static char *nameCom[] = {
#if	EXKEYS
"ignore",		" Ignore this function		",
#endif

"ctrlg",		" Abort out of things		",
"quit",			" Quit				",
"ctlxlp",		" Begin macro			",
"ctlxrp",		" End macro			",
"ctlxe",		" Execute macro		",
"fileread",		" Get a file, read only	",
"filevisit",		" Get a file, read write	",
"*",			" Also F2 ",
"filewrite",		" Write a file			",
"*",			" Also F2 ",
"filesave",		" Save current file		",
"filename",		" Adjust file name		",
"gotobol",		" Move to start of line	",
"*",			" Also HOME KEY ",
"forwchar",		" Move forward by characters	",
"*",			" Also RIGHT ARROW ",
"gotoeol",		" Move to end of line		",
"*",			" Also END KEY ",
"backchar",		" Move backward by characters	",
"*",			" Also LEFT ARROW ",
"forwline",		" Move forward by lines	",
"*",			" Also DOWN ARROW ",
"backline",		" Move backward by lines	",
"*",			" Also UP ARROW ",
"forwpage",		" Move forward by pages	",
"*",			" Also PAGE DOWN ",
"backpage",		" Move backward by pages	",
"*",			" Also PAGE UP ",
"gotobob",		" Move to start of buffer	",
"gotoeob",		" Move to end of buffer	",
"setfillcol",		" Set fill column to current col ",
"*",			" ctl-u n set fill col to n	",
"*",			" ctl-u 0 turn off word wrap	",
"fillregion",		" apply fillcol to a region",
"setmark",		" Set mark			",
"swapmark",		" Swap '.' and mark		",
"forwsearch",		" Search forward		",
"*",			" Also F3 ",
"*",			" ALT F3 is continue search ",
"forwisearch",		" forward incremental search	",
"backisearch",		" backward incremental search	",
"queryrepl",		" query/replace		",
"*",			" Also F5 ",
"searchagain",		" find next occurrance		",
"backsearch",		" Search backwards		",
"*",			" Also F4 ",
"showcpos",		" Show the cursor position	",
"nextwind",		" Move to the next window	",
"*",			" Also F6 ",
"prevwind",		" Move to the previous window	",
"onlywind",		" Make current window only one	",
"*",			" Also F10 ",
"splitwind",		" Split current window		",
"*",			" Also ALT F1 ",
"mvdnwind",		" Move window down		",
"mvupwind",		" Move window up		",
"enlargewind",		" Enlarge display window.	",
"shrinkwind",		" Shrink window.		",
"listbuffers",		" Display list of buffers	",
"usebuffer",		" Switch a window to a buffer	",
"killbuffer",		" Make a buffer go away.	",
"reposition",		" Reposition dot to line ctl-u	",
"*",			" if ctl-u 0 center		",
"*",			" if ctl-u < 0 repos from bottom ",
"refresh",		" Refresh the screen		",
"*",			" with ctl-u recenter		",
"twiddle",		" Twiddle characters		",
"tab",			" Insert tab			",
"*",			" ctl-u n  use n spaces per tab	",
"*",			" ctl-u -n tab is n spaces	",
"newline",		" Insert CR-LF			",
"*",			" Same as ctl-j in autoindent mode ",
"indent",		" Insert CR-LF, then indent	",
"openline",		" Open up a blank line		",
"deblank",		" Delete blank lines		",
"quote",		" Insert literal		",
"quoteval",		" Insert decimal literal        ",
"backword",		" Backup by words		",
"forwword",		" Advance by words		",
"forwdel",		" Forward delete		",
"*",			" Also DELETE KEY ",
"backdel",		" Backward delete		",
"kill",			" Kill forward from dot		",
"*",			" if ctl-u 0 or less backward	",
"yank",			" Yank back from killbuffer.	",
"loadBinds",		" Load flexable bindings from file. ",
"nameMac",		" Bind current macro to binding. ",
"initMac",		" Bind current macro to initial macro ",
"setPrefix",		" Set prefix codes		",
"bindFun",		" Bind function to old function. ",
"storBinds",		" Store flexable bindings on a file. ",
"upperword",		" Upper case word.		",
"lowerword",		" Lower case word.		",
"upperregion",		" Upper case region.		",
"lowerregion",		" Lower case region.		",
"capword",		" Initial capitalize word.	",
"delfword",		" Delete forward word.		",
"delbword",		" Delete backward word.	",
"killregion",		" Kill region.			",
"copyregion",		" Copy region to kill buffer.	",
"spawncli",		" Run CLI in a subjob.		",
"spawn",		" Run a command in a subjob.	",
"quickexit",		" low keystroke style exit.	",
"gotoline",		" goto line given as arg	",
"gotofline",		" goto line in file.		",
"setfold",		" set case-fold on search flag	",
"displaymod",		" toggle display mode switch   ",
"autoind",		" toggle autoindent flag       ",
"*",			" Also ALT F10 ",
"nexterr",		" Seek the next error		",
"preverr",		" Seek the previous error	",
"bracketsearch",	" seek matching bracket char	",
"bracketsw",		" toggle bracket mode		",
#if	LIBHELP
"lookupword",		" Lookup a word...		",
"*",			" Also F1 ",
"promptlook",		" Prompt for a word to look up	",
"zaphelp",		" Zap help window		",
"hlpindex",		" Help index generator		",
#endif
"zapwind",		" Zap current window		",
#if	GEM
"grabmem",		" Grab memory			",
#endif
NULL, NULL
};

/*
 * Table of control characters and function names.
 */
typedef struct control control;
struct control {
	int  key;
	char *keyn;
	char *fun;
};
control ctlName[] = {
CTRL|'@',	"CTRL|'@'",		"setmark",
CTRL|'A',	"CTRL|'A'",		"gotobol",
CTRL|'B',	"CTRL|'B'",		"backchar",
CTRL|'C',	"CTRL|'C'",		"spawncli",
CTRL|'D',	"CTRL|'D'",		"forwdel",
CTRL|'E',	"CTRL|'E'",		"gotoeol",
CTRL|'F',	"CTRL|'F'",		"forwchar",
CTRL|'G',	"CTRL|'G'",		"ctrlg",
CTRL|'H',	"CTRL|'H'",		"backdel",
CTRL|'I',	"CTRL|'I'",		"tab",
CTRL|'J',	"CTRL|'J'",		"indent",
CTRL|'K',	"CTRL|'K'",		"kill",
CTRL|'L',	"CTRL|'L'",		"refresh",
CTRL|'M',	"CTRL|'M'",		"newline",
CTRL|'N',	"CTRL|'N'",		"forwline",
CTRL|'O',	"CTRL|'O'",		"openline",
CTRL|'P',	"CTRL|'P'",		"backline",
CTRL|'Q',	"CTRL|'Q'",		"quote",
CTRL|'R',	"CTRL|'R'",		"backisearch",
CTRL|'S',	"CTRL|'S'",		"forwisearch",
CTRL|'T',	"CTRL|'T'",		"twiddle",
CTRL|'V',	"CTRL|'V'",		"forwpage",
CTRL|'W',	"CTRL|'W'",		"killregion",
CTRL|'Y',	"CTRL|'Y'",		"yank",
CTRL|'Z',	"CTRL|'Z'",		"quickexit",
PFX1|CTRL|'B',	"PFX1|CTRL|'B'",		"listbuffers",
PFX1|CTRL|'C',	"PFX1|CTRL|'C'",		"quit",
PFX1|CTRL|'F',	"PFX1|CTRL|'F'",		"filename",
PFX1|CTRL|'L',	"PFX1|CTRL|'L'",		"lowerregion",
PFX1|CTRL|'O',	"PFX1|CTRL|'O'",		"deblank",
PFX1|CTRL|'N',	"PFX1|CTRL|'N'",		"mvdnwind",
PFX1|CTRL|'P',	"PFX1|CTRL|'P'",		"mvupwind",
PFX1|CTRL|'R',	"PFX1|CTRL|'R'",		"fileread",
PFX1|CTRL|'S',	"PFX1|CTRL|'S'",		"filesave",
PFX1|CTRL|'U',	"PFX1|CTRL|'U'",		"upperregion",
PFX1|CTRL|'V',	"PFX1|CTRL|'V'",		"filevisit",
PFX1|CTRL|'W',	"PFX1|CTRL|'W'",		"filewrite",
PFX1|CTRL|'X',	"PFX1|CTRL|'X'",		"swapmark",
PFX1|CTRL|'Z',	"PFX1|CTRL|'Z'",		"shrinkwind",
PFX1|'!',	"PFX1|'!'",			"spawn",
PFX1|'<',	"PFX1|'<'",			"preverr",
PFX1|'=',	"PFX1|'='",			"showcpos",
PFX1|'>',	"PFX1|'>'",			"nexterr",
PFX1|'(',	"PFX1|'('",			"ctlxlp",
PFX1|')',	"PFX1|')'",			"ctlxrp",
PFX1|'1',	"PFX1|'1'",			"onlywind",
PFX1|'2',	"PFX1|'2'",			"splitwind",
#if	LIBHELP
PFX1|'?',	"PFX1|'?'",			"promptlook",
#endif
PFX1|'B',	"PFX1|'B'",			"usebuffer",
PFX1|'E',	"PFX1|'E'",			"ctlxe",
PFX1|'F',	"PFX1|'F'",			"setfillcol",
PFX1|'G',	"PFX1|'G'",			"gotoline",
PFX1|'H',	"PFX1|'H'",			"fillregion",
PFX1|'K',	"PFX1|'K'",			"killbuffer",
PFX1|'I',	"PFX1|'I'",			"initMac",
PFX1|'L',	"PFX1|'L'",			"loadBinds",
PFX1|'M',	"PFX1|'M'",			"nameMac",
PFX1|'N',	"PFX1|'N'",			"nextwind",
PFX1|'P',	"PFX1|'P'",			"prevwind",
PFX1|'R',	"PFX1|'R'",			"bindFun",
PFX1|'S',	"PFX1|'S'",			"storBinds",
PFX1|'X',	"PFX1|'X'",			"setPrefix",
PFX1|'Z',	"PFX1|'Z'",			"enlargewind",
PFX1|'[',	"PFX1|'['",			"bracketsearch",
PFX1|']',	"PFX1|']'",			"bracketsw",
#ifdef	VT52KEYS
META|CTRL|'B',	"META|CTRL|'B'",		"backword",
META|CTRL|'C',	"META|CTRL|'C'",		"capword",
META|CTRL|'D',	"META|CTRL|'D'",		"delfword",
#endif
META|CTRL|'H',	"META|CTRL|'H'",		"delbword",
META|CTRL|'R',	"META|CTRL|'R'",		"queryrepl",
#if	LIBHELP
META|CTRL|']',	"META|CTRL|']'",		"hlpindex",
#endif
META|'!',	"META|'!'",		"reposition",
META|'.',	"META|'.'",		"setmark",
META|'1',	"META|'1'",		"zapwind",
META|'2',	"META|'2'",		"zaphelp",
#if	GEM
META|'+',	"META|'+'",		"grabmem",
#endif
META|'>',	"META|'>'",		"gotoeob",
META|'<',	"META|'<'",		"gotobob",
META|'%',	"META|'%'",		"queryrepl",
META|'/',	"META|'/'",		"searchagain",
#if	LIBHELP
META|'?',	"META|'?'",		"lookupword",
#endif
META|'@',	"META|'@'",		"setfold",
#ifdef	VT52KEYS
META|'A',	"META|'A'",		"backline",
META|'B',	"META|'B'",		"forwline",
META|'C',	"META|'C'",		"backchar",
META|'D',	"META|'D'",		"forwchar",
#else
META|'B',	"META|'B'",		"backword",
META|'C',	"META|'C'",		"capword",
META|'D',	"META|'D'",		"delfword",
#endif
META|'F',	"META|'F'",		"forwword",
META|'G',	"META|'G'",		"gotofline",
META|'J',	"META|'J'",		"autoind",
META|'L',	"META|'L'",		"lowerword",
META|'Q',	"META|'Q'",		"quoteval",
META|'R',	"META|'R'",		"backsearch",
META|'S',	"META|'S'",		"forwsearch",
META|'T',	"META|'T'",		"displaymod",
META|'U',	"META|'U'",		"upperword",
META|'V',	"META|'V'",		"backpage",
META|'W',	"META|'W'",		"copyregion",
META|0x7F,	"META|0x7F",		"delbword",
0x7F,	"0x7F",			"backdel"
#if	EXKEYS
,FN0,	"FN0",			"ignore",
FN1,	"FN1",			"ignore",
FN2,	"FN2",			"ignore",
FN3,	"FN3",			"ignore",
FN4,	"FN4",			"ignore",
FN5,	"FN5",			"ignore",
FN6,	"FN6",			"ignore",
FN7,	"FN7",			"ignore",
FN8,	"FN8",			"ignore",
FN9,	"FN9",			"ignore",
FNA,	"FNA",			"ignore",
FNB,	"FNB",			"ignore",
FNC,	"FNC",			"ignore",
FND,	"FND",			"ignore",
FNE,	"FNE",			"ignore",
FNF,	"FNF",			"ignore",
FN10,	"FN10",			"ignore",
FN11,	"FN11",			"ignore",
FN12,	"FN12",			"ignore",
FN13,	"FN13",			"ignore",
FN14,	"FN14",			"ignore",
FN15,	"FN15",			"ignore",
FN16,	"FN16",			"ignore",
FN17,	"FN17",			"ignore",
FN18,	"FN18",			"ignore",
FN19,	"FN19",			"ignore",
FN10,	"FN10",			"ignore",
FN11,	"FN11",			"ignore",
FN12,	"FN12",			"ignore"
#endif
};

#define CTSIZE (sizeof(ctlName) / sizeof(control))

static FILE *doc;
KEYTAB keytab[CTSIZE + HASHP];
int error = 0;

/*
 * put data to stdout.
 */
pf(s)
char *s;
{
	printf("%r\n", &s);
}

/*
 * Lookup function name and return number.
 */
funNo(name)
register char *name;
{
	register char *p;
	register int no;
	
	for (no = 0; NULL != (p = nameCom[no]); no += 2) {
		if (!strcmp(p, name)) {
			*(nameCom[no + 1]) = '*';
			return (no / 2);
		}
	}
	fprintf(stderr, "Cannot find %s\n", name);
	error = 1;
}

/*
 * Adjust number to account for commentary lines.
 */
trueNo(n)
{
	register int i, j;

	n *= 2;
	for (i = j = 0; i < n; i += 2)
		if ('*' == *nameCom[i])
			j++;
	return ((i / 2) - j);
}

/*
 * Build hashed key table.
 */
buildTab()
{
	register KEYTAB *k;
	register control *p;
	int n, i, f, l;

	/* init hash table */
	for (k = keytab; k < (keytab + CTSIZE + HASHP); k++)
		k->k_code = k->k_synonym = -1;

	fprintf(doc, "PFX1 is initially bound to ctl-x\n");
	fprintf(doc, "PFX2 and PFX3 are initially unbound\n\n\n");
	fprintf(doc, "%-16s%-16s%s\n", "key binding", "function", "command");
	/* load hash table */
	l = HASHP;
	for (i = 0; i < CTSIZE; i++) {
		p = ctlName + i;
		k = keytab + (p->key % HASHP);
		n = (f = funNo(p->fun)) * 2;
		fprintf(doc, "%-16s%-16s%s\n", p->keyn, nameCom[n],
			nameCom[1 + n] + 1);
		while ((NULL != nameCom[n += 2]) && ('*' == *nameCom[n]))
			fprintf(doc, "\t\t\t\t%-16s\n", nameCom[n + 1] + 1);
		if (-1 != k->k_code) {
			 /* to end of chain */
			for (;; k = keytab + k->k_synonym) {
				if (i == k->k_code) {
					fprintf(stderr, "Duplucate code for %s\n", p->keyn);
					error = 1;
				}
				if (-1 == k->k_synonym)
					break;
			}
			k = keytab + (k->k_synonym = l++);
		}
		k->k_code = i;
		k->k_fun  = f;
	}

	/* output hash table */
	pf("KEYTAB keytab[] = {");
	for (i = 0; i < l; i++) {
		if (-1 == (k = keytab + i)->k_code)
			pf("{\t-1, 0, -1},\t/* hole in table */");
		else {
			p = ctlName + k->k_code;
			pf("{%16s, %3d, %3d },\t/* %-16s %d */",
				p->keyn, trueNo(k->k_fun),
				k->k_synonym, p->fun, i);
		}
	}
	pf("};");
}

main()
{
	register char **p;
	int i = 0;

	if (NULL == (doc = fopen("bind.doc", "w")))
		exit(1);

	pf("/*");
	pf(" * Emacs command tables.");
	pf(" * Generated by ext.c");
	pf(" * Make any changes by changing ext.c");
	pf(" */\n");
	pf("#include \"stdio.h\"");
	pf("#include \"ed.h\"\n");
	for (p = nameCom; NULL != *p; p += 2)
		if (**p != '*')
			pf("extern %16s();\t/*%s*/", p[0], p[1]);
		else
			pf("\t\t\t\t/*%s*/", p[1]);

	pf("\nFUNTAB funtab[] = {");
	for (p = nameCom; NULL != *p; p += 2)
		if (**p != '*')
			pf("%16s,\t/* %d */", p[0], i++);
	pf("};");

	buildTab();
	for (p = nameCom; NULL != p[0]; p += 2)
		if (('*' != *p[0]) && ('*' != *p[1])) {
			fprintf(stderr, "unused name %s\n", p[0]);
			error = 1;
		}
	exit(error);	/* zero if no errors else 1 */
}
