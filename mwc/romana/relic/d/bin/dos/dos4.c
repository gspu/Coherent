/* #define DEBUG 1*/
/* dos4.c */

#include "dos0.h"
char *fequiv();
/*
 * Compare two MDIR names.
 * Directories precede ordinary files.
 * Called by qsort().
 */
short
namecmp(mdpp1, mdpp2) MDIR **mdpp1, **mdpp2;
{
	register MDIR *mdp1, *mdp2;
	unsigned long t1, t2;

	mdp1 = *mdpp1;
	mdp2 = *mdpp2;
	if (isdir(mdp1) != isdir(mdp2))
		return (isdir(mdp1)) ? -1 : 1;
	if (nflag) {
		/*
		 * Compare by time with unsigned long compare.
		 * m_junk[10] is really the first byte of the time.
		 * Kludgy but it works.
		 */
		t1 = *(unsigned long *)&mdp1->m_junk[10];
		t2 = *(unsigned long *)&mdp2->m_junk[10];
		if (t1 != t2)
			return (t1 < t2) ? -1 : 1;
	}
	return strncmp(mdp1->m_name, mdp2->m_name, 11);
}

/*
 * Allocate a new DIR and link it into the DIR tree.
 * Initialize it with the supplied information.
 * Return a pointer to it.
 * The MDIR array is not allocated and read until required.
 */
DIR *
newdir(parent, mdp, files)
DIR *parent;
register MDIR *mdp;
short files;
{
	register DIR *dp;

	/* Allocate a new DIR. */
	dbprintf(("newdir(parent=\"%s\" mdp=%x files=%u): \n", (parent==NULL)?"<root>   ":parent->d_dname, mdp, files));
	if ((dp = (DIR *)malloc(sizeof(DIR))) == NULL)
		fatal("directory allocation failed");

	/* Initialize the DIR. */
	dp->d_parent = parent;
	dp->d_child = NULL;
	if (parent == NULL) {
		/* Root. */
		dp->d_sibling = NULL;
		dp->d_cluster = 0;
		strcpy(dp->d_dname, "<root>     ");
	} else {
		/* Subdirectory. */
		dp->d_sibling = parent->d_child;
		parent->d_child = dp;
		dp->d_cluster = mdp->m_cluster;
		strncpy(dp->d_dname, mdp->m_name, 11);
		dp->d_dname[11] = '\0';
	}
	dp->d_files = files;
	dp->d_dirblocks = (files * sizeof(MDIR) + ssize - 1) / ssize;
	dp->d_dir = dp->d_edp = NULL;
	dp->d_dirflag = 0;
	dbprintf(("\"%s\" cl=%x blocks=%u dp=%x\n", dp->d_dname, dp->d_cluster, dp->d_dirblocks, dp));
	return dp;
}

/*
 * Print a nonfatal error message.
 * Set the return status to 1.
 * Uses the nonportable "%r" format.
 */
void
nonfatal(x) char *x;
{
	fprintf(stderr, "dos: %r\n", &x);
	estat = 1;
}

/*
 * Allocate space for an MS-DOS directory.
 * Read in the directory unless dp is not root and its d_cluster is 0.
 */
void
readmdir(dp) register DIR *dp;
{
	register MDIR *mdp;
	register unsigned short n;

	dbprintf(("readmdir(%s) blocks=%u cl=%x\n", dp->d_dname, dp->d_dirblocks, dp->d_cluster));
	if ((mdp = (MDIR *)calloc(dp->d_dirblocks, ssize)) == NULL)
		fatal("directory allocation failed");
	dp->d_dir = mdp;
	dp->d_edp = mdp + dp->d_files;

	if (dp->d_cluster == 0) {
		if (dp->d_parent != NULL)
			return;			/* new subdirectory */
		diskread(mdp, (long)dirbase, dp->d_dirblocks, "directory");
	} else
		for (n = dp->d_cluster ; n <= CLMAX; n = getcluster(n)) {
			diskread(mdp, cltosec(n), clsize, "subdirectory");
			mdp += mdirsize;
		}

	/* Scan through the directory for volume label and subdirectories. */
	for (mdp = dp->d_dir; mdp < dp->d_edp; mdp++) {
		if ((n = mdp->m_name[0]) == MFREE || n == MEMPTY || n == MMDIR)
			continue;
		if (mdp->m_attr & MVOLUME)
			volume = mdp;
		if isdir(mdp)
			newdir(dp, mdp,	dirclusters(mdp) * mdirsize);
	}
}

/*
 * Replace one or more files on the MS-DOS file system.
 */
void
replace(nargs, args) short nargs; char *args[];
{
	register char **ap;
	struct stat s;

	if ((clbuf = malloc(clsize * ssize)) == NULL)
		fatal("cluster buffer allocation failed");
	if (nargs == 0)
		replacedir(NULL);
	else if (!strcmp(args[0], "-") || pflag) {
		pflag = 1;
		if (nargs!=1)
			fatal("replace: exactly one file required with 'p' option");
		replacefile(args[0]);
	} else for (ap=args; nargs; ap++, nargs--) {
		if (stat(*ap, &s) == -1)
			fatal("replace: \"%s\" not found", *ap);

		if (strncmp(*ap, "./", 2) == 0)
			*ap += 2;

		if (strcmp(*ap, ".") == 0)
		{
			replacedir(NULL);
			break;
		}

		base = basehold;
		strcpy(base,*ap);

		if (s.st_mode & S_IFDIR) {
			if (!sflag)
				replacedir(*ap);
		}
		else
			replacefile(*ap);
	}
	free(clbuf);
}

/*
 * Replace a directory; NULL means ".".
 * Replace subdirectories recursively unless sflag.
 */
void
replacedir(name) char *name;
{
	register struct direct *dirp;
	register char *cp;
	short fd;
	struct stat s;
	char dirbuf[sizeof(struct direct) + 1];
	char namebuf[NAMEMAX];

	dbprintf(("replacedir(%s)\n", name));
	if (name == NULL || strcmp(name, ".") == 0) {
		name = ".";
		cp = namebuf;
	} else {
		strcpy(namebuf, name);
		strcat(namebuf, "/");
		cp = &namebuf[strlen(namebuf)];
	}
	dirp = dirbuf;
	dirbuf[sizeof(struct direct)] = '\0';	/* NUL-terminate d_name */
	if ((fd = open(name, 0)) == -1)
		fatal("cannot search directory \"%s\"", name);
	while(read(fd,dirbuf, sizeof(struct direct))==sizeof(struct direct)) {
		if (dirp->d_ino == 0
		 || strcmp(dirp->d_name, ".") == 0
		 || strcmp(dirp->d_name, "..") == 0)
			continue;
		strncpy(cp, dirp->d_name, DIRSIZ);
		cp[strlen(dirp->d_name)] = '\0';

		if (stat(namebuf, &s) == -1)
			fatal("replacedir botch");
		else if (s.st_mode & S_IFDIR) {
			if (!sflag)
				replacedir(namebuf);
		}
		else if (s.st_mode & S_IFREG)
			replacefile(namebuf);
	}
	close(fd);
}

/*
 * Replace a file.
 */
void
replacefile(file) char *file;
{
	register MDIR *mdp;
	register short nread;
	DIR *dp;
	FILE *ifp;
	char *cp, *filename;
	short writesize;
	unsigned short next, prev;
	char *tmp, tmp2[6], *files;

	if (!pflag && !oldstyle) {
		if (!bflag && ((tmp = strrchr(file, '.')) != NULL)) {
			sprintf(tmp2, "%s.", tmp);
			if (!aflag)
				aflag = (strstr(ext, tmp2) != NULL) ? 1 : 0;
		}
	}

	/* Create the file in the appropriate directory. */
	dbprintf(("replacefile(%s)\n", file));
	files = makef(file, 0);

	if ((cp = strrchr(files, '/')) == NULL) {
		filename = files;
		dp = root;
	} else {
		filename = cp + 1;
		*cp = '\0';			/* NUL-terminate dirname */
		dp = creatdir(files);		/* find the directory */
		*cp = '/';			/* restore the / */
	}

	mdp = creatfile(filename, dp);		/* create the file */

	/* Read from the COHERENT file and write the MS-DOS file. */
	if (vflag)
		fprintf(stderr, "r %s\n", file);
	if (pflag)
		ifp = stdin;
	else if ((ifp = fopen(file, "r")) == NULL)
		fatal("replace: cannot open \"%s\"", file);
	writesize = clsize * ssize;
	for (prev = 0; ; prev = next) {
		if ((nread = finput(ifp, clbuf, writesize)) == 0)
			break;
		mdp->m_size += nread;
		next = freecluster();
		if (prev == 0)
			mdp->m_cluster = next;
		else
			putcluster(prev, next);
		diskwrite(clbuf, cltosec(next), clsize, file);
		if (nread < writesize)
			break;
	}
	if (nread < 0)
		fatal("replace: read error on file \"%s\"", file);
	if (!pflag)
		fclose(ifp);
	dostime(mdp, file);
}

#define NUM_D_RES	13

int istrcmp();

char * d_reserved[NUM_D_RES] =
{
    "aux",
    "clock$",
    "con",
    "com1",
    "com2",
    "com3",
    "com4",
    "lpt1",
    "lpt2",
    "lpt3",
    "lpt4",
    "nul",
    "prn"
};

/*
 * Make a full path from base and name
 */
char * makef(name, cr) char * name; short cr;
{
	static char tname[80];
	register char  *t = tname, *h;
	struct stat s;
	short done = 0, rs;
	register MDIR *mdp;
	short srcd, destd, deste;

	if (base1) {
		strcpy(t, base1);
		t += strlen(base1);

		if (cr) {
			srcd = isdir_keep;
			rs = stat(base1, &s);
			deste = (rs == 0);
			destd = (s.st_mode & S_IFDIR);
		}
		else {
			mdp = find(base1,root,NULL);
			deste = (mdp != NULL);
			destd = isdir(mdp);
			if (!pflag)
			{
				stat(base, &s);
				srcd = (s.st_mode & S_IFDIR);
			}
			else
				srcd = 0;
		}

/* dbprintf(("numargs = %d, deste = %d, destd = %d, srcd = %d\n",numargs,deste,destd,srcd));*/

		if (pflag)
			if (destd)
				fatal("Error: <%s> is a directory", base1);
			else
				done = 1;

		if ((numargs > 1) && (deste) && !(destd))
			fatal("Error: <%s> is a file", base1);
		
		if (numargs == 1) {
			if (!deste) {
				if (!srcd)
					done = 1;
			}
			else {
				if (!destd) {
					if (srcd)
						fatal("Error: <%s> is a file"
								, base1);
					else
						done = 1;
				}
			}
		}
	}

	if (!done) {
		if ((h = fequiv(name)) != NULL) {
			if (base1)
				*t++ = '/';
			strcpy(t, h);
		}
	}

	if (cr)
		maketree(tname);

        if ((t = strrchr(tname, '/')) != NULL)
	    t++;
	else
	    t = tname;

	if (bsearch(t, d_reserved, NUM_D_RES, sizeof (char *), istrcmp) != NULL)
	{
	    printf("Dos reserved name used <%s>. Changing filename to <%s!>\n",
			t, t);
            rs = strlen(t);
	    t[rs] = '!';
	    t[rs+1] = '\0';
	}


dbprintf(("base = <%s>, base1 = <%s>\n  name = <%s>, tname = <%s>\n", base, base1, name, tname));
	return tname;
}

istrcmp(a, b)
char *a, **b;
{
    return strcmp(a,*b);
}

char *fequiv(name) char *name;
{
	char *tmp;
	short basesize;

	if (!base)		/* No base at all means a full disk copy */
		return name;
				/* No path means return the whole name */
	if (strchr(name, '/') == NULL)
		return name;

	if (!strcmp(base, name))  /* No wc or subdir means return the fname */
		return strrchr(name, '/') + 1;

	tmp = strrchr(base + 1, '/');

	if (tmp != NULL)
		basesize = tmp-base;
	else
		basesize = strlen(base);

	return name + basesize + 1;
}

maketree(name) char *name;
{

	char *h, c[80];
	struct stat s;

	h = strchr(name, '/');
	while (h) {
		*h = '\0';
		if (stat(name, &s) == -1) {
			sprintf(c, "mkdir %s", name);	
			system(c);
			if (stat(name, &s) == -1)
				fatal("Cannot create directory %s", name);
			}
		*h = '/';
		h = strchr(h+1, '/');		
	}
}
				





/*
 * Produce a listing of the MS-DOS file system.
 */
void
table(nargs, args) short nargs; char *args[];
{
	register unsigned short n, i;
	register char **ap, *npap, *temp;
	register MDIR *mdp;
	DIR *dp;
	short free, bad, reserved;
	char buf[12];

	if (vflag) {
		/* Print media information. */
		free = bad = reserved = 0;
		for (n = 2; n <= maxcluster; n++)
			if ((i = getcluster(n)) == 0)
				++free;
			else if (i == CLBAD)
				++bad;
			else if (i > CLMAX && i < CLBAD)
				++reserved;
		if (volume == NULL)
			printf("Unlabelled disk:\n");
		else {
			strncpy(buf, volume->m_name, 11);
			buf[11] = '\0';
			printf("Volume in Drive %s is labeled %s\n",adev,buf);
		}
		printf("\t%lu bytes free\n",
			(long unsigned)free*clsize*ssize);
		if (bad)
			printf("\t%ld bytes in bad sectors\n",
				(long)bad*clsize*ssize);
		if (reserved)
			printf("\t%ld bytes in reserved sectors\n",
				(long)reserved*clsize*ssize);
		putchar('\n');
	}
	if (nargs == 0)
		tabledir(root, NULL);
	else for (ap = args; *ap != NULL; ap++) {
		if ((mdp = find(*ap, root, &dp)) == NULL)
			nonfatal("%s not found", *ap);
		else {
			npap = (temp = strrchr(*ap, '/')) ? temp+1 : *ap;
			do {
				if((strcmp(npap,cohn(mdp->m_name)) == 0)
							    && isdir(mdp))

					tabledir(dp, cohn(mdp->m_name));
				else
					tablefile(mdp, cohn(mdp->m_name));
			} while (mdp = findnext(&dp));
		}
	}
}

/*
 * Print a sorted directory.
 * Suppress '.', '..' and volume label.
 */
void
tabledir(dp, name) register DIR *dp; char *name;
{
	register MDIR *mdp;
	register MDIR **pp, **beg, **end;
	unsigned char c;

	if (name != NULL)
		printf("Directory %s:\n", name);
	if ((beg = pp = (MDIR **)malloc(dp->d_files * sizeof(MDIR *))) == NULL)
		fatal("cannot allocate directory pointer table");
	for (mdp = dp->d_dir; mdp < dp->d_edp; mdp++)
		if ((c = mdp->m_name[0]) != MFREE && c != MEMPTY && c != MMDIR
		 && (mdp->m_attr & MVOLUME) == 0)
			*pp++ = mdp;
	end = pp;
	qsort(beg, end - beg, sizeof(MDIR *), namecmp);
	for (pp = beg; pp < end; pp++)
		tablefile(*pp, NULL);
	free(beg);
	putchar('\n');
}

/*
 * Print a line describing a file.
 */
void
tablefile(mdp, name) register MDIR *mdp; char *name;
{
	register short attr;

	if (vflag) {
		attr = mdp->m_attr;
		putchar((attr & MRONLY ) ? 'r' : '-');
		putchar((attr & MHIDDEN) ? 'h' : '-');
		putchar((attr & MSYSTEM) ? 's' : '-');
		putchar((attr & MVOLUME) ? 'v' : '-');
		putchar((attr & MARCHIV) ? 'a' : '-');
		putchar((attr & MSUBDIR) ? 'd' : '-');

		printf("  %02d/%02d/%02d %02d:%02d %9ld  ",
			m_month(mdp), m_day(mdp), m_year(mdp)+80,
			m_hour(mdp),  m_min(mdp), (isdir(mdp)) ?
			   (long)dirclusters(mdp)*clsize*ssize : mdp->m_size);
	}

	if (name == NULL) {
		cohname(mdp->m_name, root);
		name = cohfile;
	}
	printf("%s\n", name);
}


/*
 * Convert NUL-terminated name to UPPERCASE in place.
 */
char *
uppercase(name) unsigned char *name;
{
	register unsigned char *s;
	register short c;

	for (s = name; (c = *s) != '\0'; s++)
		if (islower(c))
			*s = toupper(c);
	return name;
}

/*
 * Write the changed directories of the MS-DOS file system.
 */
void
writedir(dp) register DIR *dp;
{
	register MDIR *mdp;
	register unsigned short n;

	if (dp->d_child != NULL)
		writedir(dp->d_child);
	if (dp->d_sibling != NULL)
		writedir(dp->d_sibling);
	mdp = dp->d_dir;
	dbprintf(("writedir(%s) flag=%u cl=%x blocks=%u mdp=%x\n",
	      dp->d_dname, dp->d_dirflag, dp->d_cluster, dp->d_dirblocks, mdp));

	if (dp->d_dirflag == 0)
		return;				/* DIR was unchanged */
	if (dp->d_cluster == 0)			/* Write the root */
		diskwrite(dp->d_dir, (long)dirbase, dp->d_dirblocks, "directory");
	else					/* Write a subdirectory */
		for (mdp = dp->d_dir, n = dp->d_cluster; n <= CLMAX; n = getcluster(n)) {
			diskwrite(mdp, cltosec(n), clsize, "subdirectory");
			mdp += mdirsize;
		}
}

/* end of dos4.c */
