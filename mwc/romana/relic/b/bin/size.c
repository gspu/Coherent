#include <misc.h>
#include <canon.h>
#include <n.out.h>
#include <coff.h>
#include <setjmp.h>
#include <errno.h>
#include <dir.h>
#include <arcoff.h>
#include <ar.h>

#define xread(x, m) if(1 != fread(&x, sizeof(x), 1, ifp)) fatal(rmsg, m);

static char rmsg[] = "Error reading %s";
static FILE *ifp;
static jmp_buf env;
static char namesw;
static char cfile[100];

static int errors = 0;		/* keep track of errors from fatal() */

/*
 * Put message and longjmp to next file.
 */
void
fatal(s)
char *s;
{
	fprintf(stderr, "size: %s: %r\n", cfile, &s);
	++errors;
	longjmp(env, 1);
}

/*
 * Print members of archive.
 */
archive(filen, at)
char *filen;
long at;
{
	long arhend;
	struct ar_hdr coff_arh;
	struct old_ar_hdr arh;
	char *strchr();
	char *p;

	namesw = 0;
	for (arhend = at + SARMAG; ; ) {
		fseek(ifp, arhend, 0);
		if (1 != fread(&coff_arh, sizeof(coff_arh), 1, ifp))
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
			sprintf(cfile, "%s(%s): ", filen, arh.ar_name);
			printf("%s", cfile);
			printSize(arh.ar_name, arhend);
		}
		arhend += arh.ar_size;
		if (arhend & 1)
			arhend++;		
	}
	namesw = 1;
}

/*
 * Process old n.out files.
 */
old_nout(filen, at)
char *filen;
long at;
{
	struct ldheader ldh;
	register i;
	long total;

	if (namesw)
		printf("%s: ", filen);

	fseek(ifp, at, 0);
	xread(ldh, "n.out header");
	for (total = i = 0; i < L_DEBUG; i++) {
		cansize(ldh.l_ssize[i]);
		if (i)
			putchar('+');
		printf("%ld", ldh.l_ssize[i]);
		total += ldh.l_ssize[i];
	}
	printf(" %ld (%lx)\n", total, total);
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
		fseek(ifp, arhend, 0);
		if (1 != fread(&arh, sizeof(arh), 1, ifp))
			break;
		arhend += sizeof(arh);
		arh.ar_date = 0;	/* terminate name */
		if (strcmp(arh.ar_name, HDRNAME)) {
			sprintf(cfile, "%s(%s): ", filen, arh.ar_name);
			printf("%s", cfile);
			printSize(arh.ar_name, arhend);
		}
		cansize(arh.ar_size);
		arhend += arh.ar_size;
	}
}

/*
 * Print size of coff file or call friends.
 */
printSize(filen, at)
char *filen;
long at;
{
	FILEHDR fh;
	long total;
	int i;

	xread(fh, "file header");
	if (fh.f_magic != C_386_MAGIC) {
		if (!memcmp(ARMAG, &fh, SARMAG))
			return (archive(filen, at));

		/* Check old file types */
		canshort(fh.f_magic);
		if (fh.f_magic == OLD_ARMAG)
			return (old_ar(filen, at));
		if (fh.f_magic == L_MAGIC)
			return (old_nout(filen, at));
		fatal("Unknown filetype 0x%x", fh.f_magic);
	}

	if (namesw)
		printf("%s: ", filen);

	fseek(ifp, at + sizeof(FILEHDR) + fh.f_opthdr, 0);
	for (total = i = 0; i < fh.f_nscns; i++) {
		SCNHDR sh;

		xread(sh, "section");
		if (i)
			putchar('+');
		printf("%ld", sh.s_size);
		total += sh.s_size;
	}
	printf(" %ld (%lx)\n", total, total);
}

main(argc, argv)
char *argv[];
{
	char *filen;
	int i;

	for (i = 1; i < argc; i++) {
		/* fatal() longjmp()s here for next file */
		if (setjmp(env)) {
			if (NULL != ifp) {
				fclose(ifp);
				ifp = NULL;
			}
			continue;
		}
		strcpy(cfile, filen = argv[i]);
		ifp = xopen(filen, "rb");

		namesw = argc > 2;

		printSize(filen, 0L);

		fclose(ifp);
		ifp = NULL;
	}
	exit(errors);
}

