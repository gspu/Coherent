/*
 * Rec'd from Lauren Weinstein, 7-16-84.
 * Substitute user-id temporarily or become super user (as you wish).
 * Compile -s -n -i.
 * Hacked by steve 10/4/90 to correct password bug and for clarity.
 */

#include <stdio.h>
#include <pwd.h>

extern	char *getenv();
extern	char *getpass();
extern	char *malloc();

#define ACCNAME "remacc"	/* Remote access password dummy username */
#define	DEFSHELL "/bin/sh"	/* Default shell pathname */
#define PASSLEN 13		/* Encrypted password length */

#define	bye()	fatal("Sorry")

/* Forward. */
extern	void	addenviron();
extern	void	fatal();
extern	void	getuname();

/* Globals. */
char	*defargs[] = { "su", NULL };
/* The following are all set by getuname(). */
short	gid;
char	*password;
char	salt[3];
short	uid;

main(argc, argv) int argc; char *argv[];
{
	register int ouid, ogid;
	char *command, *prompta, *promptb, *passp;
	char **args;

	getuname(argc>1 ? argv[1] : "0");	/* check username */
	if (password[0] != '\0' && getuid()) {	/* check password if not already su */
		passp = getpass("Password: ");	/* get input password choice */
		if ((strlen(password) != PASSLEN)
		 || (strcmp(crypt(passp, salt), password) != 0)) {
			ouid = uid;
			ogid = gid;
			getuname("0");		/* check root password too */ 
			if ((strlen(password) != PASSLEN)
			 || (strcmp(crypt(passp, salt), password) != 0))
				bye();		/* failure */
			uid = ouid;
			gid = ogid;
		}
	}

	if (argc > 2) {
		command = argv[2];
		args = &argv[2];
	} else {
		command = getenv("SHELL");
		if (command == NULL || strlen(command) < 1)
			command = DEFSHELL;
		args = defargs;
	}
	setgid(gid);
	setuid(uid);
	prompta = getenv("PSN");		/* check for normal prompt */
	promptb = getenv("PSS");		/* check for desired su prompt */
	addenviron(uid == 0 ? (promptb ? promptb : "# ")
			    : (prompta ? prompta : "$ "));
						/* change prompt as appropriate */
	execvp(command, args);
	fatal("%s: not found", command);
}

/*
 * Add string 's' to the environment as "PS1".
 */
void
addenviron(s) char *s;
{
	extern char **environ;
	register char **epp1, **epp2;
	register char **newenv;
	int n;
	char *prompt;
	static char prbuf[50];

	for (epp1 = environ; *epp1!=NULL; epp1++)
		;
	n = (epp1-environ+2) * sizeof (char *);
	if ((newenv = (char **)malloc(n)) == NULL)
		fatal("Out of memory for environments");
	prompt = prbuf;
	strcpy(prompt, "PS1=");
	strcat(prompt, s);
	for (epp1=environ, epp2=newenv; *epp1 != NULL; epp1++)
		if (strncmp(*epp1, "PS1=", 4) != 0)
			*epp2++ = *epp1;
		else {
			*epp2++ = prompt;
			prompt = NULL;
		}
	*epp2++ = prompt;
	*epp2 = NULL;
	environ = newenv;
}

void
fatal(s) char *s;
{
	fprintf(stderr, "%r\n", &s);
	exit(1);
}

/*
 * Get a user-name from string 's'.
 * If the string starts with a numeric, use it directly as a uid.
 * Set globals password, salt[], gid and uid with the user's password info.
 * Die if not found or illegal.
 */
void
getuname(s) register char *s;
{
	register struct passwd *pwp;

	if (*s >= '0' && *s <= '9') {
		uid = atoi(s);
		if ((pwp = getpwuid(uid)) == NULL)
			fatal("%d: bad user number", uid);
	} else if ((pwp = getpwnam(s)) == NULL)
		fatal("%s: not a user name", s);
	if (strcmp(pwp->pw_name, ACCNAME) == 0)	/* dummy access username? */
		bye();				/* yes, sorry */
	password = pwp->pw_passwd;
	salt[0] = pwp->pw_passwd[0];
	salt[1] = pwp->pw_passwd[1];
	salt[2] = '\0';
	gid = pwp->pw_gid;
	uid = pwp->pw_uid;
}

/* end of su.c */
