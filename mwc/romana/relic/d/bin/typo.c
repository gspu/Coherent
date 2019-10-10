/*
 * Detect typographical errors.
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>

#define	oc(c)	((c)+'`')
#define	NSBRK	512		/* Amount to sbrk at once in bytes */
#define BADSBRK ((char *)-1)	/* fail from sbrk() */
#define	NWORD	200		/* Longest word (ridiculously big) */
#define	NHASH	64		/* Number of hash buckets (power of 2) */

char	dictfile[] = "/usr/dict/dict";
char	digfile[] = "/usr/dict/digrams";
char	trifile[] = "/usr/dict/trigrams";
char	tmp[] = "/tmp/typoXXXXXX";
char	sort[] = "sort";
char	*tmpfile;
char	notrimem[] = "Out of memory for trigrams";
char	nosort[] = "Cannot locate sort";
char	word[NWORD];
FILE	*ifp;
FILE	*pfp;

char	ibuf[BUFSIZ];
char	obuf[BUFSIZ];
char	xbuf[BUFSIZ];

unsigned hashval;

int	rflag;			/* Raw (no removal of nroff things) input */
int	nflag;			/* No pre-defined statistics or exceptions */
int	sflag;			/* Write out statistics */

typedef	struct	EXCEPT	{
	unsigned e_hval;
	struct	EXCEPT	*e_next;
	char	e_word[];
}	EXCEPT;
EXCEPT	*except[NHASH];

/*
 * Table of digrams, indexed by first
 * and then second character.  [1-26]
 * represent [a-z] inclusive and 0
 * represents the beginning or ending of word.
 */
unsigned	digrams[27][27];

/*
 * Table of trigrams, indexed by first and
 * second letter and allocated on the third letter.
 * After all of the input data has been
 * sampled, the t_freq entry changes from
 * frequency to an index of peculiarity.
 */
typedef	struct	TRIGRAMS {
	struct	TRIGRAMS *t_next;
	char	t_char;
	unsigned char t_freq;
}	TRIGRAMS;
TRIGRAMS	*trigrams[27][27];

/*
 * Table of logarithms scaled by 1000.
 * Table from 0-99 inclusive.
 */
int	log1[] = {
	0000, 0693, 1099, 1386, 1609, 1792, 1946, 2080, 2197,	  /* 1-9 */
	2303, 2398, 2485, 2565, 2639, 2708, 2772, 2833, 2890,	  /* 10-18 */
	2944, 2996, 3045, 3091, 3136, 3178, 3219, 3258, 3296,	  /* 19-27 */
	3332, 3367, 3401, 3434, 3466, 3497, 3526, 3555, 3584,	  /* 28-36 */
	3611, 3638, 3664, 3689, 3714, 3738, 3761, 3784, 3807,	  /* 37-45 */
	3829, 3850, 3871, 3892, 3912, 3932, 3951, 3970, 3989,	  /* 46-54 */
	4007, 4025, 4043, 4060, 4078, 4094, 4111, 4127, 4143,	  /* 55-63 */
	4159, 4174, 4190, 4205, 4220, 4234, 4249, 4263, 4277,	  /* 64-72 */
	4291, 4304, 4318, 4331, 4344, 4357, 4369, 4382, 4394,	  /* 73-81 */
	4407, 4419, 4431, 4443, 4454, 4466, 4477, 4489, 4500,	  /* 82-90 */
	4511, 4521, 4533, 4543, 4554, 4564, 4575, 4585, 4595,	  /* 91-99 */
};

/*
 * Table of logarithms for each multiple
 * of ten from 100 - 990 inclusive
 * (scaled as above).
 */
int	log2[] = {
	4605, 4701, 4788, 4868, 4942, 5011, 4075, 5136, 5193,	/* 100-180 */
	5247, 5298, 5347, 5394, 5438, 5481, 5522, 5561, 5598,	/* 190-270 */
	5635, 5670, 5704, 5737, 5768, 5799, 5829, 5858, 5886,	/* 280-360 */
	5914, 5940, 5966, 5992, 6016, 6040, 6064, 6087, 6109,	/* 370-450 */
	6131, 6153, 6174, 6194, 6215, 6234, 6254, 6273, 6292,	/* 460-540 */
	6310, 6328, 6346, 6663, 6380, 6397, 6414, 6430, 6446,	/* 550-630 */
	6462, 6477, 6492, 6507, 6522, 6537, 6551, 6565, 6579,	/* 640-720 */
	6593, 6607, 6620, 6633, 6646, 6659, 6672, 6684, 6697,	/* 730-810 */
	6709, 6721, 6733, 6745, 6757, 6769, 6780, 6791, 6802,	/* 820-900 */
	6813, 6824, 6835, 6846, 6857, 6867, 6877, 6888, 6898,	/* 910-990 */
};

/*
 * Table of logarithms of multiples
 * of 100 from 1000-9900 inclusive
 * (scaled as before).
 */
int	log3[] = {
	6908, 7003, 7090, 7170, 7244, 7313, 7378, 7438, 7496,	/* 1000-1800 */
	7550, 7601, 7650, 7696, 7741, 7783, 7824, 7863, 7901,	/* 1900-2700 */
	7937, 7973, 8006, 8039, 8071, 8102, 8132, 8161, 8189,	/* 2800-3600 */
	8216, 8243, 8269, 8294, 8319, 8343, 8366, 8389, 8412,	/* 3700-4500 */
	8434, 8455, 8476, 8497, 8517, 8537, 8556, 8576, 8594,	/* 4600-5400 */
	8613, 8631, 8648, 8666, 8683, 8700, 8716, 8732, 8748,	/* 5500-6300 */
	8764, 8780, 8795, 8810, 8825, 8839, 8854, 8868, 8882,	/* 6400-7200 */
	8896, 8909, 8923, 8936, 8949, 8962, 8975, 8987, 9000,	/* 7300-8100 */
	9012, 9024, 9036, 9048, 9060, 9071, 9083, 9094, 9105,	/* 8200-9000 */
	9116, 9127, 9138, 9149, 9159, 9170, 9180, 9190, 9200,	/* 9100-9900 */
};

char	*malloc();
char	*mktemp();
char	*sbrk();
char	*getword();
EXCEPT	*lookup();
FILE	*pinit();

main(argc, argv)
char *argv[];
{
	register char *ap;
	register int i;
	FILE *fp;

	/*
	 * Because we have our own allocator.
	 */
	setbuf(stdout, obuf);
	setbuf(stderr, NULL);
	while (argc>1 && *argv[1]=='-') {
		for (ap = &argv[1][1]; *ap != '\0'; ap++)
			switch (*ap) {
			case 'n':
				nflag = 1;
				break;

			case 'r':
				rflag = 1;
				break;

			case 's':
				sflag = 1;
				break;

			default:
				usage();
			}
		argc--;
		argv++;
	}
	if (sflag == 0) {
		readdicts();
		pfp = pinit();
	}
	if (argc > 1)
		for (i=1; i<argc; i++) {
			if ((fp = fopen(argv[i], "r")) == NULL)
				tyerr("Cannot open input `%s'", argv[i]);
			typo(fp);
		}
	else
		typo(stdin);
	if (!sflag) {
		pterm(pfp);
		precompute();
		/*
		 * Re-read the sorted word
		 * list.
		 */
		reread();
	} else
		outstats();
	rmexit(0);
}

/*
 * Called for each input word
 * to set up file pointer, read
 * of individual words and enter
 * frequency statistics into the
 * tables.
 */
typo(fp)
register FILE *fp;
{
	ifp = fp;
	setbuf(fp, ibuf);
	while (getword() != NULL) {
		stats(word);
		if (!sflag && lookup(word)==NULL)
			fprintf(pfp, "%s\n", word);
	}
	if (fp != stdin)
		fclose(fp);
}

/*
 * Initialise the trigram and digram
 * tables if the user requests help.
 * Read in the exception dictionary.
 * Put each word in a hash table.
 */
readdicts()
{
	FILE *fp;

	if (nflag)
		return;
	if ((fp = fopen(dictfile, "r")) != NULL) {
		register int l;
		register char *cp;
		register EXCEPT *ep;

		setbuf(fp, xbuf);
		while (fgets(cp = word, sizeof word, fp) != NULL) {
			l = strlen(cp);
			cp[l-1] = '\0';
			if ((ep = (EXCEPT *)malloc(l + sizeof *ep)) == NULL)
				tyerr("Out of memory for dictionary");
			hashval = 0;
			while (*cp)
				hashval += *cp++;
			l = hashval%NHASH;
			ep->e_next = except[l];
			except[l] = ep;
			ep->e_hval = hashval;
			strcpy(ep->e_word, word);
		}
		fclose(fp);
	}
	if ((fp = fopen(digfile, "r")) != NULL) {
		register int t1, t2;

		setbuf(fp, xbuf);
		while (fgets(word, NWORD, fp) != NULL) {
			t1 = cton(word[0]);
			t2 = cton(word[1]);
			digrams[t1][t2] = atoi(&word[2]);
		}
		fclose(fp);
	}
	if ((fp = fopen(trifile, "r")) != NULL) {
		register TRIGRAMS *tp;
		register int t1, t2, t3;

		setbuf(fp, xbuf);
		while (fgets(word, NWORD, fp) != NULL) {
			t1 = cton(word[0]);
			t2 = cton(word[1]);
			t3 = cton(word[2]);
			for (tp = trigrams[t1][t2]; tp!=NULL; tp = tp->t_next) {
				if (tp->t_char == t3) {
					tp->t_freq += atoi(&word[3]);
					break;
				}
			}
			if (tp == NULL) {
				if ((tp = (TRIGRAMS *)malloc(sizeof *tp))==NULL)
					tyerr(notrimem);
				tp->t_freq = atoi(&word[3]);
				tp->t_char = t3;
				tp->t_next = trigrams[t1][t2];
				trigrams[t1][t2] = tp;
			}
		}
		fclose(fp);
	}
}

/*
 * Get a character from a digram or
 * trigram file and check it.
 * Convert character to index number
 * for tables.
 */
cton(c)
register unsigned c;
{
	if ((c -= '`') >= 27)
		tyerr("Invalid digram/trigram file format");
	return (c);
}

/*
 * Get the next word from the input.
 */
char *
getword()
{
	register char *cp;
	register int c;

	while (!isalpha(c = tgetc()))
		if (c == EOF)
			return (NULL);
	cp = word;
	for (;;) {
		if (isupper(c))
			c = tolower(c);
		*cp++ = c;
		if (!isalpha(c = tgetc()))
			break;
	}
	*cp = '\0';
	return (word);
}

/*
 * Get a character.  This also checks
 * for roff stuff (if -r is not specified)
 * and hyphenated words.
 */
tgetc()
{
	static int nlflag = 1;
	register int c;

	while (nlflag) {
		if ((c = getc(ifp)) == '.')
			while ((c = getc(ifp))!='\n' && c!=EOF)
				;
		else {
			ungetc(c, ifp);
			nlflag = 0;
			break;
		}
	}
again:
	if ((c = getc(ifp)) == '\n')
		nlflag = 1;
	else if (c == '-') {
		if ((c = getc(ifp)) == '\n')
			goto again;
	} else if (c == '\'')
		goto again;
	else if (c == '\\') {	/* Fonts and sizes */
		switch (c = getc(ifp)) {
		case 'f':
			c = getc(ifp);
			goto again;

		case 's':
			while ((c = getc(ifp))>='0' && c<='9')
				;
		}
	}
	return (c);
}

/*
 * Lookup an input word
 * in the exception list.
 */
EXCEPT *
lookup(wp)
char *wp;
{
	register char *cp = wp;
	register EXCEPT *ep;

	hashval = 0;
	while (*cp != '\0')
		hashval += *cp++;
	for (ep = except[hashval%NHASH]; ep != NULL; ep = ep->e_next)
		if (ep->e_hval==hashval && strcmp(ep->e_word, word)==0)
			return (ep);
	return (NULL);
}

/*
 * Compute the trigram and digram statistics
 * on this word.
 */
stats(wp)
register char *wp;
{
	register int t1, t2, t3;
	register TRIGRAMS *tp;

	t1 = 0;
	while (*wp != '\0') {
		t2 = *wp++;
		t3 = *wp;
		if (t2>='a' && t2<='z')
			t2 -= 'a'-1;
		if (t3>='a' && t3<='z')
			t3 -= 'a'-1;
		digrams[t1][t2]++;
		for (tp = trigrams[t1][t2]; tp != NULL; tp = tp->t_next)
			if (tp->t_char == t3) {
				tp->t_freq++;
				break;
			}
		if (tp == NULL) {
			if ((tp = (TRIGRAMS *)malloc(sizeof *tp)) != NULL) {
				tp->t_freq = 1;
				tp->t_char = t3;
				tp->t_next = trigrams[t1][t2];
				trigrams[t1][t2] = tp;
			}
		}
		t1 = t2;
	}
	digrams[t2][t3]++;
}

/*
 * Output the digram and trigram statistics
 * onto file `digram' and `trigram'.
 */
outstats()
{
	char x1buf[BUFSIZ];
	register TRIGRAMS *tp;
	register int t1, t2;
	register int freq;
	FILE *dfp, *tfp;

	if ((dfp = fopen("digrams", "w")) == NULL)
		tyerr("Cannot create digrams file");
	if ((tfp = fopen("trigrams", "w")) == NULL)
		tyerr("Cannot create trigrams file");
	setbuf(dfp, xbuf);
	setbuf(tfp, x1buf);
	for (t1 = 0; t1 < 27; t1++)
		for (t2 = 0; t2 < 27; t2++) {
			if ((freq = digrams[t1][t2]) != 0)
				fprintf(dfp, "%c%c%d\n", oc(t1), oc(t2), freq);
			for (tp = trigrams[t1][t2]; tp!=NULL; tp = tp->t_next)
				fprintf(tfp, "%c%c%c%d\n", oc(t1), oc(t2),
				    oc(tp->t_char), tp->t_freq);
		}
	fclose(dfp);
	fclose(tfp);
}

/*
 * Compute index of peculiarity on a
 * word.  This is the RMS mean of the
 * index for each trigram (say XYZ).
 * This index is 1/2(log (xy) + log(yz)) - log(xyz)
 * If there is only one trigram, the RMS
 * mean is taken to be the one sample.
 */
pindex(wp)
register char *wp;
{
	register int t1, t2, t3;
	register TRIGRAMS *tp;
	register long sumsq;
	register long sum;
	register unsigned count;
	register int ntri;

	ntri = 0;
	t1 = 0;
	sum = sumsq = 0;
	while (*wp != '\0') {
		t2 = *wp++;
		t3 = *wp;
		if (t2>='a' && t2<='z')
			t2 -= 'a'-1;
		if (t3>='a' && t3<='z')
			t3 -= 'a'-1;
		for (tp = trigrams[t1][t2]; tp != NULL; tp = tp->t_next)
			if (tp->t_char == t3) {
				count = tp->t_freq;
				break;
			}
		if (tp == NULL)
			tyerr("Missing trigram");
		sum += count;
		sumsq += (long)count*count;
		ntri++;
		t1 = t2;
	}
	if (ntri > 1) {
		sum *= sum;
		sum /= ntri;
		sum = sumsq-sum;
	}
	count = (qsqrt((int)sum) + 5) / 10;
	return (count);
}

/*
 * Pre-compute the trigram statistics.
 * This essentially transforms the
 * trigram tables from a table
 * of frequencies to a table of
 * indices of peculiarity for that
 * particular trigram.
 */
precompute()
{
	register int i, j;
	register TRIGRAMS *tp;
	register int logij;

	for (i=0; i<27; i++)
		for (j=0; j<27; j++) {
			logij = qlog(digrams[i][j]-1);
			for (tp = trigrams[i][j]; tp != NULL; tp = tp->t_next) {
				tp->t_freq = ((logij
				    + qlog(digrams[j][tp->t_char]-1))/2
				    - qlog(tp->t_freq-1)) / 100;
			}
		}
}

/*
 * Evaluate a natural logarithm
 * quickly by table lookup.
 * The resulting logarithm
 * is multiplied by 1000.
 * By definition, the log of 0 (or less) is
 * -10.
 */
qlog(n)
register unsigned n;
{
	if (n <= 0)
		return (-10*1000);
	if (n < 100)
		return (log1[n-1]);
	if (n < 1000)
		return (log2[(n-95)/10]);
	if (n < 10000)
		return (log3[(n-950)/100]);
	return (10*1000);
}

/*
 * Quick version of sqare root.
 * Uses Newton's method.
 */
qsqrt(x)
register unsigned x;
{
	register int maxterm = 50;
	register unsigned old, new;

	if ((old = x/10) == 0) {
		old = 1;
		if (x == 0)
			return (0);
	}
	do {
		new = (x/old+old)/2;
		if (old == new)
			break;
		old = new;
	} while (--maxterm);
	return (new);
}

/*
 * Build pipe to a sort routine.
 */
FILE *
pinit()
{
	int pfd[2];
	register int pid;
	register int fd;
	FILE *fp;

	tmpfile = mktemp(tmp);
	if (pipe(pfd) < 0)
		tyerr("Cannot pipe to sort");
	if ((pid = fork()) < 0)
		tyerr("Cannot fork for sort");
	if (pid) {
		close(pfd[0]);
		if ((fp = fdopen(pfd[1], "w")) != NULL)
			setbuf(fp, xbuf);
		return (fp);
	} else {
		dup2(pfd[0], 0);
		close(pfd[0]);
		close(pfd[1]);
		if ((fd = creat(tmp, 0666)) < 0)
			tyerr("Cannot create temporary file");
		dup2(fd, 1);
		close(fd);
		execlp(sort, sort, "-u", NULL);
		tyerr(nosort);
	}
}

/*
 * Close off the pipe and wait
 * for the sort command to
 * complete.
 */
pterm(fp)
FILE *fp;
{
	int status;

	fclose(fp);
	while (wait(&status) >= 0)
		if (status)
			tyerr("Sort failed");
}

/*
 * Re-read the sorted word-list
 * from the temp file.  Because
 * sort -u is not implemented,
 * we have to guarantee uniqueness
 * ourselves.
 * The output is piped into a
 * `sort -nr' command.
 */
reread()
{
	FILE *fp;
	register int ind;
	int pid, status;
	int pfd[2];

	if ((fp = fopen(tmpfile, "r")) == NULL)
		tyerr("Cannot re-open temp-file");
	setbuf(fp, xbuf);
	if (pipe(pfd) < 0)
		tyerr("Cannot create pipe to sort output");
	if ((pid = fork()) < 0)
		fprintf(stderr, "Cannot fork -- output is not sorted\n");
	else if (pid) {
		dup2(pfd[1], 1);
		close(pfd[0]);
		close(pfd[1]);
	} else {
		dup2(pfd[0], 0);
		close(pfd[0]);
		close(pfd[1]);
		execlp(sort, sort, "-nr", NULL);
		tyerr(nosort);
	}
	while (fgets(word, NWORD, fp) != NULL) {
		word[strlen(word)-1] = '\0';
		ind = pindex(word);
		printf("%2d  %s\n", ind, word);
	}
	fclose(fp);
	fclose(stdout);
	while (wait(&status) >= 0)
		;
}

/*
 * A simple malloc for which there
 * is no free.  It just uses sbrk and
 * returns a value.
 * `size' is in bytes.
 */
char *
malloc(size)
register unsigned size;
{
	static int *rp, *ep;
	register int *cp;

	if ((size = (size+sizeof(int)-1)/sizeof(int)) == 0)
		return (NULL);
	if (rp==NULL || rp+size>=ep) {
		if ((ep = (int *)sbrk(NSBRK)) == BADSBRK)
			return (NULL);
		if (rp == NULL)
			rp = ep;
		ep += NSBRK/sizeof(int);
	}
	cp = rp;
	rp += size;
	return (cp);
}

/*
 * Dummy free.
 */
free(p)
char *p;
{
	tyerr("Free not allowed");
}

/*
 * Dummy realloc
 */
char *
realloc(p)
char *p;
{
	tyerr("realloc not allowed");
}

/*
 * Exit, removing the temp-file
 * if it is found.
 */
rmexit(s)
{
	if (tmpfile != NULL)
		unlink(tmpfile);
	exit(s);
}

/*
 * Error and usage messages
 */
/* VARARGS */
tyerr(x)
{
	fprintf(stderr, "typo: %r", &x);
	putc('\n', stderr);
	rmexit(1);
}

usage()
{
	fprintf(stderr, "Usage: typo [-nrs] file\n");
	rmexit(1);
}
