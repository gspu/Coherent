#!/bin/sh
# COHERENT version of command "man"
# Coherent 386 release 4.2
# Copyright (c) 1982, 1994 by Mark Williams Company.
# All rights reserved. May not be copied without permission.
#
# History:
# 10/31/90: original version, written in C (steve)
# 10/07/94: Rewritten in the shell (fwb); included support for $MANPATH

VERSION=2.0.1

# if environmental variable PAGER is not set, set it to "more"
if [ "$PAGER" = "" ]; then
	PAGER="more"
fi

# if environmental variable MANPATH is not set, set it to "/usr/man"
if [ "$MANPATH" = "" ]; then
	MANPATH="/usr/man"
fi

# find which files are to be displayed;
# $1 is a copy of MANPATH; $2 is the token being sought
split_path () {
	while : ; do
		case "$1" in
		'' | :)	break
			;;

		:*)	set -- "${1#:}" $2
			;;

		*:*)	eval "find_files ${1%:"${1#*:}"} $2"
			set -- "${1#*:} $2"
			;;

		*)	eval "find_files $1 $2"
			break
			;;
		esac
	done
	return 1
}

# find which files apply to a given token.
# $1 is the directory whose index files are to be searched;
# $2 is the token being sought
find_files () {
	display_files `awk "\$2~/^$2\$/ || \$2~/^$2\(\)\$/ \
		{ print \"$1/\" \$1 }" $1/*.index`
}

# display a bunch of files
display_files () {
	if [ $# -gt 0 ]; then
		if [ $PAGER != "echo" ]; then
			echo "\n\n\n"
		fi
		NOFILES=1
		$PAGER $*
	fi
}

# If no args, print help message and exit
if [ $# -eq 0 ]; then
	echo "
Usage: man [-wdV] article [ article ... ]
	-w displays each article's path
	-d displays all available articles
	-V displays version number
	To search for an article on a given topic, use the command 'apropos'.
"
	exit 0
fi

# check for command-line options
while : ; do
	case $1 in
		-w)	PAGER=echo ; shift ;;
		-V)	echo "man: version $VERSION" ; exit 0 ;;
		-d)	ls -C /usr/man/* | $PAGER ; exit 0 ;;
		*)	break ;;
	esac
done

for TOKEN in $*
do
	NOFILES=0

	split_path $MANPATH $TOKEN

	if [ $NOFILES -eq 0 ]; then
		echo "\nman: No manual entry for \"$TOKEN\"\n"
	fi
done

# end of "man"
