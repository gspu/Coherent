/*
 * unpatch.c - display value of kernel variable(s)
 */
#include <stdio.h>
#include <l.out.h>
#include <string.h>

#define NUMVARS 20	/* number of symbols we can look up */

/*
 * Table for namelist.
 */
struct nlist nl[NUMVARS+1];
char suffix[NUMVARS+1][3];	/* keep ":[cils]" for each variable */

/*
 * Symbols.
 */
char	*kfile;		/* Kernel data memory file */
char	*nfile;		/* Namelist file */
int	kfd;		/* Kernel memory file descriptor */
int	pflag;		/* true if patch-compatible output desired */
int	var_num;	/* Number of symbols to look up */

main(argc, argv)
char *argv[];
{
	int argn;

	initialise();
	/*
	 * Scan command line.
	 *
	 * "-c nfile" means use alternate map file.
	 * "-p" means output format suitable for patch command
	 *
	 * variable names may have ":[cils]" appended to specify
	 * size of value to be fetched, e.g.
	 *   unpatch foo:l
	 * says foo is a long
	 */
	for (argn = 1; argn < argc; argn++) {
		if (strcmp(argv[argn], "-c") == 0) {
			if (++argn < argc) {
				nfile = argv[argn];
			} else {
fprintf(stderr, "bad kfile spec\n");
				usage();
			}
		} else if (strcmp(argv[argn], "-p") == 0) {
			pflag = 1;
		} else {
			char p[40], *p2;

			strcpy(p, argv[argn]);
			if (p2 = strchr(p, ':')){
				if (strlen(p2) == 2) {
					strcpy(suffix[var_num], p2);
					*p2 = '\0';
					strncpy(nl[var_num].n_name, p, NCPLN);
				} else {
fprintf(stderr, "bad suffix size\n");
					usage();
				}
			} else
				strncpy(nl[var_num].n_name, p, NCPLN);
			var_num++;
		}
	} 
	if (var_num == 0) {
fprintf(stderr, "no variable to unpatch\n");
		usage();
	}
	execute();
	exit(0);
}

/*
 * Initialise.
 */
initialise()
{
	int i;

	for (i = 0; i < NUMVARS+1; i++) {
		nl[i].n_name[0] = '\0';
		nl[i].n_type = 0;
		suffix[i][0] = '\0';
		suffix[i][1] = '\0';
	}
	nfile = "/coherent";
	kfile = "/dev/kmem";
}

/*
 * Print out usage.
 */
usage()
{
	panic("Usage: unpatch [-c map_file] [-p] var_name[:cils]...");
}

/*
 * Display value
 */
execute()
{
	int v, size;
	long longval, result;
	unsigned int intval;
	unsigned short shortval;
	unsigned char charval;

	if ((kfd = open(kfile, 0)) < 0)
		panic("Cannot open %s", kfile);
	nlist(nfile, nl);
	for (v = 0; v < var_num; v++) {
		if (nl[v].n_type == 0)
fprintf(stderr, "can't find variable %s in %s\n", nl[v].n_name, nfile);
		else {
			switch (suffix[v][1]) {
			case 'c':
				size = sizeof(char);
				kread((long)nl[v].n_value, &charval, size);
				result = (long) charval;
				break;
			case 'i':
			default:
				size = sizeof(int);
				kread((long)nl[v].n_value, &intval, size);
				result = (long) intval;
				break;
			case 'l':
				size = sizeof(long);
				kread((long)nl[v].n_value, &longval, size);
				result = longval;
				break;
			case 's':
				size = sizeof(short);
				kread((long)nl[v].n_value, &shortval, size);
				result = (long) shortval;
				break;
			}
			if (pflag)
				printf("%s=0x%lx%s ", nl[v].n_name,
				  result, suffix[v]);
			else
				printf("%s=0x%lx\n", nl[v].n_name, result);
		}
	}
}

/*
 * Read `n' bytes into the buffer `bp' from kernel memory
 * starting at seek position `s'.
 */
kread(s, bp, n)
long s;
{
	lseek(kfd, (long)s, 0);
	if (read(kfd, bp, n) != n)
		panic("Kernel memory read error");
}

/*
 * Print out an error message and exit.
 */
panic(a1)
char *a1;
{
	fflush(stdout);
	fprintf(stderr, "%r", &a1);
	fprintf(stderr, "\n");
	exit(1);
}
