/*
 * ld/pass1.c
 *
 * One Pass Coff Loader.
 * By Charles Fiterman 3/31/92 for Mark Williams.
 */ 
#include "ld.h"

mod_t *sysHead;
static int  ifd;
static char *fname;

/*
 * Reverse bytes on the 80386 for archive pointers.
 */
#ifdef GEMDOS
#include <osbind.h>
#define flipbytes(x)
#else
flipbytes(c)
register char *c;
{
	register char t;

	t = c[0]; c[0] = c[3]; c[3] = t;
	t = c[1]; c[1] = c[2]; c[2] = t;
}
#endif

/*
 * Symbol name.
 */
char *
symName(sym, work)
register SYMENT *sym;
register char *work;
{
	if (1 == sym->n_zeroes) {	/* pointer to table record */
		sym_t *s;

		s = (sym_t *)sym->n_offset;
		sym = &(s->sym);
	}

	if (!sym->n_zeroes)		/* pointer to long symbol */
		return ((char *)sym->n_offset);

	/*
	 * Name in record may need terminator.
	 * Profiling shows this gets used a lot so do it quick
	 * even at the cost of some readability.
	 */
	((long *)work)[0] = sym->n_zeroes;
	((long *)work)[1] = sym->n_offset;
/*	memcpy(work, sym->n_name, SYMNMLEN); */
	work[SYMNMLEN] = '\0';
	return (work);
}

/*
 * complain about redefined symbol
 */
void
symredef(sp, mp)
sym_t	*sp;
mod_t	*mp;
{
	/*
	 * Simple module.
	 */
	if (mp->mname[0] == '\0')
		spmsg(sp, "redefined in file '%s'", mp->fname);
	/* A symbol is defined in incompatible ways in different files. */

	else
		spmsg(sp, "redefined in file '%s': module '%.*s'",
			mp->fname, DIRSIZ, mp->mname );
	/* A symbol is defined in incompatible ways in different files. */
}

/*
 * Return undefined reference to given symbol if any.
 */
sym_t *
symref(name)
char *name;
{
	char work[SYMNMLEN + 1];
	register sym_t	*sp;
	register SYMENT *sym;

	/* Scan internal symbol table for undefined reference. */
	for (sp=symtable[crc16(name) % NHASH]; sp != NULL; sp=sp->next) {
		sym = &(sp->sym);
		if (undefined(sym) && !strcmp(symName(sym, work), name))
			break;
	}
	return(sp); /* Return reference, or NULL. */
}

/*
 * Read input files.
 */
readFile(fn, loadsw, p)
char *fn;	/* file name */
int  loadsw;	/* 1 = load this file, 0 = use its symbol table */
register char *p;
{
	struct stat st;

	fname = fn;

	if (NULL == p) {	/* not forced */
		/*
		 * Look for rename entrys.
		 * con=atcon
		 * is not a filename with an =
		 * it means rename con to atcon.
		 * This is a drvld requirment.
		 */
		if (NULL != (p = strchr(fname, '='))) {
			ren_t	*new;
	
			new = alloc(sizeof(*new));
			new->next = rhead;
			rhead = new;
			new->from = fname;
			*p = '\0';
			new->to = p + 1;
			return;
		}

		/* all names must be *.[ao] */
		if (NULL == (p = strrchr(fname, '.')) || p[2])
			p = ".?";
		if (!loadsw)	/* drvld's read of the system */
			p = "_s.o" + 2;
#if	0
/* This is bogus, e.g. it suppresses the ld of a_s.o in "cc a_s.o". */
		/* shared libs have names like libc_s.a */
		if ((p[-2] == '_') && (p[-1] == 's'))
			loadsw = 0;
#endif
	}
#ifdef GEMDOS
	stat(fname, &st);
	ifd = qopen(fname, 0);
#else
	ifd = qopen(fname, 0);
	fstat(ifd, &st);
#endif
	switch(p[1]) {
	case 'a':
		archive(loadsw);
		break;
	case 'o':
		object("", st.st_size, loadsw);
		break;
	default:
		message("unlikely input file name '%s'", fname);
		/* Input file names must end \fB.o\fR for object or \fB.a\fR
		 * for archive. */
	}
	close(ifd);
}

/*
 * Add item to correct common
 */
static void
addComm(v)
long v;
{
	switch ((int)v & 3) {
	case 0:
		comnl += v;	/* Long aligned commons */
		break;
	case 2:
		comns += v;	/* Short aligned commons */
		break;
	default:
		comnb += v;	/* Byte aligned commons */
	}
}

/*
 * add a symbol to the symbol table.
 */
addsym(s, mp)
register SYMENT *s;
mod_t *mp;
{
	register sym_t *sp;
	ren_t *ren;
	enum state {
		local, gdef, gref, comm
	} new, old;
	int h, sec;
	char has_fsize, has_fcn, nodata;
	char *name;
	char w1[SYMNMLEN + 1], w2[SYMNMLEN + 1];

	sec = s->n_scnum;
	switch (s->n_sclass) {
	case C_EXTDEF:
		s->n_sclass = C_EXT;
		sec = s->n_value = s->n_scnum = 0;
	case C_EXT:
		if (sec)
			new = gdef;
		else if (s->n_value)
			new = comm;
		else
			new = gref;
		break;
	case C_STAT:
		new = local;
		break;
	default:
		if (!debflg)
			return;
		new = local;
	}

	name = symName(s, w1);

	/* check rename entrys */
	for (ren = rhead; NULL != ren; ren = ren->next) {
		if (!strcmp(ren->from, name)) {
			s->n_zeroes = 0;
			name = ren->to;
			s->n_offset = (long)name;
			break;
		}
	}
	h = crc16(name) % NHASH;

	/* Make symbols segment relative, if mp == NULL than sec == 0 */
	if (sec > 0)
		s->n_value += secth[sec - 1].s_size - mp->s[sec - 1].s_vaddr;
				
	if (local == new && 
	    (nolcl ||
	     (noilcl && (name[0] == '.') && (name[1] == 'L'))))
		return;

	for (sp = ((local == new) ? NULL : symtable[h]);
	     sp != NULL;
	     sp = sp->next) {
		if ((sp->sym.n_sclass != C_EXT) ||
		    (strcmp(symName(&(sp->sym), w2), name)))
			continue;
	
		if (sp->sym.n_scnum)
			old = gdef;
		else if (sp->sym.n_value)
			old = comm;
		else
			old = gref;

		switch (new) {
		/* case local: can't get here */

		case gref:
			s->n_offset = (long)sp;
			s->n_zeroes = 1;
			return;

		case gdef:
			switch (old) {
			case comm:
				spwarn(sp,
			"symbol defined as a common then a global");
			/* A symbol was defined as a common and a globl, eg
			 * .DM
			 *	int x;		// a common in one module
			 *	int x = 5;	// a globl in another module
			 * .DE
			 * Read your code and think about variable usage.
			 * We redefined the common as an external to match
			 * the UNIX linker, which fails to flag this.
			 */
				addComm(- sp->sym.n_value); /* zonk common */
				memcpy(&(sp->sym), s, sizeof(*s));
				sp->mod = mp;
				s->n_offset = (long)sp;
				s->n_zeroes = 1;
				return;

			case gref:
				nundef--;
				memcpy(&(sp->sym), s, sizeof(*s));
				sp->mod = mp;
				s->n_offset = (long)sp;
				s->n_zeroes = 1;
				return;

			default:
				symredef(sp, mp);
				return;
			}

		case comm:
			switch (old) {
			case comm:
				s->n_offset = (long)sp;
				s->n_zeroes = 1;

				if (sp->sym.n_value == s->n_value)
					return;

				spwarn(sp,  "defined with lengths %ld and %ld",
					sp->sym.n_value,
					s->n_value);
				/* A common was defined with different lengths,
				 * while this is legal it is very unusual in
				 * C programs. This warning may be turned off
				 * with the -q flag */

				addComm(- sp->sym.n_value);
				if (sp->sym.n_value < s->n_value) {
					sp->sym.n_value = s->n_value;
					sp->mod = mp;
				}
				sp->sym.n_value += 3;
				sp->sym.n_value &= ~3L;
				addComm(sp->sym.n_value);
				return;

			case gref:
				addComm(s->n_value);
				nundef--;
				memcpy(&(sp->sym), s, sizeof(*s));
				sp->mod = mp;
				s->n_offset = (long)sp;
				s->n_zeroes = 1;
				return;

			case gdef:
				spwarn(sp, "Defined as a global then a common");
					/* NODOC */
				s->n_offset = (long)sp;
				s->n_zeroes = 1;
				return;
			}
		}
	}

	/* symbol local or not found */
	sp = alloc(sizeof(*sp));
	memcpy(&(sp->sym), s, sizeof(*s));
	sp->next = symtable[h];
	symtable[h] = sp;

	switch (new) {
	case comm:
		addComm(s->n_value);
	case local:
		break;
	case gref:
		nundef++;
	}
	
	sp->mod = mp;
	s->n_offset = (long)sp;
	s->n_zeroes = 1;
}

/*
 * Read input file.
 */
static void
xread(loc, size)
char *loc;
unsigned size;
{
	if (size != read(ifd, loc, size))
		fatal("error reading '%s'", fname); /**/
}

/*
 * Inhale object file.
 */
object(mname, size, loadsw)
char *mname;
long size;
{
	register SYMENT *sym;
	register SCNHDR *s;
	SYMENT *endSym;
	mod_t *mp;
	char *endmod;
	long i, j, k;

	if (watch) {
		errCount--;
		modmsg(fname, mname, "adding");	/* NODOC */
	}
	if (size < sizeof(FILEHDR)) {
		modmsg(fname, mname, "not an object file - length %d", size);
		/* This cannot be an object file */
		exit(1);
	}
	mp    = alloc(sizeof(*mp));	/* allocate our header */
	mp->f = alloc((int)size);	/* allocate space for file */
	endmod = mp->f + size;		/* end of space for file */
	xread(mp->f, (int)size);	/* inhale file */

	if (mp->f->f_magic != C_386_MAGIC) {
		modmsg(fname, mname, "not an object file - header starts %x", mp->f->f_magic);
		/* Coff headers are expected to start 0x14C,
		 * which is called the magic number.
		 * This started with the stated hex number. */
		exit(1);
	}

	mp->fname = newcpy(fname);
	if (*mname)
		mp->mname = newcpy(mname);
	else
		mp->mname = "";

	if (loadsw) {	/* put modules on load list */
		if (head == NULL)
			head = mp;
		else
			tail->next = mp;
		tail = mp;
		fileh.f_magic = C_386_MAGIC;
	}
	else {		/* put modules on linkto list */
		if (xhead == NULL)
			xhead = mp;
		else
			xtail->next = mp;
		xtail = mp;
	}

	/*
	 * Turn disk pointers into ram pointers.
	 */
	j = ((long)(mp->f));
	mp->s = (SCNHDR *)(sizeof(FILEHDR) + j + mp->f->f_opthdr);
	mp->f->f_symptr += j;
	mp->l = (char *)(mp->f->f_symptr + (mp->f->f_nsyms * sizeof(SYMENT)));
	if ((mp->f->f_symptr > (long)endmod) ||
	    (mp->l > endmod) ||
	    (mp->f->f_nsyms < 0))
		corrupt(mp);

	/* Setup all sections */
	for (i = 0; i < mp->f->f_nscns; i++) {
		s = mp->s + i;

		if (nosym && (STYP_INFO == s->s_flags))
			continue;	/* -s strips comments */

		/* Test disk pointers for sanity and make them char pointers */
#define tst(x) if ((s->x > size) || (s->x < 0)) corrupt(mp); s->x += j;
		tst(s_scnptr);
		tst(s_relptr);
		tst(s_lnnoptr);
#undef tst

		for (k = 0; k < osegs; k++) {
			if (!strncmp(secth[k].s_name, s->s_name, 8)) {
				s->s_paddr = k;	/* remember outseg number */
				break;
			}
		}

		if ((k == osegs) && loadsw) {	/* New segment */
			if (++osegs == MAXSEG)
			   fatal("Max segment limit of %s exceeded", MAXSEG);
			w_message("adding segment '%s'", s->s_name);
			memcpy(secth + k, s, sizeof(*s));
			s->s_paddr = k;
			secth[k].s_size = secth[k].s_nreloc = 0;

			/* extra segments go to four places */
			switch(s->s_flags) {
			case STYP_TEXT:
				segMap[k] = S_TEXT;
				break;
			case STYP_DATA:
				segMap[k] = S_DATA;
				break;
			case STYP_BSS:
				segMap[k] = S_BSSD;
				break;
			case STYP_INFO:
				segMap[k] = S_COMM;
				break;
			default:
				segMap[k] = S_COMM;
				fprintf(stderr, 
			"warning: segment '%s' treated as comment\n",
					s->s_name);
			}
		}
	}

	/* Do all symbols */
	sym = (SYMENT *)mp->f->f_symptr;
	endSym =  sym + mp->f->f_nsyms;
	for (; sym < endSym; sym += sym->n_numaux + 1) {
		if (sym->n_numaux < 0)
			corrupt(mp);
		if (!sym->n_zeroes)
			sym->n_offset += (long)(mp->l);
		addsym(sym, mp);
	}

	if (loadsw) {
		/* Add to all sections */
		for (i = 0; i < mp->f->f_nscns; i++) {
			s = mp->s + i;

			if (nosym && (STYP_INFO == s->s_flags))
				continue;	/* -s strips comments */

			for (k = 0; k < osegs; k++)
				if (!strncmp(secth[k].s_name, s->s_name, 8))
					break;

			secth[k].s_size  += s->s_size;
			secth[k].s_nlnno += s->s_nlnno;
			if (reloc)
				secth[k].s_nreloc += s->s_nreloc;
		}
	}
}

/*
 * Read archive.
 */
archive(loadsw)
{
	struct old_hdr {
		char	ar_name[DIRSIZ];	/* Member name */
		time_t	ar_date;		/* Time inserted */
		short	ar_gid;			/* Group id */
		short	ar_uid;			/* User id */
		short	ar_mode;		/* Mode */
		fsize_t	ar_size;		/* File size */
	} arh;

	struct  ar_hdr in_arh;
	fsize_t	count, size, *ptrs;
	char	magic[SARMAG], *p;
	int	found;
	unsigned i;
	char 	*names, *name;

	xread(magic, sizeof(magic));	/* read archive magic string */

	if (memcmp(ARMAG, magic, SARMAG))
		fatal("'%s' is not a COFF archive", fname);
		/* All files ending \fB.a\fR should be COFF archives. */

	xread(&in_arh, sizeof(in_arh));	/* read archive header */

	memset(&arh, '\0', sizeof(arh));
	memcpy(arh.ar_name, in_arh.ar_name, DIRSIZ);
	if (NULL != (p = strchr(arh.ar_name, '/')))
		*p = '\0';

	sscanf(in_arh.ar_date, "%ld %d %d %o %ld",
		&arh.ar_date, &arh.ar_uid,
		&arh.ar_gid, &arh.ar_mode, &arh.ar_size);

	if (arh.ar_name[0])
		fatal("Library must be created with ar -s option");
		/* The \fBar \-s\fR option gives libraries a symbol table
		 * for the use of \fBld\fR. */

	/*
	 * read random libraries symbol table.
	 */
	xread(&count, sizeof(count));	/* read pointer count */
	flipbytes(&count);

	/* read file pointers */
	i = size = count * sizeof(count);
	if (i != size)
		fatal("archive '%s' is corrupt", fname);
		/* This file makes no sense as a COFF archive. */
	ptrs = alloca(i);
	xread(ptrs, i);

	/* read symbol names corresponding to pointers */
	i = size = arh.ar_size - size - sizeof(count);
	if (i != size)
		fatal("archive '%s' is corrupt", fname); /* NODOC */
	names = alloca(i);
	xread(names, i);

	w_message("reading archive '%s' nundef %d", fname, nundef);

	/* search symbol table unitl nothing found */
	do {
		for (found = i = 0, name = names;
		     (i < count) && nundef;
		     i++, name = strchr(name, '\0') + 1) {
			if(!ptrs[i] || symref(name, 0) == NULL)
				continue;

			found = 1;	/* found something this pass */
			flipbytes(ptrs + i);
			lseek(ifd, ptrs[i], 0);
			xread(&in_arh, sizeof(in_arh));

			sscanf(in_arh.ar_date,
				"%ld %d %d %o %ld",
				&arh.ar_date, &arh.ar_uid,
				&arh.ar_gid, &arh.ar_mode,
				&arh.ar_size);

			in_arh.ar_date[0] = '\0';
			if (NULL != (p = strchr(in_arh.ar_name,'/')))
				*p = '\0';

			object(in_arh.ar_name, arh.ar_size, loadsw);

			ptrs[i] = 0;	/* don't find this again */
		}
	} while (found);
}
