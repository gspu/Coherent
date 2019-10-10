/*********************************************************************
*                         COPYRIGHT NOTICE                           *
**********************************************************************
*        This software is copyright (C) 1982 by Pavel Curtis         *
*                                                                    *
*        Permission is granted to reproduce and distribute           *
*        this file by any means so long as no fee is charged         *
*        above a nominal handling fee and so long as this            *
*        notice is always included in the copies.                    *
*                                                                    *
*        Other rights are reserved except as explicitly granted      *
*        by written permission of the author.                        *
*                Pavel Curtis                                        *
*                Computer Science Dept.                              *
*                405 Upson Hall                                      *
*                Cornell University                                  *
*                Ithaca, NY 14853                                    *
*                                                                    *
*                Ph- (607) 256-4934                                  *
*                                                                    *
*                Pavel.Cornell@Udel-Relay   (ARPAnet)                *
*                decvax!cornell!pavel       (UUCPnet)                *
*********************************************************************/

/*
 *	comp_main.c --- Main program for terminfo compiler
 *
 *  $Log:	comp_main.c,v $
 * Revision 1.1  92/03/13  10:45:42  bin
 * Initial revision
 * 
 * Revision 2.1  82/10/25  14:45:37  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:16:37  pavel
 * Beta-one Test Release
 * 
 * Revision 1.3  82/08/23  22:29:36  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.2  82/08/19  19:09:49  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/12  18:36:55  pavel
 * Initial revision
 * 
 *
 */

#ifndef COHERENT
static char RCSid[] =
	"$Header: /src386/usr/bin/tic/RCS/comp_main.c,v 1.1 92/03/13 10:45:42 bin Exp $";
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include "compiler.h"

char	*source_file = "/etc/terminfo";
char	*destination = SRCDIR;
char	*usage_string = "\ttic [-v[n]] source-file\n";
char	check_only = 0;


main (argc, argv)
int	argc;
char	*argv[];
{
	int	i;
	int	argflag = FALSE;

	debug_level = 0;

	for (i=1; i < argc; i++)
	{
	    if (argv[i][0] == '-')
	    {
		switch (argv[i][1])
		{
		    case 'c':
			check_only = 1;
			break;

		    case 'v':
			debug_level = argv[i][2]  ?  atoi(&argv[i][2])  :  1;
			break;

		    default:
			fprintf(stderr, "%s: Unknown option. Usage is:\n\t%s\n",
						       argv[0], usage_string);
			exit(1);
		}
	    }
	    else if (argflag)
	    {
		fprintf(stderr, "%s: Too many file names.  Usage is:\n\t%s\n",
							argv[0], usage_string);
		exit(1);
	    }
	    else
	    {
		argflag = TRUE;
		source_file = argv[i];
	    }
	}

	init(argv[0]);
	make_hash_table();
	compile();

	exit(0);
}




/*
 *	init(progname)
 *
 *	Miscelaneous initialisations
 *
 *	Open source file as standard input
 *	Check for access rights to destination directories
 *	Create any directories which don't exist.
 *
 */

init(progname)
char	*progname;
{
	struct stat	statbuf;
	char		*dirnames = "abcdefghijklmnopqrstuvwxyz" 
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char		*getenv();
	char		dir[2];

	start_time = time(0);

	curr_line = 0;

	if (freopen(source_file, "r", stdin) == NULL)
	{
	    fprintf(stderr, "%s: Can't open %s\n", progname, source_file);
	    exit(1);
	}

	if (getenv("TERMINFO") != NULL)
	    destination = getenv("TERMINFO");

	if (access(destination, 7) < 0)
	{
	    fprintf(stderr, "%s: %s non-existant or permission denied\n",
							progname, destination);
	    exit(1);
	}

	if (chdir(destination) < 0)
	{
	    fprintf(stderr, "%s: %s is not a directory\n",
							progname, destination);
	    exit(1);
	}
	
	dir[1] = '\0';
	for (dir[0] = *dirnames; *dirnames != '\0'; dir[0] = *(++dirnames))
	{
	    if (stat(dir, &statbuf) < 0)
	    {
		mkdir(dir);
		chmod(dir, 0755);
	    }
	    else if (access(dir, 7) < 0)
	    {
		fprintf(stderr, "%s: %s/%s: Permission denied\n",
						    progname, destination, dir);
		exit(1);
	    }
	    else if ((statbuf.st_mode & S_IFMT) != S_IFDIR)
	    {
		fprintf(stderr, "%s: %s/%s: Not a directory\n",
						    progname, destination, dir);
		exit(1);
	    }
	}
}



/*
 *	mkdir(dirname)
 *
 *	forks and execs the mkdir program to create the given directory
 *
 */

mkdir(dirname)
char	*dirname;
{
	int	fork_rtn;
	int	status;

	fork_rtn = fork();

	switch (fork_rtn)
	{
	    case 0:		/* Child */
		execl("/bin/mkdir", "mkdir", dirname, 0);
		exit(1);

	    case -1:		/* Error */
		fprintf(stderr, "tic: SYSTEM ERROR!! Fork failed!!!\n");
		abort();

	    default:
		wait(&status);
		if (status != 0)
		    syserr_abort("mkdir returned bad status");
		break;
	}
}
