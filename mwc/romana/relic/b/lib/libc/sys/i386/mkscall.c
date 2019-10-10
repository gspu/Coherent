/* Copyright (c) Bureau d'Etudes Ciaran O'Donnell,1987,1990,1991 */
/*
 * mkscall.c - to make sys-call interface routine
 * usage : mkscall name number nargs [-r rel-num]
 *	   mkscall -f file
 */	
#include <stdio.h>
#include <ctype.h>

char *usage = "usage: mkscall name number nargs [flags] [-r rel]\n";

struct _args {

	char *a_name;	/* function name	*/	
	short a_num;	/* system call number   */
	short a_arg;	/* arg-count		*/ 
	short a_flag;	/* various flags - FLAG0 -> rval in d1.	*/
			/*		   FLAG1 -> arg = *arg	*/
	char *a_rel;	/* ident string		*/
};

#define FLAG0	0x01
#define FLAG1	0x02
 
main(argc, argv)
register argc;
register char *argv[];
{

	struct _args args;
	register char *fname = 0;	/* input file for arg list */

	if(argc < 4){

		if(argc == 3 && argv[1][0] == '-' && argv[1][1] == 'f') {
			fname = argv[2];
		}
		else {
			fprintf(stderr,usage);
			exit(1);
		}
	}

	
	if(fname == (char *)0) {

		args.a_name = argv[1];
		args.a_num = atoi(argv[2]);
		args.a_arg = atoi(argv[3]);
	
		if(argc > 4) {
			args.a_flag = atoi(argv[4]);
		}

		mk_func(&args);


	} else {

		register FILE *ffd;
		register lcount;
		register st;

		if((ffd = fopen(fname, "r")) == NULL) {

			fprintf(stderr, "mkscall: can't open %s\n", fname);
			exit(2);
		}

		for(lcount = 1; st = getargs(ffd, &args); lcount++) {

			if(st == -1)
				fprintf(stderr,
				"mkscall: file %s, line %d discarded\n",
							 fname, lcount);

			else
				mk_func(&args);
		}
	}

	exit(0);
}


/*
 * make system call routine.
 */


mk_func(arg)
register struct _args *arg;
{

	register FILE *fd;
	register count;
	char file[20];
	register flag;

	flag = arg->a_flag;

	if(arg->a_arg > 6) {
		fprintf(stderr, "mkscall: func %s, too much args\n",
							 arg->a_name);
		return(-1);
	}

	/* check arg count for flag = FLAG1 (arg = *arg) */
	if((flag & FLAG1) && (arg->a_arg > 1)) {

		fprintf(stderr,
		 "mkscall: func %s, only one arg allowed with flag 0x%02x.\n",
					 arg->a_name, flag & FLAG1);
		return(-1);
	}

	strcpy(file, arg->a_name);
	strcat(file, ".s");

	if((fd = fopen(file, "w")) == NULL) {

		fprintf(stderr, "mkscall: can't create %s\n", file);
		return(-1);
	}


	fprintf(fd, "/ C library - %s\n",  arg->a_name);

	fprintf(fd,"\n	.globl	%s", arg->a_name);
	fprintf(fd,"\n	.globl	.cerror");

	fprintf(fd,"\n\n%s:\n", arg->a_name);
	
	if(flag & FLAG1) {		/* arg must be *arg */

		fprintf(fd,"	movl	4(%%esp),%%edx\n");
		fprintf(fd,"	movl	(%%edx),%%edx\n");
		fprintf(fd,"	movl	%%edx,4(%%esp)\n");
	}

	/* put system-call number in d0	*/
	fprintf(fd, "	movl	$%d,%%eax\n", arg->a_num);
	fprintf(fd, "	lcall	$0x7,$0\n");
	fprintf(fd,"	jc	.cerror\n");

	if(flag & FLAG0) {		/* rval in d1 */
 
		fprintf(fd, "\n	movl	%%edx,%%eax\n");  
	}

	fprintf(fd,"	ret\n");
	fflush(fd);
	fclose(fd);

	return(0);
}

/*
 * get a line from input file and cracks args.
 */

#define BFS 20
char bfnam[BFS];

getargs(fd, arg) 	
register FILE *fd;
register struct _args *arg;
{
	register char buff[80];

	arg->a_name = NULL;
	arg->a_num = arg->a_arg = arg->a_flag = 0;

more :	if(fgets(buff, 80, fd) == NULL) {

		return(0);
	}

	else {

		char *get_word();
		register char *w;


		if(*buff == '#') {
			goto more;		/* skip comment line */
		}

		if(w = get_word(buff)) {
			strncpy(bfnam, w, BFS-1);
			arg->a_name = bfnam;	/* get func name */
		}
		else
			goto error;

		if(w = get_word(w + strlen(w) + 1))
			arg->a_num = atoi(w);	/* get sys-call number */
		else
			goto error;

		if(w = get_word(w + strlen(w) + 1))
			arg->a_arg = atoi(w);	/* get arg count */
		else
			goto error;

		if(w = get_word(w + strlen(w) + 1))
			arg->a_flag = atoi(w);	/* get flags	*/

		
		return(1);
	}

error :
	return(-1);
}


/*
 * get next string from buff and terminate it by a null.
 */

char * 
get_word(buff)
register char *buff;
{
	register char *w = (char *)0;
	register c;

	while((c = *buff)) {

		if(c == '#')		/* rest of the line is comment */
			break;

		if(isspace(c)) {

			if(w)
			{
				*buff = '\0';
				break;
			}

		} else if(!w)
			w = buff;

		buff++;
	}

	return(w);
}
