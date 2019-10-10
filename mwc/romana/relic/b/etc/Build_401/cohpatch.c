/*
 * cohpatch.c
 * 8/19/91
 * Uses common routines in build0.c: cc -o cohpatch cohpatch.c build0.c
 * Patch new copy of COHERENT for V3.2.0 update installation.
 * Hack, hack, hack.
 */

#include "build0.h"
#include <l.out.h>
#include <sys/hdioctl.h>

extern	long	lseek();

#define	COH	"/coherent"
#define	COHNEW	"/coherent.new"
#define	COHOLD	"/coherent.old"
#define	KMEM	"/dev/kmem"
#define	NENTRIES	(sizeof(nl)/sizeof(nl[0]))

char	buf[256];		/* temporary buffer */
char	cmd[512];		/* command buffer */
int	kfd;			/* kernel file desciptor */

/* Symbols patched from old /coherent to new /coherent. */
struct	nlist	nl[] = {
	{ "rootdev_", 0, 0 },
	{ "pipedev_", 0, 0 },
	{ "ronflag_", 0, 0 },
	{ "___", 0, 0 },
	{ "_entry_", 0, 0 },
	{ "LPTIME_", 0, 0 },
	{ "LPWAIT_", 0, 0 },
	{ "", 0, 0 }
};
int	nlsizes[NENTRIES] = {		/* sizes of preceding, yuk */
		sizeof (unsigned int),
		sizeof (unsigned int),
		sizeof (unsigned int),
		sizeof (unsigned long),
		sizeof (unsigned long),
		sizeof (unsigned int),
		sizeof (unsigned int),
		0
};

/* Symbol from old /coherent handled specially. */
struct	nlist	nlat[] = {
	{ "atparm_", 0, 0 },
	{ "", 0, 0 }
};

/* Symbols patched in /drv/al0. */
struct	nlist	nlal0[] = {
	{ "C1BAUD_", 0, 0 },
	{ "C3BAUD_", 0, 0 },
	{ "", 0, 0 }
};

/* Symbols patched in /drv/al1. */
struct	nlist	nlal1[] = {
	{ "C2BAUD_", 0, 0 },
	{ "C4BAUD_", 0, 0 },
	{ "", 0, 0 }
};

extern	void	kread();

main(argc, argv) int argc; char *argv[];
{
	register int i;
	unsigned char ucval;
	unsigned int uval;
	unsigned long ulval;

	argv0 = argv[0];
	if (argc > 1 && strcmp(argv[1], "-d") == 0) {
		++dflag;
		--argc;
		++argv;
	}
		
	if ((kfd = open(KMEM, 0)) < 0)
		fatal("cannot open \"%s\"", KMEM);

	/* Search /coherent for current values. */
	nlist(COH, nl);
	nlist(COH, nlat);
	nlist(COH, nlal0);
	nlist(COH, nlal1);

	/*
	 * Patch /coherent.new:
	 *	root device
	 *	pipe device
	 *	read-only flag
	 *	serial number (twice)
	 *	line printer parameters
	 */
	sprintf(cmd, "/conf/patch %s ", COHNEW);
	for (i = 0; i < NENTRIES-1; i++) {
		if (nl[i].n_type == 0)
			fatal("cannot find symbol \"%s\"", nl[i].n_name);
		if (nlsizes[i] == sizeof (unsigned int)) {
			kread(nl[i].n_value, &uval, nlsizes[i]);
			sprintf(buf, "%s=0x%x ", nl[i].n_name, uval);
		} else if (nlsizes[i] == sizeof (unsigned long)) {
			kread(nl[i].n_value, &ulval, nlsizes[i]);
			sprintf(buf, "%s=%lu:l ", nl[i].n_name, ulval);
		}
		strcat(cmd, buf);
	}
	sys(cmd, S_FATAL);

	/* Patch AT device parameters. */
	sprintf(cmd, "/conf/patch %s ", COHNEW);
	if (nlat[0].n_type != 0) {
		for (i = 0; i < sizeof(hdparm_t); i++ ) {
			kread(nlat[0].n_value+i, &ucval, sizeof(ucval));
			sprintf(buf, "atparm_+%d=%u:c ", i, ucval);
			strcat(cmd, buf);
		}
		sys(cmd, S_FATAL);
	}

	/* Patch baud rates. */
	sprintf(cmd, "/conf/patch /drv/al0 ");
	for (i = 0; i < 2; i++) {
		if (nlal0[i].n_type == 0)
			fatal("cannot find symbol \"%s\"", nlal0[i].n_name);
		kread(nlal0[i].n_value, &uval, sizeof(unsigned int));
		sprintf(buf, "%s=%u ", nlal0[i].n_name, uval);
		strcat(cmd, buf);
	}
	sys(cmd, S_FATAL);
	sprintf(cmd, "/conf/patch /drv/al1 ");
	for (i = 0; i < 2; i++) {
		if (nlal1[i].n_type == 0)
			fatal("cannot find symbol \"%s\"", nlal1[i].n_name);
		kread(nlal1[i].n_value, &uval, sizeof(unsigned int));
		sprintf(buf, "%s=%u ", nlal1[i].n_name, uval);
		strcat(cmd, buf);
	}
	sys(cmd, S_FATAL);

	/* Move original and patched versions. */
	sprintf(cmd, "/bin/mv %s %s", COH, COHOLD);
	sys(cmd, S_FATAL);
	sprintf(cmd, "/bin/mv %s %s", COHNEW, COH);

	/* Done. */
	sys(cmd, S_FATAL);
	close(kfd);
	exit(0);
}

void
kread(useek, bp, n) unsigned int useek; char *bp; int n;
{
	if (lseek(kfd, (long)useek, 0) == -1L)
		fatal("seek error on \"%s\"", KMEM);
	if (read(kfd, bp, n) != n)
		fatal("read error on \"%s\"", KMEM);
}

/* end of cohpatch.c */
