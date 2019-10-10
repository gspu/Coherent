/*
 * man.c
 * 10/31/90
 * Usage: man [ -w ] [ topic ... ]
 * Quick and dirty man hack.
 * Read manual index, print manual sections to screen via $PAGER.
 */

#include <stdio.h>
#include <string.h>

#define	BBBIGBUF	32000			/* index buffer size	*/
#define	DEFPAGER	"exec /bin/scat -1"	/* default $PAGER	*/
#define	MANHELP		"/usr/man/man.help"	/* manual help file	*/
#define	MANINX		"/usr/man/man.index"	/* manual index	file	*/
#define	NBUF		80			/* buf[] buffer size	*/
#define	NCMD		512			/* cmd[] buffer size	*/
#define	VERSION		"1.3"			/* version id		*/

extern	char	*getenv();

/* Globals. */
char	buf[NBUF];
char	cmd[NCMD];
char	*indexp;
char	manindex[BBBIGBUF];
int	wflag;

/* Forward. */
void	cmdcat();
void	fatal();
char	*lookup();
void	nonfatal();

main(argc, argv) int argc; char *argv[];
{
	int	i, fd, status, found, nfound;
	char	*cp;

	status = 0;

	/* Look for environmental $PAGER. */
	if ((cp = getenv("PAGER")) == NULL)
		cp = DEFPAGER;
	strcpy(cmd, cp);

	/* Parse option args. */		
	if (argc > 1 && strcmp(argv[1], "-w") == 0) {
		++wflag;
		--argc;
		++argv;
	}
	if (argc > 1 && strcmp(argv[1], "-V") == 0) {
		fprintf(stderr, "man: V%s\n", VERSION);
		--argc;
		++argv;
	}

	/* If no args, print manual help information. */
	if (argc == 1) {
		if (wflag)
			printf("%s\n", MANHELP);
		else {
			cmdcat(" ");
			cmdcat(MANHELP);
			system(cmd);
		}
		exit(status);
	}

	/* Args given.  First read the index. */
	if ((fd = open(MANINX, 0)) == -1)
		fatal("cannot open manual index %s, online manual probably not installed", MANINX);
	else if ((i = read(fd, manindex, BBBIGBUF)) == -1)
		fatal("cannot read index buffer");
	else if (i >= BBBIGBUF)
		fatal("index file too large");
	close(fd);
#if	DEBUG
	nonfatal("%s=%d bytes", MANHELP, i);
#endif]

	/* Look up each arg. */
	nfound = 0;
	for (i = 1; i < argc; i++) {
#if	DEBUG
		nonfatal("argv[%d]=%s", i, argv[i]);
#endif
		indexp = manindex;
		found = 0;

		/* Look up arg in index.  May find multiple hits. */
		while ((cp = lookup(argv[i])) != NULL) {
			found++;
			nfound++;
#if	DEBUG
			nonfatal("index line=%s", cp);
#endif
			if (wflag)
				printf("/usr/man/%s\n", cp);
			else {
				cmdcat(" ");
				cmdcat("/usr/man/");
				cmdcat(cp);
#if	DEBUG
				nonfatal("command=%s", cmd);
#endif
			}
		}
		if (found == 0) {
			nonfatal("%s not found in manual", argv[i]);
			status = 1;
		}
	}
	if (!wflag && nfound)
		system(cmd);
	exit(status);
}

/*
 * Concatenate given string to cmd buffer.
 * Complain if too long.
 */
void
cmdcat(s) register char *s;
{
	register int len;

	len = strlen(cmd);
	if (len + strlen(s) + 1 >= NCMD)
{printf("cmd=%s\ns=%s\nlen=%d\n", cmd, s, len);
		fatal("command buffer overflow");
}
	strcpy(&cmd[len], s);
}

/* Cry and die. */
void
fatal(s) char *s;
{
	fprintf(stderr, "man: %r\n", &s);
	exit(1);
}

/*
 * Look up string s in helpfile index starting at indexp.
 * Return name of file on match, else NULL.
 */
char *
lookup(s) char *s;
{
	register char *namep, *next;

	while ((next = strchr(indexp, '\n')) != NULL) {
		if (next - indexp + 1 >= NBUF)
			fatal("index buffer overflow");
		strncpy(buf, indexp, next - indexp);
		buf[next-indexp] = '\0';	/* index line to buf */
#if	DEBUG
		nonfatal("[%s]", buf);
#endif
		namep = strchr(buf, ' ');
		indexp = next + 1;		/* bump to next index line */
		if (namep == NULL)
			continue;
		else
			*namep++ = '\0';	/* NUL-terminate index entry */
#if	DEBUG
		nonfatal("buf=%s namep=%s", buf, namep);
#endif
		if (strcmp(namep, s) == 0)
			return buf;		/* gotcha */
	}
	return NULL;				/* no match */
}

void
nonfatal(s) char *s;
{
	fprintf(stderr, "man: %r\n", &s);
}
	
/* end of man.c */
