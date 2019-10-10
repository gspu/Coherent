/*
 * UNIX K&R version of error message functions.
 */
#include <varargs.h>

/*
 * Put message and die.
 */
/*VARARGS*/
void
fatal(va_alist)
va_dcl
{
	va_list args;
	char *fmt;
		
	if(NULL != inpc)
		printf("%d: %s: ", inpc->lineNo, inpc->name);	/* NODOC */

	va_start(args);
	fmt = va_arg(args, char *);
	vprintf(fmt, args);
	va_end(args);

	putchar('\n');
	exit(1);
}

/*
 * Put message to listing.
 */
/*VARARGS*/
void
yyerror(va_alist)	/* NODOC */
va_dcl
{
	va_list args;
	char *fmt;
		
	if(2 != pass)
		return;
	sTitle();
	if(NULL != inpc)
		printf("%d: %s: ", inpc->lineNo, inpc->name);

	va_start(args);
	fmt = va_arg(args, char *);
	vprintf(fmt, args);
	va_end(args);

	putchar('\n');
}
