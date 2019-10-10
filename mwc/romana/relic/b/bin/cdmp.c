/*
 * cdmp.c
 * 3/24/93
 * Requires libmisc functions: cc cdmp.c -lmisc
 * Read and print COFF files.
 * Usage: cdmp [ -adlrsx ] filename ...
 * Options:
 * 	-a	supress symbol aux entries
 * 	-d	supress data dumps
 * 	-l	supress line numbers
 * 	-r	supress relocation entries
 * 	-s	supress symbol entries
 *	-V	print version number
 *	-x	dump aux entries in hex
 *	-?	print usage info and exit
 * Does not know all there is to know about aux entry structure yet.
 */

#include <misc.h>		/* misc useful stuff */
#include <coff.h>
#include <errno.h>

#define	VERSION	"V2.2"
#define VHSZ	48		/* line size in vertical hex dump */
typedef	char	SECNAME[9];	/* NUL-terminated 8 character section name */

/* Some shortcut display stuff. */
#define showFlag(flag, msg) if (fh.f_flags & flag) printf("\t" msg "\n");
#define showCase(x)	showDesc(x, #x)
#define	showDesc(x, d)	case x: printf(d); break;
#define showVal(x)	case x: printf(#x "\tvalue=%ld ", se->n_value); break;
#define showHexVal(x)	case x: printf(#x "\tvalue=0x%lX ", se->n_value); break;

/* Externals. */
extern	long	ftell();
extern	char	*optarg;

/* Forward. */
void	fatal();
char	*checkStr();
void	optHeader();
void	readHeaders();
void	shrLib();
void	readSection();
void	readStrings();
void	readSymbols();
void	print_aux();
void	print_sym();
void	dump();
int	clean();
void	outc();
int	hex();

/* Globals. */
char	aswitch;		/* Suppress aux entry dumps		*/
char	buf[VHSZ];		/* Buffer for hex dump			*/
char	dswitch;		/* Suppress data dumps			*/
FILE	*fp;			/* COFF file pointer			*/
char	lswitch;		/* Suppress line number dumps		*/
long	num_sections;		/* Number of sections			*/
long	num_symbols;		/* Number of symbols			*/
char	rswitch;		/* Suppress reloc dumps			*/
SECNAME	*sec_name;		/* Section names			*/
long	section_seek;		/* Seek to seek start of section	*/
char	sswitch;		/* Suppress symbol dumps		*/
char	*str_tab;		/* String char array			*/
long	symptr;			/* File pointer to symbol table entries	*/
char	xswitch;		/* Dump aux entries in hex		*/

/*
 * Print fatal error message and die.
 */
/* VARARGS */
void
fatal(s) char *s;
{
	register int save;

	save = errno;
	fprintf(stderr, "cdmp: %r\n", &s);
	if (0 != (errno = save))
		perror("errno reports");
	exit(1);
}

/*
 * Return a printable version of string s,
 * massaging nonprintable characters if necessary.
 */
char *
checkStr(s) unsigned char *s;
{
	register unsigned char *p, c;
	register int ct, badct;
	static char *work = NULL;

	for (badct = 0, ct = 1, p = s; c = *p++; ct++)
		if ((c <= ' ') || (c > '~'))
			badct += 2;	/* not printable as is */

	if (!badct)
		return s;		/* ok as is */

	if (NULL != work)
		free(work);		/* free previous */

	work = alloc(badct + ct);
	for (p = work; c = *s++;) {
		if (c > '~') {
			*p++ = '~';
			c &= 0x7f;
		}
		if (c <= ' ') {
			*p++ = '^';
			c |= '@';
		}
		*p++ = c;
	}
	return work;
}

/*
 * Process optional file header.
 */
void
optHeader(size) unsigned int size;
{
	register AOUTHDR *ohp;
	register unsigned int tail;

	if ((tail = size - sizeof(*ohp)) < 0)
		fatal("optional header too small -- %d bytes", size);
	ohp = alloc(size);

	if (1 != fread(ohp, size, 1, fp))
		fatal("error reading optional header");

	printf("\nOPTIONAL HEADER VALUES\n");
	printf("magic            = 0x%X\n",	ohp->magic);
	printf("version stamp    = %d\n",	ohp->vstamp);
	printf("text size        = 0x%lX\n",	ohp->tsize);
	printf("init data size   = 0x%lX\n",	ohp->dsize);
	printf("uninit data size = 0x%lX\n",	ohp->bsize);
	printf("entry point      = 0x%lX\n",	ohp->entry);
	printf("text start       = 0x%lX\n",	ohp->text_start);
	printf("data start       = 0x%lX\n",	ohp->data_start);

	/* Print extra info, if any. */
	if (tail) {
		printf("\n EXTRA OPTIONAL HEADER INFO\n");
		dump(ohp + 1, tail);
	}
	free(ohp);
}

/*
 * Process file header.
 */
void
readHeaders(fn) char *fn;
{
	FILEHDR	fh;

	fp = xopen(fn, "rb");

	if (1 != fread(&fh, sizeof(fh), 1, fp))
		fatal("error reading COFF header");

	printf("FILE %s HEADER VALUES\n",	fn);
	printf("magic number   = 0x%X\n",	fh.f_magic);
	printf("sections       = %ld\n",	num_sections = fh.f_nscns);
	printf("file date      = %s",		ctime(&fh.f_timdat));
	printf("symbol ptr     = 0x%lX\n",	symptr = fh.f_symptr);
	printf("symbols        = %ld\n",	num_symbols = fh.f_nsyms);
	printf("sizeof(opthdr) = %d\n",		fh.f_opthdr);
	printf("flags          = 0x%X\n",	fh.f_flags);

	showFlag(F_RELFLG,	"Relocation info stripped from file");
	showFlag(F_EXEC,	"File is executable");
	showFlag(F_LNNO,	"Line numbers stripped from file");
	showFlag(F_LSYMS,	"Local symbols stripped from file");
	showFlag(F_MINMAL,	"Minimal object file");

	/*
	 * Allocate section name array.
	 */
	if (num_sections != 0)
		sec_name = (SECNAME *)alloc(((int)num_sections) * (sizeof(SECNAME)));

	if (fh.f_opthdr)
		optHeader(fh.f_opthdr);		/* optional header */
	section_seek = sizeof(FILEHDR) + fh.f_opthdr;
}

/*
 * Process shared library.
 */
void
shrLib()
{
	SHRLIB shr;
	register long i;
	register char *pathn;

	if (1 != fread(&shr, sizeof(shr), 1, fp))
		fatal("error reading library section");

	if (shr.pathndx -= 2) {
		long j;
		printf("\nExtra Library info:\n");

		for (j = shr.pathndx * 4;
		     j && (i = fread(buf, 1, ((j > VHSZ) ? VHSZ : (int)j), fp));
		     j -= i) {
			if (!i)
				fatal("unexpected EOF in .lib data");
			dump(buf, (int)i);
		}
	}

	pathn = alloc(i = (shr.entsz - 2) * 4);
	if (1 != fread(pathn, i, 1, fp))
		fatal("error reading library name");
	printf("\nReferences %s\n", pathn);
	free(pathn);
}

/*
 * Process sections.
 */
void
readSection(n) register int n;
{
	SCNHDR	sh;
	register long i;

	fseek(fp, section_seek, 0);
	if (1 != fread(&sh, sizeof(SCNHDR), 1, fp))
		fatal("error reading section header");

	section_seek += sizeof(SCNHDR);
	fseek(fp, sh.s_scnptr, 0);

	strncpy(sec_name[n], checkStr(sh.s_name), sizeof(SECNAME) - 1);
	printf("\n%s - SECTION HEADER -\n",	sec_name[n]);
	printf("physical address    = 0x%lX\n", sh.s_paddr);
	printf("virtual address     = 0x%lX\n", sh.s_vaddr);
	printf("section size        = 0x%lX\n", sh.s_size);
	printf("file ptr to data    = 0x%lX\n", sh.s_scnptr);
	printf("file ptr to relocs  = 0x%lX\n", sh.s_relptr);
	printf("file ptr to lines   = 0x%lX\n", sh.s_lnnoptr);
	printf("relocation entries  = %u\n",	sh.s_nreloc);
	printf("line number entries = %u\n",	sh.s_nlnno);
	printf("flags               = 0x%lX\t", sh.s_flags);
	switch((int)sh.s_flags) {

	showDesc(STYP_GROUP,	"grouped section")
	showDesc(STYP_PAD,	"padding section")
	showDesc(STYP_COPY,	"copy section")
	showDesc(STYP_INFO,	"comment section")
	showDesc(STYP_OVER,	"overlay section")
	showDesc(STYP_TEXT,	"text only")
	showDesc(STYP_DATA,	"data only")
	showDesc(STYP_BSS,	"bss only")

	case STYP_LIB:
		printf(".lib section\n");
		shrLib();
		return;

	default:
		printf("unrecognized section");
		break;
	}
	putchar('\n');

	/* Print raw data. */
	if (!dswitch && strcmp(sh.s_name, ".bss")) { /* don't output bss */
		register long j;

		fseek(fp, sh.s_scnptr, 0);
		printf("\nRAW DATA\n");

		for (j = sh.s_size;
		     j && (i = fread(buf, 1, ((j > VHSZ) ? VHSZ : (int)j), fp));
		     j -= i) {
			if (!i)
				fatal("unexpected EOF in %.8s data",
				      checkStr(sh.s_name));
			dump(buf, (int)i);
		}
	}

	/* Print relocs. */
	if (!rswitch && sh.s_nreloc) {
		fseek(fp, sh.s_relptr, 0);
		printf("\nRELOCATION ENTRIES\n");
		for (i = 0; i < sh.s_nreloc; i++) {
			RELOC	re;	/* Relocation entry structure */

			if (1 != fread(&re, RELSZ, 1, fp))
				fatal("error reading relocation entry");

			printf("address=0x%lX\tindex=%ld \ttype=",
				re.r_vaddr, re.r_symndx);
			switch(re.r_type) {
			showCase(R_DIR8)
			showCase(R_DIR16)
			showCase(R_DIR32)
			showCase(R_RELBYTE)
			showCase(R_RELWORD)
			showCase(R_RELLONG)
			showCase(R_PCRBYTE)
			showCase(R_PCRWORD)
			showCase(R_PCRLONG)
			showCase(R_NONREL)
			default:
				fatal("unexpected relocation type 0x%X",
					re.r_type);
				break;
			}
			putchar('\n');
		}
	}

	/* Print line numbers. */
	if (!lswitch && sh.s_nlnno) {
		fseek(fp, sh.s_lnnoptr, 0);
		printf("\nLINE NUMBER ENTRIES\n");

		for (i = 0; i < sh.s_nlnno; i++) {
			LINENO	le;	/* Line number entry structure */

			if (1 != fread(&le, LINESZ, 1, fp))
				fatal("error reading line number entry");

			if (le.l_lnno)
				printf("address=0x%lX\tline=%d\n",
					le.l_addr.l_paddr, le.l_lnno);
			else
				printf("function=%d\n", le.l_addr.l_symndx);
		}
	}
}

/*
 * Read the string table into memory.
 * This allows readSymbols() to work.
 */
void
readStrings()
{
	register unsigned char *str_ptr, c;
	long str_seek;
	unsigned long str_length;
	unsigned len;

	str_seek = symptr + (SYMESZ * num_symbols);
	fseek(fp, str_seek, 0);

	if (1 != fread(&str_length, sizeof(str_length), 1, fp))
		str_length = 0;

	if (str_length == 0) {
		printf("\nNO STRING TABLE\n");
		return;
	}
	printf("\nSTRING TABLE DUMP\n");
	len = str_length -= 4;
	if (len != str_length)
		fatal("bad string table length %ld", str_length);
	str_tab = alloc(len);
	if (1 != fread(str_tab, len, 1, fp))
		fatal("error reading string table %lX %d", ftell(fp), len);

	for (str_ptr = str_tab; str_ptr < str_tab + str_length; ) {
		putchar('\t');
		while (c = *str_ptr++) {
			if (c > '~') {
				c &= 0x7f;
				putchar('~');
			}
			if (c < ' ') {
				c |= '@';
				putchar('^');
			}
			putchar(c);
		}
		putchar('\n');
	}
}

/*
 * Process symbol table.
 */
void
readSymbols()
{
	SYMENT se;
	register long i, j, naux;

	if (sswitch)
		return;
	fseek(fp, symptr, 0);
	printf("\nSYMBOL TABLE ENTRIES\n");
	for (i = 0; i < num_symbols; i++) {
		if (1 != fread(&se, SYMESZ, 1, fp))
			fatal("error reading symbol entry");
		print_sym(&se, i);
		naux = se.n_numaux;
		for (j = 0; j < naux; j++)
			print_aux(i+j+1, &se);
		i += naux;
		if (i >= num_symbols)
			fatal("inconsistant sym table");
	}
}

#define showNe(x, d) if(l = ae.x) printf("\t" d "=%ld", l)
/*
 * Process a symbol aux entry.
 * This is still pretty ad hoc, it may not do all entries correctly yet.
 * Does not print 0-valued fields.
 */
void
print_aux(n, sep) int n; register SYMENT *sep;
{
	AUXENT ae;
	register int type, class;
	register long l;
	int has_fsize, has_fcn;
	unsigned short *sp;
	char fname[FILNMLEN + 1];

	if (1 != fread(&ae, AUXESZ, 1, fp))
		fatal("error reading symbol aux entry");
	if (aswitch)
		return;					/* suppressed */
	printf("%4ld\t", n);				/* symbol number */
	if (xswitch) {					/* dump in hex */
		printf("\tAUX ENTRY DUMP\n");
		dump(&ae, sizeof(ae));
		return;
	}

	class = sep->n_sclass;
	type = sep->n_type;

	if (class == C_FILE) {				/* .file */
		strncpy(fname, ae.ae_fname, FILNMLEN);
		fname[FILNMLEN] = '\0';
		printf("\tfilename=%s\n", checkStr(fname));
		return;
	} else if (class == C_STAT && type == T_NULL) {	/* section name */
		printf("\tlength=%lX\trelocs=%d\tlinenos=%d\n",
			ae.ae_scnlen,
			ae.ae_nreloc,
			ae.ae_nlinno);
		return;
	}

	/*
	 * In cases not handled above,
	 * the AUXENT is an x_sym which must be decyphered.
	 * Flags tell which members of unions to dump.
	 * The flag setting might not be quite right yet.
	 */
	has_fsize = has_fcn = 0;
	if (class == C_STRTAG || class == C_UNTAG || class == C_ENTAG
	 || class == C_BLOCK)		/* tag definitions or .bb or .eb */
		++has_fcn;
	if (ISFCN(type)) {
		++has_fsize;
		++has_fcn;
	}

	/* Print tag index. */
	showNe(ae_tagndx, "tag");

	/* Print fsize or lnsz info. */
	if (has_fsize) {
		showNe(ae_fsize, "fsize");
	} else {
		showNe(ae_lnno, "lnno");
		showNe(ae_size, "size");
	}

	/* Print fcn or ary info. */
	if (has_fcn) {
		if (l = ae.ae_lnnoptr)
			printf("\tlnnoptr=0x%lX", l);
		showNe(ae_endndx, "end");
	} else {
		sp = ae.ae_dimen;
		if (*sp != 0) {
			printf("\tdims=< ");
			while (sp < &ae.ae_dimen[DIMNUM] && *sp)
				printf("%d ", *sp++);
			putchar('>');
		}
	}

	/* Print tv index. */
	showNe(ae_tvndx, "tv");
	putchar('\n');
}
#undef showNe

/*
 * Process symbol table entry.
 */
void
print_sym(se, n) register SYMENT *se; register long n;
{
	register int i, c, flag, derived;
	
	if (se->n_sclass == C_FILE && n > 0)
		putchar('\n');			/* for readability */
	printf("%4ld\t", n);			/* index number */
	if (se->n_zeroes != 0) {		/* name in place */
		for (flag = i = 0; i < SYMNMLEN; i++) {
			if (flag)
				;
			else if ((' ' < (c = se->n_name[i])) && ('~' >= c))
				putchar(c);
			else
				flag = c ? 1 : -1;
		}
		putchar('\t');
	} else					/* name in string table */
		printf("%s ", checkStr(str_tab + se->n_offset - 4));

	/* Print section. */
	i = se->n_scnum;
	printf("section=");
	if (i >= 1 && i <= num_sections)
		printf("%s", sec_name[i-1]);
	else
		switch(i) {
		showCase(N_UNDEF)
		showCase(N_ABS)
		showCase(N_DEBUG)
		default:
			printf("%d?", i);
			break;
		}

	/* Print the type. */
	printf("\ttype=");

	/* Print derived types. */
	derived = 0;
	for (i = se->n_type; i & N_TMASK; i >>= N_TSHIFT) {
		if (derived++ == 0)
			putchar('<');
		switch((i & N_TMASK) >> N_BTSHFT) {
		showCase(DT_PTR)
		showCase(DT_FCN)
		showCase(DT_ARY)
		case DT_NON:
		default:
			fatal("unexpected derived type 0x%X", i & N_TMASK);
		}
		putchar(' ');
	}

	switch (c = (se->n_type & N_BTMASK)) {	/* base type */
	showDesc(T_NULL, "none")
	showCase(T_CHAR)
	showCase(T_SHORT)
	showCase(T_INT)
	showCase(T_LONG)
	showCase(T_FLOAT)
	showCase(T_DOUBLE)
	showCase(T_STRUCT)
	showCase(T_UNION)
	showCase(T_ENUM)
	showCase(T_MOE)
	showCase(T_UCHAR)
	showCase(T_USHORT)
	showCase(T_UINT)
	showCase(T_ULONG)

	case T_ARG:		/* What has base type (not storage class) ARG? */
	default:
		fatal("unexpected base type 0x%X", c);

	}
	if (derived)
		putchar('>');

	/* Print the storage class. */
	printf("\tclass=");
	switch (i = se->n_sclass) {

	showVal(C_EFCN)
	showVal(C_NULL)
	showVal(C_AUTO)
	showHexVal(C_STAT)
	showVal(C_REG)
	showVal(C_EXTDEF)
	showVal(C_LABEL)
	showVal(C_ULABEL)
	showVal(C_MOS)
	showVal(C_ARG)
	showVal(C_STRTAG)
	showVal(C_MOU)
	showVal(C_UNTAG)
	showVal(C_TPDEF)
	showVal(C_ENTAG)
	showVal(C_MOE)
	showVal(C_REGPARM)
	showVal(C_FIELD)
	showVal(C_BLOCK)
	showVal(C_FCN)
	showVal(C_EOS)
	showVal(C_FILE)

	case C_EXT:
		if (se->n_scnum != N_UNDEF)
			printf("C_EXT\tvalue=0x%lX", se->n_value);
		else if (se->n_value != 0)
			printf("Common\tlength=%ld", se->n_value);
		else
			printf("External");
		break;

	case C_USTATIC:			/* What is an undefined static? */
		fatal("unexpected storage class 0x%X", i);

	default:
		printf("0x%X", i);

	}

#if	0
	if (se->n_numaux)
		printf("\tnaux=%d", se->n_numaux);
#endif
	putchar('\n');

	if (1 == flag) {
		printf("*** Bad data in name **\n");
		dump(se, SYMESZ);
	}
}

/*
 * Vertical hex dump of p bytes from buffer buf.
 */
void
dump(buf, p) register char *buf; register int p;
{
	register int i;

	/* Offset. */
	printf ("\n%6lX\t", ftell(fp) - p);

	/* Printable version of character. */
	for (i = 0; i < p; i++ )
		outc(clean(buf[i]), i, ' ');
	printf("\n\t");

	/* High hex digit. */
	for (i = 0; i < p; i++)
		outc(hex((buf[i] >> 4) & 0x0f), i, '.');
	printf("\n\t");

	/* Low hex digit. */
	for (i = 0; i < p; i++)
		outc(hex(buf[i]& 0x0f), i, '.');
	putchar('\n');
}

/*
 * Return c if printable, '.' if not.
 */
int
clean(c) register int c;
{
	return (c >= ' ' && c <= '~' ) ? c : '.';
}

/*
 * Print c, preceded by s every 4 times.
 */
void
outc(c, i, s) register int i, c, s;
{
	if ((i&3) == 0 && i != 0 )
		putchar(s);
	putchar(c);
}

/*
 * Convert hex digit c to corresponding ASCII character.
 */
int
hex(c) register int c;
{
	return ( c <= 9 ) ? c + '0' : c + 'A' - 10;
}

/*
 * Mainline.
 */
main(argc, argv) int argc; char *argv[];
{
	register int i, c;

	while (EOF != (c = getargs(argc, argv, "adlrsxV?"))) {
		switch (c) {

		case 0:
			/* Process a COFF file. */
			readHeaders(optarg);
			for (i = 0; i < num_sections; i++)
				readSection(i);
			if (num_symbols) {
				readStrings();
				readSymbols();
			}
			/* Cleanup. */
			if (sec_name != NULL) {
				free(sec_name);
				sec_name = NULL;
			}
			if (str_tab != NULL) {
				free(str_tab);
				str_tab = NULL;
			}
			fclose(fp);
			break;

		case 'a':	aswitch++;	break;
		case 'd':	dswitch++;	break;
		case 'l':	lswitch++;	break;
		case 'r':	rswitch++;	break;
		case 's':	sswitch++;	break;
		case 'x':	xswitch++;	break;

		case 'V':
			fprintf(stderr, "cdmp: %s\n", VERSION);
			break;

		case '?':
		default:
			fprintf(stderr,
				"Usage: cdmp [ -adlrsx ] filename ...\n"
				"Options:\n"
				"\t-a\tsupress symbol aux entries\n"
				"\t-d\tsupress data dumps\n"
				"\t-l\tsupress line numbers\n"
				"\t-r\tsupress relocation entries\n"
				"\t-s\tsupress symbol entries\n"
				"\t-x\tdump aux entries in hex\n");
			exit(1);
			break;
		}
	}
	exit(0);
}

/* end of cdmp.c */
