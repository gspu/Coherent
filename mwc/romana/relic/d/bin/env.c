static char _version[]="env version 1.0";
/*
 * env - execute a command in an environment
 *
 * usage: env [-] [VARIABLE=value ...] [command args]
 *
 * Executes "command" with "args", modifying the existing environment
 * by doing the requested assignments.  The "-" option means that the
 * environment should be _replaced_ with the list assignements, otherwise
 * the assignments are added to the environment.
 * If "command" is ommitted, the resulting environment is printed.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BIG_NUMBER 2048
#define TRUE (1==1)
#define FALSE (1==0)
#define VAR_SEPERATOR '='	/* Seperates an environment variable
				   from its value.  */

void parse_args();
int arglen();
char **argcpy();
char **envncat();
int is_assignment();

/* Flags that tell us about our arguments */
int replace_env = FALSE;
int have_command = FALSE;

int assignment_count;	/* Number of assignments to add to environment.  */
char *first_assignment_argument = NULL;

int i;	/* Generic index, used repeatedly */

char **new_argv;	/* Build new command line here.  */
char **new_envp;	/* Build the new environment here.  */


void
main(argc, argv, envp)
	int argc;
	char *argv[];
	char *envp[];
{
/* Parse arguments.  */
	parse_args(argc, argv);

/* Build the new environment.  */

	if (! replace_env) {
		/* Make enough space for the disjoint union of the two sets of
		 * environment assignments.  This is a guarenteed upper bound.
		 */
		new_envp = (char **) malloc( sizeof(char *)
					     * (arglen(envp)
						+ assignment_count + 1)
			  		   );
		argcpy(new_envp, envp);
	} else {
		/* Otherwise, we only need enough space for the new
		 * environment.
		 */
		new_envp = (char **) malloc( sizeof(char *)
					     * (assignment_count + 1)
			  		   );
	} /* if (! replace_env) */


	envncat(new_envp, first_assignment_argument, assignment_count);


/* Do something--either a command or print the environment.  */
	if (have_command) {
		execvep(new_argv[0], new_argv, new_envp);
		perror(new_argv[0]);
		exit(1);
	} else {
		char **pntr;

		for (pntr = new_envp; *pntr != NULL; ++pntr){
			printf("%s\n", *pntr);
		} /* for pntr in new_envp */

	} /* if have_command */

	exit(0);
}


void
parse_args(argc, argv)
	int argc;
	char *argv[];
{
	int current_arg;

	if (argc > 1) {
		current_arg = 1; /* Start looking at the first argument.  */

		/* - option -- Replace the existing environment.  */
		if (strcmp(argv[current_arg], "-") == 0) {
			replace_env = TRUE;
			++current_arg;
		} /* if first argument is "-" */

		/* Count the number of arguments which are assignments.  */
		for (assignment_count = 0;
		     current_arg < argc;
		     current_arg++, assignment_count++) {

			if (! is_assignment(argv[current_arg])) {
				break;
			}

			if (first_assignment_argument == NULL) {
				first_assignment_argument = &argv[current_arg];
			}

		} /* for (Count the assignments on the command line.  ) */


		if ( current_arg < argc ) {
			/* We broke out of the assignment couting loop
			 * early--this must be a command.
			 */
			new_argv = &argv[current_arg];
			have_command = TRUE;
		} /* if ( current_arg <= argc ) */

	} /* if there are any args */

} /* void parse_args() */



/* Count the number of arguments in an argument vector.
 * arg_vector MUST be NULL terminated!
 */
int
arglen(arg_vector)
	char **arg_vector;
{
	int count;
	
	for (count = 0; arg_vector[count] != NULL; ++count) {
	}

	return(count);
} /* arglen() */


/* Copy src argument vector to dest argument vector (pointer copy only).
 */
char **
argcpy(dest, src)
	char **dest, **src;
{
	while (*src != NULL) {
		*dest++ = *src++;
	} /* while (walk two pointers down envp and new_envp) */
	*dest = NULL;
	return (dest);
} /* char **argcpy() */




/* Merge one environment onto another, overwriting duplicate
 * variable names.  Compare at most n patterns.
 */
char **
envncat(dest, src, n)
	char **dest, **src;
	int n;
{
	char **dest_walker;
	char *sep_loc;	/* Seperator ("=") location in assignment */
	int i;
	int name_size;	/* Number of characters in a given variable name */

	for (i=0; *src != NULL && i < n; ++i, ++src) {

		/* Extract the variable name from the assignment *src.  */
		if ((sep_loc = strchr(*src, VAR_SEPERATOR)) == NULL) {
			fprintf(stderr,
				"Programming error:  %s is not an assignment\n",
				*src);
			exit(1);
		}
		name_size = sep_loc - *src;
		
		/* Look for variable part of *src in dest.
		 * Point at end if not found.
		 */
		for(dest_walker = dest; *dest_walker != NULL; ++dest_walker) {

			/* NB: the + 1 is so the "=" is compared too.  */
			if(strncmp(*dest_walker, *src, name_size + 1) == 0) {
				/* We have found an exact match.  */
				break;
			} /* if *dest_walker and *src are same variable */

		} /* for Walk down dest environment */

		/* Checkpoint - *dest_walker either matches *src or is NULL.  */

		/* WARNING: possible memory leak--
		 *	    *dest_walker is always clobbered.
		 *	     Usually *dest_walker was copied from global
		 *	     envp, so it isn't really lost memory.
		 */
		*dest_walker = *src;

	} /* for (i=0; *src != NULL && i < n; ++i, ++src) */

	return(dest);
} /* char ** envncat() */

int
is_assignment(s)
	char *s;
{
	if ( strchr(s, VAR_SEPERATOR) != NULL ) {
		return(TRUE);
	} else {
		return(FALSE);
	}

} /* int is_assignment() */
