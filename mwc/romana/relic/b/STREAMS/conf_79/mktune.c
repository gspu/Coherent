/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		LOCAL
 *		USE_PROTO
 *		ARGS ()
 *	<stdio.h>
 *		NULL
 *		FILE
 *		stdout
 *		fopen ()
 *		fclose ()
 *		fprintf ()
 *	<time.h>
 *		time_t
 *		localtime ()
 *		time ()
 *		strftime ()
 *	<limits.h>
 *		INT_MAX
 *		INT_MIN
 */

#include <sys/compat.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "mtune.h"
#include "stune.h"
#include "symbol.h"
#include "ehand.h"

#include "mkconf.h"


/*
 * Write out C-language definitions for the values of the tunable parameters.
 */

#ifdef	USE_PROTO
LOCAL void (write_params) (FILE * out)
#else
LOCAL void
write_params ARGS ((out))
FILE	      *	out;
#endif
{
	mtune_t	      *	scan;
	enum {
		FIRST,
		REST
	} state = FIRST;

	for (scan = mtunes () ; scan != NULL ; scan = scan->mt_next) {
		long	value;

		/*
		 * Parameters that are within the range of representation of
		 * an integer go into an enumeration. Unless this is Coherent,
		 * numbers larger than that become "static const long".
		 *
		 * Note that Coherent compiles comparisons to INT_MIN into
		 * broken code as of this writing (April '93).
		 */

		value = scan->mt_stune == NULL ? scan->mt_default :
				scan->mt_stune->st_value;

#ifndef	__COHERENT__
		if (value < INT_MIN || value > INT_MAX) {
			if (state == REST)
				fprintf (out, "\n};\n");
			state = FIRST;

			fprintf (out, "#ifdef\t__cplusplus\n");
			fprintf (out, "static const long %s = %ld;\n",
				 scan->mt_name->s_data,	value);
			fprintf (out, "#else\n");
			fprintf (out, "#define %s %ldL\n",
				 scan->mt_name->s_data,	value);
			fprintf (out, "#endif\n");
		} else {
#endif
			fprintf (out, state == FIRST ? "enum {\n" : ",\n");
			fprintf (out, "\t%s = %ld", scan->mt_name->s_data,
				 value);
			state = REST;
#ifndef	__COHERENT__
		}
#endif
	}

	if (state == REST)
		fprintf (out, "\n};\n\n");
}


/*
 * Write out a C-language header file with definitions for the values of all
 * the tunable parameters in the system.
 */

#ifdef	__USE_PROTO__
int (write_conf_h) (CONST char * name)
#else
int
write_conf_h ARGS ((name))
CONST char    *	name;
#endif
{
	time_t		gentime;
	char		timebuf [70];
	FILE	      *	out;
	ehand_t		err;

	if (name == NULL)
		out = stdout;
	else if ((out = fopen (name, "w")) == NULL)
		throw_error ("Unable to open output file for writing");

	if (PUSH_HANDLER (err) == 0) {
		time (& gentime);

		fprintf (out, "#ifndef\t__SYS_CONF_H__\n");
		fprintf (out, "#define\t__SYS_CONF_H__\n\n");

		fprintf (out, "/*\n");
		fprintf (out, " * The code in this file was automatically "
			      "generated. Do not hand-modify!\n");

#ifdef	__COHERENT__
		strncpy (timebuf, asctime (localtime (& gentime)),
			 sizeof (timebuf) - 1);
		timebuf [sizeof (timebuf) - 1] = 0;
#else
		strftime (timebuf, sizeof (timebuf) - 1, "%x %X %Z",
			  localtime (& gentime));
#endif

		fprintf (out, " * Generated at %s\n", timebuf);
		fprintf (out, " */\n\n");

		write_params (out);

		fprintf (out, "#endif\t/* ! defined (__SYS_CONF_H__) */\n");

		if (out != stdout)
			fclose (out);
	} else {
		if (out != stdout)
			fclose (out);
		CHAIN_ERROR (err);
	}

	POP_HANDLER (err);
	return 0;
}
