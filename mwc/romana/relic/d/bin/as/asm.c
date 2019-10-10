/*
 * Segmented assembler.
 * Segmented assemblers are much
 * more of a pain then non segmented
 * ones. Here we try to make them
 * easy to use, at the expense of
 * driving the implementor absolutely
 * crazy.
 */
#include "asm.h"

main(argc, argv)
char *argv[];
{
	register char *p;
	register c, i;
	char *sfn;
	int nfile, sfx;
	static char copen[] = "%s: cannot open.\n";

	nfile = argc-1;
	efp = stdout;
	for (i=1; i<argc; ++i) {
		p = argv[i];
		if (*p == '-') {
			argv[i] = NULL;
			--nfile;
			++p;
			while (c = *p++) {
				switch(c) {

				case 'g':
					++gflag;
					break;

				case 'l':
					++lflag;
					efp = stderr;
					break;

				case 'o':
					if (++i >= argc)
						usage();
					ofn = argv[i];
					argv[i] = NULL;
					--nfile;
					break;
				case 'x':
					++xflag;
					break;

				default:
					usage();
				}
			}
		}
	}
	if (nfile == 0)
		usage();
#if CPM
	ofp = fopen(ofn, "wb");
#else
	ofp = fopen(ofn, "w");
#if !INTEL
	if (ofp != NULL)
		ofp = freopen(ofn, "r+w", ofp);
#endif
#endif
	if (ofp == NULL) {
		fprintf(stderr, "%s: cannot create.\n", ofn);
		exit(1);
	}
#if !RSX && !CPM
	setbuf(ofp, NULL);
#endif
	syminit();
	locinit();
	for (pass = 0; pass < 3; ++pass) {
		if (pass==1 && gflag)
			symglob();
		if (pass == 2) {
			locadjust();
			outinit();
		}
		sfx  = 1;
		page = 0;
		lop  = NLPP;
		if (sfp != NULL) {
			fseek(sfp, (long)0, 0);
			line = 0;
		}
		loczero();
		fuzz = 0;
#if MINIT
		minit();
#endif
		dot->s_kind = S_USER;
		dot->s_type = E_DIR;
		dot->s_addr = defloc->l_offset;
		dot->s_base.s_lp = defloc;
		setinbss();
		if (pass != 0) {
			for (i=0; i<10; ++i) {
				tsymp[i].tp_bp = NULL;
				tsymp[i].tp_fp = tsymp[i].tp_lfp;
			}
		}
		for (;;) {
			if (sfp == NULL) {
				sfn = NULL;
				while (sfx<argc && (sfn=argv[sfx++])==NULL)
					;
				if (sfn == NULL)
					break;
				if ((sfp = fopen(sfn, "r")) == NULL) {
					fprintf(stderr, copen, sfn);
					exit(1);
				}
				if (nfile != 1)
					ifn = sfn;
				line = 0;
				lop  = NLPP;
			}
			if (getline() == 0) {
				if (nfile == 1)
					break;
				fclose(sfp);
				sfp = NULL;
				continue;
			}
			++line;
			cp = cb;
			ep = eb;
			ip = ib;
			if (setjmp(env) == 0)
				asm();
			if (pass == 2)
				list();
		}
		newloc(NULL);
	}
	outfinish();
	if (nerr != 0) {
		if (lflag)
			fprintf(efp, "%d error%s detected\n",
				nerr, ((nerr != 1) ? "s" : ""));
		unlink(ofn);
		exit(1);
	}
#if	0
#if !RSX && !CPM
	chmod(ofn, 0755);
#endif
#endif
	exit(0);
}

asm()
{
	register struct sym *sp;
	register struct tsym *tp;
	register c;
	address cs;
	struct tsymp *tsp;
	struct expr e1;
	char id[NCPLN];
	char *p;
	int d;

	laddr = dot->s_addr;
	lmode = SLIST;
loop:
	while ((c=getnb()) == ';')
		;
	if (c==0 || c=='/')
		return;
	if (ctype[c] == DIGIT) {
		if (get() != ':')
			qerr("invalid local symbol");
		tsp = &tsymp[c-'0'];
		if (pass == 0) {
			tp = (struct tsym *) new(sizeof(struct tsym));
			tp->t_fp = NULL;
			tp->t_lp = dot->s_base.s_lp;
			tp->t_addr = dot->s_addr;
			if (tsp->tp_lfp == NULL)
				tsp->tp_lfp = tp; else
				tsp->tp_llp->t_fp = tp;
			tsp->tp_llp = tp;
		} else {
			tp = tsp->tp_fp;
			if (pass == 1) {
				fuzz = tp->t_addr - dot->s_addr;
				tp->t_lp = dot->s_base.s_lp;
				tp->t_addr = dot->s_addr;
			} else
				phase(tp->t_lp, tp->t_addr);
		}
		tsp->tp_bp = tp;
		tsp->tp_fp = tp->t_fp;
		goto loop;
	}
	if (ctype[c] != LETTER) 
		qerr("invalid identifier");
	getid(id, c);
	if ((c=getnb()) == ':') {
		sp = lookup(id, 1);
		if (sp == dot)
			err('.', "'.' declared as label");
		if (pass == 0)
			if (sp->s_type!=S_NEW && (sp->s_flag&S_ASG)==0)
				sp->s_flag |= S_MDF;
		if (pass != 2) {
			fuzz = sp->s_addr - dot->s_addr;
			sp->s_kind = S_USER;
			sp->s_type = E_DIR;
			sp->s_base.s_lp = dot->s_base.s_lp;
			sp->s_addr = dot->s_addr;
		} else {
			if ((sp->s_flag&S_MDF) != 0)
				err('m', "multiply defined symbol");
			phase(sp->s_base.s_lp, sp->s_addr);
		}
		lmode = ALIST;
		goto loop;
	}
	if (c == '=') {
		expr(&e1, 0);
		if ((sp=lookup(id, 1)) == dot) {
#if INTEL
			dot->s_addr = e1.e_addr;
#else
			if (okdot(&e1)) {
				if (inbss == 0)
					zblock(e1.e_addr-dot->s_addr);
				else
					dot->s_addr = e1.e_addr;
			}
#endif
		} else {
			if (sp->s_type!=S_NEW && (sp->s_flag&S_ASG)==0)
				err('m', "multiply defined symbol");
			sp->s_kind = S_USER;
			sp->s_type = e1.e_type;
			if (e1.e_type == E_ASEG)
				sp->s_base.s_segn = e1.e_base.e_segn;
			else if (e1.e_type == E_DIR)
				sp->s_base.s_lp = e1.e_base.e_lp;
			else if (e1.e_type==E_SYM || e1.e_type==E_SEG)
				sp->s_base.s_sp = e1.e_base.e_sp;
			sp->s_addr = e1.e_addr;
			sp->s_flag |= S_ASG;
		}
		laddr = e1.e_addr;
		lmode = ALIST;
		goto loop;
	}
	unget(c);
	lmode = CLIST;
	if ((sp=lookup(id, 0)) == NULL) {
		uerr(id);
		return;
	}
	switch (sp->s_kind) {

	case S_BYTE:
	case S_WORD:
		do {
			expr(&e1, 0);
			if (sp->s_kind == S_BYTE)
				outrb(&e1, 0); else
				outrw(&e1, 0);
		} while ((c = getnb()) == ',');
		unget(c);
		if (sp->s_kind == S_BYTE)
			lmode = BLIST; else
			lmode = WLIST;
		break;

	case S_ASCII:
		if ((d = getnb()) == '\0')
			qerr("end of line in string");
		while ((c = getmap(d)) >= 0)
			outab(c);
		lmode = BLIST;
		break;

	case S_BLK:
		cs = absexpr()*sp->s_addr;
		if (inbss == 0)
			zblock(cs);  else
			dot->s_addr += cs;
		lmode = ALIST;
		break;

	case S_TITLE:
		p = tb;
		if (c = getnb()) {
			do {
				if (p < &tb[NTIT-1])
					*p++ = c;
			} while (c = get());
		}
		*p = 0;
		unget(c);

	case S_PAGE:
		lop = NLPP;
		lmode = NLIST;
		break;

	case S_GLOBL:
		do {
			getid(id, -1);
			lookup(id, 1)->s_flag |= S_GBL;
		} while ((c = getnb()) == ',');
		unget(c);
		lmode = SLIST;
		break;

	case S_COMM:
		getid(id, -1);
		if (getnb() != ',')
			qerr("expected comma");
		cs = locrup(absexpr());
		sp = lookup(id, 1);
		if (sp->s_kind == S_NEW) {
			sp->s_flag |= S_GBL;
			if (cs > sp->s_addr)
				sp->s_addr = cs;
		}
		lmode = SLIST;
		break;

	case S_LOC:
		newloc((struct loc *) sp->s_addr);
		lmode = SLIST;
		break;

	default:
		machine(sp);
	}
	goto loop;
}

okdot(esp)
register struct expr *esp;
{
	if (esp->e_type!=E_DIR || esp->e_base.e_lp!=dot->s_base.s_lp) {
		rerr("not a direct address in this segment");
		return (0);
	}
	if (esp->e_addr < dot->s_addr) {
		aerr("cannot move . back");
		return (0);
	}
	return (1);
}

zblock(n)
address n;
{
	while (n--)
		outab(0);
}

usage()
{
	fprintf(stderr, "Usage: as [-glx] [-o file] file [file ...]\n");
	exit(1);
}

phase(lp, a)
struct loc *lp;
address a;
{
	if (dot->s_base.s_lp!=lp || dot->s_addr!=a)
		err('p', "phase error");
}

newloc(nlp)
register struct loc *nlp;
{
	register struct loc *olp;

	if ((olp = dot->s_base.s_lp) != NULL) {
		olp->l_fuzz = fuzz;
		olp->l_break = dot->s_addr - olp->l_offset;
	}
	if (nlp != NULL) {
		dot->s_base.s_lp = nlp;
		dot->s_addr = nlp->l_break + nlp->l_offset;
		fuzz = nlp->l_fuzz;
	}
	setinbss();
#if !INTEL
	if (pass == 2)
		outchk(HUGE, HUGE);
#endif
}

loczero()
{
	register struct loc *lp;
	register i;

	for (i=0; i<nloc; ++i) {
		lp = loc[i];
		while (lp != NULL) {
			lp->l_fuzz  = 0;
			lp->l_break = 0;
			lp = lp->l_lp;
		}
	}
}

locadjust()
{
	register struct sym  *sp;
	register struct loc  *lp;
	register struct tsym *tp;
	address offset;
	int i;

#if !SEG
	offset = 0;
#endif
	for (i=0; i<nloc; ++i) {
#if  SEG
		offset = 0;
#endif
		lp = loc[i];
		while (lp != NULL) {
			lp->l_offset = offset;
			offset += locrup(lp->l_break);
			lp = lp->l_lp;
		}
	}
	for (i=0; i<10; ++i) {
		tp = tsymp[i].tp_lfp;
		while (tp != NULL) {
			if ((lp = tp->t_lp) != NULL)
				tp->t_addr += lp->l_offset;
			tp = tp->t_fp;
		}
	}
	for (i=0; i<NHASH; ++i) {
		sp = symhash[i];
		while (sp != NULL) {
			if (sp->s_kind == S_USER &&
			    sp->s_type == E_DIR  &&
			   (lp=sp->s_base.s_lp) != NULL)
				sp->s_addr += lp->l_offset;
			sp = sp->s_sp;
		}
	}
}

setinbss()
{
	register struct loc *lp;

	inbss = 0;
	lp = dot->s_base.s_lp;
#if SEG
	if (lp->l_seg == BSS)
#else
	if (lp->l_seg==L_BSSI || lp->l_seg==L_BSSD)
#endif
		++inbss;
}
