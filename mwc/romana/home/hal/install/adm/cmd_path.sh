# (-lgl
#	Coherent 386 release 4.2
#	Copyright (c) 1982, 1994 by Mark Williams Company.
#	All rights reserved. May not be copied without permission.
#	For copying permission and licensing info, write licensing@mwc.com
# -lgl)
# Call with the $0 of the script as a parameter; this commands writes the
# name of the directory containing the script to standard output.
# Example:
#	cmd_path $0

cmd_path () {

	# if arg passed contains a slash 
	#   strip smallest suffix
	# else if "which" finds a path to it with a slash
	#   strip suffix from result of "which"
	# else use current directory

	case $1 in
	*/*) echo "${1%/*}"
		;;
	*)   WHICH=`which $1 2>/dev/null`
		case $WHICH in
		*/*) echo "${WHICH%/*}"
			;;
		*)   echo .
			;;
		esac
		;;
	esac

	return 0
}
