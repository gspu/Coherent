/*
 * An editor.
 * Main, command parser and general routines.
 */
#include <stdio.h>
#if COHERENT || GEMDOS
#include <signal.h>
#endif
#include <ctype.h>
#include "ed.h"

main(argc, argv)
char *argv[];
{
#if COHERENT
	extern int sighang();
#endif
#if COHERENT || GEMDOS
	extern int sigintr();
#endif
#if COHERENT || MSDOS || GEMDOS
	register char *cp;
#endif
	initialise();
#if COHERENT || MSDOS || GEMDOS
	if ((cp=getenv("ED")) != NULL)
		setoptf(cp);
#endif
#if COHERENT
	signal(SIGHUP, sighang);
#endif
#if COHERENT || GEMDOS
	signal(SIGINT, sigintr);
	signal(SIGQUIT, sigintr);
#endif
	setup(argc, argv);
	for (;;) {
		if (intflag) {
/*
			finit(stdout);
			clearerr(stdin);
			clearerr(stdout);
			clearerr(stderr);
*/
			savechr = '\0';
			intflag = 0;
			derror("Interrupt");
		}
		command();
	}
}

/*
 * Initialisation.
 */
initialise()
{
	register int i;
	register char *bp;
#if COHERENT || MSDOS || GEMDOS
	extern char *tempnam();
#endif

	if ((bp=malloc(BUFSIZ)) != NULL)
		setbuf(stdout, bp);
#if RSX
	if ((tmp=fopen("edtmp", "r+")) == NULL) {
#endif
#if COHERENT || MSDOS || GEMDOS
	tfn = tempnam(NULL, "ed");
	if ((tmp=fopen(tfn, "w")) == NULL) {
		terror("Cannot create temp file");
		exit(1);
	}
	if ((tmp=freopen(tfn, "r+", tmp)) == NULL) {
#endif
		terror("Cannot open temp file");
		unlink(tfn);
		exit(1);
	}
#if COHERENT
	chmod(tfn, 0);
#endif

#if RSX
	fmkdl(tmp);
	qio(001400, stdin->v_lun, 1, NULL, NULL);
	wtse(1);
#endif
	mflag = 0;
	oflag = 0;
	pflag = 0;
	sflag = 0;
	tflag = 0;
	vflag = 0;
	cflag = 1;
	intflag = 0;
	tmpseek = CLSIZE;
	rcurbno = -1;
	wcurbno = -1;
	lnsize = LNSIZE;
	if ((line=(LINE *)calloc(lnsize, sizeof(LINE))) == NULL) {
		prints(stderr, "Cannot allocate line number table\n");
		exit(1);
	}
	savechr = '\0';
	lastchr = '\n';
	gcp = NULL;
	codebuf[0] = CSNUL;
	file[0] = '\0';
	for (i=0; i<MKSIZE; i++)
		marklin[i] = 0;
	dotadd = 0;
	doladd = 0;
	saved = 1;
	subseek = 0;
	keyp = NULL;
}

/*
 * Process command line arguments.
 */
setup(argc, argv)
char *argv[];
{
	register char *cp;
	register int i;
	register int f;

	f = 0;
	for (i=1; i<argc; i++) {
		cp = argv[i];
		switch (*cp++) {
		case '-':
			switch (*cp++) {
			case '\0':
				cflag = 0;
				continue;
#if COHERENT
			case 't':
				tflag = 1;
				signal(SIGINT, SIG_IGN);
				signal(SIGQUIT, SIG_IGN);
				continue;
#endif
			case 'u':
				setbuf(stdout, NULL);
				continue;
			case 'x':
				setkey();
				continue;
			default:
				usage();
			}
		case '+':
			setoptf(argv[i]);
			continue;
		default:
			if (f++ != 0)
				usage();
			if (strlen(argv[i]) > FNSIZE)
				derror("File name too long");
			else {
				strcpy(file, argv[i]);
				edit();
			}
		}
	}
}

/*
 * Print out a usage message.
 */
usage()
{
	prints(stderr, "Usage: ed [-[x]] [+cmopsv] [file]\n");
	exit(1);
}

/*
 * Set a flag indicating that interrupt has been hit.
 * Used for SIGQUIT and SIGINT.
 */
#if COHERENT || GEMDOS
sigintr(s)
{
	intflag++;
	signal(s, sigintr);
}
#endif

/*
 * If we get a hangup signal, write the file onto the file
 * `ed.hup' and leave.
 */
#if COHERENT
sighang()
{
	if (doladd != 0)
		wfile(1, doladd, "ed.hup", "w", 1);
	leave();
}
#endif

/*
 * Leave the editor.
 */
leave()
{
	fclose(tmp);
	unlink(tfn);
	exit(0);
}

/*
 * Process a command.
 */
command()
{
	extern int sigintr();
	int c1, n, a3;
	char name[FNSIZE+1];
	register int a1, a2, c;

	for (;;) {
		addspec = 0;
		a1 = a2 = getaddr();
		if (intflag)
			return (0);
		if (adderrr)
			goto err;
		c = getx();
		if (addspec != 0) {
			while (c==',' || c==';') {
				a1 = a2;
				if (c == ';')
					dotadd = a1;
				a2 = getaddr();
				if (adderrr)
					goto err;
				if (addspec == 0) {
					derror("Missing address");
					goto err;
				}
				c = getx();
			}
		}
		if (c == '*') {
			if (addspec++ == 0) {
				a1 = 1;
				addspec++;
			}
			a2 = doladd;
			c = getx();
		}
		switch (c) {
		case EOF:
			if (intflag == 0) {
				if (saved != 0)
					leave();
				saved = 1;
				derror("File not saved");
			}
			return;
		case '\n':
			if (addques) {
				if (errstr) {
					prints(stdout, errstr);
					printc(stdout, '\n');
				}
				return (0);
			}
			if (addpage) {
				a1 = a2;
				a2 = a1 + PGSIZE;
				if (a2 > doladd)
					a2 = doladd;
				if (print(a1, a2) == 0)
					return (0);
				return (1);
			}
			if (addspec == 0)
				a2 = dotadd+1;
			if (print(a2, a2) == 0)
				return (0);
			return (1);
		case '!':
			if (rest() == 0)
				goto err;
			n = system(tempbuf);
			if (n < 0) {
				derror("Call failed");
				return (0);
			} else {
				prints(stdout, "!\n");
				return (1);
			}
		case '=':
			if (verify(0) == 0)
				goto err;
			if (addspec == 0)
				a2 = doladd;
			printd(stdout, a2);
			printc(stdout, '\n');
			break;
		case 'a':
			if (verify(1) == 0)
				goto err;
			getx();
			if (append(a2, readtty) == 0)
				return (0);
			ungetx('\n');
			break;
		case 'c':
			if (verify(1) == 0)
				goto err;
			getx();
			if (delete(a1, a2) == 0)
				return (0);
			if (append(a1-1, readtty) == 0)
				return (0);
			ungetx('\n');
			break;
		case 'd':
			if (verify(0) == 0)
				goto err;
			if (delete(a1, a2) == 0)
				goto err;
			break;
		case 'e':
			n = saved;
			if (getfile(n?file:name) == 0)
				return (0);
			if (n == 0) {
				saved = 1;
				derror("File not saved");
				return (0);
			}
			return (edit());
		case 'E':
			if (getfile(file) == 0)
				return (0);
			return (edit());
		case 'f':
			if ((c=getx()) == ' ') {
				ungetx(' ');
				getfile(file);
				c = '\n';
			}
			if (c != '\n') {
				derror("Bad command");
				goto err;
			}
			if (file[0] != '\0') {
				prints(stdout, file);
				printc(stdout, '\n');
			}
			return (1);
		case 'g':
			if (addspec == 0) {
				a1 = 1;
				a2 = doladd;
			}
			if (global(a1, a2, 0) == 0)
				goto err;
			if (verify(1) == 0)
				goto err;
			break;
		case 'i':
			if (verify(1) == 0)
				goto err;
			getx();
			if (doladd==0 && a2==0)
				a2 = 1;
			if (append(a2-1, readtty) == 0)
				return (0);
			ungetx('\n');
			break;
		case 'j':
			if (addspec < 2)
				a2 = a1+1;
			if (verify(0) == 0)
				goto err;
			if (join(a1, a2) == 0)
				goto err;
			break;
		case 'k':
			if ((c=getx()) == '\n') {
				derror("No mark name specified");
				return (0);
			}
			if (verify(0) == 0)
				goto err;
			if (a2<1 || a2>doladd) {
				derror("Illegal address range");
				goto err;
			}
			if (!isascii(c) || !islower(c)) {
				derror("Bad mark name");
				goto err;
			}
			marklin[c-'a'] = line[a2]|1;
			break;
		case 'l':
			if (verify(0) == 0)
				goto err;
			if (list(a1, a2) == 0)
				goto err;
			break;
		case 'm':
			a3 = getaddr();
			if (adderrr)
				goto err;
			if (verify(0) == 0)
				goto err;
			if (move(a1, a2, a3) == 0)
				goto err;
			break;
		case 'o':
			if (rest() == 0)
				goto err;
			if (tempbuf[0] == '\0')
				disoptf();
			else
				setoptf(tempbuf);
			return (1);
		case 'p':
		case 'P':
			if (verify(0) == 0)
				goto err;
			if (print(a1, a2) == 0)
				goto err;
			break;
		case 'q':
			if (verify(1) == 0)
				goto err;
			if (addspec != 0) {
				derror("Cannot specify address");
				goto err;
			}
			if (saved == 0) {
				saved = 1;
				derror("File not saved");
				goto err;
			}
			leave();
		case 'Q':
			if (verify(1) == 0)
				goto err;
			leave();
		case 'r':
			if (getfile(name) == 0)
				return (0);
			if (addspec == 0)
				a2 = doladd;
			if ((fp=xfopen(name, "r")) == NULL) {
				derror("Cannot open file");
				return (0);
			}
			n = 1;
			if (append(a2, readfil) == 0)
				n = 0;
			else if (ferror(fp)) {
				derror("Read error");
				n = 0;
			}
			fclose(fp);
			if (cflag != 0) {
				printl(stdout, !oflag ? cct : lct);
				printc(stdout, '\n');
			}
			return (n);
		case 's':
			if (subs1(a1, a2) == 0)
				goto err;
			if (verify(0) == 0)
				goto err;
			if (subs2(a1, a2) == 0)
				goto err;
			break;
		case 't':
			a3 = getaddr();
			if (adderrr)
				goto err;
			if (verify(0) == 0)
				goto err;
			if (copy(a1, a2, a3) == 0)
				goto err;
			break;
		case 'u':
			if (verify(0) == 0)
				goto err;
			if (a2<1 || a2>doladd) {
				derror("Illegal address range");
				goto err;
			}
			if ((line[a2]|1) != subseek) {
				derror("Cannot undo substitute");
				goto err;
			}
			line[a2] = suborig;
			subseek = 0;
			break;
		case 'v':
			if (addspec == 0) {
				a1 = 1;
				a2 = doladd;
			}
			if (global(a1, a2, 1) == 0)
				goto err;
			if (verify(1) == 0)
				goto err;
			break;
		case 'w':
		case 'W':
			if ((c1=getx()) != 'q')
				ungetx(c1);
			if (getfile(name) == 0)
				return (0);
			if (addspec == 0) {
				a1 = 1;
				a2 = doladd;
			}
			if (c=='W' && keyp!=NULL) {
				derror("Cannot append in encryption mode");
				goto err;
			}
			if (wfile(a1, a2, name, c=='w'?"w":"a", 0) && c1=='q')
				leave();
			return (1);
		case 'x':
			if (verify(1) == 0)
				goto err;
			if (setkey() == 0)
				goto err;
			break;
		default:
			derror("Bad command");
		err:
			while ((c=getx())!=EOF && c!='\n')
				;
			return (0);
		}
		if (intflag)
			return (0);
		switch (vcom) {
		case 'l':
			list(dotadd, dotadd);
			break;
		case 'p':
			print(dotadd, dotadd);
			break;
		}
		if ((c=getx()) == '\n')
			return (1);
		if (mflag == 0) {
			derror("Internal error");
			goto err;
		}
		ungetx(c);
	}
}

/*
 * Get an address.
 */
getaddr()
{
	LINE seek;
	int absolute, abs, sign;
	register int a, c, n;

	adderrr = 0;
	addpage = 0;
	addques = 0;
	absolute = 0;
	abs = 1;
	sign = 0;
	a = dotadd;
	for (;;) {
		switch (c=getx()) {
		case '.':
			n = dotadd;
			abs = 1;
			break;
		case '$':
			n = doladd;
			abs = 1;
			break;
		case '&':
			if (sign == 0) {
				addpage++;
				continue;
			}
			n = PGSIZE;
			abs = 0;
			break;
		case '\'':
			if (!isascii(c=getx()) || !islower(c)) {
				derror("Bad mark name");
				adderrr++;
				return (0);
			}
			seek = marklin[c-'a'];
			for (n=1; n<=doladd; n++) {
				if ((line[n]|1) == seek) {
					abs = 0;
					goto number;
				}
			}
			derror("Mark value not initialised");
			adderrr++;
			return (0);
		case '+':
			sign++;
			continue;
		case '^':
		case '-':
			--sign;
			continue;
		case '?':
			c = getx();
			ungetx(c);
			if (c == '\n') {
				addques++;
				return (0);
			}
			c = '?';
		case '/':
			if (compile(c) == 0) {
				adderrr++;
				return (0);
			}
			if (a<1 || a>doladd) {
				derror("Bad initial address");
				adderrr++;
				return (0);
			}
			n = a;
			abs = 1;
			do {
				if (intflag)
					return;
				if (c == '?') {
					if (--n < 1)
						n = doladd;
				} else {
					if (++n > doladd)
						n = 1;
				}
				if (n == a) {
					if (execute(n))
						break;
					derror("Search failed");
					adderrr++;
					return (0);
				}
			} while (execute(n) == 0);
			break;
		default:
			if (isascii(c) && isdigit(c)) {
				n = 0;
				do
					n = n*10 + c-'0';
				while (isascii(c=getx()) && isdigit(c));
				ungetx(c);
				abs = 0;
				break;
			}
			ungetx(c);
			if (sign) {
				a += sign;
				addspec++;
			}
			return (a);
		}
	number:
		if (sign == 0) {
			a = n;
			absolute = abs;
		} else {
			if ((absolute+=abs) > 1) {
				derror("Relocation address error");
				adderrr++;
				return (0);
			}
			a += sign>0 ? n : -n;
			sign = 0;
		}
		addspec++;
	}
}

/*
 * Get a character.
 */
getx()
{
	register int c;

	if (intflag != 0)
		return (EOF);
	if (savechr != '\0') {
		c = savechr;
		savechr = '\0';
		return (c);
	}
	if (gcp == NULL) {
		if (lastchr=='\n' && appflag==0) {
			if (pflag != 0) {
				printc(stderr, '*');
				fflush(stderr);
			}
			if (tflag != 0) {
				printc(stdout, '\000');
				printc(stdout, '\027');
				fflush(stdout);
			}
		}
		return (lastchr=getchar());
	}
	if ((c=*gcp++) != '\0')
		return (c);
	--gcp;
	return (EOF);
}

/*
 * Put back a character.
 */
ungetx(c)
{
	savechr = c;
}

/*
 * Given a line number, `a', get a line from the temp file
 * and store in the buffer `buffer'.  The number of characters read
 * including the null is returned.
 */
getline(a, buffer)
char *buffer;
{
	LINE l;
	int bno, off;
	register char *bp, *dp, *cp;

	l = line[a];
	bno = blockn(l);
	off = offset(l);
	bp = buffer;
	for (;;) {
		if ((cp=getdisk(bno++)) == NULL)
			return (0);
		dp = &cp[off];
		cp += DBSIZE;
		while (dp < cp) {
			if ((*bp++=*dp++) == '\0')
				return (bp-buffer);
		}
		off = 0;
	}
}

/*
 * Given a buffer, `bp', containing a line and a number of
 * characters, `n', in the buffer, add the line onto the end of
 * the temp file.
 */
putline(bp, n)
register char *bp;
register int n;
{
	int bno, off, inc;
	register char *dp;

	inc = (n+CLSIZE-1) & ~(CLSIZE-1);
	if (tmpseek+inc > (ULARGE/2)*CLSIZE) {
		derror("Temp file overflow");
		return (0);
	}
	bno = tmpseek / DBSIZE;
	off = tmpseek % DBSIZE;
	for (;;) {
		if (wcurbno>=0 && bno!=wcurbno)
			if (putdisk() == 0)
				return (0);
		wcurbno = bno;
		dp = &wdbcbuf[off];
		while (dp < &wdbcbuf[DBSIZE]) {
			*dp++ = *bp++;
			if (--n == 0) {
				tmpseek += inc;
				return (1);
			}
		}
		bno++;
		off = 0;
	}
}

/*
 * Given a block number, bring it into a disk buffer.  A pointer
 * to the buffer is returned.
 */
char *
getdisk(bno)
{
	if (bno == wcurbno)
		return (wdbcbuf);
	if (bno == rcurbno)
		return (rdbcbuf);
#if RSX
	/* IO.RVB = 0010400 */
	qio(010400, flun(tmp), 1, tmp->v_iosb, NULL,
		rdbcbuf, sizeof(rdbcbuf), 0, 0, bno+1, 0);
	wtse(1);
	if ((tmp->v_iosb[0]&0377) != 1)
		return (NULL);
#else
#if COHERENT || MSDOS
	lseek(fileno(tmp), (long) bno * DBSIZE, 0);
#endif
#if GEMDOS
	ffseek(tmp, (long) bno * DBSIZE, 0);
#endif
	if (read(fileno(tmp), rdbcbuf, sizeof(rdbcbuf)) != sizeof(rdbcbuf)) {
		terror("Read error");
		return (NULL);
	}
#endif
	rcurbno = bno;
	return (rdbcbuf);
}

/*
 * Write out the current disk buffer.
 */
putdisk()
{
#if RSX
	if (wcurbno >= tmp->v_hibk)
		if (grow(1) == 0)
			return (0);
	/* IO.WVB = 0011000 */
	qio(011000, flun(tmp), 1, tmp->v_iosb, NULL,
		wdbcbuf, sizeof(wdbcbuf), 0, 0, wcurbno+1, 0);
	wtse(1);
	if ((tmp->v_iosb[0]&0377) != 1)
		return (0);
#else
#if COHERENT || MSDOS
	lseek(fileno(tmp), (long) wcurbno * DBSIZE, 0);
#endif
#if GEMDOS
	ffseek(tmp, (long) wcurbno * DBSIZE, 0);
#endif
	if (write(fileno(tmp), wdbcbuf, sizeof(wdbcbuf)) != sizeof(wdbcbuf)) {
		terror("Write error");
		return (0);
	}
#endif
	return (1);
}
#if RSX

/*
 * Grow the temp file.
 */
grow(n)
{
	/* IO.EXT = 0011400 */
	qio(011400, flun(tmp), 1, tmp->v_iosb, NULL,
		0, 0, 0100000, n, 0, 0);
	wtse(1);
	if ((tmp->v_iosb[0]&0377) != 1)
		return (0);
	tmp->v_hibk += n;
	return (1);
}
#endif

/*
 * A diagnostic error.
 */
derror(str)
char *str;
{
	errstr = str;
	if (tflag != 0) {
		printc(stdout, '\000');
		printc(stdout, '\025');
		fflush(stdout);
	} else {
		printc(stderr, '?');
		if (vflag != 0)
			prints(stderr, str);
		printc(stderr, '\n');
	}
}

/*
 * A temp file error.
 */
terror(str)
char *str;
{
	errstr = str;
	if (tflag != 0) {
		printc(stdout, '\000');
		printc(stdout, '\025');
		fflush(stdout);
	} else {
		printc(stderr, '?');
		prints(stderr, str);
		printc(stderr, '\n');
	}
}

/*
 * Print out a decimal number.
 */
printd(fp, n)
FILE *fp;
register int n;
{
	register int c;

	if (n < 0) {
		printc(fp, '-');
		if ((n=-n) < 0)
			n = 0;
	}
	c = n%10 + '0';
	n /= 10;
	if (n != 0)
		printd(fp, n);
	printc(fp, c);
}

/*
 * Print out a long integer.
 */
printl(fp, n)
FILE *fp;
register long n;
{
	register int c;

	if (n < 0) {
		printc(fp, '-');
		if ((n=-n) < 0)
			n = 0;
	}
	c = n%10 + '0';
	n /= 10;
	if (n != 0)
		printl(fp, n);
	printc(fp, c);
}

/*
 * Print out an octal number.
 */
printo(fp, n)
FILE *fp;
{
	printc(fp, '0'+((n>>6)&03));
	printc(fp, '0'+((n>>3)&07));
	printc(fp, '0'+(n&07));
}

/*
 * Print out a string.
 */
prints(fp, cp)
register FILE *fp;
register char *cp;
{
	while (*cp)
		printc(fp, *cp++);
}

/*
 * Print out a character.
 */
printc(fp, c)
register FILE *fp;
{
	putc(c, fp);
	if (tflag==0 && c=='\n' && fp==stdout)
		fflush(fp);
}
