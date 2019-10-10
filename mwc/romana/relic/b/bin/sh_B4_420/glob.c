/*
 * A shell.
 * Glob interpretation.
 */
#include "sh.h"

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>

#define isdir(s)	(((s)->st_mode&S_IFMT)==S_IFDIR)

struct nmlst {
	struct nmlst *g_next;
	char g_name[];
};

char *dirn = NULL;
int pref = 0;

struct nmlst *newnm();
char *gany();
char *dread();
int nmcmp();

/*
 * Initial glob driver, set up initial directory name, pattern, and
 * suffix; call glob2; sort the results.
 */
glob1(args)
char *args;
{
	register char *patt;
	register char *suff;
	register int nsep;
	int myargc;

	if (gany(args)==NULL) {
		/* Nothing to match */
		strip(args);
		newarg(args, 0);
	} else {
		if (dirn != NULL)
			sfree(dirn);
		dirn = salloc(DIRSIZ);
		if (args[0]=='/') {
			pref = 0;
			strcpy(dirn, "/");
			patt = args + 1;
		} else {
			pref = 2;
			strcpy(dirn, "./");
			patt = args;
		}
	
		if ((suff=index(patt, '/')) != NULL)
			for (nsep=0; *suff=='/'; nsep+=1)
				*suff++ = '\0';
		else
			nsep = 0;
		myargc = nargc;
		glob2(patt, nsep, suff);
		if (myargc != nargc)
			qsort(&nargv[myargc], nargc-myargc,
				sizeof(nargv[0]), nmcmp);
		else {
			/* No match */
			while (nsep-- > 0)
				*--suff = '/';
			strip(args);
			newarg(args, 0);
		}
	}
}

nmcmp(cpp1, cpp2)
char **cpp1, **cpp2;
{
	return (strcmp(*cpp1, *cpp2));
}

glob2(patt, nsep, suff)
char *patt, *suff;
int nsep;
{
	register struct nmlst *nmlst = NULL;
	struct nmlst *np;
	register char *name;
	char *nsuff, *ndirn;
	int nnsep, dirp;

	if (gany(patt)==NULL) {
		nmlst = newnm(nmlst, patt);
		strip(nmlst->g_name);
	} else if (dopen(dirn) >= 0) {
		while ((name = dread()) != NULL) {
			if (match(patt, name)
			 && (name[0]!='.' || patt[0]=='.')) {
				nmlst = newnm(nmlst, name);
			}
		}
	}
	if (nmlst!=NULL) {
		ndirn = salloc(strlen(dirn)+DIRSIZ+nsep+1);
		strcpy(ndirn, dirn);
		sfree(dirn);
		dirn = ndirn;
		dirp = strlen(dirn);
		if ((nsuff=suff)!=NULL && (nsuff=index(nsuff, '/'))!=NULL)
			for (nnsep=0; *nsuff=='/'; nnsep+=1)
				*nsuff++ = '\0';
		else
			nnsep = 0;
		while (nmlst!=NULL) {
			name = dirn + dirp;
			strcpy(name, nmlst->g_name);
			nmlst = (np = nmlst)->g_next;
			sfree(np);
			mksep(name, nsep);
			if (suff!=NULL)
				glob2(suff, nnsep, nsuff);
			else
				newarg(dirn + pref, 1);
		}
		while (nnsep-- > 0)
			*--nsuff = '/';
		dirn[dirp] = '\0';
	}
}

/*
 * See if a pattern matches a string.
 * '\' escapes the next character.
 */
match(pp, sp)
register char *pp;
register char *sp;
{
	int c2;
	register int c1;

	while ((c1=*pp++)) {
		switch (c1) {
		case '?':
			if (*sp++)
				continue;
			return (0);
		case '*':
			do {
				if (match(pp, sp))
					return (1);
			} while (*sp++);
			return (0);
		case '[':
			if ((c2=*sp++) == '\0')
				return (0);
			for (;;) {
				if ((c1=*pp++) == '\0' || c1 == ']')
					return (0);
				if (c1 == '\\' && (c1=*pp++) == '\0')
					return (0);
				if (c1 == c2)
					break;
				if (*pp == '-') {
					pp += 1;
					if (c2 < c1)
						continue;
					if ((c1=*pp++) == '\0')
						return (0);
					if (c1 == '\\' && (c1=*pp++) == '\0')
						return (0);
					if (c2 <= c1)
						break;
				}
			}
			while ((c1 = *pp++) != ']') {
				if (c1 == '\0')
					return (0);
				if (c1 == '\\' && *pp++ == '\0')
					return (0);
			}
			continue;
		case '\\':
			if ((c1=*pp++) == '\0')
				return (0);
			/* fall through */
		default:
			if (c1 == *sp++)
				continue;
			return (0);
		}
	}
	return (*sp=='\0');
}

struct nmlst *
newnm(olst, name)
struct nmlst *olst;
char *name;
{
	register struct nmlst *np;
	register int n;

	n = strlen(name)+1+sizeof(*np);
	np = (struct nmlst *) salloc(n);
	np->g_next = olst;
	strcpy(np->g_name, name);
	return (np);
}

mksep(cp, ns)
register char *cp;
register int ns;
{
	while (*cp != '\0')
		cp += 1;
	while (ns-- > 0)
		*cp++ = '/';
	*cp = '\0';
}

newarg(p, f)
char *p;
{
	struct stat s;

	if (f && stat(p, &s) < 0)
		return;
	nargc += 1;
	nargv = addargl(nargv, duplstr(p, 0));
}

/*
 * Returns the location of the next unescaped glob character.
 */
char *
gany(s)
register char *s;
{
	register int c;

	while (*s)
		if ((c=*s++) == '\\')
			if (*s++ == '\0')
				return (NULL);
			else
				continue;
		else if (c == '*' || c == '?' || c == '[')
			return (--s);
	return (NULL);
}

/*
 * get rid of the glob escapes.
 */
strip(s)
register char *s;
{
	register char *p;

	p = s;
	while (*s)
		if ((*p = *s++) == '\\')
			if (*p++ = *s++)
				continue;
			else
				break;
		else
			p++;
	*p = *s;
}

int dfd;
int dcnt;
struct direct *dptr;
struct direct *dbuf = strt;
char dtmp[DIRSIZ+2] = { 0 };

dopen(p)
char *p;
{
	struct stat s;

	dcnt = 0;
	if (stat(p, &s) < 0
	 || ! isdir(&s)
	 || (dfd = open(p, 0)) < 0)
		dfd = -1;
	return (dfd);
}

char *
dread()
{
	register struct direct *dp;

	while (dfd >= 0) {
		while (dcnt-- > 0) {
			dp = dptr++;
			if (dp->d_ino != 0)
				return (strncpy(dtmp, dp->d_name, DIRSIZ));
		}
		dptr = dbuf;
		dcnt = read(dfd, (char *)dptr, 64*sizeof(dbuf[0]));
		dcnt /= sizeof(dbuf[0]);
		if (dcnt <= 0) {
			close(dfd);
			dfd = -1;
		}
	}
	return (NULL);
}

