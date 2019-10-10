/*
 * dos0.c
 * cc -o dos dos[01234].c
 * Read, write or format an MS-DOS filesystem (default: INFILE in dos1.h).
 * For usage, see USAGE in dos0.h.
 * Understands DOS 2.0 tree structured disks.
 * Understands 1.5 and 2-byte FAT entries;
 * does not understand 4-byte FAT entries.
 * Prints some debugging information if compiled -DDEBUG.
 */

#include "dos0.h"

/* Globals. */
short		oldstyle;		/* Indicates the this was a dos */
short		aflag;			/* ASCII text file		*/
short		bflag;			/* Binary text file		*/
unsigned char	cohfile[NAMEMAX];	/* COHERENT filename		*/
unsigned char	cmd[6 + NAMEMAX];	/* system() command buffer	*/
char *		device = INFILE;	/* Input device filename	*/
short		estat;			/* Exit status			*/
short		(*fun)();		/* Function to execute		*/
short		kflag;			/* Use mtime, not current time	*/
short		nflag;			/* Sort by time, newest first	*/
short		fflag;			/* Force removal of rdonly files*/
long		partseek;		/* Extended MS-DOS part. seek	*/
short		pflag;			/* Extract/replace is piped	*/
DIR *		root;			/* Root directory		*/
short		sflag;			/* Suppress subdirectory x/r	*/
MDIR *		volume;			/* Volume label			*/

short		xpart;			/* Extended MS-DOS partition	*/
char *		base, basehold[35];	/* Base for path-including copies */
char *		base1;

char		ext[512];		/* Ascii Extension list */
short		sext = 0;		/* size of extension list */
short		numargs;
short		isdir_keep;

char *tran_dev();
char *tran_file();
short die_signal();

main(argc, argv) short argc; char *argv[];
{
	register short mode;
	short tv, c, i = 0, dev = 2, j, k = 0, dargs = 0;
	char *tmp;

	argv0 = argv[0];

	if (strcmp((argv[0] + strlen(argv[0]) - 3), "dos")) {
		usagemsg = USAGE;
		if (argc < 2)
			usage(NULL);

		mode = 2;
		tmp = argv[0];
		if ((*argv[1] == '-') && (argv[1][1] != '\0')) {
			while ((c = *argv[1]++) != '\0') {
				dargs = 1;
				switch (c) {
					case 'a':
					case 'm':	aflag++;	break;
					case 'b':
					case 'r':	bflag++;	break;
					case 'k':	kflag++;	break;
					case 'f':	fflag++;	break;
					case 's':	sflag++;	break;
					case 'n':	nflag++;	break;
					case 'v':	vflag++;	break;
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':	xpart = c - '0';
							break;
					case '-':	break;
					default:
					fatal1("Invalid argument %c", c);
								break;
				}
			}
		}
		else
			dev--;

		if ((strstr(tmp, "cp")) || (strstr(tmp, "copy"))) {
			if (!strstr(tmp, "dir"))
				sflag++;
			if (dev+2 > argc)
				fatal1("Copy must include both a source and a"
							" destination\n");
			c=0;
			for (j=dev; j < argc; j++)
				if (strchr(argv[j], ':'))  {
					k++;
					c=j;
				}
			if (k>1)
				fatal1("Copy can only specify one DOS devic"
							    "e at a time.\n");
			else if (!k)
				fatal1("Use cp for COHERENT-COHERENT copies.\n");
			if (c == dev) {
				mode = 0;
				fun = extract;
				device = tran_dev(argv[dev]);
				if((argv[dev] = tran_file(argv[dev])) != NULL)
					tv=1;
				else
					tv=0;
				tmp = &argv[dev];
				base1 = argv[dev+1];
			}
			else {
				fun = replace;
				device = tran_dev(argv[argc-1]);
				base1 = tran_file(argv[argc-1]);
				argv[argc-1] = NULL;
				tv = argc - (1 + dev);
				tmp = &argv[dev];
			}
		}
		else if (strstr(tmp, "format")) {
			tv=argc - (2 + dargs);
			tmp = &argv[dev+1];
			device = tran_dev(argv[dev]);
			fun = format;
		}
		else if (strstr(tmp, "mkdir")) {
			if (argc > 2 + dargs)
				fatal("Too many arguments for MKDIR\n");

			device = tran_dev(argv[dev]);
			if ((argv[dev] = tran_file(argv[dev])) != NULL)
				tv=1;
			else
				fatal("Must specify a directory to create.\n");
			tmp = &argv[dev];
			fun = createdir;
		}
		else if (strstr(tmp, "cat")) {
			if (argc > 2 + dargs)
				fatal1("Too many arguments for doscat\n");

			device = tran_dev(argv[dev]);
			if ((argv[dev] = tran_file(argv[dev])) != NULL)
				tv=1;
			else
				fatal1("Must specify a file to display.\n");
			tmp = &argv[dev];
			fun = extract;
			pflag++;
			mode = 0;
		}
		else if ((strstr(tmp, "rm")) || (strstr(tmp, "del"))) {
			device = tran_dev(argv[dev]);
			if ((argv[dev] = tran_file(argv[dev])) != NULL)
				tv=1;
			else
				tv=0;
			tmp = &argv[dev];
			fun = delete;
		}
		else if ((strstr(tmp, "dir")) || (strstr(tmp, "ls"))) {
			device = tran_dev(argv[dev]);
			if ((argv[dev] = tran_file(argv[dev])) != NULL)
				tv=1;
			else
				tv=0;
			tmp = &argv[dev];
			fun = table;
			mode = 0;
		}
		else if (strstr(tmp, "label")) {
			tv=argc - (2 + dargs);
			tmp = &argv[dev+1];
			device = tran_dev(argv[dev]);
			fun = label;
		}
		else
			usage("Invalid command.\n\n");
	}
	else {
		oldstyle = 1;
		usagemsg = OUSAGE;
		if (argc < 2)
			usage(NULL);
		if (argc == 2)
			++argc;
		else
			device = argv[2];
		mode = key(argv[1]);
		tv = argc - 3;
		tmp = &argv[3];
		strcpy(adev, device);
	}

	make_lock();
	if ((fsfd = open(device, mode)) < 0)
		fatal("cannot open device %s", device);

	if (fun != format) {
		/* Read the FAT and the root directory. */
		readfat();
		root = newdir(NULL, NULL, bpb->b_files);
		readmdir(root);
	}
	numargs = tv;
	(*fun)(tv , tmp);
	if (fun != format) {
		writedir(root);
		if (fatcflag)
			writefat();
		free(fatcache);
	}
	rm_lock();
	exit(estat);
}


#define DEFFILE "/etc/default/msdos"
char adev[80], dev[80];
char file[80];

char *tran_dev(arg) char *arg;
{
	FILE *fp;
	char *b, c[80];
	short found = 0;

	strcpy(dev, arg);
	if ((b = strchr(dev, ':')) == NULL)
		fatal1("Error: No device specified\n");

	*b = '\0';

	strcpy(adev, dev);
	if (adev[0] != '/')
		adev[0] = toupper(adev[0]);

	if ((fp = fopen(DEFFILE, "r")) != NULL) {
		while (!feof(fp)) {
			if(fgets(c, 79, fp)) {
				if ((c[0] != '#') && (c[0] != '\n')) {
					if (c[0] == '.') {
						short i = 0;
						while (c[i] != '\n')
							ext[sext++] = c[i++];
					}
					else if (!found){
					   if (b = strchr(c, '=')) {
						*b++ = '\0';
						if (!strcmp(c, dev)) {
							*strpbrk(b,"\t\n #")='\0';
							strcpy(dev, b);
							if(b=strchr(dev,';')){
								*b='\0';
								xpart=atoi(b+1);
							}
							found++;
						}
					    }
					}
				}
			}
		}
	}
	fclose(fp);
	ext[sext++] = '.';
	ext[sext++] = '\0';
	return dev;
}


char *tran_file(arg) char *arg;
{
	char *t;

	t = strchr(arg, ':') + 1;
	if (*t == '/')
		t++;
	if (*t) {
		if (strcpy(file, t));
		return file;
	}
	else
		return NULL;
}


#define SPOOLDIR	"/tmp"
#define	LOCKPRE	"LCK.."
char lockfn[64];

make_lock()
{
	short lockfd, i;
	char pidstring[6];
	char *tmp = strrchr(device, '/') + 1;

#ifdef _I386
	for (i=1; i < 13; i++)
#else
	for (i=1; i < 12; i++)
#endif
		signal(i, die_signal);

	sprintf(lockfn, "%s/%s%s", SPOOLDIR, LOCKPRE, tmp ? tmp : device);
	if ((lockfd = open(lockfn, O_RDONLY)) != -1)
	{
		if ((read(lockfd, &i, sizeof(int)) != sizeof(int))
		     || (kill(i, 0) == -1))
		 {
			 close(lockfd);
		 }
		 else
			 fatal1("Device %s is in use.\n", device);
	}
	if((lockfd=creat(lockfn, 0644)) == -1)
		fatal1("Cannot lock Device %s.\n", device);

	sprintf(pidstring, "%d", getpid());
	write(lockfd, pidstring, strlen(pidstring));
	close(lockfd);
}

short die_signal(s) short s;
{
	fflush(stdout);
	fatal("received signal %d, quitting.\n", s);
}

rm_lock()
{
	short lockfd;	/* pointer to file to read */
	int chars_read;
	char gotpid[7];	/* String of the PID that was in the lockfile */
	
	/* open the lock file for read, abort on failure */
	if(-1 == (lockfd = (open(lockfn, 0))))
		fatal1("Error opening %s\n", argv0, lockfn);

	/* read the contents of the file. Abort if empty */
	if ( -1 == (chars_read = read(lockfd, gotpid, 6))) {
		close(lockfd);
		fatal1("Error reading %s\n", lockfn);
	}

	gotpid[chars_read] = '\0';	/* NUL terminate the string.  */
	if (atoi(gotpid) != getpid()){
		close(lockfd);
		fatal1("%s is not our lock file\n", lockfn);
	}else{
		if ( unlink(lockfn) < 0 ) {
			close(lockfd);
			fatal1("Can't unlink: %s\n", lockfn);
		}
		close(lockfd);
		return( 0 );
	}
}

/* end of dos0.c */







