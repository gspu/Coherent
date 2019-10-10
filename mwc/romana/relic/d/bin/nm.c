/*
 * List a coff symbol table.
 */
#include <misc.h>
#include <canon.h>
#include <l.out.h>
#include <ar.h>
#include <errno.h>

FILE *fd;	/* Current input file */

int asw;	/* list all symbols */
int dsw;	/* list only defined symbols */
int gsw;	/* print only global symbols */
int nsw;	/* list numerically not alphabetically */
int osw;	/* append file name to each line */
int psw;	/* print symbols in symbol table order */
int rsw;	/* print in reverse alpha order */
int usw;	/* print only undefined symbols */
int vsw;	/* print coff type symbols */

int namesw;	/* one if header is required */
char *str_tab;
char *fname;

/*
 * Old form sort logic.
 */
ncomp_old(s1, s2)
struct nlist *s1, *s2;
{
	long i;

	if (!(i = (s1->n_value - s2->n_value)))
		return (0);
	if (i < 0)
		return (-1);
	return (1);
}

acomp_old(s1, s2)
struct nlist *s1, *s2;
{
	return(strncmp(s1->n_name, s2->n_name, NCPLN));
}

rcomp_old(s1, s2)
struct nlist *s1, *s2;
{
	return(strncmp(s2->n_name, s1->n_name, NCPLN));
}

/*
 * Process old ar files.
 */
old_ar(filen, at)
char *filen;
long at;
{
	long arhend;
	struct old_ar_hdr arh;

	namesw = 0;
	for (arhend = at + sizeof(short); ; ) {
		fseek(fd, arhend, 0);
		if (1 != fread(&arh, sizeof(arh), 1, fd))
			break;
		arhend += sizeof(arh);
		arh.ar_date = 0;	/* terminate name */
		cansize(arh.ar_size);
		if (strcmp(arh.ar_name, HDRNAME)) {
			printf("%s(%s)\n", filen, arh.ar_name);
			readHeaders(arhend, arh.ar_size);
		}
		arhend += arh.ar_size;
	}
	namesw = 1;
}

/*
 * Process old form stuff.
 */
old_nout(at)
long at;
{
	struct ldheader ldh;
	struct ldsym *sym, *s;
	register unsigned i, syms, ct;
	long toSym;
	short type;
	extern char *realloc();

	fseek(fd, at, 0);
	xread(&ldh, sizeof(ldh), "n.out header");
	if (!ldh.l_ssize[L_SYM]) {
		printf("No symbols in %s\n", fname);
		return;
	}

	if (namesw)
		printf("%s:\n", fname);

	canshort(ldh.l_machine);
	toSym = ldh.l_entry + sizeof(ldh);

	for (i = 0; i < L_SYM; i++) {
		cansize(ldh.l_ssize[i]);
		if (i != L_BSSI && i != L_BSSD)
			toSym += ldh.l_ssize[i];
	}

	fseek(fd, at + toSym, 0);
	cansize(ldh.l_ssize[L_SYM]);
	i = ldh.l_ssize[L_SYM];
	if (i != ldh.l_ssize[L_SYM])
		fatal("Out of space");
	sym = alloc(i);
	xread(sym, i, "symbol table");
	ct = i / sizeof(*sym);

	/* squeeze out unneeded stuff before sort */
	for (i = syms = 0; i < ct; i++) {
		canshort(sym[i].ls_type);
		canshort(sym[i].ls_addr);
		type = sym[i].ls_type;
		if (gsw && !(type & L_GLOBAL))
			continue;
		if ((type & ~L_GLOBAL) == L_REF) {	/* a reference */
			if (dsw)	/* list only defined */
				continue;
			if (usw && sym[i].ls_addr)
				continue;
		}
		else if (usw)
			continue;
		if (!asw && !csymbol(sym + i))
			continue;
		if (i != syms)
			sym[syms] = sym[i];
		syms++;
	}
	sym = realloc(sym, syms * sizeof(*sym));

	if (nsw)
		qsort(sym, syms, sizeof(*sym), ncomp_old);
	else if (rsw)
		qsort(sym, syms, sizeof(*sym), rcomp_old);
	else if (!psw)
		qsort(sym, syms, sizeof(*sym), acomp_old);

	for (s = sym; s < (sym + syms); s++) {
		static char *gn[] = {
			"SI", "PI", "BI",
			"SD", "PD", "BD",
			" D", "  ", "  ",
			" A", " C", "??"
		};
		static char *ln[] = {
			"si", "pi", "bi",
			"sd", "pd", "bd",
			" d", "  ", "  ",
			" a", " c", "??"
		};
		if (osw)
			printf("%s ", fname);
		i = s->ls_type & L_GLOBAL;
		type = s->ls_type & ~ L_GLOBAL;
		if (type < L_SHRI || type > L_REF)
			type = L_REF + 1;
		if (type == L_REF && !s->ls_addr)
			printf("      %c", i ? 'U' : 'u');
		else
			printf("%04x %s",
				s->ls_addr,
				i ? gn[type] : ln[type]);
		printf(" %.*s\n", NCPLN, s->ls_id);
	}

	free(sym);
}

/*
 * This routine gets called if we
 * are not in '-a' mode. It determines if
 * the symbol pointed to by 'sp' is a C
 * style symbol (trailing '_' or longer than
 * (NCPLN-1) characters). If it is it eats the '_'
 * and returns true.
 */
csymbol(sp)
register struct ldsym *sp;
{
	register char *cp1, *cp2;

	cp1 = &sp->ls_id[0];
	cp2 = &sp->ls_id[NCPLN];
	while (cp2!=cp1 && *--cp2==0)
		;
	if (*cp2 != 0) {
		if (*cp2 == '_') {
			*cp2 = 0;
			return (1);
		}
		if (cp2-cp1 >= (NCPLN-1))
			return (1);
	}
	return (0);
}

/*
 * Note some #defines in these include files interferes with
 * items in preceeding include files. Hence the strange
 * program order.
 */
#include <coff.h>
#include <arcoff.h>

#define cx(x) case C_ ## x: printf("%6.6s ", #x); break
#define ct(x) case T_ ## x: printf("%6.6s ", #x); break

static char helpmsg[] =
	"-d list only defined symbols\n"
	"-g print only global symbols\n"
	"-n list numerically not alphabetically\n"
	"-o append file name to each line\n"
	"-p print symbols in symbol table order\n"
	"-r print in reverse alpha order\n"
	"-u print only undefined symbols\n"
	"-v print coff type symbols\n";

FILEHDR fh;
SCNHDR *scns;

/*
 * Symbol name.
 */
static char *
symName(sym, work)
SYMENT *sym;
char *work;
{
	if (!sym->n_zeroes)
		return (str_tab + sym->n_offset - 4);

	/* make sure it's zero terminated */
	memcpy(work, sym->n_name, SYMNMLEN);
	work[SYMNMLEN] = '\0';
	return (work);
}

main(argc, argv)
char *argv[];
{
	char c;
	extern int optind;
	extern char *optarg;

	while (EOF != (c = getopt(argc, argv, "adgnopruv?"))) {
		switch (c) {
		case '?':
			printf(helpmsg);
			break;
		case 'a':
			asw = 1;
		case 'd':
			dsw = 1;
			break;
		case 'g':
			gsw = 1;
			break;
		case 'n':
			nsw = 1;
			break;
		case 'o':
			osw = 1;
			break;
		case 'p':
			psw = 1;
			break;
		case 'r':
			rsw = 1;
			break;
		case 'u':
			usw = 1;
			break;
		case 'v':
			vsw = 1;
			break;
		default:
			printf("usage: nm [-dgnopruv?] file ...\n");
			exit(1);
		}
	}

	if ((psw + rsw + nsw) > 1)
		fatal("More than one sort order");

	namesw = (argc - optind) > 1;
	for (; optind < argc; optind++) {
		fd = xopen(fname = argv[optind], "rb");

		readHeaders(0L, 0L);
		fclose(fd);
	}

	exit (0);
}

static
xread(to, size, msg)
char *to;
unsigned size;
char *msg;
{
	if (1 != fread(to, size, 1, fd))
		fatal("Error reading %s - %s", fname, msg);
}

static
readHeaders(at, size)
long at, size;
{
	unsigned i;

	xread(&fh, sizeof(fh), "coff header");
	if (C_386_MAGIC != fh.f_magic) {
		if (!memcmp(ARMAG, &fh, SARMAG))
			return(archive(at));

		canshort(fh.f_magic);
		if (fh.f_magic == OLD_ARMAG)
			return (old_ar(fname, at));
		if (fh.f_magic == L_MAGIC)
			return (old_nout(at));
		printf("Inappropriate filetype %s\n", fname);
		return (0);
	}
	if (fh.f_opthdr)	/* pass opt hdr */
		fseek(fd, at + sizeof(fh) + fh.f_opthdr, 0);
	scns = alloc(i = (fh.f_nscns * sizeof(SCNHDR)));
	xread(scns, i, "section headers");

	if (fh.f_nsyms)
		readSymbols(at, size);
	else
		printf("No symbols in %s\n", fname);

}

/*
 * Sort by alpha.
 */
acomp(s1, s2)
SYMENT *s1, *s2;
{
	char w1[SYMNMLEN + 1], w2[SYMNMLEN + 1];

	return (strcmp(symName(s1, w1), symName(s2, w2)));
}

/*
 * Sort by reverse alpha.
 */
rcomp(s1, s2)
SYMENT *s1, *s2;
{
	char w1[SYMNMLEN + 1], w2[SYMNMLEN + 1];

	return (- strcmp(symName(s1, w1), symName(s2, w2)));
}

/*
 * Sort by value;
 */
ncomp(s1, s2)
SYMENT *s1, *s2;
{
	long i;

	if (!(i = (s1->n_value - s2->n_value)))
		return (0);
	if (i < 0)
		return (-1);
	return (1);
}

static
readSymbols(at, size)
long at;
{
	register SYMENT *s;
	SYMENT *sym;
	long str_len;
	unsigned len;
	int i, aux, syms;
	extern char *realloc();

	/* read whole symbol table */
	fseek(fd, at + fh.f_symptr, 0);
	len = str_len = SYMESZ * fh.f_nsyms;
	if (str_len != len)
		fatal("Cannot process small model");
	sym = alloc(len);
	xread(sym, len, "symbol table");

	/* squeeze out unneeded stuff before sort */
	for (i = aux = syms = 0; i < fh.f_nsyms; i++) {
		if (aux) {
			aux--;
			continue;
		}
		s = sym + i;
		aux = s->n_numaux;
		switch (s->n_sclass) {
		case C_EXT:
			if (s->n_scnum || s->n_value) { /* defined */
				if (usw)	/* print only undefined */
					continue;
			} else {			/* undefined */
				if (dsw)	/* print only defined */
					continue;
			}
			break;
		case C_EXTDEF:
			if (dsw)
				continue;
			break;
		case C_STAT:
			if (gsw || usw)
				continue;
			switch (s->n_scnum) {
			case 1:
				if (!strcmp(s->n_name, ".text"))
					continue;
				break;
			case 2:
				if (!strcmp(s->n_name, ".data"))
					continue;
				break;
			case 3:
				if (!strcmp(s->n_name, ".bss"))
					continue;
				break;
			}
			break;
		default:
			if (!vsw)
				continue;
		}
		if (i != syms)
			sym[syms] = *s;
		syms++;
	}
	sym = realloc(sym, syms * SYMESZ);

	str_len = 0;
	if (!size || ((fh.f_symptr + len + sizeof(long)) < size))
		if (1 != fread(&str_len, sizeof(str_len), 1, fd))
			str_len = 0;
	if (str_len) {
		len = str_len -= 4;
		if (len != str_len)
			fatal("Cannot process small model");
		str_tab = alloc(len);
		xread(str_tab, len, "string table");
	}

	if (nsw)
		qsort(sym, syms, SYMESZ, ncomp);
	else if (rsw)
		qsort(sym, syms, SYMESZ, rcomp);
	else if (!psw)
		qsort(sym, syms, SYMESZ, acomp);

	for (s = sym; s < (sym + syms); s++) {
		char w1[SYMNMLEN + 1], *n;

		i = strlen(n = symName(s, w1));

		if (osw)
			printf("%s ", fname);

		/* display section name */
		switch (s->n_scnum) {
		case N_DEBUG:
			printf("DEBUG    ");
			break;
		case N_ABS:
			printf("ABSOLUTE ");
			break;
		case N_TV:
			printf("TV       ");
			break;		
		case N_UNDEF:
			if (s->n_value)
				printf(".comm    ");
			else
				printf("UNDEF    ");
			break;
		default:
			printf("%-8.8s ", scns[s->n_scnum - 1].s_name);
		}

		if (vsw) {	/* print coff type data */
			/* display storage class */
			switch (s->n_sclass) {
			cx(EFCN);
			cx(NULL);
			cx(AUTO);
			cx(EXT);
			cx(STAT);
			cx(REG);
			cx(EXTDEF);
			cx(LABEL);
			cx(ULABEL);
			cx(MOS);
			cx(ARG);
			cx(STRTAG);
			cx(MOU);
			cx(UNTAG);
			cx(TPDEF);
			cx(USTATIC);
			cx(ENTAG);
			cx(MOE);
			cx(REGPARM);
			cx(FIELD);
			cx(BLOCK);
			cx(FCN);
			cx(EOS);
			cx(FILE);
			default:
				printf("unknown ");
			}

			if (s->n_type & N_TMASK) {
				if (ISPTR(s->n_type))
					printf("pointer ");
				else if (ISFCN(s->n_type))
					printf("function ");
				else if (ISARY(s->n_type))
					printf("array ");
				else if (ISTAG(s->n_type))
					printf("struct ");
				else if (INCREF(s->n_type))
					printf("union  ");
				else if (DECREF(s->n_type))
					printf("enum ");
			}
			else
				printf("- ");
	
			/* display type */
			switch (BTYPE(s->n_type)) {
			ct(NULL);
			ct(ARG);
			ct(CHAR);
			ct(SHORT);
			ct(INT);
			ct(LONG);
			ct(FLOAT);
			ct(DOUBLE);
			ct(STRUCT);
			ct(UNION);
			ct(ENUM);
			ct(MOE);
			ct(UCHAR);
			ct(USHORT);
			ct(UINT);
			ct(ULONG);
			default:
				printf("unknown ");
			}
		}

		printf("%08lX %s\n", s->n_value, n);
	}
	free(sym);
}

/*
 * Print members of archive.
 */
archive(at)
long at;
{
	long arhend;
	struct ar_hdr coff_arh;
	struct old_ar_hdr arh;
	char *p;
	extern char *strchr();

	namesw = 0;
	for (arhend = at + SARMAG; ; ) {
		fseek(fd, arhend, 0);
		if (1 != fread(&coff_arh, sizeof(coff_arh), 1, fd))
			break;
		memset(&arh, '\0', sizeof(arh));
		memcpy(arh.ar_name, coff_arh.ar_name, DIRSIZ);
		if (NULL != (p = strchr(arh.ar_name, '/')))
			*p = '\0';

		sscanf(coff_arh.ar_date, "%ld %d %d %o %ld",
			&arh.ar_date, &arh.ar_uid,
			&arh.ar_gid, &arh.ar_mode, &arh.ar_size);

		arhend += sizeof(coff_arh);
		if (arh.ar_name[0]) {
			printf("%s(%s)\n", fname, arh.ar_name);
			readHeaders(arhend, arh.ar_size);
		}
		arhend += arh.ar_size;
		if (arhend & 1)
			arhend++;		
	}
	namesw = 1;
}
