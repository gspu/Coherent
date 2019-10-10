# Generated from legalese.sh on Fri Sep 10 06:08:50 1993 CST
# Nigel's script to manage the legalese...
# Give it a directory name, and it will romp over the directory and all its
# children recursively. RCS files won't get processed since they have a
# different suffix from normal source files (handy, that...)
#
# We can do this several ways; the most computationally efficient and
# elegant solution is a recursive walk implemented by shell functions,
# while the most brutal is to do a `find target -print` into a shell
# variable and then iterate after that. Both have their appeal, since
# the brute-force approach seems like a good way of smoking out any
# fixed limits... so both are implemented

RELEASE="Coherent 386 release 4.2"
COPYRIGHT="Copyright (c) 1982, 1994 by Mark Williams Company."
RIGHTS="All rights reserved. May not be copied without permission."
WRITETO="For copying permission and licensing info, write licensing@mwc.com"

# Take a file name in $1, and apply the legalese transformation to it.
# We use a generated 'ed' script as the simplest method, and use the
# pattern-matching facility of 'case' to recognise exceptions such as
# the inetco and curses headers.
#
# The major problem with using 'ed' is that we have no control over some
# creature-features such as making sure there is the right amount of
# whitespace left lying around afterwards. Ah, well.

process-file () {
	set -- $1 $LEGALESE		# Preserve the value of $LEGALESE

	case $1 in

	*/curses.h | */term.h)
		speak $1 left alone
		return 0		# Pavel Curtis' curses package
		;;

	*/legalese.sh)
		speak $1 contains legalese boiler-plate, left alone
		return 0
		;;

	*.s)
		LEGALESE=\
"/ (-lgl
/	$RELEASE
/	$COPYRIGHT
/	$RIGHTS
/	$WRITETO
/ -lgl)
."
		;;

	*.spc |	*.c | *.h | *.y | *.l)
		LEGALESE=\
"/* (-lgl
 *	$RELEASE
 *	$COPYRIGHT
 *	$RIGHTS
 *	$WRITETO
 -lgl) */
."
		;;

	*/Makefile | */makefile | *.sh)
		LEGALESE=\
"# (-lgl
#	$RELEASE
#	$COPYRIGHT
#	$RIGHTS
#	$WRITETO
# -lgl)
."
		;;

	*.t | *.f | *.csd | *.a86 | *.asm | *.as2 | *.a68)
		echo "Bogus file encountered: $1"
		exit 1			# Puke on old garbage
		;;

	*.bak | *.old | *.new)		# Stale files
		echo "Old backup needing removal: $1"
		rm -i $1
		;;

	*)	return 0		# Unknown file suffixes
		;;
	esac


	# If the previous value of $LEGALESE is --strip, then don't insert
	# any new text, just remove the old.

	case $2 in
	--strip)
		speak -n $1 being stripped
		LEGALESE="."
		;;

	*)	speak -n $1 being bogotified
		;;
	esac


	# Now, apply zee elektrodes, herr Doktor...

	ed - $1 <<- EDIT_END 2>/dev/null
	/(-lgl/,/-lgl)/d
	0a
	${LEGALESE}
	w
	EDIT_END

	speak ... done

	LEGALESE=$2			# Restore $LEGALESE
}


# Perform process-file on every file in the directory given as $1, and
# recursively call ourselves to process any directories we find.

process-directory () {
	speak - --\> $1
	set -- $1 $i			# Preserve the value of $i
	for i in $1/*; do
		if [ -d $i ]; then
			case $RECURSE in
			--no-recurse)
				speak "Skipping directory $i"
				;;

			--recurse | "")
				process-directory $i
				;;
			esac
		else
			process-file $i
		fi
	done
	speak \<-- $1
	i=$2				# Restore $i
}


# Deal with a list of files somehow

process () {
	case $MODE in
	
	--nasty)
		case $RECURSE in
		--no-recurse)
			i=`for i do; echo $i/*; done`
			;;

		--recurse | "")
			i=`find $* -print`
			;;
		esac

		for i in $i; do
			process-file $i
		done
		;;
	
	--nice | "" )
		for i; do
			if [ -f $i ] ; then
				process-file $i
			else
				process-directory $i
			fi
		done
		;;
	
	esac
}


# By default, be verbose

speak () { echo "$@"; }


# Quick reference...

usage () {
	echo \
"Usage: $0 [[options] target-root-dir] ... [-- target-root-dir ...]
Where options are : --nice  --nasty  --verbose  --quiet  --look --strip
		    --recurse  --no-recurse"
	exit 1
}

# Process the arguments...

case $# in
0)	usage
	;;
esac


while true; do
	case $1 in

	--nice | --nasty)
		MODE=$1
		;;

	--look)	process-file () { : }
		;;

	--verbose)
		speak () { echo "$@"; }
		;;

	--quiet)
		speak () { : }
		;;

	--recurse | --no-recurse)
		RECURSE=$1
		;;

	--strip)
		LEGALESE=$1
		;;

	-\? | -h | --help)
		usage			# quick reference only, for now
		;;

	--)	shift			# end of switches
		process $*
		break
		;;

	--* | -*)
		echo "$0: Invalid switch "\'"$1"\'
		exit 1
		;;

	*)
		if false; then
			case $# in
			1)	;;
			*)	echo "$0: Too many arguments"
				exit 1
				;;
			esac
		fi
		
		process $1		
	esac

	shift
	case $# in
	0)	break
		;;
	esac
done

