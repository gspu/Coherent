/*
 * db/db6.c
 * A debugger.
 * Symbol handling.
 */

#include <canon.h>
#include "db.h"

/*
 * In earlier versions of db, a SYM included an id hash, a type and a symbol
 * table index, but not did not include the symbol id[] itself.
 * This allowed db to handle lots of symbols in a small address space,
 * but each symbol lookup required reading the symbol again from
 * the symbol table in the program file (i.e. from the disk).
 * Since memory is no longer as critical an issue, now db stores the id[]
 * as part of the SYM and symbol lookup does not need to access the file.
 */

/*
 * Given a value 'm' in segment 's', find the closest symbol below.
 * Return a pointer to the symbol, or NULL if none.
 * In COFF, with a flat address space, the segment can be ignored.
 * In l.out, where the same address can represent a code or a data item,
 * it had better not be ignored.
 */
SYM *
findsym(s, addr) int s; ADDR_T addr;
{
	register int n;
	register SYM *hp, *sp;

	dbprintf(("findsym(%d, %lX):\n", s, addr));
	sp = (SYM *)NULL;
	if (sfp == (FILE *)NULL)
		return sp;			/* no symbols */
	for (n = 0; n < NHASH; n++) {		/* search all hash buckets */
		for (hp = symhash[n]; hp != (SYM *)NULL; hp = hp->s_next) {

			dbprintf(("findsym: (%d, %lX) id=%s @ (%d, %lX)\n", s, addr, hp->s_id, hp->s_segn, hp->s_addr));
			if (IS_LOUT && hp->s_segn != s)
				continue;	/* segment mismatch */

			/* If below and better than previous best guess... */
			if (hp->s_addr <= addr
			 && (sp==(SYM *)NULL || hp->s_addr > sp->s_addr)) {
				sp = hp;		/* best so far */
				dbprintf((" best so far: %s addr=%lX s=%d\n", sp->s_id, sp->s_addr, sp->s_segn));
				if (sp->s_addr == addr)
					return sp;	/* gotcha! */
			}
		}
	}
	return sp;				/* NULL or best guess */
}

/*
 * Hash a symbol name.
 * The '_' character has no effect on the hash, so "foo" and "foo_" hash
 * to the same value; this is convenient for l.out symbol lookup,
 * where is_symbol() understands "foo" to mean "foo_" if there is no "foo".
 */
int
hash(id) register char *id;
{
	register int c, h;

	for (h = 0; (c = *id++) != 0; )
		if (c != '_')
			h += c;
	return h % NHASH;
}

/*
 * See if the given symbol is a special symbol "d", "i" or "u".
 */
int
is_special(sp) register SYM *sp;
{
	register int s;

	if (sp->s_id[1] != '\0')
		return 0;
	switch (sp->s_id[0]) {
	case 'd':	s = DSEG;		break;
	case 'i':	s = ISEG;		break;
	case 'u':	s = USEG;		break;
	default:	return 0;
	}
	sp->s_addr = 0;
	sp->s_segn = s;
	return 1;
}

/*
 * Find the value of a symbol.
 * Return 1 on success, 0 on failure.
 * For l.out, "foo" matches "foo_" if "foo" is not found.
 */
int
is_symbol(sp) SYM *sp;
{
	register char *cp;
	register SYM *hp;
	register int len;

	dbprintf(("is_symbol(%s): "));
	cp = sp->s_id;
	for (hp = symhash[hash(cp)]; hp != (SYM *)NULL; hp = hp->s_next) {
		if (strcmp(cp, hp->s_id) == 0) {
			dbprintf(("addr=%lX segn=%d\n", hp->s_addr, hp->s_segn));
			sp->s_addr = hp->s_addr;
			sp->s_segn = hp->s_segn;
			return 1;
		}
	}

	/* The symbol was not found.  If l.out, try again with trailing '_'. */
	if (IS_LOUT) {
		len = strlen(cp);
		for (hp = symhash[hash(cp)]; hp != (SYM *)NULL; hp = hp->s_next) {
			if (strncmp(cp, hp->s_id, len) == 0
			 && hp->s_id[len] == '_'
			 && hp->s_id[len+1] == '\0') {
				dbprintf(("(%s) addr=%lX segn=%d\n", hp->s_id, hp->s_addr, hp->s_segn));
				sp->s_addr = hp->s_addr;
				sp->s_segn = hp->s_segn;
				return 1;
			}
		}
	}
	dbprintf((" is_symbol(): %s not found\n", cp));
	return 0;
}

/*
 * Allocate a new SYM with the given id, address and segment.
 * Initialize it and link it into the hash chain.
 * db.h declares char s_id[1], but the nalloc() call below
 * leaves room for the actual id char s_id[len+1].
 */
void
new_sym(id, addr, s) char *id; ADDR_T addr; int s;
{
	register int len, h;
	register SYM *sp;

	h = hash(id);
	len = strlen(id);			/* len+1 is s_id len with NUL */
	dbprintf(("new_sym(%s, %x, %lX) len=%d hash=%x\n", id, type, addr, len, h));
	sp = (SYM *)nalloc(sizeof(SYM) + len, "symbol");
	sp->s_next = symhash[h];
	symhash[h] = sp;
	sp->s_addr = addr;
	sp->s_segn = s;
	strcpy(sp->s_id, id);
}

/*
 * Read COFF symbols.
 */
int
read_coff_sym()
{
	SYMENT		coffsym;
	long		n;
	register int	len, segn;
	char		*strtab;
	SCNHDR		*shp;

	dbprintf(("read_coff_sym()\n"));

	/* Read the COFF string table. */
	strtab = read_strtab();

	/* Read the COFF section headers. */
	len = coff_hdr.f_nscns * sizeof(SCNHDR);
	shp = (SCNHDR *)nalloc(len, "COFF section headers");
	if (fseek(sfp, (long)(sizeof(FILEHDR) + coff_hdr.f_opthdr), SEEK_SET) == -1
	 || fread(shp, len, 1, sfp) != 1)
		return printe("Cannot read section headers");

	/* Read symbols. */
	if (fseek(sfp, (long)coff_hdr.f_symptr, SEEK_SET) == -1)
		return printe("COFF symbol seek failed");
	for (nsyms = 0, n = coff_hdr.f_nsyms; n--; ) {
		/* Read a COFF symbol. */
		if (fread(&coffsym, sizeof(SYMENT), 1, sfp) != 1)
			return printe("Cannot read COFF symbol");
		switch (coffsym.n_sclass) {
		case C_EXT:
		case C_EXTDEF:
			break;
		default:
			continue;
		}
		/* Ignore undefined and absolute symbols. */
		if (coffsym.n_scnum == N_UNDEF || coffsym.n_scnum == N_ABS)
			continue;

		switch((int)shp[coffsym.n_scnum-1].s_flags) {
		case STYP_TEXT:	segn = ISEG;	break;
		case STYP_DATA:
		case STYP_BSS:	segn = DSEG;	break;
		default:
			panic("Unallowed section flags 0x%lX for symbol in section %d",
				shp[coffsym.n_scnum-1].s_flags, coffsym.n_scnum-1);
		}
		++nsyms;
		new_sym(symName(&coffsym, strtab), coffsym.n_value, segn);
	}
	dbprintf(("COFF nsyms=%d\n", nsyms));
	if (strtab != NULL)
		nfree(strtab);
	nfree(shp);
	return 1;
}

/*
 * Read l.out symbols.
 */
int
read_lout_sym(symseek) long symseek;
{
	register int n, segn;
	struct ldsym lds;
	char id[NCPLN+1];

	if (fseek(sfp, symseek, SEEK_SET) == -1)
		return printr("Cannot seek to l.out symbols");
	for (n = nsyms; n--; ) {
		/* Read an l.out symbol. */
		if (fread(&lds, sizeof(struct ldsym), 1, sfp) != 1)
			return printr("Cannot read l.out symbol");
		canint(lds.ls_type);
		switch(lds.ls_type & LR_SEG) {
		case L_ABS:
		case L_SHRI:
		case L_PRVI:
		case L_BSSI:
			segn = ISEG;
			break;
		case L_SHRD:
		case L_PRVD:
		case L_BSSD:
			segn = DSEG;
			break;
		default:
			dbprintf(("Unexpected l.out segment %d\n", lds.ls_type & LR_SEG));
			continue;
		}
		canvaddr(lds.ls_addr);
		strncpy(id, lds.ls_id, NCPLN);
		id[NCPLN] = '\0';
		new_sym(id, (ADDR_T)lds.ls_addr, segn);
	}
	dbprintf(("l.out nsyms=%d\n", nsyms));
	return 1;
}

/*
 * Read the COFF string table.
 * Return a pointer to the allocated table, or NULL if empty.
 */
char *
read_strtab()
{
	register unsigned int len;
	int llen;
	char *strtab;

	dbprintf(("read_strtab()\n"));
	len = llen = sizeof(SYMENT) * coff_hdr.f_nsyms;
	if (len == 0)
		return NULL;			/* no symbols */
	if (llen != len)
		panic("COFF symbol table too big");

	/* Seek to strtab length. */
	if (fseek(sfp, coff_hdr.f_symptr+len, SEEK_SET) == -1
	 || fread(&llen, sizeof(llen), 1, sfp) != 1)
		llen = 0;
	if (llen == 0)
		return NULL;			/* no string table */
	len = llen -= 4;
	if (len != llen)
		panic("COFF string table too big");
	strtab = nalloc((int)len, "COFF string table");
	if (fread(strtab, len, 1, sfp) != 1) {
		printe("Cannot read symbol string table");
		return NULL;
	}
	return strtab;
}

/*
 * Given a COFF symbol table entry, return its NUL-terminated name.
 */
char *
symName(sym, strtab) SYMENT *sym; char *strtab;
{
	static	char	work[SYMNMLEN+1];

	if (sym->n_zeroes == 0)
		return strtab + sym->n_offset - 4;	/* name in string table */

	/* Copy name from n_name to make sure it's NUL-terminated. */
	memcpy(work, sym->n_name, SYMNMLEN);
	work[SYMNMLEN] = '\0';
	return work;
}

/*
 * Read a symbol and return its value in the given 'VAL' structure.
 * The symbol name can contain alphanumeric characters and '_';
 * a leading '%' indicates a register name.
 */
int
symval(vp) VAL *vp;
{
	register int c;
	register char *cp;
	int regf;
	static SYM *sp;
	static int sym_len = 128;

	/* Allocate a SYM first time through. */
	if (sp == (SYM *)NULL)
		sp = (SYM *)nalloc(sizeof(SYM)+sym_len+1, "symbol id buffer");

	/* Read the symbol name into the SYM id[] buffer. */
	for (regf = 0, cp = sp->s_id; ; cp++) {
		c = getn();
		/* Allow leading '%' to indicate register name. */
		if (cp==sp->s_id && c=='%') {
			c = getn();
			++regf;
		}
		if (!isascii(c) || (!isalnum(c) && c!= '_'))
			break;
		if (cp == &sp->s_id[sym_len]) {
			register SYM *sp2;

			/* Grow the symbol id buffer when required. */
			sp2 = (SYM *)nalloc(sizeof(SYM)+sym_len+sym_len+1, "symbol id buffer");
			strncpy(sp2->s_id, sp->s_id, sym_len);
			cp = &sp2->s_id[sym_len];
			sym_len += sym_len;
			nfree(sp);
			sp = sp2;
		}		
		*cp = c;
	}
	*cp = '\0';
	ungetn(c);
	if (regf) {
		if (is_reg(sp) == 0)
			return printe("Register not found");
	} else if (!is_special(sp) && !is_symbol(sp) && !is_reg(sp))
		return printe("Symbol not found");

	/* Copy the value and segment to vp. */
	vp->v_nval = (long)sp->s_addr;
	vp->v_segn = sp->s_segn;
	vp->v_flag = VSEGN;
	return 1;
}

/* end of db/db6.c */

