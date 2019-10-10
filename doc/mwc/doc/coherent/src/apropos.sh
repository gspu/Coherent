#!/bin/sh
# COHERENT version of Berkeley command "apropos"
# Coherent 386 release 4.2
# Copyright (c) 1982, 1994 by Mark Williams Company.
# All rights reserved. May not be copied without permission.
#
# History:
# 03/17/1993: original version, written in C (fwb)
# 10/07/94: Rewrote in the shell; included support for $MANPATH

VERSION=2.0

# find which files are to be displayed;
# $1 is a copy of MANPATH; $2 is the token being sought
split_path () {
	while : ; do
		case "$1" in
		'' | :)	break
			;;

		:*)	set -- "${1#:}" $2
			;;

		*:*)	eval "find_descrip ${1%:"${1#*:}"} $2"
			set -- "${1#*:} $2"
			;;

		*)	eval "find_descrip $1 $2"
			break
			;;
		esac
	done
	return 1
}

# find descriptions for a given token.
# $1 is the directory whose index files are to be searched;
# $2 is the token being sought
find_descrip () {
	grep $2 $1/*.index |
		awk '{ for (i = 2 ; i <= NF; i++)
			if (i == 2)
				printf ("%s:\t", $i)
			else {
				if (i == NF)
					printf ("%s\n", $i)
				else
					printf ("%s ", $i) 
			}
		}'
}

# if environmental variable PAGER is not set, set it to "more"
if [ "$PAGER" = "" ]; then
	PAGER="more"
fi

# if environmental variable MANPATH is not set, set it to "/usr/man"
if [ "$MANPATH" = "" ]; then
	PAGER="/usr/man"
fi

# If no args, print help message and exit
if [ $# -eq 0 ]; then
	echo "Usage: apropos [-V] [token [ ... ] ]"
	exit 0
fi

# Print version number
if [ $1 = "-V" ]; then
	echo "apropos: version $VERSION"
	exit 0
fi

# execute the command
for i in $*
do
	split_path $MANPATH $i
done

# end of "apropos"
