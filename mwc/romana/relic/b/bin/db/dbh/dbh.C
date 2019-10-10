// File:	dbh.C
//
// Purpose:	main file for C++ debugger


// ----------------------------------------------------------------------
// Includes.

#include <coff.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stream.h>
#include <string.h>
#include "sys.h"

// ----------------------------------------------------------------------
// Definitions.
//	Constants.
//	Macros with argument lists.
//	Typedefs.
//	Enums.

const int	STR_LEN = 256;	// general-purpose buffer length
const int	MAX_ARGS = 10;	// limit on number of args, minus switches

// command syntax in effect (command line parser selects one of these)
const int	DBH_NULL = 0;	// no valid syntax
const int	DBH_COFF = 1;	// dbh <coff file>

// Class for keeping track of the main coff file under investigation.
class CoffObj {
public:		// public fields
	char *		fname;	// file name
public:		// constructors/destructors
	CoffObj(void): fname(NULL) {}
};

// Class for command line parser.
class DbhCmd {
public:		// public fields
	int		cmdSyntax;
	const char *	args[MAX_ARGS];

public:		// constructors/destructors
	DbhCmd(void): cmdSyntax(DBH_NULL) { args[0] = NULL; }
};

// ----------------------------------------------------------------------
// Functions.
//	Import Functions.
//	Export Functions.
//	Local Functions.

void	coffLoad(const char *);
void	cmdLine(int argc, char * argv[], DbhCmd &);
void	dbhCoff(DbhCmd &);
void	doCmd(const char *);
void	doCmdStr(const char *);
void	fatal(const char *);
void	usage(void);

// ----------------------------------------------------------------------
// Global Data.
//	Import Variables.
//	Export Variables.
//	Local Variables.

const char *	cmd;			// alias for argv[0]
char		errstr[STR_LEN];		// error message buffer

// ----------------------------------------------------------------------
// Code.

// command line forms:
//	dbh <coff file>

int main(int argc, char * argv[]) {

	DbhCmd	dbhCmd;

	// process command line
	cmdLine(argc, argv, dbhCmd);

	// do whatever command syntax dictates
	switch(dbhCmd.cmdSyntax) {
	case DBH_COFF:		// dbh <coff file>
		dbhCoff(dbhCmd);
		break;
	}
}

// Print a string to stderr and abort the debug session.
void
fatal(const char * msg) {
	cerr << cmd << " error: " << msg << endl;
	exit(1);
}

// Given raw command arguments, process switches and parse
// the line to determine the type of db session.  Return
// parse info in "dbhCmd".
void
cmdLine(int argc, char * argv[], DbhCmd & dbhCmd) {

	// copy command name to "cmd", mainly for error messages
	cmd = argv[0];

	// check number of arguments
	if (argc != 2) {
		usage();
	}

	// get command syntax and set args for subcommand
	dbhCmd.cmdSyntax = DBH_COFF;
	dbhCmd.args[0] = argv[1];
}

// Display usage message and abort the debug session.
void
usage(void) {
	cerr << "Usage:\t" << cmd << " <coff program>" << endl;
	exit(1);
}

// Run a dbh session for command syntax
//	dbh <coff file>
void
dbhCoff(DbhCmd & dbhCmd) {
	// get needed info from coff file
	coffLoad(dbhCmd.args[0]);

	// enter command loop
	doCmdStr(":x");
}

// Load info from COFF binary.
void
coffLoad(const char * fname) {
	FILEHDR coffh;
	int	coff_fd;

	if ((coff_fd = open(fname, O_RDONLY)) == -1) {
		sprintf(errstr, "Can't open object file %s", fname);
		fatal(errstr);
	}

	if (read(coff_fd, &coffh, sizeof(coffh)) != sizeof(coffh)) {
		sprintf(errstr, "Can't read object file %s", fname);
		fatal(errstr);
	} 

	if (coffh.f_magic != C_386_MAGIC) {
		sprintf(errstr, "Object file %s is not COFF", fname);
		fatal(errstr);
	} 

	getCoffSyms(coff_fd);

	getCoffSegs(coff_fd);

	close(coff_fd);
}

// Perform a db command string.
// String consists of zero or more commands separated by newlines.
void
doCmdStr(const char * cmdStr) {
	char	cmd1[STR_LEN];

	// copy each line, minus trailing newline, into "cmd1"
	// and invoke single command processor
	for (const char * cp = cmdStr; cp < cmdStr + strlen(cmdStr);) {
		const char * delim = strchr(cp, '\n');
		if (delim) {
			int copyLen = STR_LEN;
			if (delim - cp < copyLen)
				copyLen = delim - cp;
			strncpy(cmd1, cp, copyLen);
			doCmd(cmd1);
			cp = delim + 1;
		} else {
			strncpy(cmd1, cp, STR_LEN);
			doCmd(cmd1);
			break;
		}
	}
}

// Do a single db command.
void
doCmd(const char * cmdStr) {
	cout << cmdStr << endl;
}
