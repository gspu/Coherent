/*#define DEBUG 1*/
/* dos.c */

#include "dos0.h"

/*
 * Convert an MS-DOS filename and path
 * to a NUL-terminated COHERENT filename in cohfile[].
 * Return a pointer to the NUL terminator.
 */
char *
cohname(name, dp) register char *name; DIR *dp;
{
	register char *cp;

	if (dp == root)
		cp = cohfile;
	else {
		cp = cohname(dp->d_dname, dp->d_parent);
		*cp++ = '/';
		*cp = '\0';
	}
	cp = lcname(cp, name, 8);
	name += 8;
	if (*name != ' ') {
		*cp++ = '.';
		cp = lcname(cp, name, 3);
	}
	return cp;
}

/*
 * Change the COHERENT time of the given file to the MS-DOS time of mdp.
 * There should be a COHERENT function which converts struct tm to time_t,
 * but in the meantime this does it by hand.  What a pain.
 */
void
cohtime(file, mdp) char *file; MDIR *mdp;
{
	time_t cohtime, times[2];
	static short mdays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

	settz();
	cohtime = ((10 * 365L) + 2);		/* days 1/1/70 to 1/1/80 */
	cohtime += (m_year(mdp) * 365L);	/* to 1/1/year */
	if (m_month(mdp) > 2)			/* leap year adjust */
		cohtime += ((m_year(mdp)+4)/4);
	else
		cohtime += ((m_year(mdp)+3)/4);
	cohtime += mdays[m_month(mdp)-1];		/* to mon/1/year */
	cohtime += (m_day(mdp)-1);		/* to mon/day/year */
	cohtime *= 24L;				/* hours */
	cohtime += m_hour(mdp);
	cohtime *= 60L;
	cohtime += m_min(mdp);			/* minutes */
	cohtime *= 60L;
	cohtime += m_sec(mdp) * 2;		/* seconds */
	cohtime += timezone;
	times[0] = times[1] = cohtime;
	if (utime(file, times) == -1)
		fatal("cannot set time of file \"%s\"", file);
}

/*
 * Find or create an MS-DOS directory.
 * Return a pointer to the DIR.
 * Failure is always fatal.
 */
DIR *
creatdir(name) char *name;
{
	register MDIR *mdp;
	register char *cp;
	DIR *dp, *ndp;

	/* Check if directory exists. */

	if ((mdp = find(name, root, &dp)) != NULL) {
		if (!isdir(mdp))
		{
			if (mdp->m_attr & MVOLUME)
				fatal("\"%s\" is the volume name, not a subdirectory", name);
			else
				fatal("\"%s\" is an MS-DOS file, not a subdirectory", name);
		}

		return dp;
	}

	/* Directory does not exist, create it. */
	dbprintf(("creatdir(%s)\n", name));
	if ((cp = strrchr(name, '/')) == NULL) {
		dp = root;
		mdp = creatfile(name, dp);	/* create the dir in root */
	} else {
		*cp = '\0';			/* NUL-terminate dirname */
		dp = creatdir(name);		/* create the parent */
		*cp = '/';			/* restore the / */
		mdp = creatfile(cp + 1, dp);	/* create the directory */
	}
	if (vflag)
		fprintf(stderr, "r %s/\n", name);
	mdp->m_attr = MSUBDIR;
	ndp = newdir(dp, mdp, mdirsize);
	readmdir(ndp);
	mdp->m_cluster = ndp->d_cluster = freecluster();
	mdp = ndp->d_dir;
	mdirinit(mdp, ndp, ".          ", MSUBDIR, ndp->d_cluster);
	mdirinit(++mdp, ndp, "..         ", MSUBDIR, dp->d_cluster);
	return ndp;
}

/*
 * Create an empty MS-DOS file.
 * Delete the old one if it exists.
 * Return a pointer to the initialized MDIR entry
 * (name initialized, attribute==0, cluster==0, size==0L).
 * Failure is always fatal.
 */
MDIR *
creatfile(name, dp) char *name; DIR *dp;
{
	register MDIR *mdp;
	unsigned short n, ofiles, next;

	/* Check if file exists. */
	dbprintf(("creatfile(%s, %s)\n", name, dp->d_dname));
	if ((mdp = find(name, dp, &dp)) != NULL) {
		if isdir(mdp)
			fatal("\"%s\" is an MS-DOS subdirectory", name);
		deletefile(mdp, dp);		/* delete the old one */
	}

	/* Find a slot for the file. */
	for (mdp = dp->d_dir; mdp < dp->d_edp; mdp++)
		if (mdp->m_name[0] == MEMPTY || mdp->m_name[0] == MFREE)
			break;
	if (mdp == dp->d_edp) {
		/* Out of directory space.  Fatal on root. */
		if (dp == root)
			fatal("replace: out of root directory space for \"%s\"", name);
		/* Add a cluster to a subdirectory. */
		for (n = dp->d_cluster; (next = getcluster(n)) <= CLMAX; n = next)
			;
		putcluster(n, freecluster());
		dp->d_dirblocks += clsize;
		ofiles = dp->d_files;
		dp->d_files += mdirsize;
		if ((dp->d_dir = (MDIR *)realloc(dp->d_dir, dp->d_dirblocks * ssize)) == NULL)
			fatal("replace: out of space to grow subdirectory");
		dbprintf(("creatfile: grow \"%s\" dp=%x at d_dir=%x\n", dp->d_dname, dp, dp->d_dir));
		dp->d_edp = dp->d_dir + dp->d_files;
		for (mdp = dp->d_dir + ofiles + 1; mdp < dp->d_edp; mdp++)
			mdp->m_name[0] = MFREE;
		mdp = dp->d_dir + ofiles;
	}
	mdirinit(mdp, dp, dosname(name), MARCHIV, 0);
	return mdp;
}

/*
 * Delete specified files.
 */
void
delete(nargs, args) short nargs; char *args[];
{
	register MDIR *mdp;
	register char **ap;
	DIR *dp;

	if (nargs == 0)
		fatal("delete: must specify files");
	for (ap = args; *ap != NULL; ap++) {
		if (vflag)
			fprintf(stderr, "d %s\n", *ap);
		if ((mdp = find(*ap, root, &dp)) == NULL)
			nonfatal("delete: \"%s\" not found", *ap);
		else {
			do {
				if isdir(mdp)
					deletedir(mdp, dp, cohn(mdp->m_name));
				else
					deletefile(mdp, dp);
 			} while (mdp = findnext(&dp));
		}
	}
}

/*
 * Delete a subdirectory.
 * Make sure it is empty first.
 */
void
deletedir(mdp, dp, name) MDIR *mdp; DIR *dp; char *name;
{
	register MDIR *mdp2;
	register DIR **dpp;
	short c;

	/* Make sure subdirectory is empty. */
	for (mdp2 = dp->d_dir; mdp2 < dp->d_edp; mdp2++)
		if ((c = mdp2->m_name[0])!=MFREE && c!=MEMPTY && c!=MMDIR) {
		       nonfatal("delete: subdirectory \"%s\" not empty",name);
		       return;
		}

	/* Delete the subdirectory entry and free its MDIR. */
	deletefile(mdp, dp->d_parent);
	free(dp->d_dir);

	/* Remove the DIR from the chain and free it. */
	for (dpp = &(dp->d_parent->d_child); *dpp != NULL; dpp = &((*dpp)->d_sibling))
		if (*dpp == dp)
			break;
	if (*dpp == NULL)
		fatal("deletedir botch");
	*dpp = dp->d_sibling;
	free(dp);
}

/*
 * Delete a file.
 * Free the clusters in its FAT chain.
 * Free the directory entry.
 * Set the dirflag requiring the directory to be updated.
 */
void
deletefile(mdp, dp) MDIR *mdp; DIR *dp;
{
	register unsigned short n, next;

	if ((mdp->m_attr & MRONLY) && !fflag)
	{
		nonfatal("cannot delete %s\n", cohn(mdp->m_name));
		return;
	}

	dp->d_dirflag = 1;
	mdp->m_name[0] = MEMPTY;		/* zap the MDIR entry */
	if (mdp->m_cluster == 0)
		return;				/* no chain */
	for (n = mdp->m_cluster; n <= CLMAX; n = next) {
		next = getcluster(n);
		putcluster(n, CLFREE);
	}
	mdp->m_cluster = CLFREE;
}

/*
 * Return the size in clusters of the directory starting at n.
 */
unsigned short
dirclusters(mdp) MDIR *mdp;
{
	register unsigned short n, count;

	for (n = mdp->m_cluster, count = 1; (n = getcluster(n)) <= CLMAX; ++count)
		;
	return count;
}

/*
 * Map a COHERENT filename to an 11-character MS-DOS name (plus NUL terminator)
 * in the static buffer and return a pointer to it.
 * There is no good way, thanks to different filename lengths and '.'.
 * Special hacks: ".profile" -> "_PROFILE", "a.b.c" -> "A_B.C".
 */
char *
dosname(name) register char *name;
{
	register char *s, *dotp;
	char c;
	static char buf[16];

	dotp = strrchr(name, '.');
	if (dotp == name)
		dotp = NULL;
	for (s = buf; name != dotp && (c = *name) != '\0'; name++)
		if (s < &buf[8])		/* copy name */
			*s++ = (c == '.') ? '_' : c;
	while (s < &buf[8])
		*s++ = ' ';			/* space-pad name */
	if (name++ == dotp)
		for ( ; *name != '\0'; name++)
			if (s <= &buf[11])
				*s++ = *name;	/* copy extension */
	while (s < &buf[11])
		*s++ = ' ';			/* space-pad extension */
	*s = '\0';				/* NUL terminate */
	uppercase(buf);				/* map to UPPER */
	return buf;
}

/*
 * Set the time of an MS-DOS directory entry.
 * Use the mtime of file if kflag and file is not NULL,
 * otherwise use the current time.
 */
void
dostime(mdp, file) register MDIR *mdp; char *file;
{
	register struct tm *tmp;
	struct stat s;
	time_t t;

	if (kflag && file != NULL) {
		if (stat(file, &s) == -1)
			fatal("cannot stat \"%s\"", file);
		t = s.st_mtime;
	} else
		t = time(NULL);
	tmp = localtime(&t);
	mdp->m_time =
	   c_sec(tmp->tm_sec/2) | c_min(tmp->tm_min) | c_hour(tmp->tm_hour);
	mdp->m_date =
	   c_day(tmp->tm_mday)|c_month(tmp->tm_mon+1)|c_year(tmp->tm_year-80);
}

/*
 * creat a directory on a dos disk
 */
void
createdir(nargs, args) short nargs; char *args[];
{
	creatdir(*args);
}


/*
 * Extract files from MS-DOS file system.
 */
void
extract(nargs, args) short nargs; char *args[];
{
	register MDIR *mdp;
	register char **ap;
	char *argsd[512], *argsf[512];
	short dnargs = 0, fnargs = 0;
	char *tmp;
	DIR *dp;

	if ((clbuf = malloc(clsize * ssize)) == NULL)
		fatal("cluster buffer allocation failed");
	if (pflag && nargs!=1)
		fatal("extract: exactly one file required with 'p' option");
	if (nargs == 0)
		extractdir(root);
	else {
		if ((mdp = find(args[0], root, &dp)) == NULL)
			nonfatal("extract: file \"%s\" not found", *ap);
		else {
			do {
				cohname(mdp->m_name, dp);
				if (isdir(mdp)) {
					if (!sflag) {
						argsd[dnargs] =
						    malloc(strlen(cohfile)+1);
						strcpy(argsd[dnargs++],
								cohfile);
					}
				}
				else {
					argsf[fnargs] =
						malloc(strlen(cohfile) + 1);
					strcpy(argsf[fnargs++],cohfile);
				}
	 		} while (mdp = findnext(&dp));
			numargs = fnargs + (sflag ? 0 : dnargs);
			if (fnargs > 0) {
				for (ap = argsf; fnargs ; ap++, fnargs--) {
					isdir_keep = 0;
					mdp = find(*ap, root, &dp);
					base = basehold;
					strcpy(base,*ap);
					extractfile(mdp, dp);
				}
			}
			if ((!sflag) && (dnargs > 0)) {
				for (ap = argsd; dnargs ; ap++, dnargs--) {
					isdir_keep = 1;
					mdp = find(*ap, root, &dp);
					base = basehold;
					strcpy(base,*ap);
					extractdir(dp);
				}
			}
		}
	}
	free(clbuf);
}

/*
 * Extract a directory.
 * Extract subdirectories recursively unless sflag.
 */
void
extractdir(dp) register DIR *dp;
{
	register MDIR *mdp;
	DIR *dp2;
	unsigned char c;
	struct stat s;
	short freeflag;

	freeflag = 0;
	if (!sflag) {
		if (dp->d_dir == NULL) {
			readmdir(dp);		/* read the DOS directory */
			freeflag = 1;		/* and set a flag */
		}
		if (dp != root) {
			cohname(dp->d_dname, dp->d_parent);
			if (stat(cohfile, &s) == -1) {
				/* COHERENT directory not found, create it. */
				if (vflag)
					fprintf(stderr, "x %s/\n", cohfile);
				if (oldstyle)
					if (system(strcat(strcpy(cmd, "mkdir "), cohfile)) != 0)
						fatal("cannot create directory \"%s\"", cohfile);
			} else if ((s.st_mode & S_IFDIR) == 0)
				fatal("\"%s\" exists but is not a directory",
					cohfile);
		}
		for (dp2 = dp->d_child; dp2 != NULL; dp2 = dp2->d_sibling)
			extractdir(dp2);	/* extract subdirectory */
	}
	for (mdp = dp->d_dir; mdp < dp->d_edp; mdp++)
		if ((c = mdp->m_name[0]) != MEMPTY && c != MFREE && c != MMDIR && !isdir(mdp))
			extractfile(mdp, dp);
	if (freeflag) {
		free(dp->d_dir);
		dp->d_dir = dp->d_edp = NULL;
	}
}

/*
 * Extract a file.
 */
void
extractfile(mdp, dp) register MDIR *mdp; DIR *dp;
{
	register unsigned short cluster;
	short readsize, i;
	long size;
	FILE *ofp;
	char *tmp, tmp2[6], *mcohfile;

	if (mdp->m_attr & MVOLUME)
		return;

	tmp = &(mdp->m_name[8]);
	if (!oldstyle) {
		if (!bflag && (*tmp != ' ')) {
			tmp2[0] = '.';
			for (i=0; i< 3; i++) {
				if (tmp[i] != ' ')
					tmp2[i+1] = tolower(tmp[i]);
				else
					break;
			}
			tmp2[i+1] = '.';
			tmp2[i+2] = '\0';
			if (!aflag)
				aflag = (strstr(ext, tmp2) != NULL) ? 1 : 0;
		}
	}

dbprintf(("bflag=%d, aflag=%d, mdp->m_name=%s, tmp2=%s\n", bflag, aflag, mdp->m_name, tmp2));

	/* Convert filename to COHERENT format and open the output file. */
	cohname(mdp->m_name, dp);

	if (pflag)
			ofp = stdout;
	else if ((ofp = fopen(mcohfile = makef(cohfile, 1), "w")) == NULL)
		fatal("extract: cannot create file \"%s\"", mcohfile);

	if (vflag)
		fprintf(stderr, "x %s\n", mcohfile);

	/* Read the MS-DOS file and write it to the COHERENT file. */
	readsize = clsize * ssize;
	size = mdp->m_size;
	for(cluster= mdp->m_cluster; size > 0; cluster = getcluster(cluster)){
		if (cluster > CLMAX)
			fatal("extract: early EOF on file \"%s\"",mcohfile);
		if (diskread(clbuf, cltosec(cluster), clsize))
			fatal("extract: read error on file \"%s\"", mcohfile);
		if (size < readsize)
			readsize = size;
		foutput(mcohfile, ofp, clbuf, readsize);
		size -= readsize;
	}
	if (!pflag)
		fclose(ofp);
	if (kflag)
		cohtime(mcohfile, mdp);
}

/* end of dos2.c */
