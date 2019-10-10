/*
 * Strip symbols, lines, and relocation from executable.
 */
#include <misc.h>
#include <errno.h>
#include <coff.h>
#include <setjmp.h>
#include <sys/stat.h>

extern char *alloca();

static jmp_buf env;	/* setjmp longjmp buffer */
static char *filen;	/* current file in process */
static int errCt;

/*
 * Put message and longjmp to next file.
 */
void
fatal(s)
char *s;
{
	fprintf(stderr, "strip: %s %r.\n", filen, &s);
	errCt++;
	longjmp(env, 1);
}

/*
 * Strip a file
 */
strip()
{
	register SCNHDR *sh;
	register FILEHDR *fh;	
	int fd, rv;
	long i, top, hi;
	struct stat sb;

	/* inhale input file */
	if (-1 == (fd = open(filen, 0)))
		fatal("Can't open for read");

	if (-1 == fstat(fd, &sb))
		fatal("Can't find file attributes");

	fh = alloca(sb.st_size);	/* get space to inhale file */

	rv = read(fd, fh, sb.st_size);
	close(fd);

	if (-1 == rv)
		fatal("Error in read");

	if ((fh->f_magic != C_386_MAGIC) ||
	    !fh->f_opthdr ||
	    !(fh->f_flags & F_EXEC))
		fatal("Not COFF executable");

	fh->f_symptr = fh->f_nsyms = 0;
	fh->f_flags |= F_RELFLG | F_LNNO | F_LSYMS;

	/* pass segments and find top address */
	sh = ((char *)fh) + fh->f_opthdr + sizeof(*fh);
	top = (long)(sh + fh->f_nscns);
	for (top = i = 0; i < fh->f_nscns; i++, sh++) {
		/* find top of sector data */
		if (sh->s_scnptr && (sh->s_flags != STYP_BSS)) {
			hi = sh->s_size + sh->s_scnptr;
			if (top < hi)
				top = hi;
		}
		sh->s_relptr = sh->s_lnnoptr = sh->s_nreloc = sh->s_nlnno = 0;
	}

	if (top > sb.st_size)
		fatal("Corrupt file");

	if (top < sb.st_size) {	/* file not already stripped */
		/* exhale stripped file */
		if (-1 == (fd = creat(filen, sb.st_mode)))
			fatal("Can't create new copy of file");

		rv = write(fd, fh, top);
		close(fd);

		if (-1 == rv)
			fatal("Error in write");
	}
}

main(argc, argv)
char *argv[];
{
	register int i;

	for (i = 1; i < argc; i++) {
		filen = argv[i];
		if (!setjmp(env))
			strip();
	}

	if(!errCt)
		return (0);

	fprintf(stderr, "%d error(s) flagged\n", errCt);
	return (1);
}
