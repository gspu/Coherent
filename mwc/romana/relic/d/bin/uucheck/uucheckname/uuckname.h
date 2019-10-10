/* sitename.h - include file for chk_sitename.c 
 * part of uucheck
 */

#ifndef EXTERN
#	ifdef MAIN
#		define EXTERN
#		define EQUALS(v)	= v
#	else
#		define EXTERN	extern
#		define EQUALS(v)
#	endif
#endif EXTERN

#ifdef MAIN
char *well_known_names[] = {
	"clout",
	"decwrl",
	"hp",
	"kgbvax",
	"prep",
	"seismo",
	"ucbvax",
	"uunet",
	NULL
}; /* char *well_known_names[] */

char *yes_table[] = {
	"y",
	"Y",
	"yes",
	"Yes",
	"YES",
	"true",
	"True",
	"TRUE",
	"sure",
	"yep",
	"ok",
	"uhhuh",
	"fersher",
	NULL
}; /* char *yes_table[] */

char *no_table[] = {
	"n",
	"N",
	"no",
	"No",
	"NO",
	"false",
	"False",
	"FALSE",
	"nah",
	"nope",
	"uhuh",
	"noway",
	"getoutahere",
	NULL
}; /* char *no_table[] */

#else
extern char *well_known_names[];
extern char *yes_table[];
extern char *no_table[];
#endif

char *lookup();
