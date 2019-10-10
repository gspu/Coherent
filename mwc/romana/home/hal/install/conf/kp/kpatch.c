/*
 * File:	kpatch.c
 *
 * Purpose:
 * This program is not expected to work other than on PC Coherent.
 * Certain hot patches may not be effective, since some values are only
 * referenced once at system initialization.
 *
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <stdio.h>
#include <errno.h>
#include <sys/patch.h>
#include "patchl.h"

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define PATCH_DEVICE_NAME	"/dev/patch"

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */

void main();
int arg_getc();

static int kpatch();
static void usage();
static int peek_patch();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
extern struct PatchInfo argInfo;	/* yyparse puts info here. */

static char * lex_input;
static int lex_index;

static int verbose = 0;			/* Are we printing feedback?  */
static int kfd;				/* File descriptor for patch device. */

static char version[] =	"kpatch version 1.0 for COHERENT v.4.2";

static char short_helpmessage[] = "\
kpatch -- alter installation kernel\n\
Usage: kpatch [-v] \n\
	{ symbol? | symbol=value | symbol:maj:in | symbol:maj:out } ...\n\
";

static char helpmessage[] = "\
	-v		Verbose mode--print what's being done.\n\
	symbol?		Display present value of symbol.\n\
	symbol=value	Store value into symbol.\n\
	symbol:maj:in	If major number maj is not already in use,\n\
			attach CON struct symbol to driver table at major\n\
			number maj and call the load routine for the driver.\n\
	symbol:maj:out	If named CON symbol is installed at major number maj,\n\
			call unload routine for the driver and remove the\n\
			driver from the driver table.\n\
'Value' is a numeric constant (decimal by default, hexadecimal if preceded\n\
by '0x', octal if preceded by '0' or '0o'.\n\
'Value' may also be makedev(major-number, minor-number).\n\
";

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*************************************************************************
 * main
 ************************************************************************/
void
main(argc, argv)
int argc;
char *argv[];
{
	int c;			/* For reading options from getopt().  */
	int i;
	int patch_err;
	char errmsg[80];

	extern int optind;
	extern char *optarg;

	while ((c = getopt(argc, argv, "vh")) != EOF) {
		switch (c) {
		case 'v':
			verbose++;
			break;
		case 'h':
			fprintf(stderr, "%s\n", version);
			usage(1);	/* Does not return.  */
		default:
			usage(0);	/* Does not return.  */
		}
	}

	if ((kfd = open(PATCH_DEVICE_NAME, 0)) == -1) {
		sprintf(errmsg, "%s: device %s",
		argv[0], PATCH_DEVICE_NAME);
		perror(errmsg);
		exit(1);
	}

	/*
	 * There must be at least 2 arguments left.
	 */
	if (argc - optind < 1) {
		fprintf(stderr, "Missing arguments.\n");
		usage(0);	/* Does not return */
	}

	/*
	 * Handle patch symbols one at a time.
	 * If an error occurs, display failure and save error status
	 * for exit value.
	 */
	patch_err = 0;
	for (i = optind;  i < argc;  i++) {
		if (kpatch(argv[i])) {
			sprintf(errmsg, "%s: %s", argv[0], argv[i]);
			if (errno)
				perror(errmsg);
			else
				fprintf(stderr, "ERROR in %s: %s",
				argv[0], argv[i]);
			patch_err = 1;
		}
	}
	close(kfd);
	exit(patch_err);
}

/*************************************************************************
 * kpatch
 *
 * Given a patch argument, attempt to do the patch.
 * Return 0 if successful, -1 if failure.
 ************************************************************************/
static int
kpatch(patch_arg)
char * patch_arg;
{
	int result;

	/*
	 * Parse patch argument.
	 * If its type can be identified, pass the parsed info to
	 * the required action routine.
	 */
	lex_input = patch_arg;
	lex_index = 0;
	argInfo.piType = bad_arg;
	if(yyparse())
		result = -1;
	else {	/* parse succeeded */
		pDump();
		switch(argInfo.piType) {
		case peek_arg:
			result = peek_patch(&argInfo);
			break;
		case assign_arg:
			result = assign_patch(&argInfo);
			break;
		case con_in_arg:
			result = con_in_patch(&argInfo);
			break;
		case con_out_arg:
			result = con_out_patch(&argInfo);
			break;
		default:
			result = -1;
		}
	}

	return result;
}

/*************************************************************************
 * usage
 *
 * Display usage message and terminate abnormally.
 ************************************************************************/
/*
 */
void
usage(verbose)
int verbose;
{
	fprintf(stderr, short_helpmessage);
	if (verbose) {
		fprintf(stderr, helpmessage);
	}
	exit(1);
}

/*************************************************************************
 * arg_getc
 *
 * This replaces the default lex input function, taking input from global
 * character pointer lex_input at position lex_index.
 ************************************************************************/
int
arg_getc()
{
	int result;

	/*
	 * Fetch the next available character.
	 * If at end of string
	 *   Return EOF.
	 * Else
	 *   Advance character pointer.
	 */
	result = lex_input[lex_index];
	if (result == 0)
		result = -1;
	else
		lex_index++;
	return result;
}

/*************************************************************************
 * assign_patch
 *
 * Try to modify a kernel variable.
 * Return 0 if success, -1 if failure.
 ************************************************************************/
static int
assign_patch(pip)
struct PatchInfo * pip;
{
	struct patchVar pvar;
	int result;
	int retval = -1;

	strncpy(pvar.patch_vname, pip->piData.assignData.vname,
	  PATCH_VAR_NAME_LENGTH);
	pvar.patch_data = & pip->piData.assignData.newValue;
	result = ioctl(kfd, PATCH_WR, &pvar);

	if (result >= 0 && verbose) {
		printf("Value assigned: %s=0x%08x\n",
		pvar.patch_vname, pip->piData.assignData.newValue);
		retval = 0;
	}

	return retval;
}

/*************************************************************************
 * con_in_patch
 *
 * Try to install a device driver.
 * Return 0 if success, -1 if failure.
 ************************************************************************/
static int
con_in_patch(pip)
struct PatchInfo * pip;
{
	struct patchCon pcon;
	int result;
	int retval = -1;

	strncpy(pcon.patch_vname, pip->piData.conData.vname,
	  PATCH_VAR_NAME_LENGTH);
	pcon.patch_maj = pip->piData.conData.maj;
	result = ioctl(kfd, PATCH_CON_IN, &pcon);

	if (result >= 0 && verbose) {
		printf("Device %s attached at major number %d\n",
		pcon.patch_vname, pcon.patch_maj);
		retval = 0;
	}

	return retval;
}

/*************************************************************************
 * con_out_patch
 *
 * Try to uninstall a device driver.
 * Return 0 if success, -1 if failure.
 ************************************************************************/
static int
con_out_patch(pip)
struct PatchInfo * pip;
{
	struct patchCon pcon;
	int result;
	int retval = -1;

	strncpy(pcon.patch_vname, pip->piData.conData.vname,
	  PATCH_VAR_NAME_LENGTH);
	pcon.patch_maj = pip->piData.conData.maj;
	result = ioctl(kfd, PATCH_CON_OUT, &pcon);

	if (result >= 0 && verbose) {
		printf("Device %s detached from major number %d\n",
		pcon.patch_vname, pcon.patch_maj);
		retval = 0;
	}

	return retval;
}

/*************************************************************************
 * peek_patch
 *
 * Try to examine a kernel variable.
 * Return 0 if success, -1 if failure.
 ************************************************************************/
static int
peek_patch(pip)
struct PatchInfo * pip;
{
	int vdata = 0;
	struct patchVar pvar;
	int result;
	int retval = -1;

	strncpy(pvar.patch_vname, pip->piData.peekData.vname,
	  PATCH_VAR_NAME_LENGTH);
	pvar.patch_data = &vdata;
	result = ioctl(kfd, PATCH_RD, &pvar);

	if (result >= 0) {
		printf("Value inspected: %s=0x%08x\n", pvar.patch_vname, vdata);
		retval = 0;
	}

	return retval;
}

pDump()
{
#if 0
	switch(argInfo.piType) {
	case peek_arg:
		printf("What is the value of %s?\n",
		  argInfo.piData.peekData.vname);
		break;
	case assign_arg:
		printf("Set %s equal to %d\n",
		  argInfo.piData.assignData.vname,
		  argInfo.piData.assignData.newValue);
		break;
	case con_in_arg:
		printf("Attach device %s to major number %d\n",
		  argInfo.piData.conData.vname,
		  argInfo.piData.conData.maj);
		break;
	case con_out_arg:
		printf("Detach device %s from major number %d\n",
		  argInfo.piData.conData.vname,
		  argInfo.piData.conData.maj);
		break;
	default:
		printf("Bad argument! %s\n", lex_input);
	}
#endif
}
