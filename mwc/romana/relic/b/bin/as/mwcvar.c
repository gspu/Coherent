/*
 * MWC K&R C version of error message functions.
 */
#include <asm.h>
#include <errno.h>

/*
 * Show line numbers if no listing.
 */
static void
showLine()
{
	if (!((NULL == inpc) || lswitchX))
		fprintf(errdev, "%d: %s: ", inpc->lineNo, inpc->name);
}

/*
 * Print error msg and die.
 */
/*VARARGS1*/
void
fatal(s)
char *s;
{
	int save = errno;

	if (Qswitch)
		exit(1);

	showLine();
	fprintf(errdev, "%r\n", &s);

	if (0 != (errno = save))
		perror("errno reports");

	exit(1);
}

/*
 * Print error msg to listing.
 */
/*VARARGS1*/
void
yyerror(s)
char *s;
{
	if (2 != pass)
		return;
	errCt++;
	sTitle();

	if (Qswitch)
		return;

	showLine();
	fprintf(errdev, "%r\n", &s);
}

/*
 * Print warning msg to listing.
 */
/*VARARGS1*/
void
yywarn(s)
char *s;
{
	if ((2 != pass) || wswitch || Qswitch)
		return;
	sTitle();

	showLine();
	fprintf(errdev, "Warning %r\n", &s);
}
